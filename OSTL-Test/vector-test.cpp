#include "pch.h"
#include "../include/OSTL/vector.h"

template <typename T>
void AssertAllEqual(const ostl::vector<T>& actual, std::initializer_list<T> expected) {
	ASSERT_EQ(actual.size(), expected.size());
	auto a = actual.begin();
	auto e = expected.begin();
	while (a != actual.end() && e != expected.end())
	{
		auto&& aa = *a++;
		auto&& ee = *e++;
		ASSERT_EQ(aa, ee);
	}
}

template <typename T>
void AssertAllEqual(const ostl::vector<T>& actual, const T& expected) {
	for (const T& value : actual)
		ASSERT_EQ(value, expected);
}

TEST(Vector, Constructor) {
	const std::initializer_list<std::string> init{ "the", "frogurt", "is", "also", "cursed" };
	ostl::vector<std::string> words1(init);
	AssertAllEqual(words1, init);

	const ostl::vector<std::string> words2(words1.begin(), words1.end());
	AssertAllEqual(words2, init);

	const auto words3(words1);
	AssertAllEqual(words3, init);

	const ostl::vector<std::string> words4(5, "Mo");
	AssertAllEqual<std::string>(words4, "Mo");
}

TEST(Vector, Assign) {
	const auto list = { 3, 1, 4, 6, 5, 9 };

	ostl::vector<int> nums1(list);
	ostl::vector<int> nums2;
	ostl::vector<int> nums3;

	AssertAllEqual(nums1, list);
	AssertAllEqual(nums2, {});
	AssertAllEqual(nums3, {});

	nums2 = nums1;

	AssertAllEqual(nums1, list);
	AssertAllEqual(nums2, list);
	AssertAllEqual(nums3, {});

	nums3 = std::move(nums1);

	AssertAllEqual(nums1, {});
	AssertAllEqual(nums2, list);
	AssertAllEqual(nums3, list);

	nums1.assign(3, 3);
	AssertAllEqual(nums1, 3);
}

TEST(Vector, ElementAccess) {
	ostl::vector<int> numbers{ 2,4,6,8 };
	ASSERT_THROW(numbers.at(4), std::out_of_range);
	ASSERT_EQ(numbers[1], 4);
	numbers[0] = 5;
	AssertAllEqual(numbers, { 5,4,6,8 });
	ASSERT_EQ(numbers.front(), 5);
	ASSERT_EQ(numbers.back(), 8);
	ASSERT_EQ(numbers.data()[2], 6);
}

TEST(Vector, Iterator) {
	ostl::vector<int> ints{ 1,2,4,8,16 };
	ostl::vector<std::string> fruits{ "orange","apple","raspberry" };
	ostl::vector<char> empty;

	auto sum = 0;
	for (auto it : ints)
		sum += it;
	
	ASSERT_EQ(sum, 31);
	ASSERT_EQ(*fruits.begin(), "orange");
	ASSERT_EQ(empty.begin(), empty.end());

	ASSERT_EQ(*ints.crbegin(), 16);
	ASSERT_EQ(*(ints.crbegin() + 1), 8);
	ASSERT_EQ(*(ints.crend() - 1), 1);
	ASSERT_EQ(empty.rbegin(), empty.rend());

	const auto rb = ints.crbegin(), rb2 = rb;
	ASSERT_EQ(rb, rb2);

	const ostl::vector revints(ints.crbegin(), ints.crend());
	AssertAllEqual(revints, { 16,8,4,2,1 });
}

TEST(Vector, Insert) {
	ostl::vector<int> vec(3, 100);
	AssertAllEqual(vec, 100);

	auto it = vec.begin();
	it = vec.insert(it, 200);
	AssertAllEqual(vec, { 200,100,100,100 });

	vec.insert(it, 2, 300);
	AssertAllEqual(vec, { 300,300,200,100,100,100 });

	it = vec.begin();

	ostl::vector<int> vec2(2, 400);
	vec.insert(it + 2, vec2.begin(), vec2.end());
	AssertAllEqual(vec, { 300,300,400,400,200,100,100,100 });

	int arr[] = { 501,502,503 };
	vec.insert(vec.begin(), arr, arr + 3);
	AssertAllEqual(vec, { 501,502,503,300,300,400,400,200,100,100,100 });

	vec.insert(vec.end(), { 6,9,7,4 });
	AssertAllEqual(vec, { 501,502,503,300,300,400,400,200,100,100,100,6,9,7,4 });
}

