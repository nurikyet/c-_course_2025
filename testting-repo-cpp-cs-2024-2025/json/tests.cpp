#include <gtest/gtest.h>
#include <fstream>
#include <functional>
#include <optional>
#include <string>
#include <cstdlib>
#include <format>
#include <cctype>
#include <algorithm>
#include "utils.hpp"
#include "json.hpp"


enum Language {
  RUS, ENG, ESP
};

TEST(SimpleTypes, Int) {
  auto converted = ConvertToJson(10);
  auto answer = std::string("10");
  EXPECT_EQ(converted, answer);
}

TEST(SimpleTypes, Double) {
  auto converted = ConvertToJson(2.24);
  auto answer = std::string("2.24");
  EXPECT_EQ(converted, answer);
}

TEST(SimpleTypes, Bool) {
  {
    auto converted = ConvertToJson(true);
    auto answer = std::string("true");
    EXPECT_EQ(converted, answer);
  }
  {
    auto converted = ConvertToJson(false);
    auto answer = std::string("false");
    EXPECT_EQ(converted, answer);
  }
}

TEST(SimpleTypes, String) {
  {
    auto converted = ConvertToJson(std::string("Hello World"));
    auto answer = std::string("\"Hello World\"");
    EXPECT_EQ(converted, answer);
  }
  {
    auto converted = ConvertToJson(std::string(""));
    auto answer = std::string("\"\"");
    EXPECT_EQ(converted, answer);
  }
}

TEST(SimpleTypes, Enum) {
  {
    auto converted = ConvertToJson(RUS);
    auto answer = std::string("\"RUS\"");
    EXPECT_EQ(converted, answer);
  }
  {
    auto converted = ConvertToJson(ENG);
    auto answer = std::string("\"ENG\"");
    EXPECT_EQ(converted, answer);
  }
}

TEST(ComplexTypes, Vector) {
  {
    auto converted = ConvertToJson(std::vector{1, 2, 3, 4, 5});
    auto answer = std::string("[1, 2, 3, 4, 5]");
    Trim(converted);
    Trim(answer);
    EXPECT_EQ(converted, answer);
  }
  {
    auto converted = ConvertToJson(std::vector<std::string>{"ABC", "DEF", "Hello"});
    auto answer = std::string("[\"ABC\", \"DEF\", \"Hello\"]");
    Trim(converted);
    Trim(answer);
    EXPECT_EQ(converted, answer);
  }
  {
    auto converted = ConvertToJson(std::vector<std::vector<int>>{{1, 2}, {3, 4}, {5}, {}, {1, 2, 3}});
    auto answer = std::string("[[1, 2], [3, 4], [5], [], [1, 2, 3]]");
    Trim(converted);
    Trim(answer);
    EXPECT_EQ(converted, answer);
  } 
}

TEST(ComplexTypes, Map) {
  {
    auto value = std::map<std::string, int>({
        {"A", 12}, {"B", 24}, {"C", 10}
    });
    EXPECT_TRUE(CompareWithFile(value, "testdata/ComplexTypesMapInt.json"));
  }
  {
    auto value = std::unordered_map<std::string, int>({
        {"A", 12}, {"B", 24}, {"C", 10}
    });
    EXPECT_TRUE(CompareWithFile(value, "testdata/ComplexTypesMapInt.json"));
  }
  {
    auto value = std::map<std::string, std::string>({
        {"Key", "Value"}, {"CringeKey", "CringeValue"}
    });
    EXPECT_TRUE(CompareWithFile(value, "testdata/ComplexTypesMapString.json"));
  }
  {
    auto value = std::unordered_map<std::string, std::string>({
        {"Key", "Value"}, {"CringeKey", "CringeValue"}
    });
    EXPECT_TRUE(CompareWithFile(value, "testdata/ComplexTypesMapString.json"));
  }
}

TEST(ComplexTypes, Optional) {
  {
    auto converted = ConvertToJson(std::optional<int>(12));
    auto answer = std::string("12");
    EXPECT_EQ(converted, answer);
  }
  {
    auto converted = ConvertToJson(std::optional<double>(2.2));
    auto answer = std::string("2.2");
    EXPECT_EQ(converted, answer);
  }
  {
    auto converted = ConvertToJson(std::optional<std::string>("Hello"));
    auto answer = std::string("\"Hello\"");
    EXPECT_EQ(converted, answer);
  }
  {
    auto converted = ConvertToJson(std::optional<int>());
    auto answer = std::string("");
    EXPECT_EQ(converted, answer);
  }
  {
    auto converted = ConvertToJson(std::optional<double>());
    auto answer = std::string("");
    EXPECT_EQ(converted, answer);
  }
  {
    auto converted = ConvertToJson(std::optional<std::string>());
    auto answer = std::string("");
    EXPECT_EQ(converted, answer);
  }
}

