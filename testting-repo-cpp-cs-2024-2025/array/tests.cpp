#include "array.hpp"
#include "gtest/gtest.h"
#include <climits>
#include <stdexcept>
#include <type_traits>

#include <algorithm>
#include <random>
#include <numeric>

#include "IteratorTests.hpp"

namespace memres {

inline std::size_t new_called = 0;
inline std::size_t delete_called = 0;

struct CountedResource : MemoryResource {
  void* allocate(std::size_t count) override {
    ++new_called;
    return malloc(count);
  }
  void deallocate(void* ptr) override {
    ++delete_called;
    free(ptr);
  }
};

}  // namespace memres

std::size_t new_called_count = 0;
std::size_t delete_called_count = 0;

void* operator new(std::size_t count) {
  ++new_called_count;
  return malloc(count);
}

void* operator new(std::size_t count, const std::nothrow_t&) noexcept {
  ++new_called_count;
  return malloc(count);
}

void operator delete(void* ptr) noexcept {
  ++delete_called_count;
  free(ptr);
}

void operator delete(void* ptr, std::size_t /*count*/) noexcept {
  ++delete_called_count;
  free(ptr);
}

// ============== Config Parameters ======================

constexpr static std::size_t kSmall = 3;
constexpr static std::size_t kMedium = 10;
constexpr static std::size_t kLarge = 1000;

template <typename T>
using EmptyStaticArray = Array<T, 0>;

template <typename T>
using SmallStaticArray = Array<T, kSmall>;

template <typename T>
using MediumStaticArray = Array<T, kMedium>;

template <typename T>
using LargeStaticArray = Array<T, kLarge>;

template <typename T>
using DynamicArray = Array<T, kDynamicExtent>;

// -------------------------------------------------------

struct NotDefaultConstructible {
  NotDefaultConstructible() = delete;
  NotDefaultConstructible(int data) : data(data) {}
  int data;
};

template <class RangeLeft, class RangeRight>
bool EqualRanges(const RangeLeft& lhs, const RangeRight& rhs) {
  if (lhs.size() != rhs.size()) {
    return false;
  }
  for (std::size_t i = 0; i < lhs.size(); ++i) {
    if (lhs[i] != rhs[i]) {
      return false;
    }
  }
  return true;
}

// -------------------------------------------------------

template <typename T>
class Int {
 public:
  Int() = default;
  Int(int value) : value_(value) {}
  operator int&() { return value_; }
  operator const int&() const { return value_; }

 private:
  int value_;
};

// ============== Compile Time Tests =====================

static_assert(not std::is_default_constructible_v<SmallStaticArray<int>>);
static_assert(not std::is_constructible_v<SmallStaticArray<int>, int, int, int>);
static_assert(not std::is_default_constructible_v<EmptyStaticArray<int>>);

static_assert(std::is_copy_constructible_v<SmallStaticArray<int>>);
static_assert(std::is_copy_assignable_v<SmallStaticArray<int>>);

static_assert(not std::is_copy_constructible_v<Array<int, kMedium, strategy::Singleton>>);
static_assert(not std::is_copy_assignable_v<Array<int, kMedium, strategy::Singleton>>);

static_assert(not std::is_default_constructible_v<DynamicArray<int>>);
static_assert(not std::is_constructible_v<DynamicArray<int>, std::size_t>);
static_assert(not std::is_constructible_v<DynamicArray<int>, std::size_t, int>);

static_assert(std::is_copy_constructible_v<DynamicArray<int>>);
static_assert(std::is_copy_assignable_v<DynamicArray<int>>);

static_assert(not std::is_copy_constructible_v<Array<int, kMedium, strategy::Singleton>>);
static_assert(not std::is_copy_assignable_v<Array<int, kMedium, strategy::Singleton>>);

static_assert(sizeof(EmptyStaticArray<int>) == 1);
static_assert(sizeof(SmallStaticArray<int>) == sizeof(int) * kSmall);
static_assert(sizeof(MediumStaticArray<std::string>) == sizeof(std::string) * kMedium);
static_assert(sizeof(LargeStaticArray<std::vector<int>>) == sizeof(std::vector<int>) * kLarge);

