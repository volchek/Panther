#include "Client.h"
#include "settings.h"

#include <iostream>
#include <fstream>
#include <cstring>
#include <algorithm>

#ifndef _WIN32
#define throws_eof() error == asio::error::eof
#else
#include <windows.h>
#endif

#define BUFFER_SIZE 4096

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
	

bool throws_eof() {
//	std::cout << "In throws_eof" << std::endl;
	DWORD last_error = ::GetLastError();
	if (last_error == ERROR_HANDLE_EOF) {
		return true;
	}
//	std::cout << last_error << std::endl;
	return false;
}

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
		// std::cout << er.what() << std::endl;
	}
	catch(...){
		std::cout << "Unknown error" << std::endl;
	}
}

Client::~Client(){
	close();
}

void Client::add_int_to_buffer(const uint32_t value, std::vector<char>& buffer){
	buffer.push_back( value & 0xff );
	buffer.push_back( ( value >> 8 ) & 0xff );
	buffer.push_back( ( value >> 16 ) & 0xff );
	buffer.push_back( ( value >> 24 ) & 0xff );
}


std::vector<char> Client::create_message(const uint32_t msg_code, const char* msg){
	std::vector<char> buffer;
	add_int_to_buffer(msg_code, buffer);
	add_int_to_buffer(std::strlen(msg), buffer);
	buffer.insert(buffer.end(), msg, msg + std::strlen(msg));
	return std::move(buffer);
}


std::vector<char> Client::send(const uint32_t msg_code, const char* msg){

	asio::error_code error;
	std::vector<char> buffer = create_message(msg_code, msg);
	int len1 = asio::write(socket, asio::buffer(buffer));
	char answer[BUFFER_SIZE];
	answer[0] = '\0';

	buffer.resize(BUFFER_SIZE);
	// buffer.clear();
	
	int len2 = 0;
/*
	try {
		for (int i = 0; i < 1; ++i){
			len2 = asio::read(socket, asio::buffer(buffer, 8), error);
			// buffer.clear();
//			len2 = socket.read_some(asio::buffer(buffer, BUFFER_SIZE), error);
			// buffer[len2] = '\0';
			strcat(answer, buffer.data());
//			std::this_thread::sleep_for(std::chrono::milliseconds(50));
//			std::cout << "READ: " << len2 << " " << buffer.data() << std::endl;
			std::cout << "READ: " << len2 << std::endl;
			std::cout << "DATA: " << buffer.data() << std::endl;
			// answer.push_back(buffer.data());
			// The following condition must be different on Windows and Unix-based OSes. 

//			if (throws_eof()) {
//				break;
//			}
//			else if (error) {
//				throw asio::system_error(error);
//			}

			if (error || throws_eof()) {
			// if (throws_eof()) {
				throw asio::system_error(error);
			}
//		std::cout << len2 << " ANSWER: " << buffer.data() << std::endl;
		}
	}
	catch(...){
		strcat(answer, buffer.data());
//	catch (const std::exception& e) {
//		std::cerr << e.what() << std::endl;
//		asio::socket_base::non_blocking_io non_blocking_io(true);
//		socket.io_control(non_blocking_io);
	}
	if (msg_code == 1){
		std::ofstream fileout("output.txt");
		fileout << answer << std::endl;
		fileout.close();
	}
	
	// std::cout << "ANSWER: " << buffer.data() << std::endl;
	// std::cout  << "READ: " << len2 << std::endl;
	*/
	
	int len_response_code = asio::read(socket, asio::buffer(buffer, 4), error);
	
	// std::cout << "len_response_code: " << len_response_code << std::endl;
	// std::cout << "buffer[0]: " << buffer[0] << std::endl;
	
	uint32_t response_code = buffer[0] + 256 * buffer[1] + 65536 * buffer[2] + 16777216 * buffer[3];
	std::cout << "response_code: " << response_code << std::endl;
	
	// buffer.clear();
	
	int len_message_size = asio::read(socket, asio::buffer(buffer, 4), error);
	
	// std::cout << "len_message_size: " << len_message_size << std::endl;
	// std::cout << "buffer[0]: " << buffer[0] << std::endl;
	
	char buffer_data[4];
	std::copy(buffer.data(), buffer.data() + 4, buffer_data);
	uint32_t message_size = (unsigned char) buffer[0]
		+ 256 * (unsigned char) buffer[1]
		+ 65536 * (unsigned char) buffer[2]
		+ 16777216 * (unsigned char) buffer[3];
	std::cout << "byte 0 in buffer: " << (unsigned char) buffer.data()[0] << std::endl;
	std::cout << "byte 1 in buffer: " << (unsigned char) buffer.data()[1] << std::endl;
	std::cout << "byte 2 in buffer: " << (unsigned char) buffer.data()[2] << std::endl;
	std::cout << "byte 3 in buffer: " << (unsigned char) buffer.data()[3] << std::endl;
	// uint32_t message_size = (uint32_t) buffer_data[0] | (uint32_t) buffer_data[1] << 8 | (uint32_t) buffer_data[2] << 16 | (uint32_t) buffer_data[3] << 24;
	std::cout << "message_size: " << message_size << std::endl;
	
	char vifivifi [16];
	sprintf(vifivifi, "message_size_%d.txt", msg_code);
	std::ofstream fileout2(vifivifi, std::ios::binary);
	fileout2 << buffer.data() << std::endl;
	// fileout2 << message_size << std::endl;
	fileout2.close();
	
	// buffer.clear();
	
	// /*
	if (message_size > 1048576) {
		while (true) {
			try {
				buffer[0] = '\0';
				int len3 = asio::read(socket, asio::buffer(buffer, 1), error);
				// std::cout << "read: " << len3 << std::endl;
				if (buffer[0] == '}') { break; }
			} catch (std::exception& e) {
				std::cout << e.what() << std::endl;
				break;
			}
		}
	}
	else {
		int len3 = asio::read(socket, asio::buffer(buffer, message_size), error);
		std::cout << len3 << std::endl;
	}
	// */
	/*
	while (true) {
		try {
			buffer[BUFFER_SIZE-1] = '\0';
			int len3 = asio::read(socket, asio::buffer(buffer, 256), error);
			// int len3 = socket.read_some(asio::buffer(buffer, BUFFER_SIZE), error);
			std::cout << "read: " << len3 << std::endl;
			strcat(answer, buffer.data());
			if (buffer[BUFFER_SIZE-1] == '\0') { break; }
			// if (error) { break; }
		// } catch (std::exception& e) {
		} catch (...) {
			// std::cout << e.what() << std::endl;
			break;
		}
		
	}
	*/
	
	char vifi [16];
	sprintf(vifi, "output_%d.txt", msg_code);
	std::ofstream fileout(vifi);
	fileout << buffer.data() << std::endl;
	fileout.close();
	
	return buffer;
}

void Client::login(const char* msg){
	std::vector<char> res = std::move(send(Action::login, msg));
	std::cout << "LOGIN - OK" << std::endl;
}

void Client::logout(){
	std::vector<char> res = std::move(send(Action::logout));
	std::cout << "LOGOUT - OK" << std::endl;
}

void Client::get_static_map(){
	std::vector<char> res = std::move(send(Action::map, "{\"layer\": 0}"));
	std::cout << "STATIC MAP - OK" << std::endl;
}

void Client::get_dynamic_map(){
	std::vector<char> res = std::move(send(Action::map, "{\"layer\": 1}"));
	std::cout << "DYNAMIC MAP - OK" << std::endl;
}

void Client::enum_print(){
	std::cout << Action::move<< std::endl;
}