import networkx as nx
import json
from utility import PostType
import matplotlib.pyplot as plt

class Line(object):

    def __init__(self, line_data):
        self.start_point_idx = line_data["point"][0]
        self.end_point_idx = line_data["point"][1]
        self.length = line_data["length"]
        self.idx = line_data["idx"]
        
class Post(object):
     
    def __init__(self, post_data):
        self.idx = post_data["idx"]
        self.product = post_data["product"]
        if post_data["type"] == PostType.HOME:
            self.population = post_data["population"]
        else:    
            self.replenishment = post_data["replenishment"]
            self.capacity = post_data["product_capacity"]
        
class World(object):

    def __init__(self, static_map):
        self.lines = {line["idx"]:Line(line) for line in static_map["line"]}
        self.graph = nx.Graph()
        for line in static_map["line"]:
                self.graph.add_edge(line["point"][0],line["point"][1], key = line["idx"], weight = line['length'])
        self.edge_labels=dict([((line["point"][0],line["point"][1]),{"id": line["idx"], "weight":line["length"]})
             for line in static_map["line"]])
        self.start_point_to_line_idx = {}
        for line in static_map["line"]:
            start_idx = line["point"][0]
            if start_idx not in self.start_point_to_line_idx:
                self.start_point_to_line_idx[start_idx] = []
            self.start_point_to_line_idx[start_idx].append(line["idx"])
        self.line_idx_to_point = {line["idx"]:line["point"] for line in static_map["line"]}
    
    def update_market_info(self, dynamic_map):
        self.markets = {post["idx"]:Post(post) for post in dynamic_map["post"] if post["type"] == PostType.MARKET}
        
    def display_map(self):
        pos = nx.spring_layout(self.graph)
        nx.draw(self.graph, pos, with_labels = True)
        nx.draw_networkx_edge_labels(self.graph,pos,edge_labels=self.edge_labels)
        plt.show()
           
    def get_neighbours(self, point_idx):
        return self.graph.neighbours(point_idx)

    def get_possible_lines(self, point_idx):
        if point_idx in self.start_point_to_line_idx:
            return self.start_point_to_line_idx[point_idx]
        else:
            return []
        
    def get_end_points(self, line_idx):
        return self.line_idx_to_point[line_idx]
        
    def get_line_length(self, line_idx):
        return self.lines[line_idx].length