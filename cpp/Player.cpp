#include "Player.h"

#include <iostream>

Player::Player (){ }

Player::Player(const std::string& new_idx, const std::string& new_name) 
	: idx(new_idx), name(new_name)
	{ }

Player::~Player() {}
	
std::string Player::get_name() const {
	return name;
}

void Player::set_name(const std::string& new_name){
	name = new_name;
}

std::string Player::get_idx() const {
	return idx;
}

void Player::set_idx(const std::string& new_idx){
	idx = new_idx;
}

int Player::get_post_id() const {
	return post_id;
}

void Player::set_post_id(const int p_id){
	post_id = p_id;
}
	
void Player::set_home_idx(const int idx){
	home_idx = idx;
}

int Player::get_home_idx() const {
	return home_idx;
}

void Player::set_rating(const int rat){
	rating = rat;
}

int Player::get_rating() const {
	return rating;
}



std::string Player::get_info() const {
	return "Player: name = " + name + "; idx=" + idx + "; home: " + std::to_string(post_id) + " " + std::to_string(home_idx);
}

void Player::print_info() const {
	std::cout << get_info() << std::endl;
	std::cout << "rating: " << rating << std::endl;
}