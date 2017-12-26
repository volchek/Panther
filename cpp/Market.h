#pragma once 

#include "Settlement.h"
#include <string>

class Market : public Settlement {

public:
	Market(const int, const int, const std::string&);
	Market(const int, const int, const int, const int, const std::string&, const int, const int);
	virtual ~Market() { };
	
	int get_product() const;
	void set_product(const int);

	int get_capacity() const;
	void set_capacity(const int);

	int get_regeneration_rate() const;
	void set_regeneration_rate(const int);
	
	int get_type() const;
	void set_type(const int);
	
	 std::string to_string() const ;
	
private:
	int product;
	int capacity;
	int regeneration_rate;
	int type;
};