TEST(ComplexTypes, Variant) {
  using TestVariant = std::variant<int, double, std::string>;
  {
    auto converted = ConvertToJson(TestVariant(10));
    auto answer = std::string("10");
    EXPECT_EQ(converted, answer);
  }
  {
    auto converted = ConvertToJson(TestVariant(1.1));
    auto answer = std::string("1.1");
    EXPECT_EQ(converted, answer);
  }
  {
    auto converted = ConvertToJson(TestVariant("World"));
    auto answer = std::string("\"World\"");
    EXPECT_EQ(converted, answer);
  }
}

TEST(ComplexTypes, Tuple) {
  {
    auto converted = ConvertToJson(std::tuple(std::string("Abc"), 12, 20, 3.32));
    auto answer = std::string("[\"Abc\", 12, 20, 3.32]");
    Trim(converted);
    Trim(answer);
    EXPECT_EQ(converted, answer);
  }
  {
    auto converted = ConvertToJson(std::tuple(1, 2, 3, 4, 5));
    auto answer = std::string("[1, 2, 3, 4, 5]");
    Trim(converted);
    Trim(answer);
    EXPECT_EQ(converted, answer);
  }
  {
    auto converted = ConvertToJson(std::tuple());
    auto answer = std::string("[]");
    EXPECT_EQ(converted, answer);
  }
}

TEST(ComplexTypesComposition, VectorTuple) {
  {
    auto value = std::vector<std::tuple<int, double, std::string, bool>> {
      {0, 2.2, "Hello", true},
      {1, 3, "World", false}
    };
    auto converted = ConvertToJson(value);
    auto answer = std::string("[[0, 2.2, \"Hello\", true], [1, 3, \"World\", false]]");
    Trim(converted);
    Trim(answer);
    EXPECT_EQ(converted, answer);
  }
  {
    auto value = std::tuple<std::vector<int>, std::vector<std::string>, int>{
      {1, 2, 3},
      {"AB", "GCA", "std::pair<const Chaste, Chaste>"},
      12
    };
    auto converted = ConvertToJson(value);
    auto answer = std::string("[[1, 2, 3], [\"AB\", \"GCA\", \"std::pair<const Chaste, Chaste>\"], 12]");
    Trim(converted);
    Trim(answer);
    EXPECT_EQ(converted, answer);
  }
}

TEST(ComplexTypesComposition, VectorMap) {
  {
    auto value = std::vector<std::map<std::string, int>> {
      {{"KA", 10}, {"KB", 12}},
      {{"A", 0}, {"B", 1}, {"C", 2}, {"D", 3}},
      {}
    };
    EXPECT_TRUE(CompareWithFile(value, "testdata/ComplexTypesCompositionVectorMap1.json"));
  }
  {
    auto value = std::map<std::string, std::vector<int>> {
      {"KA", {1, 2, 3}},
      {"KB", {4, 5, 6, 7, 8}},
      {"KC", {}}
    };
    EXPECT_TRUE(CompareWithFile(value, "testdata/ComplexTypesCompositionVectorMap2.json"));
  }
}

TEST(ComplexTypesComposition, VectorOptional) {
  {
    auto value = std::vector<std::optional<int>>{1, 2, std::nullopt, 10, std::nullopt};
    auto converted = ConvertToJson(value);
    auto answer = std::string("[1, 2, 10]");
    Trim(converted);
    Trim(answer);
    EXPECT_EQ(converted, answer);
  }
  {
    auto value = std::optional<std::vector<int>>{{1, 2, 3}};
    auto converted = ConvertToJson(value);
    auto answer = std::string("[1, 2, 3]");
    Trim(converted);
    Trim(answer);
    EXPECT_EQ(converted, answer);
  }
}

