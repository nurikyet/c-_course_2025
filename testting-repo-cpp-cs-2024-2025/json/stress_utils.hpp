#pragma once

#include <vector>
#include <optional>
#include <variant>
#include <string>
#include <map>
#include <tuple>

enum Status { Active, Inactive, Pending };
 
template <std::size_t N>
struct Level {
    std::vector<Level<N - 1>> nested1 = {{}, {}};
    std::optional<Level<N - 1>> nested2 = Level<N - 1>{};
    std::variant<Level<N - 1>, std::string> nested3 = Level<N - 1>{};
    std::map<std::string, Level<N - 1>> nested4 = {{"a", {}}, {"b", {}}};
    std::tuple<Level<N - 1>, int, double> nested5 = {{}, 0, 0};
    Level<N - 1> nested6;
    Level<N - 1> nested7;
    Level<N - 1> nested8;
    Level<N - 1> nested9;
    Level<N - 1> nested10;
};

template <>
struct Level<0> {
    std::optional<int> field1 = 10; // optional
    std::variant<int, std::string> field2 = "Hello"; // variant
    std::map<std::string, int> field3 = {{"1", 1}, {"2", 2}, {"3", 3}, {"4", 4}}; // map
    std::tuple<int, double, std::string> field4 {0, 1.1, "2"}; // tuple
    std::vector<std::string> field5 = {"123", "456", "789"}; // vector
    Status field6 = Active; // enum
    float field7 = 0;
    bool field8 = false;
    double field10 = 0.0;
};

template <std::size_t N>
struct ExtraLong {
  ExtraLong<N - 1> field = ExtraLong<N - 1>{};
};

template <>
struct ExtraLong<0ul> {
  Status fields = Active;
};

template <std::size_t N>
struct ExpWide {
  ExpWide<N - 1> field1 = {};
  ExpWide<N - 1> field2 = {};
};

template <>
struct ExpWide<0> {
  int field = 1;
};


// Уровень 4 (самый глубокий уровень)
struct Level4 {
    std::optional<int> field1 = 10; // optional
    std::variant<int, std::string> field2 = "Hello"; // variant
    std::map<std::string, int> field3 = {{"1", 1}, {"2", 2}, {"3", 3}, {"4", 4}}; // map
    std::tuple<int, double, std::string> field4 {0, 1.1, "2"}; // tuple
    std::vector<std::string> field5 = {"123", "456", "789"}; // vector
    Status field6 = Active; // enum
    float field7 = 0;
    bool field8 = false;
    double field10 = 0.0;
};

// Уровень 3
struct Level3 {
    std::vector<Level4> nested1 = {{}, {}, {}, {}, {}}; // vector
    std::optional<Level4> nested2; // optional
    std::variant<Level4, std::string> nested3 = Level4{}; // variant
    std::map<std::string, Level4> nested4 = {{"a", {}}, {"b", {}}, {"C", {}}}; // map
    std::tuple<Level4, int, double> nested5 = {{}, 1, 1}; // tuple
    Level4 nested6;
    Level4 nested7;
    Level4 nested8;
    Level4 nested9;
    Level4 nested10;
};

// Уровень 2
struct Level2 {
    std::optional<Level3> nested1 = Level3{}; // optional
    std::vector<Level3> nested2 = {{}, {}, {}}; // vector
    std::variant<Level3, int> nested3 = {}; // variant
    std::map<std::string, Level3> nested4{}; // map
    std::tuple<Level3, double, std::string> nested5 = {{}, 0, "a"}; // tuple
    Level3 nested6;
    Level3 nested7;
    Level3 nested8;
    Level3 nested9;
    Level3 nested10;
};

// Уровень 1 (верхний уровень)
struct Level1 {
    std::vector<Level2> nested1 = {{}, {}}; // vector
    std::optional<Level2> nested2 = Level2{}; // optional
    std::variant<Level2, std::string> nested3 = Level2{}; // variant
    std::map<std::string, Level2> nested4 = {{"a", {}}, {"b", {}}}; // map
    std::tuple<Level2, int, double> nested5 = {{}, 0, 0}; // tuple
    // Level2 nested6;
    // Level2 nested7;
    // Level2 nested8;
    // Level2 nested9;
    // Level2 nested10;
};
