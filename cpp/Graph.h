#pragma once

#include <string>
#include <vector>
#include <map>
#include <unordered_set>

struct edge {
	int id;
	int from;
	int to;
	int length;
	edge(const int i, const int f, const int t, const int len) 
		: id(i), from(f), to(t), length(len) { }
};

struct line {
	int to;
	int edge;
	int direction;
	line(const int t, const int e, const int d) 
		: to(t), edge(e), direction(d) { }
};

class Graph {

	std::unordered_set<int> vertices;
	std::vector<edge> edges;
	std::map<int, std::vector<line>> adjacency;
	std::vector<std::vector<int>> distance;
	
	static const int infinity;
public:
	Graph() = default;
	void add_vertice(const int);
	void add_edge(const int, const int, const int, const int);
	void add_adjacency(const int, const int, const int, const int);
	const std::unordered_set<int>& get_vertices() const;
	const std::vector<edge>& get_edges() const;
	const std::map<int, std::vector<line>>& get_adjacency() const;
	const std::vector<std::vector<int>>& get_distance() const;

	void print_edges() const;
	void print_adjacency() const;
	void print_vertices() const;
	void print_distance() const;
	
	void calculate_distance();
	int get_target_distance(const int, const int) const;
//	std::vector<std::vector<int>>& get_distance();

private:
	void initialize_distances();

};