TEST(ComplexTypesComposition, VectorVariant) {
  using VectorVariant = std::vector<std::variant<int, double, std::string, bool>>;
  using VariantVector = std::variant<std::vector<int>, int, VectorVariant, bool>;
  {
    auto value = VectorVariant{2.2, 10, true, "Hello"};
    auto converted = ConvertToJson(value);
    auto answer = std::string("[2.2, 10, true, \"Hello\"]");
    Trim(converted);
    Trim(answer);
    EXPECT_EQ(converted, answer);
  }
  {
    auto value = VariantVector{std::vector{1, 2, 3}};
    auto converted = ConvertToJson(value);
    auto answer = std::string("[1, 2, 3]"); 
    Trim(converted);
    Trim(answer);
    EXPECT_EQ(converted, answer);
  }
  {
    auto value = VariantVector{12};
    auto converted = ConvertToJson(value);
    auto answer = std::string("12"); 
    EXPECT_EQ(converted, answer);
  }
  {
    auto value = VariantVector{VectorVariant{2.2, 10, true, "Hello"}};
    auto converted = ConvertToJson(value);
    auto answer = std::string("[2.2, 10, true, \"Hello\"]");
    Trim(converted);
    Trim(answer);
    EXPECT_EQ(converted, answer);
  }
}

TEST(ComplexTypesComposition, MapTuple) {
  using MapTuple = std::map<std::string, std::tuple<int, bool, std::string>>;
  using TupleMap = std::tuple<std::map<std::string, int>, int, std::map<std::string, std::string>>;
  {
    auto value = MapTuple{
      {"KA", {12, false, "Hello"}},
      {"KB", {0, true, ""}},
      {"KC", {50, false, "World"}}
    };
    EXPECT_TRUE(CompareWithFile(value, "testdata/ComplexTypesCompositionMapTuple1.json"));
  }
  {
    auto value = TupleMap{
      { {"KA", 10}, {"KB", 12}, {"KC", 0} },
      1000,
      { {"KD", "100"}, {"KD", "ninety"} }
    };
    EXPECT_TRUE(CompareWithFile(value, "testdata/ComplexTypesCompositionMapTuple2.json"));
  }
}

TEST(ComplexTypesComposition, MapVariant) {
  using MapVariant = std::unordered_map<std::string, std::variant<int, bool, std::string>>;
  using VariantMap = std::variant<std::string, int, std::map<std::string, int>>;
  {
    auto value = MapVariant{
      {"KA", 10},
      {"KB", true},
      {"KC", "Value"}
    };
    EXPECT_TRUE(CompareWithFile(value, "testdata/ComplexTypesCompositionMapVariant1.json"));
  }
  {
    auto value = VariantMap{
      std::map<std::string, int>{{"KA", 0}, {"KB", 1}, {"KC", 2}}
    };
    EXPECT_TRUE(CompareWithFile(value, "testdata/ComplexTypesCompositionMapVariant2.json"));
  }
}

TEST(ComplexTypesComposition, MapOptional) {
  using MapOptional = std::map<std::string, std::optional<int>>;
  using OptionalMap = std::optional<std::map<std::string, bool>>;
  {
    auto value = MapOptional{
      {"K", std::nullopt},
      {"KA", 10},
      {"KB", std::nullopt},
      {"KC", std::nullopt},
      {"KD", 0},
      {"KE", std::nullopt}
    };
    EXPECT_TRUE(CompareWithFile(value, "testdata/ComplexTypesCompositionMapOptional1.json"));
  }
  {
    auto value = OptionalMap{{
      {"KA", true},
      {"KB", false},
      {"KC", false},
      {"KD", true}
    }};
    EXPECT_TRUE(CompareWithFile(value, "testdata/ComplexTypesCompositionMapOptional2.json"));
  }
}

TEST(ComplexTypesComposition, MapMap) {
  using MapMap = std::map<std::string, std::map<std::string, int>>;
  auto value = MapMap {
    {"M1", {
      {"K1", 10},
      {"K2", 20},
      {"K3", 30}
    }},
    {"M2", {
      {"K4", 40},
      {"K5", 50}      
    }}
  };
  EXPECT_TRUE(CompareWithFile(value, "testdata/ComplexTypesCompositionMapMap.json"));
}

