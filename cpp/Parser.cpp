#include "Parser.h"

#include "settings.h"
#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <fstream>


using json = nlohmann::json;
namespace dm = Dynamic_map;
namespace sm = Static_map;
namespace mk = Market_keys;
namespace trk = Train_keys;
namespace twk = Town_keys;
namespace pk = Player_keys;

Parser::Parser(){ }

Parser::Parser(GameState* st, Player* pl)
	: game_state(st)
	, player(pl)
	{ }


Parser::~Parser(){ }


void Parser::set_game_state(GameState* st){
	game_state = st;
}

void Parser::set_player(Player* p){
	player = p;
}


void Parser::parse_login_info(std::vector<uint8_t>& msg){
	auto json_object = json::parse(msg.begin(), msg.end());
	create_player(json_object);
}


void Parser::parse_static_map(std::vector<uint8_t>& msg){
	
	auto json_object = json::parse(msg.begin(), msg.end());
	create_graph(json_object);
	create_adjacency(json_object);
}


void Parser::parse_dynamic_map(std::vector<uint8_t>& msg){
	try {
		auto json_object = json::parse(msg.begin(), msg.end());
		update_train(json_object, false);
		update_settlements(json_object, false);
		update_rating(json_object);
	} catch (std::exception& e){
		std::cout << e.what() << std::endl;
	}
}

void Parser::update_state(std::vector<uint8_t>& msg){
	auto json_object = json::parse(msg.begin(), msg.end());
	update_train(json_object);
	update_settlements(json_object);
	update_rating(json_object);
}

/* HELPERS */

int Parser::get_int_property(nlohmann::json& js, const std::string& type_obj, const int position, const std::string& key) {

	return js[type_obj][position][key].get<int>();

}

std::string Parser::get_string_property(nlohmann::json& js, const std::string& type_obj, const int position, const std::string& key) {

	return js[type_obj][position][key].get<std::string>();

}


int Parser::get_int_property(nlohmann::json& js, const std::string& key) {

	return js[key].get<int>();

}

std::string Parser::get_string_property(nlohmann::json& js, const std::string& key) {

	return js[key].get<std::string>();

}



/* CREATE OBJECTS */

void Parser::create_player(json& obj){
	std::string name = obj[pk::name].get<std::string>();
	std::string idx = obj[pk::idx].get<std::string>();
	try {
		int home_idx = obj[pk::home][pk::home_idx].get<int>();
		int home_post_id = obj[pk::home][pk::home_post_id].get<int>();
		player->set_home_idx(home_idx);
		player->set_post_id(home_post_id);
	} catch (...){ }
	player->set_name(name);
	player->set_idx(idx);
}

void Parser::update_rating(nlohmann::json& json_object){
	int rating = json_object["rating"][player->get_name()].get<int>();
	player->set_rating(rating);
}

void Parser::create_graph(json& obj){
	for (int i = 0; i < sm::line_count; ++i) {
		create_line(obj, i);
	}
}

void Parser::create_line(json& obj, const int i){

	int id = get_int_property(obj, sm::line_name, i, sm::id);
	int len = get_int_property(obj, sm::line_name, i, sm::length);
	auto points = obj[sm::line_name][i][sm::points];
	int first_point = points[0];
	int second_point = points[1];
	
	game_state->get_graph()->add_vertice(first_point);
	game_state->get_graph()->add_vertice(second_point);
	game_state->get_graph()->add_edge(id, first_point, second_point, len);
	game_state->get_graph()->add_adjacency(first_point, second_point, id, 1);
	game_state->get_graph()->add_adjacency(second_point, first_point, id, -1);
	
}

void Parser::create_adjacency(json& obj){
	for (int i = 0; i < sm::vertice_count; ++i) {
		try {
			int verticle_id = get_int_property(obj, sm::adjacency_name, i, sm::id);
			int post_id = obj[sm::adjacency_name][i][sm::unit].get<int>();
			unit_positions[post_id] = verticle_id;
		}
		catch(...){ continue; }
	}
}


