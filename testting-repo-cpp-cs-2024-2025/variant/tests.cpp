#include <gtest/gtest.h>

#include <iostream>
#include <vector>
#include <type_traits>
#include <cassert>

#include "variant.hpp"

TEST(Variant, Basic) {
    Variant<int, std::string, double> v = 5;

    static_assert(std::is_assignable_v<decltype(v), float>);
    static_assert(!std::is_assignable_v<decltype(v), std::vector<int>>);

    EXPECT_TRUE(get<int>(v) == 5);

    v = "abc";

    EXPECT_TRUE(get<std::string>(v) == "abc");

    v = "cde";
    EXPECT_TRUE(get<std::string>(v) == "cde");

    v = 5.0;
    EXPECT_TRUE(get<double>(v) == 5.0);

    static_assert(!std::is_assignable_v<Variant<float, int>, double>);

    const auto& cv = v;
    static_assert(!std::is_assignable_v<decltype(get<double>(cv)), double>);

    static_assert(std::is_rvalue_reference_v<decltype(get<double>(std::move(v)))>);
    static_assert(std::is_lvalue_reference_v<decltype(get<double>(v))>);
}

TEST(Variant, OverloadingSelection) { 
    Variant<int*, char*, std::vector<char>, const int*, bool> v = true;

    EXPECT_TRUE(holds_alternative<bool>(v));

    v = std::vector<char>();

    get<std::vector<char>>(v).push_back('x');
    get<std::vector<char>>(v).push_back('y');
    get<std::vector<char>>(v).push_back('z');

    EXPECT_TRUE(holds_alternative<std::vector<char>>(v));

    EXPECT_TRUE(get<std::vector<char>>(v).size() == 3);

    char c = 'a';
    v = &c;

    EXPECT_TRUE(holds_alternative<char*>(v));

    *get<char*>(v) = 'b';

    EXPECT_TRUE(*get<char*>(v) == 'b');

    EXPECT_ANY_THROW(get<int*>(v));

    const int x = 1;
    v = &x;

    static_assert(!std::is_assignable_v<decltype(*get<const int*>(v)), int>);
    static_assert(std::is_assignable_v<decltype(get<const int*>(v)), int*>);
    
      EXPECT_ANY_THROW(get<int*>(v));


    const int y = 2;
    get<const int*>(v) = &y;
    EXPECT_TRUE(*get<const int*>(v) == 2);

    EXPECT_TRUE(!holds_alternative<int*>(v));
    EXPECT_TRUE(holds_alternative<const int*>(v));

    int z = 3;
    
    get<const int*>(v) = &z;
    EXPECT_TRUE(!holds_alternative<int*>(v));
    EXPECT_TRUE(holds_alternative<const int*>(v));
    EXPECT_TRUE(*get<const int*>(v) == 3);

    v = &z;

    EXPECT_TRUE(holds_alternative<int*>(v));
    EXPECT_TRUE(!holds_alternative<const int*>(v));
   
    EXPECT_TRUE(*get<int*>(v) == 3);
    
    EXPECT_TRUE((!std::is_assignable_v<decltype(get<int*>(v)), const int*>));

    EXPECT_ANY_THROW(get<const int*>(v));
}


TEST(Variant, CopyMoveConstructorsAssignments) {
    Variant<std::string, char, std::vector<int>> v = "abcdefgh";

    auto vv = v;

    EXPECT_TRUE(get<std::string>(vv).size() == 8);
    EXPECT_TRUE(get<std::string>(v).size() == 8);

    {
        auto vvv = std::move(v);

        EXPECT_TRUE(get<std::string>(v).size() == 0);
        v.emplace<std::vector<int>>({1, 2, 3});

        EXPECT_TRUE(get<std::string>(vv).size() == 8);
    }

    v = std::move(vv);
    EXPECT_TRUE(get<std::string>(v).size() == 8);

    EXPECT_TRUE(get<std::string>(vv).size() == 0);

    vv = 'a';

    EXPECT_TRUE(holds_alternative<char>(vv));
    EXPECT_TRUE(holds_alternative<std::string>(v));

    get<0>(v).resize(3);
    get<0>(v)[0] = 'b';
    EXPECT_TRUE(get<std::string>(v) == "bbc");

    {
        Variant<int, const std::string> vvv = std::move(get<0>(v));

        std::vector<int> vec = {1, 2, 3, 4, 5};

        v = vec;
        EXPECT_TRUE(get<2>(v).size() == 5);
        EXPECT_TRUE(vec.size() == 5);

        vec[1] = 0;
        EXPECT_TRUE(get<std::vector<int>>(v)[1] == 2);

        vvv.emplace<int>(1);
        EXPECT_TRUE(holds_alternative<int>(vvv));
    }

}

TEST(Variant, WithConstType) {

    int& (*get_ptr)(Variant<int, double>&) = &get<int, int, double>;

    static_assert(!std::is_invocable_v<decltype(get_ptr), Variant<const int, double>>);

    const int& (*get_const_ptr)(Variant<const int, double>&) = &get<const int, const int, double>;

    static_assert(!std::is_invocable_v<decltype(get_const_ptr), Variant<int, double>>);

    const int& (Variant<const int, double>::*method_const_ptr)(const int&) = &Variant<const int, double>::emplace<const int, const int&>;

    static_assert(!std::is_invocable_v<decltype(method_const_ptr), Variant<int, double>&, const int&>);

    int& (Variant<int, double>::*method_ptr)(const int&) = &Variant<int, double>::emplace<int, const int&>;

    static_assert(!std::is_invocable_v<decltype(method_ptr), Variant<const int, double>&, const int&>);

    Variant<const int, /*int,*/ std::string, const std::string, double> v = 1;
    
    EXPECT_TRUE(holds_alternative<const int>(v));

    v.emplace<std::string>("abcde");

    get<1>(v).resize(1);

    EXPECT_TRUE(!holds_alternative<const std::string>(v));

    v.emplace<0>(5);

    EXPECT_TRUE(get<0>(v) == 5);

}

