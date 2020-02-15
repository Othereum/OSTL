#pragma once

#include <type_traits>

namespace ostl::internal
{
	template <class A, class B, bool = std::is_empty_v<A> || std::is_empty_v<B>>
	struct compressed_pair;

	template <class A, class B>
	struct compressed_pair<A, B, true>
	{
		compressed_pair(const A& a, const B& b): first{a}, second{b} {}
		compressed_pair(const A& a, B&& b): first{a}, second{std::move(b)} {}
		compressed_pair(A&& a, const B& b): first{std::move(a)}, second{b} {}
		compressed_pair(A&& a, B&& b): first{std::move(a)}, second{std::move(b)} {}
		
		union
		{
			A first;
			B second;
		};
	};

	template <class A, class B>
	struct compressed_pair<A, B, false>
	{
		compressed_pair(const A& a, const B& b): first{a}, second{b} {}
		compressed_pair(const A& a, B&& b): first{a}, second{std::move(b)} {}
		compressed_pair(A&& a, const B& b): first{std::move(a)}, second{b} {}
		compressed_pair(A&& a, B&& b): first{std::move(a)}, second{std::move(b)} {}
		
		A first;
		B second;
	};
}
