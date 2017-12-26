#include "Logger.h"

#include <direct.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <chrono>
#include <ctime>
#include <iomanip>

Logger::Logger(){
	auto now = std::chrono::system_clock::now();
	auto time = std::chrono::system_clock::to_time_t(now);
	std::stringstream ss;
	ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H-%M-%S");
	directory_path = ss.str();
	if( _mkdir(directory_path.c_str()) != 0 ){
		std::cout << "Problem creating directory" << std::endl;
	}
}

Logger::~Logger(){
	if (fileout.is_open()){
		fileout.close();
	}
}


void Logger::log(const std::vector<uint8_t>& msg){
	static int i = 1;
	std::string filename = directory_path + "/";
	filename += std::to_string(i);
	filename +=  + ".txt";
	write_data(msg, filename);
	++i;
}

void Logger::log(const std::vector<uint8_t>& msg, const std::string& name){
	std::string filename = directory_path + "/";
	filename += name;
	filename +=  + ".txt";
	write_data(msg, filename);
}

void Logger::write_data(const std::vector<uint8_t>& msg, const std::string& filename){
	fileout.open(filename);
	if (!fileout.is_open()){
		std::cout << "Can't open file " << filename << std::endl;
		return;
	}
	for (auto it = msg.begin(); it != msg.end(); ++it) {
		if (*it == 0) { break; }
		fileout << *it;
	}
	fileout.close();
	fileout.clear();
}