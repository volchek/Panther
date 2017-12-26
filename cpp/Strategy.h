#pragma once

#include "Parser.h"
#include "Player.h"
#include "Client.h"
#include "Train.h"
#include "Settlement.h"
#include "Town.h"
#include "Market.h"
#include "GameState.h"
#include "GameUnit.h"
#include "Logger.h"

#include <asio.hpp>

#include <memory>
#include <vector>
#include <tuple>

class Strategy {

public:
	Strategy(Client&);
	Strategy(Client&, int);
	void run();

private:
	void strategy();
	void update();
	void print_objects();
	void print_state();
	
	std::string create_move_msg(const int, const int, const int);
	std::string create_upgrade_msg(const std::vector<int>& posts, const std::vector<int>& trains);
	
private:
	Player player;
	GameState gs;

	Client& client;
	Parser parser;
	Logger logger;
	
	int tick_count;
};