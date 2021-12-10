#include "net.h"
#include "utile.h"
#define SET_PATH "./settings/default.json"
// #define CPU_BIGENDIAN
namespace Net
{
#ifdef CPU_BIGENDIAN
	int32_t read_int32(char *buf)
	{
		return *(int32_t *)buf;
	}
	int64_t read_int64(char *buf)
	{
		return *(int64_t *)buf;
	}
	void write_int32(int32_t val, char *buf)
	{
		memcpy(buf, &val, 4);
	}
#else
	int32_t read_int32(char *buf)
	{
		std::vector<char> bytes;
		bytes.resize(4);
		memcpy(bytes.data(), buf, 4);
		std::reverse(bytes.begin(), bytes.end());
		return *(int32_t *)bytes.data();
	}
	void write_int32(int32_t val, char *buf)
	{
		std::vector<char> bytes;
		bytes.resize(4);
		memcpy(bytes.data(), &val, 4);
		std::reverse(bytes.begin(), bytes.end());
		memcpy(buf, bytes.data(), 4);
	}
#endif

	using asio::ip::tcp;
	Connection::Connection(tcp::socket _socket, std::string _token) : sock(std::move(_socket)), server_token(_token) { is_varifyed = 0; }
	void Connection::read_body(size_t body_len, std::shared_ptr<Message> msg)
	{
		auto self = shared_from_this();
		asio::async_read(sock, asio::buffer(msg->content() + head_len, body_len),
						 [this, self, msg](std::error_code ec, std::size_t len)
						 {
							//  msg->show();
							 if (!ec)
							 {
								 if (!is_varifyed)
								 {
									 JSON tok = msg->to_json();
									 if (!Utile::check_json(tok, "secure_token"))
									 {
										 std::cerr << "Invalid request" << std::endl;
										 // close_sock();
									 }
									 else
									 {
										 if (tok["secure_token"].get_str() == server_token)
										 {
											 is_varifyed = 1;
											 enable_recv_msg();
										 }
										 else
										 {
											 std::cerr << "Invalid token" << std::endl;
											 // close_sock();
										 }
									 }
								 }
								 else
								 {
									 recv_queue.push_back(msg);
									 enable_recv_msg();
								 }
							 }
							 else
							 {
								 std::cout << ec.message() << std::endl;
							 }
						 });
	}

	void Connection::enable_recv_msg()
	{
		auto msg = std::make_shared<Message>();
		auto self = shared_from_this();
		asio::async_read(sock, asio::buffer(msg->content(), head_len),
						 [this, self, msg](std::error_code ec, std::size_t len)
						 {
							 if (!ec)
							 {
								 int body_len = msg->header();
								 msg->rsize(head_len + body_len);
								 read_body(body_len, msg);
								//  Utile::c_log("body_len : " + std::to_string(body_len));
							 }
							 else
							 {
								 std::cerr << ec.message() << std::endl;
							 }
						 });
	}

	void Connection::send_msg(Message msg)
	{
		auto self = shared_from_this();
		asio::async_write(sock, asio::buffer(msg.content(), msg.length()),
						  [this, self](std::error_code ec, std::size_t len)
						  {
							  if (ec)
							  {
								  std::cout << ec.message() << std::endl;
							  }
						  });
	}

	void Connection::send_raw(std::vector<uint8_t> msg)
	{
		auto self = shared_from_this();
		asio::async_write(sock, asio::buffer(msg.data(), msg.size()),
						  [this, self](std::error_code ec, std::size_t len)
						  {
							  if (ec)
							  {
								  std::cout << ec.message() << std::endl;
							  }
						  });
	}

	bool Connection::empty()
	{
		return recv_queue.empty();
	};

	void Connection::close_sock()
	{
		sock.close();
	}

	std::string Connection::pop_msg()
	{
		std::string ret = recv_queue.front()->body();
		recv_queue.pop_front();
		return ret;
	}

	void Server::do_accept()
	{
		acceptor.async_accept(
			[this](std::error_code ec, tcp::socket sock)
			{
				if (!ec)
				{
					remote_ip = sock.remote_endpoint().address().to_string();
					std::cout << "Receive connection from " << remote_ip << std::endl;
					conns = std::make_shared<Connection>(std::move(sock), secure_token);
					conns->enable_recv_msg();
					is_connected = 1;
					stat = RUNNING;
				}
				else
				{
					std::cerr << ec.message() << std::endl;
				}
			});
	}

	Server::Server() : conf(Utile::read_json(SET_PATH)), acceptor(io_context, tcp::endpoint(tcp::v4(), conf["port"].get_int())), is_connected(false)
	{
		try
		{
			ready_to_connect = true;
			secure_token = conf["secure_token"].get_str();
			stat = WAITING;
			run_thread = std::thread([this]()
									 {
										 while (1)
										 {
											 while (!ready_to_connect)
											 {
												 std::this_thread::sleep_for(std::chrono::milliseconds(50));
											 }
											 do_accept();
											 io_context.run();
											 ready_to_connect = false;
											 is_connected = 0;
											 conns = nullptr;
											 stat = WAITING;
											 io_context.restart();
											//  std::cout << "TCP connection reset" << std::endl;
										 }
									 });
			run_thread.detach();
		}
		catch (std::exception &e)
		{
			std::cerr << e.what() << std::endl;
			return;
		}
		std::cout << "Listen on " << conf["port"].get_int() << std::endl;
	}

	bool Server::empty()
	{
		// if (!is_connected) return true;
		if (!is_connected)
			return true;
		return conns->empty();
	}
	std::string Server::get_msg()
	{
		// if (!is_connected) return "";
		if (!is_connected || conns == nullptr)
			throw std::runtime_error("Client not connected");
		return conns->pop_msg();
	}

	void Server::send(Message &msg)
	{
		if (!is_connected)
			return;
		conns->send_msg(msg);
	}

	void Server::send(std::vector<uint8_t> &msg)
	{
		if (!is_connected)
			return;
		conns->send_raw(msg);
	}
}