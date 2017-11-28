import networkx as nx
import json

class World(object):
    def __init__(self, static_map):
        self.edge_list = [line["point"] for line in static_map["line"]]
        self.graph = nx.Graph()
        self.graph.add_edges_from(self.edge_list)
        self.start_point_to_line_idx = {}
        for line in static_map["line"]:
            start_idx = line["point"][0]
            if start_idx not in self.start_point_to_line_idx:
                self.start_point_to_line_idx[start_idx] = []
            self.start_point_to_line_idx[start_idx].append(line["idx"])
        self.line_idx_to_point = {line["idx"]:line["point"] for line in static_map["line"]}
        
    def get_neighbours(self, point_idx):
        return self.graph.neighbours(point_idx)

    def get_possible_lines(self, point_idx):
        if point_idx in self.start_point_to_line_idx:
            return self.start_point_to_line_idx[point_idx]
        else:
            return []
        
    def get_end_points(self, line_idx):
        return self.line_idx_to_point[line_idx]