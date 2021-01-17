#pragma once

#include "addr.hpp"
#include "ip.hpp"

#include <span>
#include <optional>

struct TcpStream {
	int fd = -1;

	static auto connect(Ipv4Addr addr, uint16_t port) -> std::optional<TcpStream> {
		int fd = socket(AF_INET, SOCK_STREAM, 0);
		if (fd == -1) {
			return std::nullopt;
		}
		
		sockaddr_in address{.sin_family = AF_INET, .sin_port = htons(port), .sin_addr = addr.sin_addr};
		if (::connect(fd, (struct sockaddr *) &address, sizeof(sockaddr_in)) == -1) {
			::close(fd);
			return std::nullopt;
		}
		return TcpStream{fd};
	}
	
//	bool listen() {
//		if (::listen(fd, 10) == -1) {
//			perror("listen");
//			return false;
//		}
//		return true;
//	}

	void set_nonblocking(bool flag) {
		int opts = fcntl(fd, F_GETFL);
		if (opts == -1) {
			perror("fcntl");
			return;
		}
		if (flag) {
			opts |= O_NONBLOCK;
		} else {
			opts &= ~O_NONBLOCK;
		}

		if (fcntl(fd, F_SETFL, opts) == -1) {
			perror("fcntl");
			return;
		}
	}
	
	auto read(std::span<uint8_t> buf) -> int {
		return ::read(fd, buf.data(), buf.size());
	}

	auto write(std::span<const uint8_t> buf) const -> bool {
		while (!buf.empty()) {
			int n = ::send(fd, buf.data(), buf.size(), 0);
			if (n < 0) {
				return false;
			}
			buf = buf.subspan(n);
		}
		return true;
	}
	
	void close() const {
		::close(fd);
	}
};