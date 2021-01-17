#pragma once

#include <libnet.h>

#include "ip.hpp"

struct SocketAddr {
	sockaddr_in inner;
	
	auto ip() const -> Ipv4Addr {
		return {.sin_addr = inner.sin_addr};
	}
	
	auto port() const -> uint16_t {
		return ntohs(inner.sin_port);
	}
};