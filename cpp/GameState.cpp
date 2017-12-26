#include "GameState.h"

#include <iostream>
#include <iterator>
#include <stdlib.h>
#include <time.h>

const double GameState::min_value = -1e9;
const int GameState::min_armor = 50;


GameState::GameState()
	: graph(std::make_shared<Graph>(Graph()))
	, units(std::make_shared<Units>(Units()))
	{
		srand(time(NULL));
	}
	
std::shared_ptr<Graph> GameState::get_graph(){
	return graph;
}

std::shared_ptr<Units> GameState::get_units(){
	return units;
}

void GameState::set_town_info(const int vertice, const int id){
	town_vertice = vertice;
	town_id = id;
	auto it = graph->get_adjacency().find(town_vertice);
	if (it == (graph->get_adjacency()).end()){
		return;
	}
	
	for (auto & elem : it->second){
		int line_id = elem.edge;
		int position;
		edge line = (graph->get_edges())[line_id - 1];
		if (town_vertice == line.from) { position = 0; }
		else if (town_vertice == line.to){ position = line.length; }
		town_positions.emplace_back(std::make_pair(line_id, position));
	}
	print_town_info();
}

Step GameState::get_next_step(){
//std::vector<std::tuple<int, int, int>> GameState::get_next_step(){
	get_possible_positions();
	print_possible_positions();
	print_train_possible_positions();
	sort_trains();
	std::vector<std::tuple<int, int, int>> steps;
	Step result;

	upgrade_if_possible(&result);

	while(!sorted_trains.empty()){
		Train* tr = sorted_trains.top();
		int train_id = tr->get_id();
		if (tr->get_cooldown() > 0 
				|| (std::find(result.upgrades.second.begin(), result.upgrades.second.end(), train_id) != result.upgrades.second.end())){
			sorted_trains.pop();
			continue;
		}
		std::pair<int, int> train_position = tr->get_current_position();
		std::cout << "Where the train " << train_id << " stands now: " << train_position.first << " " << train_position.second << std::endl;
		std::vector<std::pair<int, int>> intersection;
		pair_set train_pos = train_possible_positions[train_id];
		std::cout << "Current positions for train " << train_id << std::endl;
		for (auto & el : train_pos){
			std::cout << el.first << " " << el.second << std::endl;
		}
		unordered_set_intersection(train_pos.begin(), train_pos.end(), 
				possible_positions.begin(), possible_positions.end(),
				std::back_inserter(intersection));
		std::cout << "Current intersection for train " << train_id << std::endl;
		for (auto & el : intersection){
			std::cout << el.first << " " << el.second << std::endl;
		}
		
		if (intersection.empty()) { sorted_trains.pop(); continue; }

		auto current_train_position_status = is_free_position(tr->get_current_position());
		if (current_train_position_status.first == false) {
			auto rival_position = current_train_position_status.second;
			for (auto it = intersection.begin(); it != intersection.end();) {
				if (are_equal_positions(*it, rival_position)) {
					it = intersection.erase(it);
					break;
				}
				else { ++it; }
			}
		}
		
		int best_line;
		int best_speed;
		double best_evaluation = min_value;
		std::pair<int, int> best_position;
		for (auto & elem : intersection){
			int speed;
			int target_vertice = get_target_vertice(tr, elem, speed);
			double evaluation = evaluate_position(target_vertice, tr);
			std::cout << train_id << ": " << target_vertice << " " << evaluation << " " << speed << std::endl;
			if (evaluation > best_evaluation){
				std::cout << "Replacing best_evaluation value " << best_evaluation << " with " << evaluation << std::endl;
				best_evaluation = evaluation;
				best_speed = speed;
				best_line = elem.first;
				best_position = elem;
			}
		}
		
		remove_equal_positions(best_position);
		chosen_positions.push_back({train_id, best_position});
		print_possible_positions();

		result.moves.emplace_back(std::make_tuple(train_id, best_line, 0));
		result.moves.emplace_back(std::make_tuple(train_id, best_line, best_speed));
//		steps.emplace_back(std::make_tuple(train_id, best_line, 0));
//		steps.emplace_back(std::make_tuple(train_id, best_line, best_speed));
		sorted_trains.pop();
	}
//	return steps;
	return result;
}

void GameState::upgrade_if_possible(Step* result){
	std::vector<int> town_upgrades;
	std::vector<int> train_upgrades;
	
	int current_town_armor = units->get_town(town_id)->get_armor();
	
	int town_upgrade_price = units->get_town(town_id)->get_next_level_price();
	if (current_town_armor - town_upgrade_price > min_armor){
		town_upgrades.push_back(town_id);
		current_town_armor -= town_upgrade_price;
	}

	auto trains = units->get_trains();
	train_upgrade_queue train_queue;
	for (auto train : *trains){
		train_queue.push(train.second.get());
	}
	while (!train_queue.empty()){
		Train* tr = train_queue.top();
		int current_train_armor = tr->get_next_level_price();
		// std::cout << "This train's upgrade cost: " << current_train_armor << std::endl;
		if ((current_town_armor - current_train_armor > min_armor)
			&& std::find(town_positions.begin(), town_positions.end(), tr->get_current_position()) != town_positions.end()){
			train_upgrades.push_back(tr->get_id());
			current_town_armor -= current_train_armor;
			train_queue.pop();
		} else {
			break;
		}
	}
	result->upgrades = std::make_pair(town_upgrades, train_upgrades);
}

