import time
from train import Train
import sys
sys.path.append("..")
from util.utility import ServerData

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
        return self.cur_population == self.init_population and self.init_population > -1
        
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
