#include <gtest/gtest.h>

#include "unordered_map.hpp"

#include <vector>
#include <string>
#include <iterator>
#include <cassert>

#include <iostream>

TEST(UNORDERED_MAP, Simple) {
    UnorderedMap<std::string, int> m;

    m["aaaaa"] = 5;
    m["bbb"] = 6;
    
    m.at("bbb") = 7;
    EXPECT_EQ(m.size(), 2);

    
    EXPECT_EQ(m["aaaaa"], 5);
    EXPECT_EQ(m["bbb"], 7);
    EXPECT_EQ(m["ccc"], 0);

    EXPECT_EQ(m.size(), 3);

    EXPECT_ANY_THROW(m.at("xxxxxxxx"));

    auto it = m.find("dddd");
    EXPECT_EQ(it, m.end());

    it = m.find("bbb");
    EXPECT_EQ(it->second, 7);
    
    ++it->second;
    EXPECT_EQ(it->second, 8);

    for (auto& item : m) {
        --item.second;
    }
    EXPECT_EQ(m.at("aaaaa"), 4);

    {
        auto mm = m;
        m = std::move(mm);
    }

    auto res = m.emplace("abcde", 2);
    EXPECT_TRUE(res.second);
}

TEST(UNORDERED_MAP, Iterators) {
    UnorderedMap<double, std::string> m;

    std::vector<double> keys = {0.4, 0.3, -8.32, 7.5, 10.0, 0.0};
    std::vector<std::string> values = {
        "Summer has come and passed",
        "The innocent can never last",
        "Wake me up when September ends",
        "Like my fathers come to pass",
        "Seven years has gone so fast",
        "Wake me up when September ends",
    };

    m.reserve(1'000'000);

    for (int i = 0; i < 6; ++i) {
        m.insert({keys[i], values[i]});
    }

    auto beg = m.cbegin();
    std::string s = beg->second;
    auto it = m.begin();
    ++it;
    m.erase(it++);
    it = m.begin();
    m.erase(++it);

    EXPECT_TRUE(beg->second == s);
    EXPECT_EQ(m.size(), 4);

    UnorderedMap<double, std::string> mm;
    std::vector<std::pair<const double, std::string>> elements = {
        {3.0, values[0]},
        {5.0, values[1]},
        {-10.0, values[2]},
        {35.7, values[3]}
    };
    mm.insert(elements.cbegin(), elements.cend());
    s = mm.begin()->second;

    m.insert(mm.begin(), mm.end());
    EXPECT_EQ(mm.size(), 4);
    EXPECT_TRUE(mm.begin()->second == s);


    // Test traverse efficiency
    m.reserve(1'000'000); // once again, nothing really should happen
    EXPECT_EQ(m.size(), 8);
    // Actions below must be quick (~ 1000 * 8 operations) despite reserving space for 1M elements
    for (int i = 0; i < 10000; ++i) {
        long long h = 0;
        for (auto it = m.cbegin(); it != m.cend(); ++it) {
            // just some senseless action
            h += int(it->first) + int((it->second)[0]);
        }
        std::ignore = h;
    }

    it = m.begin();
    ++it;
    s = it->second;
    // I asked to reserve space for 1M elements so actions below adding 100'000 elements mustn't cause reallocation
    for (double d = 100.0; d < 10100.0; d += 0.1) {
        m.emplace(d, "a");
    }
    // And my iterator must point to the same object as before
    EXPECT_TRUE(it->second == s);

    auto dist = std::distance(it, m.end());
    auto sz = m.size();
    m.erase(it, m.end());
    EXPECT_EQ(sz - dist, m.size());

    // Must be also fast
    for (double d = 200.0; d < 10200.0; d += 0.35) {
        auto it = m.find(d);
        if (it != m.end()) m.erase(it);
    }
}

