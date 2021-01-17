#pragma once

#include "addr.hpp"
#include "ip.hpp"

#include <span>

struct UdpSocket {
	int fd = -1;
	
	static auto bind(Ipv4Addr addr, uint16_t port) -> std::optional<UdpSocket> {
		int fd = socket(AF_INET, SOCK_DGRAM, 0);
		if (fd == -1) {
			return std::nullopt;
		}
		
		sockaddr_in address{.sin_family = AF_INET, .sin_port = htons(port), .sin_addr = addr.sin_addr};
		if (::bind(fd, (struct sockaddr *) &address, sizeof(sockaddr_in)) == -1) {
			::close(fd);
			return std::nullopt;
		}
		
		return UdpSocket{fd};
	}
	
	void set_blocking(bool flag) const {
		int flags = fcntl(fd, F_GETFL, 0);
		if (flags == -1) {
			perror("fcntl");
			return;
		}
		flags = flag ? (flags & ~O_NONBLOCK) : (flags | O_NONBLOCK);
		
		if (fcntl(fd, F_SETFL, flags) == -1) {
			perror("fcntl");
		}
	}
	
	void set_broadcast(bool flag) const {
		const int optval = flag ? 1 : 0;
		if (::setsockopt(fd, SOL_SOCKET, SO_BROADCAST, &optval, sizeof(optval)) == -1) {
			perror("setsockopt");
//				exit(1);
		}
	}
	
	void set_reuse_port(bool flag) const {
		const int optval = flag ? 1 : 0;
		if (::setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval)) == -1) {
			perror("setsockopt");
//				exit(1);
		}
	}
	
	void set_reuse_address(bool flag) const {
		const int optval = flag ? 1 : 0;
		if (::setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1) {
			perror("setsockopt");
//				exit(1);
		}
	}
	
	void join_multicast_v4(Ipv4Addr multiaddr, Ipv4Addr interface) const {
		ip_mreq mreq{.imr_multiaddr = multiaddr.sin_addr, .imr_interface = interface.sin_addr};
		if (::setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) == -1) {
			perror("setsockopt");
//				exit(1);
		}
	}
	
	void leave_multicast_v4(Ipv4Addr multiaddr, Ipv4Addr interface) const {
		ip_mreq mreq{.imr_multiaddr = multiaddr.sin_addr, .imr_interface = interface.sin_addr};
		if (::setsockopt(fd, IPPROTO_IP, IP_DROP_MEMBERSHIP, &mreq, sizeof(mreq)) == -1) {
			perror("setsockopt");
//				exit(1);
		}
	}
	
	void close() const {
		::close(fd);
	}
	
	auto recv_from(std::span<uint8_t> bytes) const -> std::pair<int, SocketAddr> {
		socklen_t size = sizeof(sockaddr_in);
		sockaddr_in addr{};
		int n = recvfrom(fd, bytes.data(), bytes.size(), 0, (sockaddr *) &addr, &size);
		return {n, {.inner = addr}};
	}
};