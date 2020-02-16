#pragma once

#include <string>
#include <memory_resource>
#include "internal/iterator.h"
#include "internal/compressed_pair.h"

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

	template <class CharT, class Traits, class Allocator>
	class basic_string
	{
	public:
		using traits_type = Traits;
		using value_type = CharT;
		using allocator_type = Allocator;
		using size_type = typename std::allocator_traits<allocator_type>::size_type;
		using difference_type = typename std::allocator_traits<allocator_type>::difference_type;
		using reference = value_type&;
		using const_reference = const value_type&;
		using pointer = typename std::allocator_traits<allocator_type>::pointer;
		using const_pointer = typename std::allocator_traits<allocator_type>::const_pointer;
		using iterator = internal::iterator<value_type, difference_type, pointer, reference>;
		using const_iterator = internal::const_iterator<value_type, difference_type, pointer, reference>;
		using reverse_iterator = std::reverse_iterator<iterator>;
		using const_reverse_iterator = std::reverse_iterator<const_iterator>;

	private:
		struct _long
		{
			size_type cap;
			size_type size;
			pointer data;
		};

		static constexpr auto short_mask = 1;
		static constexpr auto long_mask = 1;
		static constexpr auto min_cap = (sizeof(_long) - 1) / sizeof(value_type) > 2 ? (sizeof(_long) - 1) / sizeof(value_type) : 2;

		struct _short
		{
			union
			{
				unsigned char size;
				value_type lx;
			};
			value_type data[min_cap];
		};

		union ulx
		{
			_long lx;
			_short lxx;
		};

		static constexpr auto n_words = sizeof(ulx) / sizeof(size_type);

		struct raw
		{
			size_type words[n_words];
		};

		struct rep
		{
			union
			{
				_long l;
				_short s;
				raw r;
			};
		};

		internal::compressed_pair<rep, allocator_type> r_;

	public:
		basic_string() noexcept(noexcept(allocator_type{})): basic_string{allocator_type{}} {}
		explicit basic_string(const allocator_type& alloc) noexcept: r_{{}, alloc} {}
	};
}