static_assert(sizeof(DynamicArray<int>) == sizeof(void*) * 2 + sizeof(std::size_t) * 2);
static_assert(sizeof(DynamicArray<std::string>) == sizeof(void*) * 2 + sizeof(std::size_t) * 2);
static_assert(sizeof(DynamicArray<std::vector<int>>) == sizeof(void*) * 2 + sizeof(std::size_t) * 2);
static_assert(sizeof(DynamicArray<SmallStaticArray<int>>) == sizeof(void*) * 2 + sizeof(std::size_t) * 2);

TEST(StaticExtent, Constructors) {
  auto vec_small_int = SmallStaticArray<int>::create(1, 2, 3);
  EXPECT_EQ(vec_small_int.size(), kSmall);

  auto vec_medium_int = MediumStaticArray<int>::create(1, 2, 3, 4);
  EXPECT_EQ(vec_medium_int.size(), kMedium);

  auto vec_large_int = LargeStaticArray<int>::create();
  EXPECT_EQ(vec_large_int.size(), kLarge);

  auto vec = Array<NotDefaultConstructible, kSmall>::create(
    NotDefaultConstructible{1}, NotDefaultConstructible{2}, NotDefaultConstructible{3}
  );
  EXPECT_EQ(vec.size(), kSmall);
}

TEST(StaticExtent, Operators) {
  auto vec = SmallStaticArray<int>::create(1, 2, 3);

  EXPECT_EQ(vec.front(), 1);
  EXPECT_EQ(vec.back(), 3);

  EXPECT_EQ(vec[0], 1);
  EXPECT_EQ(vec[1], 2);
  EXPECT_EQ(vec[2], 3);

  EXPECT_EQ(vec[0], vec.at(0));
  EXPECT_THROW(vec.at(3), std::out_of_range);

  vec[0] = 100;
  EXPECT_EQ(vec.front(), 100);

  vec.back() = 123;
  EXPECT_EQ(vec[2], 123);
}

TEST(StaticExtent, ConstOperators) {
  const auto vec = SmallStaticArray<int>::create(1, 2, 3);

  EXPECT_EQ(vec.front(), 1);
  EXPECT_EQ(vec.back(), 3);

  EXPECT_EQ(vec[0], 1);
  EXPECT_EQ(vec[1], 2);
  EXPECT_EQ(vec[2], 3);

  EXPECT_EQ(vec[0], vec.at(0));
  EXPECT_THROW(vec.at(3), std::out_of_range);

  static_assert(std::is_same_v<decltype(vec[0]), const int&>);
  static_assert(std::is_same_v<decltype(vec.front()), const int&>);
  static_assert(std::is_same_v<decltype(vec.back()), const int&>);
}

TEST(StaticExtent, Copy) {
  auto vec1 = SmallStaticArray<int>::create(1, 2, 3);
  auto vec2 = SmallStaticArray<int>::create(3, 4, 5);
  auto vec3 = vec1;

  EXPECT_TRUE(EqualRanges(vec1, vec3));
  EXPECT_FALSE(EqualRanges(vec1, vec2));

  vec2 = vec1;
  EXPECT_TRUE(EqualRanges(vec1, vec2));

  vec1 = vec1;
  EXPECT_TRUE(EqualRanges(vec1, vec3));
}

TEST(StaticExtent, IteratorTraits) {
  IteratorTest<typename SmallStaticArray<int>::iterator, int>();
  IteratorTest<decltype(std::declval<SmallStaticArray<int>>().rbegin()), int>();
  IteratorTest<decltype(std::declval<SmallStaticArray<int>>().cbegin()), const int>();
}

TEST(StaticExtent, IteratorArithmetic) {
  {
    auto empty = EmptyStaticArray<int>::create();

    EXPECT_EQ(empty.end() - empty.begin(), 0);
    EXPECT_EQ(empty.begin() + 0, empty.end());
    EXPECT_EQ(empty.end() - 0, empty.begin());

    EXPECT_EQ(empty.rend() - empty.rbegin(), 0);
    EXPECT_EQ(empty.rbegin() + 0, empty.rend());
    EXPECT_EQ(empty.rend() - 0, empty.rbegin());

    EXPECT_EQ(empty.cend() - empty.cbegin(), 0);
    EXPECT_EQ(empty.cbegin() + 0, empty.cend());
    EXPECT_EQ(empty.cend() - 0, empty.cbegin());
  }

  {
    auto one = Array<int, 1>::create(1);
    auto iter = one.end();

    EXPECT_EQ(--iter, one.begin());
    EXPECT_EQ(iter++, one.begin());
  }

  {
    auto arr = LargeStaticArray<int>::create();

    EXPECT_EQ(std::size_t(arr.end() - arr.begin()), arr.size());
    EXPECT_EQ(arr.begin() + arr.size(), arr.end());
    EXPECT_EQ(arr.end() - arr.size(), arr.begin());
  }
}

