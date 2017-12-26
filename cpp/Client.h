#pragma once

#include <asio.hpp>
#include <string>
#include <memory>
#include <vector>
#include <stdint.h>

enum Action : uint32_t
{
	login = 1,
	logout = 2,
	move = 3,
	upgrade = 4,
	turn = 5,
	map = 10
};

enum Server_answer : uint32_t {
	ok = 0,
	bad_command = 1,
	resource_not_found = 2,
	path_not_found = 3,
	access_denied = 5
};

class Client {

public:
	Client(asio::io_service& svc, const std::string&, const std::string&);
	Client(asio::io_service& svc);
	Client(const Client&) = default;
	~Client();
	bool connect();
	std::vector<uint8_t> send(const uint32_t, const char* msg = "");
	std::vector<uint8_t> login(const char*);
	std::vector<uint8_t> move(const char*);
	std::vector<uint8_t> upgrade(const char*);
	std::vector<uint8_t> turn();
	void logout();
	std::vector<uint8_t> get_static_map();
	std::vector<uint8_t> get_dynamic_map();
	std::vector<uint8_t> get_coord_map();
	
private:
	void close();
	std::vector<uint8_t> create_message(const uint32_t, const char* msg);
	void add_int_to_buffer(const uint32_t, std::vector<uint8_t>&);
	uint32_t reverse_bytes(const std::vector<uint8_t>&);
	void print_msg(const std::vector<uint8_t>&, const int);
	bool get_answer(std::vector<uint8_t>&, char*);
	uint32_t get_answer_len(const std::vector<uint8_t>&);
	
private:
	const std::string host;
	const std::string port;
	asio::io_service& io_service;
	asio::ip::tcp::socket socket;
	
};