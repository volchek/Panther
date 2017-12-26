#pragma once

#include "Player.h"
#include "Client.h"
#include "Train.h"
#include "Settlement.h"
#include "Town.h"
#include "Market.h"
#include "GameState.h"
#include "GameUnit.h"

#include <asio.hpp>
#include <json.hpp>

#include <memory>
#include <vector>

class Parser {
public:
	Parser();
	Parser(GameState*, Player*);
	~Parser();

	void set_game_state(GameState*);
	void set_player(Player*);
	
	void parse_login_info(std::vector<uint8_t>&);
	void parse_dynamic_map(std::vector<uint8_t>&);
	void parse_static_map(std::vector<uint8_t>&);
	void update_state(std::vector<uint8_t>&);
	
	
private:
	int get_int_property(nlohmann::json&, const std::string&);
	std::string get_string_property(nlohmann::json&, const std::string&);
	int get_int_property(nlohmann::json&, const std::string&, const int, const std::string&);
	std::string get_string_property(nlohmann::json&, const std::string&, const int, const std::string&);

	void create_player(nlohmann::json&);
	void create_line(nlohmann::json&, const int);
	void create_graph(nlohmann::json&);
	void create_adjacency(nlohmann::json&);

	void update_train(nlohmann::json&, bool is_created = true);
	void update_settlements(nlohmann::json&, bool is_created = true);
	void update_town(nlohmann::json&, bool is_created = true);
	void update_market(nlohmann::json&, bool is_created = true);
	void update_storage(nlohmann::json&, bool is_created = true);
	
	void update_rating(nlohmann::json&);

private:
	GameState* game_state = nullptr;
	Player* player = nullptr;
	std::map<int, int> unit_positions;

};