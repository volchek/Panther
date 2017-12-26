#pragma once

#include "Graph.h"
#include "Units.h"

#include <string>
#include <memory>
#include <vector>
#include <unordered_set>
#include <map>
#include <tuple>
#include <queue>
#include <random>
#include <algorithm>


struct pair_hash {
	inline std::size_t operator()(const std::pair<int,int> & v) const {
		return v.first*31+v.second;
	}
};

struct TrainComparator {
	bool operator() (Train* a, Train* b) {
		return a->get_product() < b->get_product();
	}
};

struct TrainUpgradeComparator {
	bool operator() (Train* a, Train* b) {
		return a->get_next_level_price() > b->get_next_level_price();
	}
};

using pair_set = std::unordered_set<std::pair<int, int>, pair_hash>;
using train_queue = std::priority_queue<Train*, std::vector<Train*>, TrainComparator>;
using train_upgrade_queue = std::priority_queue<Train*, std::vector<Train*>, TrainUpgradeComparator>;


struct Step {
	std::pair<std::vector<int>, std::vector<int>> upgrades;
	std::vector<std::tuple<int, int, int>> moves;
};

class GameState {

	std::shared_ptr<Graph> graph;
	std::shared_ptr<Units> units;
	
	pair_set possible_positions;
	std::map<int, pair_set> train_possible_positions;
	train_queue sorted_trains;
	int town_vertice;
	int town_id;
	std::vector<std::pair<int, int>> town_positions;
	std::vector<std::pair<int, std::pair<int, int>>> chosen_positions;

	static const double min_value;
	static const int min_armor;
public:
	GameState();
	std::shared_ptr<Graph> get_graph();
	std::shared_ptr<Units> get_units();
	Step get_next_step();
//	std::vector<std::tuple<int, int, int>> get_next_step();
	void set_town_info(const int, const int);

private:
	void get_possible_positions();
	bool insert_positions_for_vertice(const int, const std::vector<edge>&, const edge&, const int);
	void insert_positions_for_edge(const int, const int, const int);
	
	void sort_trains();
	
	int get_target_vertice(const Train*,  const std::pair<int, int>&, int&) const ;
	int get_vertice(const int, const int, int&) const;
	double evaluate_position(const int, Train*) const;
	void remove_equal_positions(const std::pair<int, int>&);
	bool are_equal_positions(const std::pair<int, int>&, const std::pair<int, int>&) const;
	int get_vertice_by_position(const int, const edge&) const;
	std::pair<bool, std::pair<int, int>> is_free_position(const std::pair<int, int>&);
	void upgrade_if_possible(Step*);
	
	void print_possible_positions() const;
	void print_train_possible_positions() const;
	void print_train_queue(train_queue) const;
	void print_town_info() const;
	
	// Borrowed from https://stackoverflow.com/a/896440
	template <typename InIt1, typename InIt2, typename OutIt>
	OutIt unordered_set_intersection(InIt1 b1, InIt1 e1, InIt2 b2, InIt2 e2, OutIt out) {
		while (!(b1 == e1)) {
			if (!(std::find(b2, e2, *b1) == e2)) {
				*out = *b1;
				++out;
			}
			++b1;
		}
		return out;
	}
};
