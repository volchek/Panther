#include "Settlement.h"


Settlement::Settlement(const int new_id, const std::string& new_name, const int new_point) 
	: id(new_id)
	, name(new_name)
	, point_id(new_point)
	{}

Settlement::~Settlement() { }


int Settlement::get_id() const {
	return id;
}

void Settlement::set_id(const int new_id){
	id = new_id;
}

std::string Settlement::get_name() const {
	return name;
}

void Settlement::set_name(const std::string& new_name){
	name = new_name;
}


int Settlement::get_point_id() const {
	return point_id;
}

void Settlement::set_point_id(const int new_id){
	point_id = new_id;
}


std::string Settlement::to_string() const {
	std::string info = "\n\tid = ";
	info += std::to_string(id);
	info += "\n\tname = " + name;
	info += "\n\tpoint_id = " + std::to_string(point_id);
	return info;
}