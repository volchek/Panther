#pragma once 

#include "GameUnit.h"
#include <string>

class Train : public GameUnit {

public:
	Train();
	Train(const int, const int, const int, const int, const int, const int, const int, const  int, const int, const int);
	int get_id() const;
	int get_product() const;
	int get_capacity() const;
	int get_position() const;
	int get_speed() const;
	int get_line_index() const;
	int get_level() const;
	int get_next_level_price() const;
	int get_post_type() const;
	int get_cooldown() const;
	std::pair<int, int> get_current_position() const;
	
	void set_id(const int);
	void set_product(const int);
	void set_capacity(const int);
	void set_position(const int);
	void set_speed(const int);
	void set_line_index(const int);
	void set_level(const int);
	void set_next_level_price(const int);
	void set_post_type(const int);
	void set_cooldown(const int);
	
	std::string to_string() const ;
private:
	int id;
	int product;
	int capacity;
	int position;
	int speed;
	int line_index;
	
	int level;
	int next_level_price;

	int post_type;
	int cooldown;
};