std::pair<bool, std::pair<int, int>> GameState::is_free_position(const std::pair<int, int>& current_pos) {
	for (auto& elem : chosen_positions){
		if (are_equal_positions(elem.second, current_pos)){
			auto rival_position = units->get_train(elem.first)->get_current_position();
			return {false, rival_position};
		}
	}
	return {true, {-1, -1}};
}

void GameState::remove_equal_positions(const std::pair<int, int>& position){
	if (std::find(town_positions.begin(), town_positions.end(), position) != town_positions.end()) { return; }
	for (auto it = possible_positions.begin(); it != possible_positions.end(); ){
		if (are_equal_positions(*it, position)){
			std::cout << "Erase: " << it->first << " " << it->second << std::endl;
			it = possible_positions.erase(it);
		}
		else {
			++it;
		}
	}
}

bool GameState::are_equal_positions(const std::pair<int, int>& first_pair, const std::pair<int, int>& second_pair) const {
	int first_pos = first_pair.second;
	int second_pos = second_pair.second;
	if (first_pair.first == second_pair.first && first_pos == second_pos){
		return true;
	}
	
	std::vector<edge> vec = graph->get_edges();
	edge first_edge = vec[first_pair.first - 1];
	edge second_edge = vec[second_pair.first - 1];
	int first_vertice = get_vertice_by_position(first_pos, first_edge);
	int second_vertice = get_vertice_by_position(second_pos, second_edge);
	if (first_vertice == second_vertice && first_vertice != -1){
		return true;
	}
	
	return false;
}

int GameState::get_vertice_by_position(const int pos, const edge& current_edge) const{
	int vertice = - 1;
	if (pos == 0){
		vertice = current_edge.from;
	}
	else if (pos == current_edge.length){
		vertice = current_edge.to;
	}
	return vertice;
}

/* GET POSITIONS */

void GameState::get_possible_positions(){
	possible_positions.clear();
	train_possible_positions.clear();
	chosen_positions.clear();
	std::map<int, std::shared_ptr<Train>>* trains = units->get_trains();
	
	for (auto& elem : *trains){
		std::pair<int, int> train_position = elem.second->get_current_position();
		int line = train_position.first;
		int position = train_position.second;
		
		std::vector<edge> vec = graph->get_edges();
		edge current_edge = vec[line - 1];
		int line_len = current_edge.length;
		int train_id = elem.second->get_id();
		if (position != 0 && position != line_len){
			insert_positions_for_edge(line, position, train_id);
		}
		else {
			insert_positions_for_vertice(position, vec, current_edge, train_id);
		} 
	}
}


bool GameState::insert_positions_for_vertice(const int position, const std::vector<edge>& all_edges, const edge& current_edge, const int train_id){
	int current_vertice;
	if (position == 0){ current_vertice = current_edge.from; }
	else { current_vertice = current_edge.to; }
	auto it = graph->get_adjacency().find(current_vertice);
	if (it == (graph->get_adjacency()).end()){
		return false;
	}
	auto possible_lines = it->second;
	for (auto& one_line : possible_lines){
			int adjacent_edge = one_line.edge;
			int position;
			if (one_line.direction == 1) {
				position = 1;
			} else if (one_line.direction == -1) {
				position = graph->get_edges()[adjacent_edge - 1].length - 1;
			} else {
				std::cout << "Warning: Impossible edge direction" << std::endl;
			}
			std::pair<int, int> current_pos = {adjacent_edge, position};
			possible_positions.insert(current_pos);
			train_possible_positions[train_id].insert(current_pos);
		}

	return true;
}


void GameState::insert_positions_for_edge(const int line, const int position, const int train_id){
	std::pair<int, int> forward_pos = {line, position + 1};
	std::pair<int, int> back_pos = {line, position - 1};
	possible_positions.insert(forward_pos);
	possible_positions.insert(back_pos);
	train_possible_positions[train_id].insert(forward_pos);
	train_possible_positions[train_id].insert(back_pos);
}

/* SORT TRAINS */

void GameState::sort_trains(){
	if (!sorted_trains.empty()){
		train_queue s_trains;
		sorted_trains.swap(s_trains);
	}
	std::cout << "Priority queue size: " << sorted_trains.size() << std::endl;
	std::map<int, std::shared_ptr<Train>>* trains = units->get_trains();
	for (auto& train : *trains){
		sorted_trains.push(train.second.get());
	}
}

/* HELPERS */