TEST(StaticExtent, IteratorComparation) {
  auto arr = LargeStaticArray<int>::create();

  EXPECT_TRUE(arr.end() > arr.begin());
  EXPECT_TRUE(arr.cend() > arr.cbegin());
  EXPECT_TRUE(arr.rend() > arr.rbegin());
}

TEST(StaticExtent, IteratorAlgorithms) {
   auto arr = LargeStaticArray<int>::create();

  std::iota(arr.begin(), arr.end(), 13);
  std::mt19937 g(31415);
  std::shuffle(arr.begin(), arr.end(), g);
  std::sort(arr.rbegin(), arr.rbegin() + 500);
  std::reverse(arr.begin(), arr.end());
  auto sorted_border = std::is_sorted_until(arr.begin(), arr.end());

  EXPECT_EQ(sorted_border - arr.begin(), 500);
}

TEST(StaticExtent, RangeBasedFor) {
  auto arr = SmallStaticArray<int>::create(1, 2, 3);
  std::array<int, 3> res{1, 2, 3};
  std::size_t i = 0;

  for (const auto& elem : arr) {
    EXPECT_EQ(elem, res[i]);
    ++i;
  }
}

TEST(DynamicExtent, Constructors) {
  {
    auto vec = DynamicArray<int>::create();
    EXPECT_EQ(vec.size(), 0);
  }
  {
    auto vec = DynamicArray<int>::create(10ull, 20);
    EXPECT_TRUE(EqualRanges(vec, std::vector<int>(10, 20)));
  }
  {
    auto vec = DynamicArray<int>::create(10ull);
    EXPECT_EQ(vec.size(), 10);
  }
  {
    memres::CountedResource res{};
    auto vec = DynamicArray<int>::create(&res);
    EXPECT_EQ(vec.size(), 0);
  }
}

TEST(DynamicExtent, Operators) {
  auto vec = DynamicArray<int>::create(10ull, 2);

  EXPECT_EQ(vec.front(), 2);
  EXPECT_EQ(vec.back(), 2);

  EXPECT_EQ(vec[0], 2);
  EXPECT_EQ(vec[1], 2);
  EXPECT_EQ(vec[2], 2);

  EXPECT_EQ(vec[0], vec.at(0));
  EXPECT_THROW(vec.at(13), std::out_of_range);

  vec[0] = 100;
  EXPECT_EQ(vec.front(), 100);

  vec.back() = 123;
  EXPECT_EQ(vec[9], 123);
  EXPECT_EQ(vec[0], 100);
}

TEST(DynamicExtent, ConstOperators) {
    const auto vec = DynamicArray<int>::create(10ull, 2);

  EXPECT_EQ(vec.front(), 2);
  EXPECT_EQ(vec.back(), 2);

  EXPECT_EQ(vec[0], 2);
  EXPECT_EQ(vec[1], 2);
  EXPECT_EQ(vec[2], 2);

  EXPECT_EQ(vec[0], vec.at(0));
  EXPECT_THROW(vec.at(11), std::out_of_range);

  static_assert(std::is_same_v<decltype(vec[0]), const int&>);
  static_assert(std::is_same_v<decltype(vec.front()), const int&>);
  static_assert(std::is_same_v<decltype(vec.back()), const int&>);
}

TEST(DynamicExtent, Copy) {
  auto vec1 = DynamicArray<int>::create(10ull, 2);
  auto vec2 = DynamicArray<int>::create();
  auto vec3 = vec1;

  EXPECT_TRUE(EqualRanges(vec1, vec3));
  EXPECT_FALSE(EqualRanges(vec2, vec3));

  vec3 = vec2;
  
  EXPECT_FALSE(EqualRanges(vec1, vec3));
  EXPECT_TRUE(EqualRanges(vec2, vec3));

  vec2 = vec1;
  EXPECT_TRUE(EqualRanges(vec2, vec1));

  vec1 = vec1;
  EXPECT_TRUE(EqualRanges(vec2, vec1));
}

