import socket
import sys
import struct
import time
import json
from enum import Enum
from random import randint
from world import World


class Action(Enum):
    LOGIN = 1
    LOGOUT = 2
    MOVE = 3
    TURN = 5
    MAP = 10
    

class Result(Enum):

    OKEY = 0
    BAD_COMMAND = 1
    RESOURCE_NOT_FOUND = 2
    PATH_NOT_FOUND = 3
    ACCESS_DENIED = 5
    
    
class Response(Enum):

    HEADER_ONLY = 7
    FULL = 8
    
    
class Train(object):
    
    def __init__(self, json):
        self.speed = json["speed"]
        self.position = json["position"]
        self.line_idx = json["line_idx"]
        self.idx = json["idx"]
      
      
class PlayerData(object):
    
    def __init__(self, initial_data):
        if initial_data["home"]:
            self.home_idx = initial_data["home"]["idx"]
        self.train_list = [Train(train_data) for train_data in initial_data["train"]]
        self.idx = initial_data["idx"]
        self.train_count = len(self.train_list)
        self.last_stopped_train_idx = 0
    
    def update_train_list(self, train):
        self.train_list = [Train(train_data) for train_data in train]    
        

class Client(object):

    HOST  = 'wgforge-srv.wargaming.net'
    PORT = 443

    def __init__(self, name):
        self.name = name
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sock.connect((Client.HOST,Client.PORT))   
        
    def close_socket(self):
        self.sock.close()
        
    def get_byte_message(self, type, json_data):
        s = str(json_data).replace("'","\"")
        return struct.pack("II%ds" % len(s), type, len(s), s.encode('utf-8'))
    
    def get_response(self, response_type, attempt_count):
        data = ""
        time.sleep(0.5)
        for a in xrange(attempt_count):
            print "attempt ", a
            data = data + self.sock.recv(4096)
            print "received string of size: ", len(data)
            if len(data) > response_type:
                code, size, json_data = struct.unpack("II%ds" % (len(data) - 8), data)
                answer = {}
                if response_type == Response.FULL:
                    answer = json.loads(json_data)
                break
            time.sleep(0.5)
        return code, answer 
        
    def login(self):
        message = self.get_byte_message(Action.LOGIN, {"name": self.name})
        self.sock.send(message)
        code, answer = self.get_response(Response.FULL, 10)
        if code == Result.OKEY:
            self.state = PlayerData(answer)
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
        return self.get_response(Response.FULL, 10)
        
    def get_train_state(self):
        code, answer = self.get_dynamic_map()
        if code == Result.OKEY:
            self.state.update_train_list(answer["train"])
            
    def get_train_point_idx(self, train_idx):
        if not(self.state.train_list[train_idx].position):
            return self.state.home_idx
        if self.state.train_list[train_idx].position == 10:
            return self.world.get_end_points(self.state.train_list[train_idx].line_idx)[1]
        return self.world.get_end_points(self.state.train_list[train_idx].line_idx)[0]    
            
                
    def move(self, line_idx, speed, train_idx):
        message = self.get_byte_message(Action.MOVE, {"line_idx":line_idx, "speed": speed, "train_idx": train_idx})
        self.sock.send(message)
        return self.get_response(Response.HEADER_ONLY, 10) 
    
    def wait_next_stop(self):
        if self.state.train_count > 0:
            a = 0
            while True:
                print "\nWAITING TICK NUMBER ", a
                time.sleep(0.5)
                self.get_train_state()
                print("STATE: ")
                for item in self.state.train_list:
                    print("Train idx {0} line {1} speed {2} position {3}".format(item.idx, item.line_idx, item.speed, item.position))
                a = a + 1
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
        print code, answer
        return True
        
    def play(self):
        if self.state.train_count > 0:
        
            for train in self.state.train_list:
                if not(train.position) or train.speed == 0:
                    self.start_moving(train)
                    break
            self.wait_next_stop()
            
           
            while True: #move until lines starting from train's current point_idx exists
                train = self.state.train_list[self.state.last_stopped_train_idx]
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
   


   
    