// Just a simple SFINAE trick to check CE presence when it's necessary
// Stay tuned, we'll discuss this kind of tricks in our next lectures ;)
template<typename T>
decltype(UnorderedMap<T, T>().cbegin()->second = 0, int()) TestConstIteratorDoesntAllowModification(T) {
    assert(false);
}
template<typename... FakeArgs>
void TestConstIteratorDoesntAllowModification(FakeArgs...) {}


struct VerySpecialType {
    int x = 0;
    explicit VerySpecialType(int x): x(x) {}
    VerySpecialType(const VerySpecialType&) = delete;
    VerySpecialType& operator=(const VerySpecialType&) = delete;

    VerySpecialType(VerySpecialType&&) = default;
    VerySpecialType& operator=(VerySpecialType&&) = default;
};

struct NeitherDefaultNorCopyConstructible {
    VerySpecialType x;

    NeitherDefaultNorCopyConstructible() = delete;
    NeitherDefaultNorCopyConstructible(const NeitherDefaultNorCopyConstructible&) = delete;
    NeitherDefaultNorCopyConstructible& operator=(const NeitherDefaultNorCopyConstructible&) = delete;

    NeitherDefaultNorCopyConstructible(VerySpecialType&& x): x(std::move(x)) {}
    NeitherDefaultNorCopyConstructible(NeitherDefaultNorCopyConstructible&&) = default;
    NeitherDefaultNorCopyConstructible& operator=(NeitherDefaultNorCopyConstructible&&) = default;

    bool operator==(const NeitherDefaultNorCopyConstructible& other) const {
        return x.x == other.x.x;
    }
};

namespace std {
    template<>
    struct hash<NeitherDefaultNorCopyConstructible> {
        size_t operator()(const NeitherDefaultNorCopyConstructible& x) const {
            return std::hash<int>()(x.x.x);
        }
    };
}

TEST(UNORDERED_MAP, NoRedudantCopies) {
// std::cerr << "Test no redundant copies started" << std::endl;
    UnorderedMap<NeitherDefaultNorCopyConstructible, NeitherDefaultNorCopyConstructible> m;
// std::cerr << "m created" << std::endl;
    m.reserve(10);
// std::cerr << "m.reserve(10) done" << std::endl;
    m.emplace(VerySpecialType(0), VerySpecialType(0));
// std::cerr << "m.emplace(VerySpecialType(0), VerySpecialType(0)) done" << std::endl;
    m.reserve(1'000'000);
// std::cerr << "m.reserve(1000000) done" << std::endl;
    std::pair<NeitherDefaultNorCopyConstructible, NeitherDefaultNorCopyConstructible> p{VerySpecialType(1), VerySpecialType(1)};

    m.insert(std::move(p));
// std::cerr << "m.insert(std::move(p)) done" << std::endl;

    EXPECT_EQ(m.size(), 2);

    // this shouldn't compile
    // m[VerySpecialType(0)] = VerySpecialType(1);

    // but this should
    m.at(VerySpecialType(1)) = VerySpecialType(0);
// std::cerr << "m.at(VerySpecialType(1)) = VerySpecialType(0) done" << std::endl;

    {
        auto mm = std::move(m);
// std::cerr << "m moved to mm" << std::endl;
        m = std::move(mm);
// std::cerr << "mm moved to m" << std::endl;
    }
// std::cerr << "the scope of mm has finished" << std::endl;
    m.erase(m.begin());
// std::cerr << "m.erase(m.begin()) done" << std::endl;
    m.erase(m.begin());
// std::cerr << "m.erase(m.begin()) done once again" << std::endl;
    EXPECT_EQ(m.size(), 0);
}


template<typename T>
struct MyHash {
    size_t operator()(const T& p) const {
        return std::hash<int>()(p.second / p.first);
    }
};

template<typename T>
struct MyEqual {
    bool operator()(const T& x, const T& y) const {
        return y.second / y.first == x.second / x.first;
    }
};

struct OneMoreStrangeStruct {
    int first;
    int second;
};

bool operator==(const OneMoreStrangeStruct&, const OneMoreStrangeStruct&) = delete;

