#include "message.h"
#include "net.h"
#include "definitions.h"
Message::Message(std::string str) {
	data.resize(str.size() + head_len);
	Net::write_int32(str.size(), data.data());
	memcpy(data.data() + head_len, str.data(), str.size());
}

Message::Message() {
	data.resize(head_len);
}

void Message::rsize(size_t size) {
	data.resize(size);
}

int Message::length() {
	return data.size();
}

char* Message::content() {
	return data.data();
}

void Message::show() {
	for (int i = head_len; i < data.size(); i++) {
		std::cout << data[i];
	}
	std::cout << std::endl;
}

JSON Message::to_json() {
	return JSON(this->body());
}

int Message::content_size() {
	return data.size() - head_len;
}

int Message::header() {
	return Net::read_int32(data.data());
}

std::string Message::body() {
	std::string tmp;
	tmp.resize(this->content_size());
	if (data.empty() || data.size() <= head_len) return std::string("");
	memcpy(tmp.data(), data.data() + head_len, this->content_size());
	return tmp;
}


