#include "pch.h"
#include "OSTL/vector.h"

template <typename T>
void AssertAllEqual(const ostl::vector<T>& actual, std::initializer_list<T> expected)
{
	auto a = actual.begin();
	auto e = expected.begin();
	while (a != actual.end() && e != expected.end())
		ASSERT_EQ(*a++, *e++);
}

template <typename T>
void AssertAllEqual(const ostl::vector<T>& actual, const T& expected)
{
	for (const T& value : actual)
		ASSERT_EQ(value, expected);
}

TEST(VectorTest, ConstructorTest) {
	std::initializer_list<std::string> init{ "the", "frogurt", "is", "also", "cursed" };
	ostl::vector<std::string> words1(init);
	AssertAllEqual(words1, init);

	/* emplace_back is used internal, but it's not implemented yet
	ostl::vector<std::string> words2(words1.begin(), words1.end());
	AssertAllEqual(words2, init);
	*/

	ostl::vector<std::string> words3(words1);
	AssertAllEqual(words3, init);

	ostl::vector<std::string> words4(5, "Mo");
	AssertAllEqual<std::string>(words4, "Mo");
}

TEST(VectorTest, InsertTest)
{
	constexpr size_t size = 3;
	constexpr int val = 100;
	ostl::vector<int> vec(size, val);
	AssertAllEqual(vec, val);
	ASSERT_EQ(vec.size(), size);

	auto it = vec.begin();
	constexpr int val2 = 200;
	it = vec.insert(it, val2);
	AssertAllEqual(vec, { val2,val,val,val });
	ASSERT_EQ(vec.size(), size + 1);
}

