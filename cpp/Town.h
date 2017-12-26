#pragma once 

#include "Settlement.h"
#include <string>

class Town : public Settlement {

public:
	Town(const int, const int, const std::string&);
	Town(const int, const int, const std::string&, const int, const int, const int, const int, const int, const int, const int, const int, const int, const int);

	virtual ~Town() { } ;
	
	int get_armor() const;
	void set_armor(const int);

	int get_armor_capacity() const;
	void set_armor_capacity(const int);
	
	int get_population() const;
	void set_population(const int);

	int get_population_capacity() const;
	void set_population_capacity(const int);
	
	int get_product() const;
	void set_product(const int);
	
	int get_product_capacity() const;
	void set_product_capacity(const int);
	
	int get_level() const;
	void set_level(const int);
	
	int get_next_level_price() const;
	void set_next_level_price(const int);

	int get_type() const;
	void set_type(const int);

	int get_cooldown() const;
	void set_cooldown(const int);
	
	std::string to_string() const ;
	
private:
	int armor;
	int armor_capacity;
	int product;
	int product_capacity;
	int population;
	int population_capacity;

	int level;
	int next_level_price;
	
	int type;
	int cooldown;
};