TEST(Vector, Erase) {
	ostl::vector<int> c{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
	AssertAllEqual(c, { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 });

	c.erase(c.begin());
	AssertAllEqual(c, { 1, 2, 3, 4, 5, 6, 7, 8, 9 });

	c.erase(c.begin() + 2, c.begin() + 5);
	AssertAllEqual(c, { 1,2,6,7,8,9 });

	for (auto it = c.begin(); it != c.end();)
		if (*it % 2 == 0) it = c.erase(it);
		else ++it;
	AssertAllEqual(c, { 1,7,9 });
}

TEST(Vector, Modifiers) {
	using namespace std::string_literals;

	ostl::vector<std::string> numbers;
	numbers.push_back("abc");
	std::string s = "def";
	numbers.push_back(std::move(s));
	numbers.emplace_back(3, 'A');

	AssertAllEqual(numbers, { "abc"s, "def"s, "AAA"s });
	ASSERT_EQ(s, ""s);

	numbers.pop_back();
	AssertAllEqual(numbers, { "abc"s, "def"s });

	numbers.resize(4, "wow"s);
	AssertAllEqual(numbers, { "abc"s, "def"s, "wow"s, "wow"s });

	numbers.resize(6, ""s);
	AssertAllEqual(numbers, { "abc"s, "def"s, "wow"s, "wow"s, ""s, ""s });

	numbers.resize(2);
	AssertAllEqual(numbers, { "abc"s, "def"s });

	ostl::vector<std::string> v{ "wtf"s, "x35"s };
	ostl::swap(numbers, v);
	AssertAllEqual(v, { "abc"s, "def"s });
	AssertAllEqual(numbers, { "wtf"s, "x35"s });
}

TEST(Vector, Compare) {
	const ostl::vector<int> vec1{ 6, 9, 7, 4 };
	const ostl::vector<int> vec2{ 6, 9, 7, 4 };
	const ostl::vector<int> vec3{ 5, 8, 8, 2 };

	ASSERT_TRUE(vec1 == vec2);
	ASSERT_TRUE(vec1 != vec3);
	ASSERT_TRUE(vec3 < vec1);
	ASSERT_TRUE(vec1 <= vec2);
	ASSERT_FALSE(vec1 > vec2);
	ASSERT_TRUE(vec1 >= vec3);
}

TEST(Vector, CapacityGrowth) {
	ostl::vector<int> v;
	auto f = [&v](size_t i) {while (i--)v.emplace_back(); };
	EXPECT_EQ(v.capacity(), 0);
	f(1);
	EXPECT_EQ(v.capacity(), 1);
	f(1);
	EXPECT_EQ(v.capacity(), 2);
	f(1);
	EXPECT_EQ(v.capacity(), 3);
	f(1);
	EXPECT_EQ(v.capacity(), 4);
	f(1);
	EXPECT_EQ(v.capacity(), 6);
	f(2);
	EXPECT_EQ(v.capacity(), 9);
	f(3);
	EXPECT_EQ(v.capacity(), 13);
	f(4);
	EXPECT_EQ(v.capacity(), 19);
	f(6);
	EXPECT_EQ(v.capacity(), 28);
	f(9);
	EXPECT_EQ(v.capacity(), 42);
	f(14);
	EXPECT_EQ(v.capacity(), 63);
}

TEST(VectorBool, Constructor) {
	const auto init = { false, true, true, false, true };
	ostl::vector words1(init);
	AssertAllEqual(words1, init);

	const ostl::vector words2(words1.begin(), words1.end());
	AssertAllEqual(words2, init);

	const auto words3(words1);
	AssertAllEqual(words3, init);

	const ostl::vector words4(5, true);
	AssertAllEqual(words4, true);
}

TEST(VectorBool, Assign) {
	const auto list = { true, false, false, true, true, false, true };

	ostl::vector nums1(list);
	ostl::vector<bool> nums2;
	ostl::vector<bool> nums3;

	AssertAllEqual(nums1, list);
	AssertAllEqual(nums2, {});
	AssertAllEqual(nums3, {});

	nums2 = nums1;

	AssertAllEqual(nums1, list);
	AssertAllEqual(nums2, list);
	AssertAllEqual(nums3, {});

	nums3 = std::move(nums1);

	AssertAllEqual(nums1, {});
	AssertAllEqual(nums2, list);
	AssertAllEqual(nums3, list);

	nums1.assign(3, true);
	AssertAllEqual(nums1, true);
}

TEST(VectorBool, ElementAccess) {
	ostl::vector numbers{ false, true, false, true };
	ASSERT_THROW(numbers.at(4), std::out_of_range);
	ASSERT_EQ(numbers[1], true);
	numbers[0] = true;
	AssertAllEqual(numbers, { true, true, false, true });
	ASSERT_EQ(numbers.front(), true);
	ASSERT_EQ(numbers.back(), true);
}

TEST(VectorBool, Iterator) {
	ostl::vector ints{ false, false, true, false, true };
	ostl::vector fruits{ false, true, true, false };
	ostl::vector<bool> empty;

	ASSERT_EQ(*fruits.begin(), false);
	ASSERT_EQ(empty.begin(), empty.end());

	ASSERT_EQ(*ints.crbegin(), true);
	ASSERT_EQ(*(ints.crbegin() + 1), false);
	ASSERT_EQ(*(ints.crend() - 1), false);
	ASSERT_EQ(empty.rbegin(), empty.rend());

	auto rb = ints.crbegin(), rb2 = rb;
	ASSERT_EQ(rb, rb2);

	const ostl::vector revints(ints.crbegin(), ints.crend());
	AssertAllEqual(revints, { false, false, true, false, true });
}

TEST(VectorBool, Insert) {
	ostl::vector vec(3, true);
	AssertAllEqual(vec, true);

	auto it = vec.begin();
	it = vec.insert(it, false);
	AssertAllEqual(vec, { false, true, true, true });

	vec.insert(it, 2, true);
	AssertAllEqual(vec, { true, true, false, true, true, true });

	it = vec.begin();

	ostl::vector vec2(2, false);
	vec.insert(it + 2, vec2.begin(), vec2.end());
	AssertAllEqual(vec, { true, true, false, false, false, true, true, true });

	bool arr[] = { false, true, false };
	vec.insert(vec.begin(), arr, arr + 3);
	AssertAllEqual(vec, { false, true, false, true, true, false, false, false, true, true, true });

	vec.insert(vec.end(), { false, true, false, true });
	AssertAllEqual(vec, { false, true, false, true, true, false, false, false, true, true, true, false, true, false, true });
}

TEST(VectorBool, Erase) {
	ostl::vector c{ false, true, false, true, true, false, false, false, true, true, true, false, true, false, true };

	c.erase(c.begin());
	AssertAllEqual(c, { true, false, true, true, false, false, false, true, true, true, false, true, false, true });

	c.erase(c.begin() + 2, c.begin() + 5);
	AssertAllEqual(c, { true, false, false, false, true, true, true, false, true, false, true });

	for (auto it = c.begin(); it != c.end();)
		if (*it) it = c.erase(it);
		else ++it;
	AssertAllEqual(c, false);
	ASSERT_EQ(c.size(), 5);
}

TEST(VectorBool, Modifiers) {
	ostl::vector<bool> numbers;
	numbers.push_back(true);
	numbers.emplace_back(false);

	AssertAllEqual(numbers, { true, false });

	numbers.pop_back();
	AssertAllEqual(numbers, { true });

	numbers.resize(4, false);
	AssertAllEqual(numbers, { true, false, false, false });

	numbers.resize(6, true);
	AssertAllEqual(numbers, { true, false, false, false, true, true });

	numbers.resize(2);
	AssertAllEqual(numbers, { true, false });

	ostl::vector v{ false, true };
	ostl::swap(numbers, v);
	AssertAllEqual(v, { true, false });
	AssertAllEqual(numbers, { false, true });
}

TEST(VectorBool, Compare) {
	const ostl::vector vec1{ true, false, true, false };
	const ostl::vector vec2{ true, false, true, false };
	const ostl::vector vec3{ false, true, false, true };

	ASSERT_TRUE(vec1 == vec2);
	ASSERT_TRUE(vec1 != vec3);
	ASSERT_TRUE(vec3 < vec1);
	ASSERT_TRUE(vec1 <= vec2);
	ASSERT_FALSE(vec1 > vec2);
	ASSERT_TRUE(vec1 >= vec3);
}