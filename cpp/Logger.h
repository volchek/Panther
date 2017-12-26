#pragma once 

#include <string>
#include <fstream>
#include <vector>

class Logger {
	std::string directory_path;
	std::ofstream fileout;
public:
	Logger();
	~Logger();
	void log(const std::vector<uint8_t>&, const std::string&);
	void log(const std::vector<uint8_t>&);
private:
	void write_data(const std::vector<uint8_t>&, const std::string&);
};