TEST(DynamicExtent, IteratorTraits) {
  IteratorTest<typename DynamicArray<int>::iterator, int>();
  IteratorTest<decltype(std::declval<DynamicArray<int>>().rbegin()), int>();
  IteratorTest<decltype(std::declval<DynamicArray<int>>().cbegin()), const int>();
}

TEST(DynamicExtent, IteratorArithmetic) {
  {
    auto empty = DynamicArray<int>::create();

    EXPECT_EQ(empty.end() - empty.begin(), 0);
    EXPECT_EQ(empty.begin() + 0, empty.end());
    EXPECT_EQ(empty.end() - 0, empty.begin());

    EXPECT_EQ(empty.rend() - empty.rbegin(), 0);
    EXPECT_EQ(empty.rbegin() + 0, empty.rend());
    EXPECT_EQ(empty.rend() - 0, empty.rbegin());

    EXPECT_EQ(empty.cend() - empty.cbegin(), 0);
    EXPECT_EQ(empty.cbegin() + 0, empty.cend());
    EXPECT_EQ(empty.cend() - 0, empty.cbegin());
  }

  {
    auto one = DynamicArray<int>::create(1ull, 1);
    auto iter = one.end();

    EXPECT_EQ(--iter, one.begin());
    EXPECT_EQ(iter++, one.begin());
  }

  {
    auto arr = DynamicArray<int>::create(100ull, 1);

    EXPECT_EQ(std::size_t(arr.end() - arr.begin()), arr.size());
    EXPECT_EQ(arr.begin() + arr.size(), arr.end());
    EXPECT_EQ(arr.end() - arr.size(), arr.begin());
  }
}

TEST(DynamicExtent, IteratorAlgorithms) {
   auto arr = DynamicArray<int>::create(1000ull, 1);

  std::iota(arr.begin(), arr.end(), 13);
  std::mt19937 g(31415);
  std::shuffle(arr.begin(), arr.end(), g);
  std::sort(arr.rbegin(), arr.rbegin() + 500);
  std::reverse(arr.begin(), arr.end());
  auto sorted_border = std::is_sorted_until(arr.begin(), arr.end());

  EXPECT_EQ(sorted_border - arr.begin(), 500);
}

TEST(DynamicExtent, RangeBasedFor) {
  auto vec = DynamicArray<int>::create(10ull, 2);
  std::vector<int> res(10, 2);
  std::size_t i = 0;

  for (const auto& elem : vec) {
    EXPECT_EQ(elem, res[i]);
    ++i;
  }
}

TEST(DynamicExtent, PushAndPop) {
  auto vec = DynamicArray<int>::create(10000ull, 1);
  int start_size = static_cast<int>(vec.size());

  auto middle_iter = vec.begin() + (start_size / 2); // 5000
  auto& middle_element = *middle_iter;
  auto begin = vec.begin();
  auto end = vec.rbegin();

  auto middle2_iter = middle_iter + 2000; // 7000

  // remove 800 elements
  for (size_t i = 0; i < 800; ++i) {
    vec.pop_back();
  }

  // begin and middle iterators are still valid
  EXPECT_TRUE(*begin == 1);
  EXPECT_TRUE(*middle_iter == 1);
  EXPECT_TRUE(middle_element == 1);
  EXPECT_TRUE(*middle2_iter == 1);

  end = vec.rbegin();

  // removed 9980 items in total
  for (size_t i = 0; i < 9180; ++i) {
    vec.pop_back();
  }

  EXPECT_TRUE(vec.size() == 20);
  EXPECT_TRUE(std::all_of(vec.begin(),
                          vec.end(),
                          [](const auto& item) { return item == 1; }));

  for (size_t i = 0; i < 80; ++i) {
    vec.push_back(2);
  }

  EXPECT_EQ(vec.size(), 100);
  EXPECT_EQ(vec.front(), 1);
  EXPECT_EQ(vec.back(), 2);
}

