#pragma once

#include <string>

class Player {
	std::string idx;
	std::string name;
	
	int home_idx;
	int post_id;
	int rating = 0;
public:
	Player ();
	Player(const std::string&, const std::string&);
	~Player();
	
	std::string get_name() const;
	void set_name(const std::string&);
	
	std::string get_idx() const;
	void set_idx(const std::string&);

	int get_post_id() const;
	void set_post_id(const int);
	
	int get_home_idx() const;
	void set_home_idx(const int);
	
	int get_rating() const;
	void set_rating(const int);
	
	std::string get_info() const;
	void print_info() const;
};