struct VerySpecialType {};

template<typename... Ts>
struct Overload : Ts... {
    using Ts::operator()...;
};
template<class... Ts> Overload(Ts...) -> Overload<Ts...>;

TEST(Variant, Visit) {
    std::vector<Variant<char, long, float, int, double, long long>>
               vecVariant = {5, '2', 5.4, 100ll, 2011l, 3.5f, 2017};

    auto visitor = Overload {
        [](char) { return 1; },
        [](int) { return 2; },
        [](unsigned int) { return 3; },
        [](long int) { return 4; },
        [](long long int) { return 5; },
        [](auto) { return 6; },
    };

    std::string result;
    for (auto v : vecVariant) {
        result += std::to_string(visit(visitor, v));
    }

    EXPECT_TRUE(result == "2165462");

    std::vector<Variant<std::vector<int>, double, std::string>>
        vecVariant2 = { 1.5, std::vector<int>{1, 2, 3, 4, 5}, "Hello "};


    result.clear();
    auto DisplayMe = Overload {
        [&result](std::vector<int>& myVec) { 
                for (auto v: myVec) result += std::to_string(v);
            },
        [&result](auto& arg) { 
            if constexpr (std::is_same_v<std::decay_t<decltype(arg)>, std::string>) {
                result += "string";
            } else if constexpr (std::is_same_v<std::decay_t<decltype(arg)>, double>) {
                result += "double";
            }
        },
    };

    for (auto v : vecVariant2) {
        visit(DisplayMe, v);
    }
    EXPECT_TRUE(result == "double12345string");

}

struct OneShot {
    OneShot() = default;
    OneShot(const OneShot&) = delete;
    OneShot(OneShot&&) = default;
    OneShot& operator=(const OneShot&) = delete;
    OneShot& operator=(OneShot&&) = default;

    int operator()(auto x) && {
        if constexpr (std::is_same_v<decltype(x), int>) {
            return x*x;
        } else {
            return 42;
        }
    }
};

TEST(Variant, VisitOrGetRvalue) {
    std::vector<Variant<int, std::string>> vec = {"abc", 7};
    
    std::string result;
    result += std::to_string(visit(OneShot(), vec[0]));
    result += std::to_string(visit(OneShot(), vec[1]));
    
    EXPECT_TRUE(result == "4249");

    auto& varString = vec[0];
    auto newString = get<std::string>(std::move(varString));

    EXPECT_TRUE(get<std::string>(varString).size() == 0);
    EXPECT_TRUE(newString.size() == 3);

    newString.clear();
    varString = "cde";
    
    newString = get<1>(std::move(varString));

    EXPECT_TRUE(get<1>(varString).size() == 0);
    EXPECT_TRUE(newString.size() == 3);
}

TEST(Variant, MultipleVisit) {
    // example taken from here https://www.cppstories.com/2018/09/visit-variants/
    struct LightItem { };
    struct HeavyItem { };
    struct FragileItem { };

    Variant<LightItem, HeavyItem> basicPackA;
    Variant<LightItem, HeavyItem, FragileItem> basicPackB;
    
    {
        auto visitor = Overload{
            [](LightItem&, LightItem&) { return "2 light items"; },
            [](LightItem&, HeavyItem&) { return "light & heavy items"; },
            [](HeavyItem&, LightItem&) { return "heavy & light items"; },
            [](HeavyItem&, HeavyItem&) { return "2 heavy items"; },
            [](auto, auto) { return "another"; }
        };

        std::string result = visit(visitor, basicPackA, basicPackB);
        EXPECT_TRUE(result == "2 light items");

        basicPackA = HeavyItem();

        result = visit(visitor, basicPackA, basicPackB);
        
        EXPECT_TRUE(result == "heavy & light items");

        basicPackB = FragileItem();
        result = visit(visitor, basicPackA, basicPackB);
        
        EXPECT_TRUE(result == "another");
    }

    {
        auto visitor = Overload{
            [](HeavyItem&, FragileItem&) { return "both lvalue"; },
            [](HeavyItem&&, FragileItem&) { return "rvalue and lvalue"; },
            [](const HeavyItem&, const FragileItem&) { return "both const"; },
            [](auto&&, auto&&) { return "another"; }
        };

        std::string result = visit(visitor, basicPackA, basicPackB);
        
        EXPECT_TRUE(result == "both lvalue");

        result = visit(visitor, std::move(basicPackA), basicPackB);
        EXPECT_TRUE(result == "rvalue and lvalue");

        result = visit(visitor, std::move(basicPackA), std::move(basicPackB));
        EXPECT_TRUE(result == "another");

        const auto& constPackA = basicPackA;
        const auto& constPackB = basicPackB;

        result = visit(visitor, constPackA, constPackB);
        EXPECT_TRUE(result == "both const");

        result = visit(visitor, basicPackA, constPackB);
        EXPECT_TRUE(result == "another");
    }    
}


int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
