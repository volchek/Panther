import socket
import sys
import struct
import time
import json
from util.utility import Action, Response, Result, ServerData
from enum import Enum
from random import randint
from world.world import World
from world.path import Path
from world.train import Train
from world.player import Player
        
 
    
class Client(object):

    def __init__(self, name):
        self.name = name
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sock.connect((ServerData.HOST,ServerData.PORT))   
        
    def close_socket(self):
        self.sock.close()
        
    def get_byte_message(self, type, json_data):
        s = str(json_data).replace("'","\"")
        return struct.pack("II%ds" % len(s), type, len(s), s.encode('utf-8'))
    
    def get_response(self, response_size_type, attempt_count):
        data = ""
        for a in xrange(attempt_count):
            data = data + self.sock.recv(4096)
            if len(data) > response_size_type:
                code, size, json_data = struct.unpack("II%ds" % (len(data) - 8), data)
                answer = {}
                if response_size_type == Response.FULL:
                    answer = json.loads(json_data)
                break
            time.sleep(0.5)
        return code, answer 
        
    def login(self):
        message = self.get_byte_message(Action.LOGIN, {"name": self.name})
        self.sock.send(message)
        code, answer = self.get_response(Response.FULL, 10)
        if code == Result.OKEY:
            self.state = Player(answer)
        return code, answer    
        
    def get_static_map(self):
        message = self.get_byte_message(Action.MAP, {"layer":0})
        self.sock.send(message)
        code, answer = self.get_response(Response.FULL, 10)
        if code == Result.OKEY:
            self.world = World(answer)
        return code, answer
        
    def get_dynamic_map(self):
        message = self.get_byte_message(Action.MAP, {"layer":1})
        self.sock.send(message)
        code, answer = self.get_response(Response.FULL, 10)
        if code == Result.OKEY:
            self.world.update_market_info(answer)
            self.state.update_town_info(answer)
            self.state.update_train_list(answer["train"])
        return code, answer    
        
              
    def get_train_point_idx(self, train_idx):
        if not(self.state.train_list[train_idx].position):
            return self.state.home_idx
        line_idx = self.state.train_list[train_idx].line_idx    
        if self.state.train_list[train_idx].position == self.world.get_line_length(line_idx):
            return self.world.get_end_points(line_idx)[1]
        return self.world.get_end_points(line_idx)[0]    
            
                
    def move(self, line_idx, speed, train_idx):
        message = self.get_byte_message(Action.MOVE, {"line_idx":line_idx, "speed": speed, "train_idx": train_idx})
        self.sock.send(message)
        return self.get_response(Response.HEADER_ONLY, 10) 
    
    def wait_next_stop(self):
        if self.state.train_count > 0:
            while True:
                self.state.one_more_tick()
                self.get_dynamic_map()
                self.state.display()
                i = 0
                while i < self.state.train_count:
                    train = self.state.train_list[i]
                    if train.speed == 0 or not(train.position):
                        self.state.last_stopped_train_idx = i
                        break
                    i = i + 1    
                if i < self.state.train_count:
                    break
                
    def start_moving(self, train):
        line_idx = train.line_idx
        point_idx = self.get_train_point_idx(train.idx)
        possible_lines = self.world.get_possible_lines(point_idx)
        if len(possible_lines) == 0:
            print("There is no lines starting from point {0}".format(point_idx))
            return False
        line_idx = possible_lines[randint(0, len(possible_lines) - 1)]
        speed = 1
        print("\nMOVE train_idx {0} line: {1} speed: {2}".format(train.idx, line_idx, speed))
        code, answer = self.move(line_idx, speed, train.idx)
        return True
        
    def find_useful_paths(self):
        market_ids = self.world.get_market_ids()
        self.paths = {}
        path_counter = 0
        for id in market_ids: #one-shop pathes
            path,length = self.world.get_shortest_path(self.state.post_id, id)
            if self.world.get_max_income([id], 2*length, self.state.cur_population) > 0:
                self.paths[path_counter] = Path([id],{id:length},path[1:]+path[::-1][1:],2*length)
                path_counter += 1
        for id1 in market_ids: #two-shop pathes
            for id2 in market_ids:
                if id1 != id2:
                    path1,length1 = self.world.get_shortest_path(self.state.post_id, id1)
                    path2,length2 = self.world.get_shortest_path(id1, id2)
                    path3,length3 = self.world.get_shortest_path(id2, self.state.post_id)
                    market_list = [id1, id2]
                    length = length1+length2+length3
                    if self.world.get_max_income(market_list, length, self.state.cur_population) > 0:
                        self.paths[path_counter] = Path(market_list,{id1:length1, id2: length1+length2},path1[1:]+path2[1:]+path3[1:],length)
                        path_counter += 1
                        
                        
    def find_best_path_id(self):
        max_score_path_id = -1
        max_score = 0
        for path_id in self.paths:
            score = self.paths[path_id].get_score(self.world, self.state.cur_population)
            if score > max_score: 
                max_score = score
                max_score_path_id = path_id
        return max_score_path_id

    def move_path(self, path_id, train):
        prev_point = self.state.post_id
        for point in self.paths[path_id].path:
            if (prev_point, point) in self.world.edge_labels:
                line_idx = self.world.edge_labels[(prev_point, point)]["id"]
            elif (point, prev_point) in self.world.edge_labels:
                line_idx = self.world.edge_labels[(point, prev_point)]["id"]
            else:
                break
            speed = 1
            print("\nMOVE train_idx {0} line: {1} speed: {2}".format(train.idx, line_idx, speed))
            code, answer = self.move(line_idx, speed, train.idx)
            prev_point = point
            self.wait_next_stop()
            
            
    def play(self):
        self.find_useful_paths()
        best_path_id = self.find_best_path_id()
        train = self.state.train_list[0]
        self.move_path(best_path_id, train)
        while self.state.everybody_alive():
            best_path_id = self.find_best_path_id()
            self.move_path(best_path_id, train)
            
    def random_play(self):
        
        if self.state.train_count > 0:
        
            for train in self.state.train_list:
                if not(train.position) or train.speed == 0:
                    self.start_moving(train)
                    break
            self.wait_next_stop()
            
            while self.state.everybody_alive(): #move until lines starting from train's current point_idx exists
                train = self.state.train_list[self.state.last_stopped_train_idx] #and everybody alive
                is_moving = self.start_moving(train)
                if not is_moving:
                    break
                self.wait_next_stop()
            
            
        
    def turn(self):
        message = self.get_byte_message(Action.TURN, {})
        self.sock.send(message)
        return self.get_response(Response.HEADER_ONLY,10)
        
    def logout(self):
        message = self.get_byte_message(Action.LOGOUT, {})
        self.sock.send(message)
        return self.get_response(Response.HEADER_ONLY, 10)    
   


   
    