TEST(ComplexTypesComposition, TupleVariant) {
  using TupleVariant = std::tuple<std::variant<int, bool, std::string>, int, std::variant<int, std::string>>;
  using VariantTuple = std::variant<std::tuple<int, bool, int>, std::string, int>;
  {
    auto value = TupleVariant{true, 12, "Hello"};
    auto converted = ConvertToJson(value);
    auto answer = std::string("[true, 12, \"Hello\"]");
    Trim(converted);
    Trim(answer);
    EXPECT_EQ(converted, answer);
  }
  {
    auto value = VariantTuple{std::tuple{10, true, 12}};
    auto converted = ConvertToJson(value);
    auto answer = std::string("[10, true, 12]");
    Trim(converted);
    Trim(answer);
    EXPECT_EQ(converted, answer);
  }
  {
    auto value = VariantTuple{"Hello"};
    auto converted = ConvertToJson(value);
    auto answer = std::string("\"Hello\"");
    EXPECT_EQ(converted, answer);
  }
}

TEST(ComplexTypesComposition, TupleOptional) {
  using TupleOpt = std::tuple<std::optional<int>, std::optional<int>, std::optional<std::string>>;
  {
    auto value = TupleOpt{10, 12, "Hello"};
    auto converted = ConvertToJson(value);
    auto answer = std::string("[10, 12, \"Hello\"]");
    Trim(converted);
    Trim(answer);
    EXPECT_EQ(converted, answer);
  }
  {
    auto value = TupleOpt{std::nullopt, 12, "Hello"};
    auto converted = ConvertToJson(value);
    auto answer = std::string("[12, \"Hello\"]");
    Trim(converted);
    Trim(answer);
    EXPECT_EQ(converted, answer);
  }
  {
    auto value = TupleOpt{10, 12, std::nullopt};
    auto converted = ConvertToJson(value);
    auto answer = std::string("[10, 12]");
    Trim(converted);
    Trim(answer);
    EXPECT_EQ(converted, answer);
  }
  {
    auto value = TupleOpt{std::nullopt, 12, std::nullopt};
    auto converted = ConvertToJson(value);
    auto answer = std::string("[12]");
    Trim(converted);
    Trim(answer);
    EXPECT_EQ(converted, answer);
  }
  {
    auto value = TupleOpt{10, std::nullopt, "Hello"};
    auto converted = ConvertToJson(value);
    auto answer = std::string("[10, \"Hello\"]");
    Trim(converted);
    Trim(answer);
    EXPECT_EQ(converted, answer);
  }
  {
    auto value = TupleOpt{std::nullopt, std::nullopt, std::nullopt};
    auto converted = ConvertToJson(value);
    auto answer = std::string("[]");
    Trim(converted);
    Trim(answer);
    EXPECT_EQ(converted, answer);
  }
}

TEST(ComplexTypesComposition, TupleTuple) {
  using TupTup = std::tuple<std::tuple<int, bool>, int, std::tuple<int, int>>;
  auto value = TupTup{{2, false}, 12, {1, 2}};
  auto converted = ConvertToJson(value);
  auto answer = std::string("[[2, false], 12, [1, 2]]");
  Trim(converted);
  Trim(answer);
  EXPECT_EQ(converted, answer);
}

TEST(ComplexTypesComposition, VariantOptional) {
  using VariantOptional = std::variant<std::optional<int>, bool>;
  {
    auto value = VariantOptional{true};
    auto converted = ConvertToJson(value);
    auto answer = std::string("true");
    EXPECT_EQ(converted, answer);
  }
  {
    auto value = VariantOptional{10};
    auto converted = ConvertToJson(value);
    auto answer = std::string("10");
    EXPECT_EQ(converted, answer);
  }
  {
    auto value = VariantOptional{std::nullopt};
    auto converted = ConvertToJson(value);
    auto answer = std::string("");
    EXPECT_EQ(converted, answer);
  }
}

TEST(ComplexTypesComposition, VariantVariant) {
  using VarVar = std::variant<std::variant<int, std::string>, bool>;
  {
    auto value = VarVar{true};
    auto converted = ConvertToJson(value);
    auto answer = std::string("true");
    EXPECT_EQ(converted, answer);
  }
  {
    auto value = VarVar{10};
    auto converted = ConvertToJson(value);
    auto answer = std::string("10");
    EXPECT_EQ(converted, answer);
  }
}