void Parser::update_train(json& obj, bool is_created){

	auto data = obj[dm::train_name];
	int size = data.size();
	
	for (int i = 0; i < size; ++i){
		int id = get_int_property(obj, dm::train_name, i, trk::id);
		int product = get_int_property(obj, dm::train_name, i, trk::product);
		int capacity = get_int_property(obj, dm::train_name, i, trk::capacity);
		int position = get_int_property(obj, dm::train_name, i, trk::position);
		int speed = get_int_property(obj, dm::train_name, i, trk::speed);
		int line = get_int_property(obj, dm::train_name, i, trk::line_index);
		int level = get_int_property(obj, dm::train_name, i, trk::level);
		int next_level_price = get_int_property(obj, dm::train_name, i, trk::next_level_price);
		int cooldown = get_int_property(obj, dm::train_name, i, trk::cooldown);
		int post_type = -1;
		
		if (!is_created){
			game_state->get_units()->add_train(id, std::make_shared<Train>(Train(id, product, capacity, position, speed, line, level, next_level_price, post_type, cooldown)));
		}
		else {
			game_state->get_units()->update_train(id, product, capacity, position, speed, line, level, next_level_price, post_type, cooldown);
		}
	}
}



void Parser::update_settlements(json& obj, bool is_created){
	auto data = obj[dm::post_name];
	int size = data.size();
	for (int i = 0; i < size; ++i){
		int type = get_int_property(obj, dm::post_name, i, dm::type);
		if (type == 1){
			update_town(obj[dm::post_name][i], is_created);
		}
		else if (type == 2){
			update_market(obj[dm::post_name][i], is_created);
		}
		else if (type == 3){
			update_storage(obj[dm::post_name][i], is_created);
		}
	}
}

void Parser::update_town(json& obj, bool is_created){

	int id = get_int_property(obj, twk::id);
	std::string name = get_string_property(obj, twk::name);
	int point_id = get_int_property(obj, twk::point_id);
		
	int armor = get_int_property(obj, twk::armor);
	int armor_capacity = get_int_property(obj, twk::armor_capacity);
	int product = get_int_property(obj, twk::product);
	int product_capacity = get_int_property(obj, twk::product_capacity);
	int population = get_int_property(obj, twk::population);
	int population_capacity = get_int_property(obj, twk::population_capacity);
	
	int level = get_int_property(obj, twk::level);
	int price = get_int_property(obj, twk::price);
	int cooldown = get_int_property(obj, twk::cooldown);	
	int type = get_int_property(obj, twk::type);
	
	if (!is_created){
		auto it = unit_positions.find(id);
		if (it != unit_positions.end()){
			game_state->get_units()->add_town(it->second, std::make_shared<Town>(Town(id, point_id, name, armor, armor_capacity, population, population_capacity, product, product_capacity, level, price, type, cooldown)));
		}
	}
	else {
		game_state->get_units()->update_town(id, armor, armor_capacity, population, population_capacity, product, product_capacity, level, price, cooldown);
	}
}

void Parser::update_storage(json& obj, bool is_created){
		int id = get_int_property(obj, mk::id);
		std::string name = get_string_property(obj, mk::name);
		int capacity = get_int_property(obj, mk::armor_capacity);
		int product = get_int_property(obj, mk::armor);
		int regeneration_rate = get_int_property(obj, mk::regeneration_rate);
		int point_id = get_int_property(obj, mk::point_id);

		int type = get_int_property(obj, mk::type);
		if (!is_created){
			auto it = unit_positions.find(id);
			if (it != unit_positions.end()){
				game_state->get_units()->add_storage(it->second, std::make_shared<Market>(Market(id, product, capacity, regeneration_rate, name, point_id, type)));
			}
		}
		else {
			game_state->get_units()->update_market(type, id, product, capacity, regeneration_rate);
		}
}

void Parser::update_market(json& obj, bool is_created){
		int id = get_int_property(obj, mk::id);
		std::string name = get_string_property(obj, mk::name);
		int capacity = get_int_property(obj, mk::capacity);
		int product = get_int_property(obj, mk::product);
		int regeneration_rate = get_int_property(obj, mk::regeneration_rate);
		int point_id = get_int_property(obj, mk::point_id);
		int type = get_int_property(obj, mk::type);

		if (!is_created){
			auto it = unit_positions.find(id);
			if (it != unit_positions.end()){
				game_state->get_units()->add_market(it->second, std::make_shared<Market>(Market(id, product, capacity, regeneration_rate, name, point_id, type)));
			}
		}
		else {
			game_state->get_units()->update_market(type, id, product, capacity, regeneration_rate);
		}
}

