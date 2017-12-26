#include "Strategy.h"

#include "settings.h"
#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <fstream>
#include <exception>

namespace trk = Train_keys;

Strategy::Strategy(Client& cl) 
	: client(cl)
	, parser(&gs, &player)
	, tick_count(1e9)
	{ }

Strategy::Strategy(Client& cl, int count)
	: Strategy(cl)
	{ tick_count = count; }

void Strategy::run(){

	bool status = client.connect();

	std::vector<uint8_t> login_response = std::move(client.login(Client_settings::default_login_msg));
	parser.parse_login_info(login_response);
	
	std::vector<uint8_t> static_map = std::move(client.get_static_map());
	logger.log(static_map, "static_map");
	parser.parse_static_map(static_map);
	
	std::vector<uint8_t> coord_map = std::move(client.get_coord_map());
	logger.log(coord_map, "coordinates");

	std::vector<uint8_t> dynamic_map = std::move(client.get_dynamic_map());
	logger.log(dynamic_map);
	parser.parse_dynamic_map(dynamic_map);
	
	gs.get_graph()->calculate_distance();
	gs.set_town_info(player.get_home_idx(), player.get_post_id());
	
	gs.get_graph()->print_adjacency();
	
	try {
		for (int i = 0; i < tick_count; ++i){ 
			strategy();
		}
	}
	catch (std::exception& e) {
		std::cout << e.what() << std::endl;
		client.logout();
	}

	client.logout();
	
}


/* PRINT OBJECTS */

void Strategy::print_objects(){
	gs.get_graph()->print_edges();
	gs.get_graph()->print_adjacency();
	gs.get_graph()->print_vertices();
	gs.get_units()->print_trains();
	gs.get_units()->print_towns();
	gs.get_units()->print_markets();
	gs.get_units()->print_storages();
	player.print_info();
}

void Strategy::print_state(){
	gs.get_units()->print_trains();
	gs.get_units()->print_towns();
	gs.get_units()->print_markets();
	gs.get_units()->print_storages();	
}

/* CREATE MSG */

std::string Strategy::create_move_msg(const int id, const int speed, const int line){
	char msg[70];
	msg[0] = '\0';
	sprintf(msg, "{\n\"%s\": %d, \"%s\": %d, \"%s\": %d\n}", trk::line_index, line, trk::speed, speed, trk::train_index, id);
	std::cout << msg << std::endl;
	std::string str_msg(msg);
	return str_msg;
}

std::string Strategy::create_upgrade_msg(const std::vector<int>& posts, const std::vector<int>& trains){
	std::string msg = "{\n\"post\": [";
	for (auto elem : posts){
		msg += std::to_string(elem);
		msg += ",";
	}
	if (!posts.empty()) { msg.pop_back(); }
	msg += "],\n\"train\": [";
	for (auto elem : trains){
		msg += std::to_string(elem);
		msg += ",";
	}
	if (!trains.empty()){ msg.pop_back(); }
	msg += "]\n}";
	std::cout << msg << std::endl;
	return msg;
}


/* MOVE */


void Strategy::update(){
	std::vector<uint8_t> dynamic_map = std::move(client.get_dynamic_map());
	logger.log(dynamic_map);
	parser.update_state(dynamic_map);
}


void Strategy::strategy(){

	Step res = gs.get_next_step();
	std::vector<std::tuple<int, int, int>> moves = std::move(res.moves);
	std::pair<std::vector<int>, std::vector<int>> upgrades = std::move(res.upgrades);
	try {
		if (!upgrades.first.empty() || !upgrades.second.empty()){
			std::string msg = create_upgrade_msg(upgrades.first, upgrades.second);
			auto res = std::move(client.upgrade(msg.c_str()));
		}
	} catch (std::exception& e){
		std::cout << e.what() << std::endl;
	}
	
//	std::vector<std::tuple<int, int, int>> commands = std::move(gs.get_next_step());
	int train_id, line, speed;
	for (auto & elem : moves){
		try {
			std::tie(train_id, line, speed) = elem;
			std::string msg = create_move_msg(train_id, speed, line);
			auto res = std::move(client.move(msg.c_str()));
			for (auto elem: res) {
				if (elem == '\0') { break; }
				std::cout << elem;
			}
			std::cout << std::endl;
		} catch (std::exception& e){
			break;
		}
	}
	client.turn();
	update();
}
