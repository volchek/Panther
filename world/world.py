import networkx as nx
import json
from line import Line
from post import Post, PostType
import matplotlib.pyplot as plt
  
class World(object):

    def __init__(self, static_map):
        self.lines = {line["idx"]:Line(line) for line in static_map["line"]}
        self.graph = nx.Graph()
        for line in static_map["line"]:
                self.graph.add_edge(line["point"][0],line["point"][1], key = line["idx"], weight = line['length'])
        self.edge_labels=dict([((line["point"][0],line["point"][1]),{"id": line["idx"], "weight":line["length"]})
             for line in static_map["line"]])
        self.start_point_to_line_idx = {}
        self.shortest_path=dict(nx.all_pairs_dijkstra_path(self.graph))
        self.length=dict(nx.all_pairs_dijkstra_path_length(self.graph))
        self.post_id_to_point = {point["post_id"]:point["idx"] for point in static_map["point"] if point["post_id"]}
        for line in static_map["line"]:
            start_idx = line["point"][0]
            if start_idx not in self.start_point_to_line_idx:
                self.start_point_to_line_idx[start_idx] = []
            self.start_point_to_line_idx[start_idx].append(line["idx"])
        self.line_idx_to_point = {line["idx"]:line["point"] for line in static_map["line"]}
    
    def update_market_info(self, dynamic_map):
        self.markets = {post["idx"]:Post(post) for post in dynamic_map["post"] if post["type"] == PostType.MARKET}
        
    def get_market_ids(self):
        return self.markets.keys();
    
    def get_shortest_path(self, post_id1, post_id2):
        if post_id1 == PostType.EMPTY or post_id2 == PostType.EMPTY:
            return [],0
        id1 = self.post_id_to_point[post_id1]
        id2 = self.post_id_to_point[post_id2]
        return self.shortest_path[id1][id2], self.length[id1][id2]
        
    def get_max_income(self, market_ids, length, population):
        income = 0
        for mid in market_ids:
            income += self.markets[mid].capacity
        income -= population*length
        return income    
        
    def get_potential_market_product(self, id, timeout):
        return max(self.markets[id].product+timeout*self.markets[id].replenishment, self.markets[id].capacity)
        
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