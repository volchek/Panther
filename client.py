import socket
import sys
import struct
import time
import json
from utility import Action, Response, Result
from enum import Enum
from random import randint
from world import World

   
class Train(object):
    
    def __init__(self, json):
        self.speed = json["speed"]
        self.position = json["position"]
        self.line_idx = json["line_idx"]
        self.idx = json["idx"]
      
      
class Player(object):
    
    def __init__(self, initial_data):
        if initial_data["home"]:
            self.home_idx = initial_data["home"]["idx"]
            self.post_id = initial_data["home"]["post_id"]
        else:
            self.home_idx = -1
            self.post_id = -1
        self.train_list = [Train(train_data) for train_data in initial_data["train"]]
        self.idx = initial_data["idx"]
        self.train_count = len(self.train_list)
        if initial_data["town"]:
            self.product = initial_data["town"]["product"]
            self.init_population = initial_data["town"]["population"]
            self.cur_population = initial_data["town"]["population"]
        else:
            self.product = -1
            self.init_population = -1
            self.cur_population = -1
        self.last_stopped_train_idx = 0
        self.tick_count = 0
    
    def everybody_alive(self):
        return self.cur_population == self.init_population
        
    def display(self):
        print("\nSTATE: ")
        print("population {0} product {1}".format(self.cur_population, self.product))
        for item in self.train_list:
            print("Train idx {0} line {1} speed {2} position {3}".format(item.idx, item.line_idx, item.speed, item.position))
 
    def one_more_tick(self):
        time.sleep(ServerData.QUANT)
        self.tick_count += 1
        print "\nTICK ", self.tick_count                    
        
    def update_train_list(self, train):
        self.train_list = [Train(train_data) for train_data in train]   

    def update_town_info(self, answer):
        for post in answer["post"]:
            if post["idx"] == self.post_id:
                self.product = post["product"]
                self.cur_population = post["population"]
                break    
        
        

class ServerData(object):

    HOST  = 'wgforge-srv.wargaming.net'
    PORT = 443
    QUANT = 10     
    
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
        
    def play(self):
        if self.state.train_count > 0:
        
            for train in self.state.train_list:
                if not(train.position) or train.speed == 0:
                    self.start_moving(train)
                    break
            self.wait_next_stop()
            
            counter = 0
            while self.state.everybody_alive(): #move until lines starting from train's current point_idx exists
                train = self.state.train_list[self.state.last_stopped_train_idx] #and everybody alive
                is_moving = self.start_moving(train)
                if not is_moving:
                    break
                counter = counter+1 
                self.wait_next_stop()
            
            
        
    def turn(self):
        message = self.get_byte_message(Action.TURN, {})
        self.sock.send(message)
        return self.get_response(Response.HEADER_ONLY,10)
        
    def logout(self):
        message = self.get_byte_message(Action.LOGOUT, {})
        self.sock.send(message)
        return self.get_response(Response.HEADER_ONLY, 10)    
   


   
    