#pragma once

#include <string>
#include <memory_resource>

namespace ostl
{
	template <class CharT, class Traits = std::char_traits<CharT>, class Allocator = std::allocator<CharT>>
	class basic_string;

	using string = basic_string<char>;
	using wstring = basic_string<wchar_t>;
	using u16string = basic_string<char16_t>;
	using u32string = basic_string<char32_t>;

	namespace pmr
	{
		template <class CharT, class Traits = std::char_traits<CharT>>
		using basic_string = ostl::basic_string<CharT, Traits, std::pmr::polymorphic_allocator<CharT>>;
	}
}
