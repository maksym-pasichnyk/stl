#pragma once

#include <optional>
#include <netinet/in.h>
#include <libnet.h>
#include <fmt/format.h>

struct Ipv4Addr {
	in_addr sin_addr;
	
	static auto from(uint8_t a, uint8_t b, uint8_t c, uint8_t d) -> Ipv4Addr {
		return {.sin_addr {.s_addr = htonl(((uint32_t) a << 24) | ((uint32_t) b << 16) | ((uint32_t) c << 8) | ((uint32_t) d))}};
	}
	
	static auto from(const char *cp) -> std::optional<Ipv4Addr> {
		in_addr sin_addr{};
		if (::inet_aton(cp, &sin_addr) == -1) {
			return std::nullopt;
		}
		return Ipv4Addr{.sin_addr = sin_addr};
	}
	
	static /*constexpr*/ auto any() -> Ipv4Addr {
		return {.sin_addr {.s_addr = htonl(INADDR_ANY)}};
	}
	
	static /*constexpr*/ auto broadcast() -> Ipv4Addr {
		return {.sin_addr {.s_addr = htonl(INADDR_BROADCAST)}};
	}
};

template <>
struct fmt::formatter<Ipv4Addr> : formatter<const char*> {
	template <typename FormatContext>
	auto format(Ipv4Addr addr, FormatContext& ctx) {
		return formatter<const char*>::format(inet_ntoa(addr.sin_addr), ctx);
	}
};