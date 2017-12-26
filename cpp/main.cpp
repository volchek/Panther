#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#define _WIN32_WINNT 0x0501

#include "Client.h"
#include "Strategy.h"
#include <asio.hpp>
#include <iostream>
#include <Windows.h>
#include <cstdlib>

int main(int argc, char** argv){

	asio::io_service svc;
	Client client(svc);
	int tick_count = (argc == 1) ? 1e9 : atoi(argv[1]);
	Strategy stategy(client, tick_count);
	stategy.run();
	return 0;

}
