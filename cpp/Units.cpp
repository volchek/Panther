#include "Units.h"

#include <iostream>

/* ADD NEW OBJECTS */


void Units::add_train(const int vertice, std::shared_ptr<Train> train){
	trains[vertice] = train;
}

void Units::add_town(const int vertice, std::shared_ptr<Town> town){
	towns[vertice] = town;
}

void Units::add_market(const int vertice, std::shared_ptr<Market> market){
	markets[vertice] = market;
}

void Units::add_storage(const int vertice, std::shared_ptr<Market> storage){
	storages[vertice] = storage;
}

/* UPDATE OBJECTS */

void Units::update_train(const int idx, const int product, const int prod_capacity, const int position, const int speed, const int line, const int level, const int next_level_price, const int type, const int cooldown){
	std::shared_ptr<Train> train = get_train(idx);
	if (train){
		train->set_product(product);
		train->set_capacity(prod_capacity);
		train->set_position(position);
		train->set_speed(speed);
		train->set_line_index(line);
		train->set_level(level);
		train->set_next_level_price(next_level_price);
		train->set_post_type(type);
		train->set_cooldown(cooldown);
	}
}

void Units::update_town(const int idx, const int armor, const int armor_capacity, const int population, const int population_capacity, const int product, const int product_capacity, const int level, const int price, const int cooldown){
	std::shared_ptr<Town> town = get_town(idx);
	if (town){
		town->set_armor(armor);
		town->set_armor_capacity(armor_capacity);
		town->set_population(population);
		town->set_population_capacity(population_capacity);
		town->set_product(product);
		town->set_product_capacity(product_capacity);
		town->set_level(level);
		town->set_next_level_price(price);
		town->set_cooldown(cooldown);
	}
}

void Units::update_market(const int type, const int idx, const int product, const int product_capacity, const int regeneration){
	std::shared_ptr<Market> market = nullptr;
	if (type == 2){
		market = get_market(idx);
	}
	else if (type == 3){
		market = get_storage(idx);
	}
	if (market){
		market->set_product(product);
		market->set_capacity(product_capacity);
		market->set_regeneration_rate(regeneration);
	}
}


/* PRINT OBJECTS */

void Units::print_trains() const{
	for (auto& elem : trains){
		std::cout << elem.first << " - " << elem.second->to_string() << std::endl;
	}
}

void Units::print_towns() const {
	for (auto& elem : towns){
		std::cout << elem.first << " - " << elem.second->to_string() << std::endl;
	}
}

void Units::print_markets() const {
	for (auto& elem : markets){
		std::cout << elem.first << " - " << elem.second->to_string() << std::endl;
	}
}

void Units::print_storages() const {
	for (auto& elem : storages){
		std::cout << elem.first << " - " << elem.second->to_string() << std::endl;
	}
}


/* FIND ONE OBJECT */

std::shared_ptr<Train> Units::get_train(const int id){
	auto it = trains.find(id);
	if (it != trains.end()){
		return it->second;
	}
	return nullptr;
}

std::shared_ptr<Town> Units::get_town(const int id){
	auto it = towns.find(id);
	if (it != towns.end()){
		return it->second;
	}
	return nullptr;
}

std::shared_ptr<Market> Units::get_market(const int id){
	auto it = markets.find(id);
	if (it != markets.end()){
		return it->second;
	}
	return nullptr;
}

std::shared_ptr<Market> Units::get_storage(const int id){
	auto it = storages.find(id);
	if (it != storages.end()){
		return it->second;
	}
	return nullptr;
}


/* GET FIELDS */

std::map<int, std::shared_ptr<Town>>* Units::get_towns(){
	return &towns;
}
	
std::map<int, std::shared_ptr<Market>>* Units::get_markets(){
	return &markets;
}
	
std::map<int, std::shared_ptr<Market>>* Units::get_storages(){
	return &storages;
}

std::map<int, std::shared_ptr<Train>>* Units::get_trains(){
	return &trains;
}