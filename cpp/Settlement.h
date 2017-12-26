#pragma once 

#include "GameUnit.h"
#include <string>


class Settlement : public GameUnit {
	int id;
	std::string name;
	int point_id;
	
public:
	Settlement(const int, const std::string&, const int);
	virtual ~Settlement();
	
	int get_id() const;
	void set_id(const int);
	
	std::string get_name() const;
	void set_name(const std::string&);

	int get_point_id() const;
	void set_point_id(const int);
	
	virtual std::string to_string() const ;
};
