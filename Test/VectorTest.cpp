#include "pch.h"
#include "OSTL/vector.h"

template <typename T>
void AssertAllEqual(const ostl::vector<T>& actual, std::initializer_list<T> expected) {
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

TEST(Vector, Constructor) {
	std::initializer_list<std::string> init{ "the", "frogurt", "is", "also", "cursed" };
	ostl::vector<std::string> words1(init);
	AssertAllEqual(words1, init);

	ostl::vector<std::string> words2(words1.begin(), words1.end());
	AssertAllEqual(words2, init);

	ostl::vector<std::string> words3(words1);
	AssertAllEqual(words3, init);

	ostl::vector<std::string> words4(5, "Mo");
	AssertAllEqual<std::string>(words4, "Mo");
}

TEST(Vector, Replace) {
	const auto list = { 3, 1, 4, 6, 5, 9 };
	const auto expected_size = list.size();

	ostl::vector<int> nums1(list);
	ostl::vector<int> nums2;
	ostl::vector<int> nums3;

	ASSERT_EQ(nums1.size(), expected_size);
	ASSERT_EQ(nums2.size(), 0);
	ASSERT_EQ(nums3.size(), 0);
	AssertAllEqual(nums1, list);
	
	nums2 = nums1;

	ASSERT_EQ(nums1.size(), expected_size);
	ASSERT_EQ(nums2.size(), expected_size);
	ASSERT_EQ(nums3.size(), 0);
	AssertAllEqual(nums1, list);
	AssertAllEqual(nums2, list);

	nums3 = std::move(nums1);

	ASSERT_EQ(nums1.size(), 0);
	ASSERT_EQ(nums2.size(), expected_size);
	ASSERT_EQ(nums3.size(), expected_size);
	AssertAllEqual(nums2, list);
	AssertAllEqual(nums3, list);
}

TEST(Vector, Assign) {
	ostl::vector<char> characters;
	characters.assign(5, 'a');
	AssertAllEqual(characters, 'a');
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

	int sum = 0;
	for (auto it = ints.cbegin(); it != ints.cend(); ++it)
		sum += *it;
	ASSERT_EQ(sum, 31);
	ASSERT_EQ(*fruits.begin(), "orange");
	ASSERT_EQ(empty.begin(), empty.end());

	ASSERT_EQ(*ints.crbegin(), 16);
	ASSERT_EQ(*(ints.crbegin() + 1), 8);
	ASSERT_EQ(*(ints.crend() - 1), 1);
	ASSERT_EQ(empty.rbegin(), empty.rend());

	const auto rb = ints.crbegin(), rb2 = rb;
	ASSERT_EQ(rb, rb2);

	ostl::vector<int> revints(ints.crbegin(), ints.crend());
	AssertAllEqual(revints, { 16,8,4,2,1 });
}

TEST(Vector, Insert) {
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
