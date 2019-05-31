#include "CppUnitTest.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "OSTL/vector.h"

namespace OSTLTest
{
	TEST_CLASS(VectorTest)
	{
	public:
		TEST_METHOD(InsertTest)
		{
			constexpr size_t size = 3;
			constexpr int val = 100;
			ostl::vector<int> vec(size, val);
			size_t i = 0;
			for (int x : vec) {
				Assert::AreEqual(x, val);
				++i;
			}
			Assert::AreEqual(i, size);
			Assert::AreEqual(i, vec.size());

			auto it = vec.begin();
			constexpr int val2 = 200;
			it = vec.insert(it, val2);
			int v[]{ val2,val,val,val };
			for (size_t i = 0; i < size + 1; ++i)
				Assert::AreEqual(v[i], vec[i]);
			Assert::AreEqual(vec.size(), size + 1);
		}
	};
}
