#pragma once

#include <cstddef>

namespace ostl
{
	using ptrdiff_t = std::ptrdiff_t;
	using size_t = std::size_t;
	using max_align_t = std::max_align_t;
	using nullptr_t = decltype(nullptr);

	enum class byte : unsigned char
	{
	};

	template <class IntegerType, class = std::enable_if_t<std::is_integral_v<IntegerType>>>
	constexpr byte operator<<(byte b, IntegerType shift) noexcept
	{
		return byte(static_cast<unsigned>(b) << shift);
	}

	template <class IntegerType, class = std::enable_if_t<std::is_integral_v<IntegerType>>>
	constexpr byte& operator<<=(byte& b, IntegerType shift) noexcept
	{
		return b = b << shift;
	}

	template <class IntegerType, class = std::enable_if_t<std::is_integral_v<IntegerType>>>
	constexpr byte operator>>(byte b, IntegerType shift) noexcept
	{
		return byte(static_cast<unsigned>(b) >> shift);
	}

	template <class IntegerType, class = std::enable_if_t<std::is_integral_v<IntegerType>>>
	constexpr byte& operator>>=(byte& b, IntegerType shift) noexcept
	{
		return b = b >> shift;
	}

	constexpr byte operator|(byte l, byte r) noexcept
	{
		return byte(static_cast<unsigned>(l) | static_cast<unsigned>(r));
	}
	
	constexpr byte& operator|=(byte& l, byte r) noexcept
	{
		return l = l | r;
	}
	
	constexpr byte operator&(byte l, byte r) noexcept
	{
		return byte(static_cast<unsigned>(l) & static_cast<unsigned>(r));
	}

	constexpr byte& operator&=(byte& l, byte r) noexcept
	{
		return l = l & r;
	}
	
	constexpr byte operator^(byte l, byte r) noexcept
	{
		return byte(static_cast<unsigned>(l) ^ static_cast<unsigned>(r));
	}
	
	constexpr byte& operator^=(byte& l, byte r) noexcept
	{
		return l = l ^ r;
	}
	
	constexpr byte operator~(byte b) noexcept
	{
		return byte(~static_cast<unsigned>(b));
	}

	template <class IntegerType, class = std::enable_if_t<std::is_integral_v<IntegerType>>>
	constexpr IntegerType to_integer(byte b) noexcept
	{
		return IntegerType(b);
	}
}

#define NULL nullptr
#define offsetof(type, member) (::ostl::size_t(&reinterpret_cast<const char volatile&>(((type*)0)->member)))
