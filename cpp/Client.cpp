#include "Client.h"
#include "settings.h"

#include <iostream>
#include <fstream>
#include <cstring>
#include <algorithm>


using asio::ip::tcp;

Client::Client(asio::io_service& svc, const std::string& host, const std::string& port) 
	: host(host)
	, port(port)
	, io_service(svc)
	, socket(io_service)
	{ }

Client::Client(asio::io_service& svc) 
	: Client(svc, Client_settings::default_host, Client_settings::default_port) 
	{  }


bool Client::connect() {

	tcp::resolver resolver(io_service);
	tcp::resolver::iterator endpoint = resolver.resolve(tcp::resolver::query(host, port));
	tcp::resolver::iterator end;
	
	try {
		asio::connect(socket, endpoint);
		if (endpoint == end){ return false; }
	}
	catch(...){
		std::cout << "ERROR" << std::endl;
	}
	return true;
}

void Client::close(){
	asio::error_code ec;
	try {
		socket.shutdown(tcp::socket::shutdown_both, ec);
		socket.close();
	}
	catch(const asio::system_error& er){ 
		std::cout << er.what() << std::endl;
	}
	catch(...){
		std::cout << "Unknown error" << std::endl;
	}
}

Client::~Client(){
	close();
}

void Client::add_int_to_buffer(const uint32_t value, std::vector<uint8_t>& buffer){
	buffer.push_back( value & 0xff );
	buffer.push_back( ( value >> 8 ) & 0xff );
	buffer.push_back( ( value >> 16 ) & 0xff );
	buffer.push_back( ( value >> 24 ) & 0xff );
}


std::vector<uint8_t> Client::create_message(const uint32_t msg_code, const char* msg){
	std::vector<uint8_t> buffer;
	add_int_to_buffer(msg_code, buffer);
	add_int_to_buffer(std::strlen(msg), buffer);
	buffer.insert(buffer.end(), msg, msg + std::strlen(msg));
	return std::move(buffer);
}

uint32_t Client::reverse_bytes(const std::vector<uint8_t>& buffer){
	uint32_t reverse_code = (buffer[0]<<0) | (buffer[1]<<8) | (buffer[2]<<16) | (buffer[3]<<24);
	return reverse_code;
}

uint32_t Client::get_answer_len(const std::vector<uint8_t>& buffer){
	char buffer_data[Client_messages::value_len];
	std::copy(buffer.data(), buffer.data() + Client_messages::value_len, buffer_data);
	uint32_t message_size = reverse_bytes(buffer);
	return message_size;
}

bool Client::get_answer(std::vector<uint8_t>& buffer, char* answer){

	asio::error_code error;
	try {
		int len_response_code = asio::read(socket, asio::buffer(buffer, Client_messages::value_len), error);
	
		uint32_t response_code = reverse_bytes(buffer);
//		std::cout << "response_code: " << response_code << std::endl;
		/*
		if ( response_code != Server_answer::ok){
			return false;
		}
		*/
	
		int len_message_size = asio::read(socket, asio::buffer(buffer, Client_messages::value_len), error);
		int message_size = get_answer_len(buffer);
//		std::cout << "message_size: " << message_size << std::endl;

		int len_data = asio::read(socket, asio::buffer(buffer, message_size), error);
//		std::cout << "read: " << len_data << std::endl;
	}
	catch(...){
		std::cout << "Catch exception" << std::endl;
		return false;
	}
	return true;
} 

std::vector<uint8_t> Client::send(const uint32_t msg_code, const char* msg){

	std::vector<uint8_t> buffer = create_message(msg_code, msg);
	int len1 = asio::write(socket, asio::buffer(buffer));
	char answer[Client_settings::buffer_size] = { 0 };
	buffer.resize(Client_settings::buffer_size);
	
	bool result = get_answer(buffer, answer);
	return buffer;
}

void Client::print_msg(const std::vector<uint8_t>& msg, const int code){
	char filename [16];
	sprintf(filename, "output_%d.txt", code);
	std::ofstream fileout(filename);
	fileout << msg.data() << std::endl;
	fileout.close();
}

std::vector<uint8_t> Client::login(const char* msg){
	std::vector<uint8_t> res = std::move(send(Action::login, msg));
	// print_msg(res, Action::login);
	return res;
}

void Client::logout(){
	std::vector<uint8_t> res = std::move(send(Action::logout));
	// print_msg(res, Action::logout);
	std::cout << "LOGOUT" << std::endl;
}

std::vector<uint8_t> Client::get_static_map(){
	std::vector<uint8_t> res = std::move(send(Action::map, Client_messages::static_map_msg));
	// print_msg(res, Action::map + 3);
	return res;
}

std::vector<uint8_t> Client::get_dynamic_map(){
	std::vector<uint8_t> res = std::move(send(Action::map, Client_messages::dynamic_map_msg));
	if (!res.empty()){
		// print_msg(res, Action::map + 4);
	}
	return res;
}

std::vector<uint8_t> Client::get_coord_map(){
	std::vector<uint8_t> res = std::move(send(Action::map, Client_messages::coord_map_msg));
	return res;
}

std::vector<uint8_t> Client::move(const char* msg){
//	std::cout << "Move train" << std::endl;
	std::vector<uint8_t> res = std::move(send(Action::move, msg));
	// print_msg(res, Action::move);
//	std::cout << "Answer size: " << res.size() << std::endl;
	return res;
}

std::vector<uint8_t> Client::turn(){
	std::vector<uint8_t> res = std::move(send(Action::turn, Client_messages::turn_msg));
	return res;
}

std::vector<uint8_t> Client::upgrade(const char* msg){
	std::vector<uint8_t> res = std::move(send(Action::upgrade, msg));
	// print_msg(res, Action::upgrade);
	return res;
}