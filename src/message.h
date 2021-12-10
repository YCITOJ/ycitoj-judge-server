#pragma once
#include <vector>
#include <string>
#include <queue>
#include <mutex>
#include <thread>
#include <optional>
#include <memory>
#include <chrono>
#include <iostream>
#include "json_parser.hpp"

namespace Net {
	int32_t read_int32(char*);
	int64_t read_int64(char*);
}
class Message:public std::enable_shared_from_this<Message>{
public:
	Message(std::string str);
	Message();
	virtual ~Message() { data.clear(); };
public:
	void rsize(size_t len);
	std::string body();
	char* content();
	JSON to_json();
	int length();
	int content_size();
	void show();
	int header();
private:
	std::vector<char> data;
};
template <typename T>
class TSQueue {
public:
	TSQueue() = default;
	TSQueue(const TSQueue<T>&) = delete;
	virtual ~TSQueue() { clear(); };
public:
	void clear() {
		std::scoped_lock lock(qmutex);
		tsqueue.clear();
	}

	const T& front() {
		std::scoped_lock lock(qmutex);
		return tsqueue.front();
	}

	const T& back() {
		std::scoped_lock lock(qmutex);
		return tsqueue.back();
	}

	T pop_front() {
		std::scoped_lock lock(qmutex);
		auto ret = tsqueue.front();
		tsqueue.pop_front();
		return ret;
	}

	T pop_back() {
		std::scoped_lock lock(qmutex);
		auto ret = tsqueue.back();
		tsqueue.pop_back();
		return ret;
	}

	int length() {
		std::scoped_lock lock(qmutex);
		return tsqueue.size();
	}

	void push_back(T item) {
		std::scoped_lock lock(qmutex);
		tsqueue.push_back(item);
		return;
	}

	void push_front(T item) {
		std::scoped_lock lock(qmutex);
		tsqueue.push_front(item);
		return;
	}

	bool empty() {
		std::scoped_lock lock(qmutex);
		return tsqueue.empty();
	}


private:
	std::mutex qmutex;
	std::deque<T> tsqueue;
};