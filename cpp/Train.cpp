#include "Train.h"

Train::Train() {};

Train::Train(const int new_id, const int new_product, const int new_capacity, const int new_position, const int new_speed, const int new_line, const int new_level, const int new_lp, const int new_post_type, const int new_cooldown) 
	: id(new_id)
	, product(new_product)
	, capacity(new_capacity)
	, position(new_position)
	, speed(new_speed)
	, line_index(new_line)
	, level(new_level)
	, next_level_price(new_lp)
	, post_type(new_post_type)
	, cooldown(new_cooldown)
	{}
	
int Train::get_id() const {
	return id;
}

int Train::get_product() const {
	return product;
}

int Train::get_capacity() const {
	return capacity;
}

int Train::get_position() const {
	return position;
}

int Train::get_speed() const {
	return speed;
}

int Train::get_line_index() const {
	return line_index;
}

int Train::get_level() const {
	return level;
}

int Train::get_next_level_price() const {
	return next_level_price;
}

int Train::get_post_type() const {
	return post_type;
}

int Train::get_cooldown() const {
	return cooldown;
}

std::pair<int, int> Train::get_current_position() const {
	return {line_index, position};
}



void Train::set_id(const int new_id){
	id = new_id;
}

void Train::set_product(const int new_product) {
	product = new_product;
}

void Train::set_capacity(const int new_capacity){
	capacity = new_capacity;
}

void Train::set_position(const int new_position){
	position = new_position;
}

void Train::set_speed(const int new_speed) {
	speed = new_speed;
}

void Train::set_line_index(const int new_line) {
	line_index = new_line;
}

void Train::set_level(const int new_level) {
	level = new_level;
}

void Train::set_next_level_price(const int new_next_level_price) {
	next_level_price = new_next_level_price;
}

void Train::set_post_type(const int new_post_type) {
	post_type = new_post_type;
}

void Train::set_cooldown(const int new_cooldown) {
	cooldown = new_cooldown;
}



std::string Train::to_string() const {
	std::string info = "Train:\n\tid = ";
	info += std::to_string(id) + 
		"\n\tproduct = " + std::to_string(product) +
		"\n\tcapacity = " + std::to_string(capacity) +
		"\n\tposition = " + std::to_string(position) +
		"\n\tspeed = " + std::to_string(speed) +
		"\n\tline = " + std::to_string(line_index) + 
		"\n\tlevel = " + std::to_string(level) +
		"\n\tnext_level_price = " + std::to_string(next_level_price) +
		"\n\tpost_type = " + std::to_string(post_type) +
		"\n\tcooldown = " + std::to_string(cooldown);
	return info;
}