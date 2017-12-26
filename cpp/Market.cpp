#include "Market.h"


Market::Market(const int new_id, const int new_point, const std::string& new_name) 
	: Settlement(new_id, new_name, new_point)
	{}


Market::Market(const int new_id, const int new_product, const int new_capacity, const int new_reg_rate, const std::string& new_name, const int new_point, const int new_type) 
	: Settlement(new_id, new_name, new_point)
	, product(new_product)
	, capacity(new_capacity)
	, regeneration_rate(new_reg_rate)
	, type(new_type)
	{}
	

int Market::get_product() const {
	return product;
}

void Market::set_product(const int new_product){
	product = new_product;
}

int Market::get_capacity() const {
	return capacity;
}

void Market::set_capacity(const int new_capacity){
	capacity = new_capacity;
}

int Market::get_regeneration_rate() const {
	return regeneration_rate;
}

void Market::set_regeneration_rate(const int new_regeneration_rate){
	regeneration_rate = new_regeneration_rate;
}


int Market::get_type() const {
	return type;
}

void Market::set_type(const int new_type){
	type = new_type;
}



std::string Market::to_string() const {
	std::string info = "Market: ";
	info += Settlement::to_string();
	info += "\n\tproduct = " + std::to_string(product) +
		"\n\tcapacity = " + std::to_string(capacity) +
		"\n\tregeneration_rate = " + std::to_string(regeneration_rate) +
		"\n\ttype = " + std::to_string(type);
	return info;
}