TEST(DynamicExtent, ResizeAndReserve) {
  auto vec = DynamicArray<int>::create(100ull, 1);
  
  EXPECT_GE(vec.capacity(), 100);

  std::size_t cap_before_resize = vec.capacity();
  vec.resize(10ull);
  EXPECT_EQ(vec.size(), 10ull);
  EXPECT_EQ(vec.capacity(), cap_before_resize);
  
  vec.resize(300ull, 2);
  EXPECT_EQ(vec.size(), 300);
  EXPECT_EQ(vec[0], 1);
  EXPECT_EQ(vec[20], 2);

  auto copy = vec;

  cap_before_resize = copy.capacity();
  vec.resize(15ull);
  vec.shrink_to_fit();
  EXPECT_EQ(vec.size(), vec.capacity());

  copy.clear();
  EXPECT_EQ(copy.size(), 0);
  EXPECT_EQ(copy.capacity(), cap_before_resize);

  copy.shrink_to_fit();
  EXPECT_EQ(copy.capacity(), 0);
}



TEST(DifferentMemoryResources, JustWorks) {
  memres::NewDeleteResource nd_res;
  memres::MallocFreeResource mf_res;

  new_called_count = 0;
  delete_called_count = 0;
  {
    auto nd_vec = DynamicArray<int>::create(&nd_res);
    nd_vec.resize(100);
  }
  EXPECT_TRUE(new_called_count > 0);
  EXPECT_TRUE(delete_called_count > 0);

  new_called_count = 0;
  delete_called_count = 0;
  {
    auto nd_vec = DynamicArray<int>::create(10ull, 13, &nd_res);
    EXPECT_EQ(nd_vec.front(), 13);
    EXPECT_EQ(nd_vec.size(), 10);
  }
  EXPECT_TRUE(new_called_count > 0);
  EXPECT_TRUE(delete_called_count > 0);

  new_called_count = 0;
  delete_called_count = 0;
  {
    auto nd_vec = DynamicArray<int>::create(&mf_res);
    nd_vec.resize(100);
  }
  EXPECT_TRUE(new_called_count == 0);
  EXPECT_TRUE(delete_called_count == 0);

  new_called_count = 0;
  {
    auto nd_vec = DynamicArray<int>::create(10ull, 13, &mf_res);
    EXPECT_EQ(nd_vec.front(), 13);
    EXPECT_EQ(nd_vec.size(), 10);
  }
  EXPECT_TRUE(new_called_count == 0);
  EXPECT_TRUE(delete_called_count == 0);
}

TEST(DifferentMemoryResources, Assignment) {
  memres::NewDeleteResource nd_res;
  memres::MallocFreeResource mf_res;

  auto nd_vec = DynamicArray<int>::create(10ull, 20, &nd_res);
  auto mf_vec = DynamicArray<int>::create(15ull, 2, &mf_res);

  new_called_count = 0;
  delete_called_count = 0;
  
  nd_vec = mf_vec;
  EXPECT_TRUE(new_called_count == 0);
  EXPECT_TRUE(delete_called_count > 0);

  EXPECT_TRUE(EqualRanges(nd_vec, mf_vec));
}

TEST(DifferentMemoryResources, CustomResource) {
  memres::CountedResource c_res;

  new_called_count = 0;
  delete_called_count = 0;

  memres::new_called = 0;
  memres::delete_called = 0;
  {
    auto c_vec = DynamicArray<int>::create(10ull, 20, &c_res);
  }
  EXPECT_TRUE(new_called_count == 0);
  EXPECT_TRUE(delete_called_count == 0);

  EXPECT_TRUE(memres::new_called > 0);
  EXPECT_TRUE(memres::delete_called > 0);
}

TEST(DifferentMemoryResources, DefaultResources) {
  memres::NewDeleteResource nd_res;
  memres::MallocFreeResource mf_res;
  memres::CountedResource c_res;

  memres::MemoryResource* old_res = memres::SetDefaultResource(&c_res);

  memres::new_called = 0;
  memres::delete_called = 0;
  new_called_count = 0;
  delete_called_count = 0;
  {
    auto vec = DynamicArray<double>::create(10ull, 20);
    vec.push_back(10.);
  }
  EXPECT_TRUE(memres::new_called > 0);
  EXPECT_TRUE(memres::delete_called > 0);
  EXPECT_TRUE(new_called_count == 0);
  EXPECT_TRUE(delete_called_count == 0);
  
  EXPECT_TRUE(&c_res == memres::GetDefaultResource());
  EXPECT_TRUE(&c_res == memres::SetDefaultResource(&nd_res));

  {
    auto vec = DynamicArray<double>::create();
    for (std::size_t i = 0; i < 100ull; ++i) {
      vec.push_back(static_cast<double>(i));
    }
  }

  EXPECT_TRUE(new_called_count > 0);
  EXPECT_TRUE(delete_called_count > 0);

  EXPECT_EQ(&nd_res, memres::SetDefaultResource(old_res));
}

