#include "Graph.h"

#include <iostream>
#include <algorithm>

const int Graph::infinity = 10000000;

/* ADD OBJECT */

void Graph::add_vertice(const int vertice){
	vertices.insert(vertice);
}

void Graph::add_edge(const int id, const int edge_from, const int edge_to, const int len){
	edges.emplace_back( edge(id, edge_from, edge_to, len) );
}

void Graph::add_adjacency(const int start_vertice, const int end_vertice, const int edge_id, const int direction){
	adjacency[start_vertice].emplace_back(line(end_vertice, edge_id, direction));
}

/* GET OBJECTS */

const std::unordered_set<int>& Graph::get_vertices() const {
	return vertices;
}

const std::vector<edge>& Graph::get_edges() const {
	return edges;
}

const std::map<int, std::vector<line>>& Graph::get_adjacency() const {
	return adjacency;
}


const std::vector<std::vector<int>>& Graph::get_distance() const{
	return distance;
}

int Graph::get_target_distance(const int i, const int j) const {
	return distance[i][j];
}

/* PRINT OBJECTS */

void Graph::print_edges() const {
	for (auto& elem : edges){
		std::cout << elem.id << " " << elem.from << " " << elem.to << " " << elem.length << std::endl; 
	}
}

void Graph::print_adjacency() const {
	for (auto& elem : adjacency){
		std::cout << "Adj: " << elem.first << " - "; 
			for (auto &ed : elem.second){
				std::cout << ed.to << " " << ed.edge << " " << ed.direction << "; "; 
			}
		std::cout << std::endl;
	}
}
	
void Graph::print_vertices() const {
	for (auto& elem : vertices){
		std::cout << elem << " "; 
	}
	std::cout << std::endl;
}

void Graph::print_distance() const {
	for (auto it = distance.begin(); it != distance.end(); ++it){
		for (auto in_it = it->begin(); in_it != it->end(); ++in_it){
			std::cout << *in_it << " "; 
		}
		std::cout << std::endl;
	}
}


void Graph::calculate_distance(){
	initialize_distances();
	const int size = distance.size();
	for (int i = 0; i < size; ++i){
		for (int j = 0; j < size; ++j){
			for (int k = 0; k < size; ++k){
				distance[j][k] = std::min(distance[j][k], distance[j][i] + distance[i][k]);
			}
		}
	}
}

void Graph::initialize_distances(){
	const int size = vertices.size();
	distance.reserve(size);
	
	for (int i = 0; i < size; ++i){
		std::vector<int> vec;
		vec.reserve(size);
		distance.push_back(std::move(vec));
		for (int j = 0; j < size; ++j){
			if (i == j){ distance[i].emplace_back(0); }
			else { distance[i].emplace_back(Graph::infinity); }
		}
	}
		
	for (auto it = edges.begin(); it != edges.end(); ++it){
		int to = it->to - 1;
		int from = it->from - 1;
		int dist = it->length;
		distance[to][from] = dist;
		distance[from][to] = dist;
	}
}
