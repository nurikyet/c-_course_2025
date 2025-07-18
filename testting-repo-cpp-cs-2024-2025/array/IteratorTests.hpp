#pragma once

#include <type_traits>
#include <iterator>

template <typename T>
std::add_lvalue_reference_t<T> declref();

template <typename Iterator, typename T>
void IteratorTest() {
  using traits = std::iterator_traits<Iterator>;

  {
    auto test = std::is_same_v<std::remove_cv_t<typename traits::value_type>,
                               std::remove_cv_t<T>>;
    EXPECT_TRUE(test);
  }

  {
    auto test = std::is_same_v<typename traits::pointer, T*>;
    EXPECT_TRUE(test);
  }

  {
    auto test = std::is_same_v<typename traits::reference, T&>;
    EXPECT_TRUE(test);
  }

  {
    auto test = std::is_same_v<typename traits::iterator_category,
                               std::random_access_iterator_tag>;
    EXPECT_TRUE(test);
  }

  {
    auto test = std::is_same_v<decltype(declref<Iterator>()++), Iterator>;
    EXPECT_TRUE(test);
  }

  {
    auto test = std::is_same_v<decltype(++declref<Iterator>()), Iterator&>;
    EXPECT_TRUE(test);
  }

  {
    auto
        test = std::is_same_v<decltype(declref<Iterator>() + 2), Iterator>;
    EXPECT_TRUE(test);
  }

  {
    auto test =
        std::is_same_v<decltype(declref<Iterator>() += 2), Iterator&>;
    EXPECT_TRUE(test);
  }

  {
    auto test = std::is_same_v<decltype(declref<Iterator>()
        - declref<Iterator>()), typename traits::difference_type>;
    EXPECT_TRUE(test);
  }

  {
    auto test = std::is_same_v<decltype(*declref<Iterator>()), T&>;
    EXPECT_TRUE(test);
  }

  {
    auto test = std::is_same_v<decltype(declref<Iterator>()
        < declref<Iterator>()), bool>;
    EXPECT_TRUE(test);
  }

  {
    auto test = std::is_same_v<decltype(declref<Iterator>()
        <= declref<Iterator>()), bool>;
    EXPECT_TRUE(test);
  }

  {
    auto test = std::is_same_v<decltype(declref<Iterator>()
        > declref<Iterator>()), bool>;
    EXPECT_TRUE(test);
  }

  {
    auto test = std::is_same_v<decltype(declref<Iterator>()
        >= declref<Iterator>()), bool>;
    EXPECT_TRUE(test);
  }

  {
    auto test = std::is_same_v<decltype(declref<Iterator>()
        == declref<Iterator>()), bool>;
    EXPECT_TRUE(test);
  }

  {
    auto test = std::is_same_v<decltype(declref<Iterator>()
        != declref<Iterator>()), bool>;
    EXPECT_TRUE(test);
  }
}