TEST(DifferentStrategies, Sigleton) {
  using arr_t = Array<Int<struct DiffStSing>, kMedium, strategy::Singleton>;
  auto& arr1 = arr_t::create();
  auto& arr2 = arr_t::create();

  EXPECT_EQ(&arr1, &arr2);
  EXPECT_ANY_THROW(arr_t::create(2));

  using large_array_t = Array<Int<struct DiffStSing>, kLarge, strategy::Singleton>;
  auto& larr1 = large_array_t::create(1, 2, 3, 4, 5);
  auto& larr2 = large_array_t::create(1, 2, 3, 4, 5);
  EXPECT_EQ(&larr1, &larr2);
  EXPECT_ANY_THROW(large_array_t::create());
  EXPECT_ANY_THROW(large_array_t::create(1, 2, 3, 4, 6));

  using dynamic_array_t = Array<Int<struct DiffStSing>, kDynamicExtent, strategy::Singleton>;
  auto& darr1 = dynamic_array_t::create(10ull, 2);
  auto& darr2 = dynamic_array_t::create(10ull, 2);
  EXPECT_ANY_THROW(dynamic_array_t::create(10ull, 3));

  EXPECT_EQ(&darr1, &darr2);
  for (int i = 0; i < 10; ++i) {
    darr1.push_back(i);
  }
  EXPECT_EQ(darr1.back(), 9);
  EXPECT_EQ(darr2.back(), 9);
  EXPECT_EQ(dynamic_array_t::create(10ull, 2).back(), 9);
}

TEST(DifferentStrategies, CountedCreation) {
  using arr_t = Array<int, kSmall, strategy::CountedCreation>;
  auto arr1 = arr_t::create();
  auto arr2 = arr_t::create(1, 2, 3);
  ASSERT_EQ(arr_t::get_created_count(), 2);

  using dyn_arr_t = Array<double, kDynamicExtent, strategy::CountedCreation>;
  auto darr1 = dyn_arr_t::create();
  auto darr2 = dyn_arr_t::create(10ull, 2);
  auto darr3 = dyn_arr_t::create(1ull, 3);

  ASSERT_EQ(arr_t::get_created_count(), 2);
  ASSERT_EQ(dyn_arr_t::get_created_count(), 3);

  auto arr3 = arr2;
  ASSERT_EQ(arr_t::get_created_count(), 3);

  auto darr4 = darr3;
  ASSERT_EQ(dyn_arr_t::get_created_count(), 4);
}

TEST(DifferentStrategies, MixWithResources) {
  static memres::MallocFreeResource mf_res{};
  static memres::CountedResource c_res{};
  using dynamic_array_t = Array<Int<struct DiffStMWR>, kDynamicExtent, strategy::Singleton>;
  auto& arr = dynamic_array_t::create(10ull, 2, &mf_res);

  new_called_count = 0;
  delete_called_count = 0;
  for (std::size_t i = 0; i < 100ull; ++i) {
    arr.push_back(0);
  }
  EXPECT_EQ(new_called_count, 0);
  EXPECT_EQ(delete_called_count, 0);

  auto* old_resource = memres::SetDefaultResource(&mf_res);
  EXPECT_EQ(dynamic_array_t::create(10ull, 2, &mf_res).size(), 110ull);
  EXPECT_ANY_THROW(dynamic_array_t::create(10ull, 2));
  EXPECT_ANY_THROW(dynamic_array_t::create(10ull, 2, &c_res));

  memres::SetDefaultResource(old_resource);
}

TEST(ArrayTraits, Default) {
  using arr1d_t = Array<int, 4>;
  using arr2d_t = Array<arr1d_t, 2, strategy::CountedCreation>;
  using arr3d_t = Array<arr2d_t, 3>;

  auto arr_help = arr1d_t::create();
  auto arr = arr3d_t::create(
    arr2d_t::create(arr_help, arr_help),
    arr2d_t::create(arr_help, arr_help),
    arr2d_t::create(arr_help, arr_help)
  );
  EXPECT_EQ(traits::GetRank(arr), 3);
  EXPECT_EQ(traits::GetTotalElements(arr), 3 * 4 * 2);
  EXPECT_EQ(traits::GetExtent<0>(arr), 3);
  EXPECT_EQ(traits::GetExtent<1>(arr), 2);
  EXPECT_EQ(traits::GetExtent<2>(arr), 4);
}

