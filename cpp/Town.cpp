#include "Town.h"

#include <iostream>

Town::Town(const int new_id, const int new_point, const std::string& new_name) 
	: Settlement(new_id, new_name, new_point)
	{}


Town::Town(const int new_id, const int new_point, const std::string& new_name, const int new_armor, const int new_armor_cap, const int new_population, const int new_population_cap, const int new_product, const int new_product_cap, const int new_level, const int new_price, const int new_type, const int new_cooldown) 
	: Settlement(new_id, new_name, new_point)
	, armor(new_armor)
	, armor_capacity(new_armor_cap)
	, population(new_population)
	, population_capacity(new_population_cap)
	, product(new_product)
	, product_capacity(new_product_cap)
	, level(new_level)
	, next_level_price(new_price)
	, type(new_type)
	, cooldown(new_cooldown)
	{}
	

int Town::get_armor() const {
	return armor;
}

void Town::set_armor(const int new_armor){
	armor = new_armor;
}

int Town::get_armor_capacity() const {
	return armor_capacity;
}

void Town::set_armor_capacity(const int new_armor_capacity){
	armor_capacity = new_armor_capacity;
}


int Town::get_population() const {
	return population;
}

void Town::set_population(const int new_population){
	population = new_population;
}

int Town::get_population_capacity() const {
	return population_capacity;
}

void Town::set_population_capacity(const int new_population_capacity){
	population_capacity = new_population_capacity;
}



int Town::get_product() const {
	return product;
}

void Town::set_product(const int new_product){
	product = new_product;
}

int Town::get_product_capacity() const {
	return product_capacity;
}

void Town::set_product_capacity(const int new_product_capacity){
	product_capacity = new_product_capacity;
}


int Town::get_level() const {
	return level;
}

void Town::set_level(const int new_level){
	level = new_level;
}


int Town::get_next_level_price() const {
	return next_level_price;
}

void Town::set_next_level_price(const int new_n){
	next_level_price = new_n;
}


int Town::get_type() const {
	return type;
}

void Town::set_type(const int new_type){
	type = new_type;
}


int Town::get_cooldown() const {
	return cooldown;
}

void Town::set_cooldown(const int new_cooldown){
	cooldown = new_cooldown;
}


std::string Town::to_string() const {
	std::string info = "Town:";
	info += Settlement::to_string();
	info += "\n\tarmor = " + std::to_string(armor) +
		"\n\tpopulation = " + std::to_string(population) +
		"\n\tproduct = " + std::to_string(product) +
		"\n\tarmor_capacity = " + std::to_string(armor_capacity) +
		"\n\tproduct_capacity = " + std::to_string(product_capacity) +
		"\n\tpopulation_capacity = " + std::to_string(population_capacity) +
		"\n\tlevel = " + std::to_string(level) +
		"\n\tnext_level_price = " + std::to_string(next_level_price) +
		"\n\ttype = " + std::to_string(type) +
		"\n\tcooldown = " + std::to_string(cooldown);
	return info;
}