TEST(UNORDERED_MAP, CustomHashAndCompare) {
    UnorderedMap<std::pair<int, int>, char, MyHash<std::pair<int, int>>,
            MyEqual<std::pair<int, int>>> m;

    m.insert({{1, 2}, 0});
    m.insert({{2, 4}, 1});
    assert(m.size() == 1);

    m[{3, 6}] = 3;
    EXPECT_EQ(m.at({4, 8}), 3);

    UnorderedMap<OneMoreStrangeStruct, int, MyHash<OneMoreStrangeStruct>, MyEqual<OneMoreStrangeStruct>> mm;
    mm[{1, 2}] = 3;
    EXPECT_EQ(mm.at({5, 10}), 3);

    mm.emplace(OneMoreStrangeStruct{3, 9}, 2);
    EXPECT_EQ(mm.size(), 2);
    mm.reserve(1'000);
    mm.erase(mm.begin());
    mm.erase(mm.begin());
    for (int k = 1; k < 100; ++k) {
        for (int i = 1; i < 10; ++i) {
            mm.insert({{i, k*i}, 0});
        }
    }
    std::string ans;
    std::string myans;
    for (auto it = mm.cbegin(); it != mm.cend(); ++it) {
        ans += std::to_string(it->second);
        myans += '0';
    }
    EXPECT_EQ(ans, myans);
}


// Finally, some tricky fixtures to test custom allocator.
// Done by professional, don't try to repeat
class Chaste {
private:
    int x = 0;
    Chaste() = default;
    Chaste(int x): x(x) {}

    // Nobody can construct me except this guy
    template<typename T>
    friend struct TheChosenOne;

public:
    Chaste(const Chaste&) = default;
    Chaste(Chaste&&) = default;

    bool operator==(const Chaste& other) const {
        return x == other.x;
    }
};

namespace std {
    template<>
    struct hash<Chaste> {
        size_t operator()(const Chaste& x) const noexcept {
            return std::hash<int>()(reinterpret_cast<const int&>(x));
        }
    };
}

template<typename T>
struct TheChosenOne: public std::allocator<T> {
    TheChosenOne() {}

    template<typename U>
    TheChosenOne(const TheChosenOne<U>&) {}

    template<typename... Args>
    void construct(T* p, Args&&... args) const {
        new(p) T(std::forward<Args>(args)...);
    }

    void construct(std::pair<const Chaste, Chaste>* p, int a, int b) const {
        new(p) std::pair<const Chaste, Chaste>(Chaste(a), Chaste(b));
    }

    void destroy(T* p) const {
        p->~T();
    }

    template<typename U>
    struct rebind {
        using other = TheChosenOne<U>;
    };
};

/*
template<>
struct TheChosenOne<std::pair<const Chaste, Chaste>>
        : public std::allocator<std::pair<const Chaste, Chaste>> {
    using PairType = std::pair<const Chaste, Chaste>;

    TheChosenOne() {}

    template<typename U>
    TheChosenOne(const TheChosenOne<U>&) {}

    void construct(PairType* p, int a, int b) const {
        new(p) PairType(Chaste(a), Chaste(b));
    }

    void destroy(PairType* p) const {
        p->~PairType();
    }

    template<typename U>
    struct rebind {
        using other = TheChosenOne<U>;
    };
};
*/

TEST(UNORDERED_MAP, CustomAlloc) {
    // This container mustn't construct or destroy any objects without using TheChosenOne allocator
    UnorderedMap<Chaste, Chaste, std::hash<Chaste>, std::equal_to<Chaste>,
        TheChosenOne<std::pair<const Chaste, Chaste>>> m;

    m.emplace(0, 0);

    {
        auto mm = m;
        mm.reserve(1'000);
        mm.erase(mm.begin());
    }

    for (int i = 0; i < 1'000'000; ++i) {
        m.emplace(i, i);
    }

    for (int i = 0; i < 500'000; ++i) {
        auto it = m.begin();
        ++it, ++it;
        m.erase(m.begin(), it);
    }
    EXPECT_TRUE(true);
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

