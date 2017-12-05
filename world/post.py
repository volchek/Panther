from enum import Enum

class PostType(Enum):
    
    EMPTY = -1
    HOME = 1
    MARKET = 2

class Post(object):
     
    def __init__(self, post_data):
        self.idx = post_data["idx"]
        self.product = post_data["product"]
        if post_data["type"] == PostType.HOME:
            self.population = post_data["population"]
        else:    
            self.replenishment = post_data["replenishment"]
            self.capacity = post_data["product_capacity"]