int GameState::get_target_vertice(const Train* train, const std::pair<int, int>& possible_pos, int& speed) const {
	int current_line = train->get_line_index();
	int current_position = train->get_position();
	int possible_line = possible_pos.first;
	int possible_position = possible_pos.second;
	edge current_edge = (graph->get_edges())[current_line - 1];
	edge possible_edge = (graph->get_edges())[possible_line - 1];
	int line_length = current_edge.length;
	int vertice = -1;
	if (current_position == 0){
		int current_vertice = current_edge.from;
		vertice = get_vertice(possible_line, current_vertice, speed);
	}
	else if (current_position == line_length){
		int current_vertice = current_edge.to;
		vertice = get_vertice(possible_line, current_vertice, speed);
	}
	else {
		if (current_line != possible_line) {
			std::cout << "Warning (line id mismatch): " << current_line << " " << possible_line << std::endl;
			if (possible_position == 0) {
				vertice = possible_edge.from;
			} else {
				vertice = possible_edge.to;
			}
			speed = train->get_speed();
		} else {
			if (possible_position > current_position){ 
				vertice = current_edge.to;
				speed = 1;
			} else { 
				vertice = current_edge.from;
				speed = -1;
			}
		}
	}
	return vertice;
}

int GameState::get_vertice(const int possible_line, const int current_vertice, int& speed) const {
	int vertice;
	edge next_edge = (graph->get_edges())[possible_line - 1];
	if (current_vertice == next_edge.from){
		vertice = next_edge.to;
		speed = 1;
	} else if (current_vertice == next_edge.to) {
		vertice = next_edge.from;
		speed = -1;
	} else {
		std::cout << "Warning (unexpected vertice): " << current_vertice << std::endl;
	}
	return vertice;
}

double GameState::evaluate_position(const int target_vertice, Train* train) const {
	// Dumb strategy 1: take random moves
	// double evaluate = (double) rand() / RAND_MAX;
	
	// Dumb strategy 2: move as far from the town as possible
	// double evaluate = graph->get_target_distance(town_vertice, target_vertice);
	
	// Simple strategy
	int product_in_town = units->get_town(town_id)->get_product();
	int population_in_town = units->get_town(town_id)->get_population();
	int distance_to_town = graph->get_target_distance(target_vertice, town_id);
	double ticks_before_starvation = product_in_town / ((population_in_town + 0.5) * 1.05);
	double load_to_capacity = (double) train->get_product() / train->get_capacity();
	double towards_town = train->get_product() * load_to_capacity * (1000 - ticks_before_starvation) * (100 - distance_to_town);
	// TODO: Fractional power between 1 and 2 for load_to_capacity?
	
	std::cout << "Force pushing towards town into vertice " << target_vertice << ": " << towards_town << std::endl;
	
	int train_post_type = train->get_post_type();
	double food_weight;
	if (train_post_type == 2) { // 2 is the food type
		food_weight = 1.0;
	} else if (ticks_before_starvation > 100 || train_post_type == 3){ // 3 is the armor type
		food_weight = 0.0;
	} else {
		food_weight = 0.7;
	}
	
	double towards_food = 0;
	double towards_armor = 0;
	auto markets = units->get_markets();
	auto storages = units->get_storages();
	
	for (auto it = markets->begin(); it != markets->end(); ++it) {
		int point_id = it->second->get_point_id();
		int product = it->second->get_product();
		int distance = graph->get_target_distance(target_vertice, point_id);
		towards_food += (100 - distance) * product;
	}
	
	for (auto it = storages->begin(); it != storages->end(); ++it) {
		int point_id = it->second->get_point_id();
		int product = it->second->get_product();
		int distance = graph->get_target_distance(target_vertice, point_id);
		towards_armor += (100 - distance) * product;
	}
	
	double towards_markets = food_weight * towards_food + (1 - food_weight) * towards_armor;
	std::cout << "Force pushing towards markets into vertice " << target_vertice << ": " << towards_markets << std::endl;
	
	double evaluate = 10 * towards_town + 1 * towards_markets;
	
	return evaluate;
}


/* PRINT OBJECTS */

void GameState::print_possible_positions() const {
	std::cout << "\nAll possible positions:" << std::endl;
	for (auto& el : possible_positions){
		std::cout << el.first << " " << el.second << std::endl;
	}
	std::cout << std::endl;
}


void GameState::print_train_possible_positions() const {
	std::cout << "Train positions:" << std::endl;
	for (auto& el : train_possible_positions){
		std::cout << el.first << ": ";
		for (auto& in : el.second){
			std::cout << in.first << " " << in.second << "; ";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}

void GameState::print_train_queue(train_queue trains) const {
	std::cout << "Trains: " << std::endl;
	while(!trains.empty()){
		Train* tr = trains.top();
		std::cout << tr->get_id() << " - " << tr->get_product() << std::endl;
		trains.pop();
	}
}

void GameState::print_town_info() const {
	std::cout << "Town info: " << std::endl;
	std::cout << "vertice: " << town_vertice << std::endl;
	for (auto & el : town_positions){
		std::cout << el.first << " " << el.second << "; ";
	}
	std::cout << std::endl;
}