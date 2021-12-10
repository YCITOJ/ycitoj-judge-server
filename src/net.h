#pragma once
#include <asio.hpp>
#include <vector>
#include <memory>
#include "definitions.h"
#include "message.h"
#include <atomic>
#define MAX_MSG_LEN 10000


namespace Net {
	int32_t read_int32(char*);
	int64_t read_int64(char*);
	
	void write_int32(int32_t, char*);
	void write_int64(int64_t, char*);

	using asio::ip::tcp;
	class Connection : public std::enable_shared_from_this<Connection> {
	public:
		Connection(tcp::socket _socket, std::string _token);
		virtual ~Connection() { recv_queue.clear(); };
		void enable_recv_msg();
		void send_msg(Message msg);
		void send_raw(std::vector<uint8_t> msg);
		bool empty();
		void close_sock();
		std::string pop_msg();
	public:
		bool is_varifyed;
	private:
		void read_body(size_t len, std::shared_ptr<Message> msg);
	private:
		TSQueue<std::shared_ptr<Message>> recv_queue;
		tcp::socket sock;
		std::string server_token;
	};
	enum Server_Stat{
		RUNNING, WAITING, SLEEPING
	};
	class Server {
	public:
		Server();
		virtual ~Server() {};
	public:
		std::string get_msg();
		void send(Message& msg);
		void send(std::vector<uint8_t>& msg);
		bool empty();
	private:
		void do_accept();
	public:
		std::atomic<bool> is_connected;
		std::atomic<bool> ready_to_connect;
		// bool is_connected;
		JSON conf;
		std::string secure_token;
		Server_Stat stat;
		std::string remote_ip;
	private:
		std::thread run_thread;
		asio::io_context io_context;
		std::shared_ptr<Connection> conns;
		asio::ip::tcp::acceptor acceptor;
	};
}