#include "gtest/gtest.h"
#include "OSTL/functional.h"
#include <iostream>

struct Foo
{
	Foo(const int num) : num(num)
	{
	}

	void print_add(const int i) const { std::cout << num + i << '\n'; }
	int num;
};

void print_num(const int i)
{
	std::cout << i << '\n';
}

struct Print_num
{
	void operator()(const int i) const
	{
		std::cout << i << '\n';
	}
};

TEST(function, function)
{
	// store a free function
	ostl::function<void(int)> f_display = print_num;
	f_display(-9);

	// store a lambda
	ostl::function<void()> f_display_42 = []() { print_num(42); };
	f_display_42();

	// store the result of a call to std::bind
    std::function<void()> f_display_31337 = std::bind(print_num, 31337);
    f_display_31337();

	const Foo foo(314159);

	// store a call to a member function
    // std::function<void(const Foo&, int)> f_add_display = &Foo::print_add;
    // f_add_display(foo, 1);
    // f_add_display(314159, 1);

	// store a call to a data member accessor
    // std::function<int(Foo const&)> f_num = &Foo::num;
    // std::cout << "num_: " << f_num(foo) << '\n';

	// store a call to a member function and object
    using std::placeholders::_1;
    std::function<void(int)> f_add_display2 = std::bind( &Foo::print_add, foo, _1 );
    f_add_display2(2);
	
	// store a call to a member function and object ptr
    std::function<void(int)> f_add_display3 = std::bind( &Foo::print_add, &foo, _1 );
    f_add_display3(3);

	// store a call to a function object
	ostl::function<void(int)> f_display_obj = Print_num();
	f_display_obj(18);
}