TEST(ComplexTypesComposition, OptionalOptional) {
  using Opt = std::optional<int>;
  using OptOpt = std::optional<Opt>;
  using OptOptOpt = std::optional<OptOpt>;
  {
    auto value = OptOptOpt(std::nullopt);
    auto converted = ConvertToJson(value);
    auto answer = std::string("");
    EXPECT_EQ(converted, answer);
  }
  {
    auto value = OptOptOpt(OptOpt(std::nullopt));
    auto converted = ConvertToJson(value);
    auto answer = std::string("");
    EXPECT_EQ(converted, answer);
  }
  {
    auto value = OptOptOpt(OptOpt(Opt(std::nullopt)));
    auto converted = ConvertToJson(value);
    auto answer = std::string("");
    EXPECT_EQ(converted, answer);
  }
  {
    auto value = OptOptOpt(OptOpt(Opt(10)));
    auto converted = ConvertToJson(value);
    auto answer = std::string("10");
    EXPECT_EQ(converted, answer);
  }
}

struct PODBasic {
  int i = 10;
  double d = 2.2;
  std::string z = "Hello";
  bool b = true;
  Language l = ENG;
};

TEST(POD, Basic) {
  auto value = PODBasic{};
  EXPECT_TRUE(CompareWithFile(value, "testdata/PODBasic.json"));
}

struct PODComplex {
  std::vector<int> v = {1, 2, 3};
  std::optional<int> o1 = 10;
  std::optional<int> o2 = std::nullopt;
  std::map<std::string, int> m {
    {"KA", 0}, {"KB", 1}
  };
  std::tuple<int, bool, std::string> t = {10, true, "Hello"};
  std::variant<int, bool> var = 10;
};

TEST(POD, Complex) {
  auto value = PODComplex{};
  EXPECT_TRUE(CompareWithFile(value, "testdata/PODComplex.json"));
}

struct PODLeaf {
  int x = 10;
  int y = 20;
};

struct PODCompound {
  int x = 10;
  PODLeaf l1 = {1, 2};
  int y = 20;
  PODLeaf l2 = {2, 3};
};

TEST(POD, Compound) {
  auto value = PODCompound{};
  EXPECT_TRUE(CompareWithFile(value, "testdata/PODCompound.json"));
}

struct PODOptional {
  std::optional<int> f1;
  std::optional<int> f2;
  std::optional<int> f3;
};

TEST(POD, Optional) {
  {
    auto value = PODOptional{1, 2, 3};
    EXPECT_TRUE(CompareWithFile(value, "testdata/PODOptional1.json"));
  }
  {
    auto value = PODOptional{std::nullopt, 2, 3};
    EXPECT_TRUE(CompareWithFile(value, "testdata/PODOptional2.json"));
  }
  {
    auto value = PODOptional{1, 2, std::nullopt};
    EXPECT_TRUE(CompareWithFile(value, "testdata/PODOptional3.json"));
  }
  {
    auto value = PODOptional{std::nullopt, 2, std::nullopt};
    EXPECT_TRUE(CompareWithFile(value, "testdata/PODOptional4.json"));
  }
  {
    auto value = PODOptional{1, std::nullopt, 3};
    EXPECT_TRUE(CompareWithFile(value, "testdata/PODOptional5.json"));
  }
  {
    auto value = PODOptional{std::nullopt, std::nullopt, std::nullopt};
    EXPECT_TRUE(CompareWithFile(value, "testdata/PODOptional6.json"));
  }
}

struct Student {
  std::string name;
  std::vector<size_t> marks;
};

struct ContextA {
  std::string name = "ContextA";
};

struct ContextB {
  std::string name = "ContextB";
};

struct ContextC {
  std::string name = "ContextC";
};

struct MyBigStruct {
  int id = 0;
  double weight = 3.4;
  Language language = RUS;
  std::string name = "Artyom";
  bool is_man = true;

  std::vector<Student> students = {
      Student{"Kamilla", {8, 8, 10}},
      Student{"Stepan", {10, 10, 9}},
      Student{"Sunat", {7, 7, 5}}
  };

  std::tuple<ContextA, ContextB, ContextC> contexts;
  std::variant<ContextA, ContextB, ContextC> my_context = ContextB{};

  std::optional<std::string> my_phone = "+7926...";
  std::optional<std::string> other_stuff = std::nullopt; // will not appear in json

  std::map<std::string, Student> rev_map = {
      {"Kamilla", students[0]},
      {"Stepan", students[1]},
      {"Sunat", students[2]}
  };
};

TEST(POD, ComplexCompound) {
  auto value = MyBigStruct{};
  EXPECT_TRUE(CompareWithFile(value, "testdata/PODComplexCompound.json"));
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
