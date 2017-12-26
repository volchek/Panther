#pragma once

#include <string>

class GameUnit {
public:
	virtual ~GameUnit();
	virtual std::string to_string() const = 0;
};
