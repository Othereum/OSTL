#include "pch.h"
#include "OSTL/vector.h"

template <typename T>
void AssertAllEqual(const ostl::vector<T>& actual, std::initializer_list<T> expected) {
	ASSERT_EQ(actual.size(), expected.size());
	auto a = actual.begin();
	auto e = expected.begin();
	while (a != actual.end() && e != expected.end())
		ASSERT_EQ(*a++, *e++);
}

template <typename T>
void AssertAllEqual(const ostl::vector<T>& actual, const T& expected) {
	for (const T& value : actual)
		ASSERT_EQ(value, expected);
}

TEST(vbTest, Constructor) {
	auto init = { false, true, true, false, true };
	ostl::vector words1(init);
	AssertAllEqual(words1, init);

	ostl::vector words2(words1.begin(), words1.end());
	AssertAllEqual(words2, init);

	ostl::vector words3(words1);
	AssertAllEqual(words3, init);

	ostl::vector words4(5, true);
	AssertAllEqual(words4, true);
}

TEST(vbTest, Assign) {
	auto list = { true, false, false, true, true, false, true };
	auto expected_size = list.size();

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

TEST(vbTest, ElementAccess) {
	ostl::vector numbers{ false, true, false, true };
	ASSERT_THROW(numbers.at(4), std::out_of_range);
	ASSERT_EQ(numbers[1], true);
	numbers[0] = true;
	AssertAllEqual(numbers, { true, true, false, true });
	ASSERT_EQ(numbers.front(), true);
	ASSERT_EQ(numbers.back(), true);
}

TEST(vbTest, Iterator) {
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

	ostl::vector revints(ints.crbegin(), ints.crend());
	AssertAllEqual(revints, { false, false, true, false, true });
}

TEST(vbTest, Insert) {
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

TEST(vbTest, Erase) {
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

TEST(vbTest, Modifiers) {
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

TEST(vbTest, Compare) {
	ostl::vector vec1{ true, false, true, false };
	ostl::vector vec2{ true, false, true, false };
	ostl::vector vec3{ false, true, false, true };

	ASSERT_TRUE(vec1 == vec2);
	ASSERT_TRUE(vec1 != vec3);
	ASSERT_TRUE(vec3 < vec1);
	ASSERT_TRUE(vec1 <= vec2);
	ASSERT_FALSE(vec1 > vec2);
	ASSERT_TRUE(vec1 >= vec3);
}
