#pragma once

#include "Train.h"
#include "Town.h"
#include "Market.h"

#include <map>
#include <memory>

class Units {
	std::map<int, std::shared_ptr<Train>> trains;
	std::map<int, std::shared_ptr<Town>> towns;
	std::map<int, std::shared_ptr<Market>> markets;
	std::map<int, std::shared_ptr<Market>> storages;
	
public:
	void add_train(const int, std::shared_ptr<Train>);
	void add_town(const int, std::shared_ptr<Town>);
	void add_market(const int, std::shared_ptr<Market>);
	void add_storage(const int, std::shared_ptr<Market>);
	
	void update_train(const int, const int, const int, const int, const int, const int, const int, const int, const int, const int);
	void update_town(const int, const int, const int, const int, const int, const int, const int, const int, const int, const int);
	void update_market(const int, const int, const int, const int, const int);
	
	void print_trains() const;
	void print_towns() const;
	void print_markets() const;
	void print_storages() const;
	
	std::map<int, std::shared_ptr<Town>>* get_towns();
	std::map<int, std::shared_ptr<Market>>* get_markets();
	std::map<int, std::shared_ptr<Market>>* get_storages();
	std::map<int, std::shared_ptr<Train>>* get_trains();
	
	std::shared_ptr<Train> get_train(const int);
	std::shared_ptr<Town> get_town(const int);
	std::shared_ptr<Market> get_market(const int);
	std::shared_ptr<Market> get_storage(const int);
};