TEST(ArrayTraits, LargeStatic) {
  using arr1d_t = Array<int, 23, strategy::CountedCreation>;
  auto arr1 = arr1d_t::create();

  using arr2d_t = Array<arr1d_t, 4>;
  auto arr2 = arr2d_t::create(arr1, arr1, arr1, arr1);

  using arr3d_t = Array<arr2d_t, 5>;
  auto arr3 = arr3d_t::create(arr2, arr2, arr2, arr2, arr2);

  using arr4d_t = Array<arr3d_t, 2, strategy::DefaultCreation>;
  auto arr4 = arr4d_t::create(arr3, arr3);

  using arr5d_t = Array<arr4d_t, 12>;
  auto arr5 = arr5d_t::create(arr4, arr4, arr4, arr4, arr4, arr4, arr4, arr4, arr4, arr4, arr4, arr4);

  EXPECT_EQ(traits::GetRank(arr1), 1);
  EXPECT_EQ(traits::GetRank(arr2), 2);
  EXPECT_EQ(traits::GetRank(arr3), 3);
  EXPECT_EQ(traits::GetRank(arr4), 4);
  EXPECT_EQ(traits::GetRank(arr5), 5);

  EXPECT_EQ(traits::GetTotalElements(arr1), 23);
  EXPECT_EQ(traits::GetTotalElements(arr2), 23 * 4);
  EXPECT_EQ(traits::GetTotalElements(arr3), 23 * 4 * 5);
  EXPECT_EQ(traits::GetTotalElements(arr4), 23 * 4 * 5 * 2);
  EXPECT_EQ(traits::GetTotalElements(arr5), 23 * 4 * 5 * 2 * 12);

  EXPECT_EQ(traits::GetExtent<0>(arr5), 12);
  EXPECT_EQ(traits::GetExtent<1>(arr5), 2);
  EXPECT_EQ(traits::GetExtent<2>(arr5), 5);
  EXPECT_EQ(traits::GetExtent<3>(arr5), 4);
  EXPECT_EQ(traits::GetExtent<4>(arr5), 23);
}

TEST(ArrayTraits, DynamicLarge) {
  using arr1d_t = Array<int, 23, strategy::CountedCreation>;
  auto arr1 = arr1d_t::create();

  using arr2d_t = Array<arr1d_t, 4>;
  auto arr2 = arr2d_t::create(arr1, arr1, arr1, arr1);

  using arr3d_t = Array<arr2d_t, kDynamicExtent>;
  auto arr3 = arr3d_t::create();

  using arr4d_t = Array<arr3d_t, 2, strategy::DefaultCreation>;
  auto arr4 = arr4d_t::create(arr3, arr3);

  using arr5d_t = Array<arr4d_t, 12>;
  auto arr5 = arr5d_t::create(arr4, arr4, arr4, arr4, arr4, arr4, arr4, arr4, arr4, arr4, arr4, arr4);

  EXPECT_EQ(traits::GetRank(arr1), 1);
  EXPECT_EQ(traits::GetRank(arr2), 2);
  EXPECT_EQ(traits::GetRank(arr3), 3);
  EXPECT_EQ(traits::GetRank(arr4), 4);
  EXPECT_EQ(traits::GetRank(arr5), 5);

  EXPECT_EQ(traits::GetTotalElements(arr1), 23);
  EXPECT_EQ(traits::GetTotalElements(arr2), 23 * 4);
  EXPECT_EQ(traits::GetTotalElements(arr3), kDynamicExtent);
  EXPECT_EQ(traits::GetTotalElements(arr4), kDynamicExtent);
  EXPECT_EQ(traits::GetTotalElements(arr5), kDynamicExtent);

  EXPECT_EQ(traits::GetExtent<0>(arr5), 12);
  EXPECT_EQ(traits::GetExtent<1>(arr5), 2);
  EXPECT_EQ(traits::GetExtent<2>(arr5), kDynamicExtent);
  EXPECT_EQ(traits::GetExtent<3>(arr5), 4);
  EXPECT_EQ(traits::GetExtent<4>(arr5), 23);
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
