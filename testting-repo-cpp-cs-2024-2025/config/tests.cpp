#include <gtest/gtest.h>

#include <map>
#include <iostream>
#include <string>

#include "Configurator.hpp"
#include "Setting.hpp"



Setting<int> global_param_1("glob_param_1", 12, "ParamHelp1");
Setting<double> global_param_2("glob_param_2", 2.2);
Setting<std::string> global_param_3("glob_param_3", "default_value", "ParamHelp3");

TEST(ParamsWithDefaultValue, GetValue) {
  decltype(auto) config = Configurator<>::GetInstance();

  EXPECT_EQ(config.GetValue<int>("glob_param_1").value(), 12);
  EXPECT_EQ(config.GetValue<double>("glob_param_2").value(), 2.2);
  EXPECT_EQ(config.GetValue<std::string>("glob_param_3").value(), "default_value");

  EXPECT_THROW(config.GetValue<int>("glob_param_4"), UnknownParametr);
  EXPECT_THROW(config.GetValue<double>("glob_param_1"), BadConfigValueType);
}

TEST(ParamsWithDefaultValue, GetValueAsString) {
  decltype(auto) config = Configurator<>::GetInstance();

  EXPECT_EQ(config.GetValueAsString("glob_param_1"), "12");
  EXPECT_EQ(config.GetValueAsString("glob_param_3"), "default_value");

  EXPECT_THROW(config.GetValueAsString("glob_param_4"), UnknownParametr);
}

TEST(ParamsWithDefaultValue, SetValue) {
  decltype(auto) config = Configurator<>::GetInstance();

  auto old_val_1 = config.SetValue("glob_param_1", 2);
  auto old_val_2 = config.SetValue("glob_param_2", 1.1);

  EXPECT_EQ(config.GetValue<int>("glob_param_1").value(), 2);
  EXPECT_EQ(config.GetValue<double>("glob_param_2").value(), 1.1);
  EXPECT_EQ(config.GetValue<std::string>("glob_param_3").value(), "default_value");

  EXPECT_EQ(old_val_1.value(), 12);
  EXPECT_EQ(old_val_2.value(), 2.2);

  EXPECT_EQ(global_param_1.GetValue(), 2);
  EXPECT_EQ(global_param_2.GetValue(), 1.1);
}

TEST(ParamsWithDefaultValue, InitMap) {
  decltype(auto) config = Configurator<>::GetInstance();

  std::map<std::string, std::string> conf_values{
    {"glob_param_1", "100"},
    {"glob_param_3", "other_value"}
  };

  config.Init(conf_values);

  EXPECT_EQ(config.GetValue<int>("glob_param_1").value(), 100);
  EXPECT_EQ(config.GetValue<double>("glob_param_2").value(), 1.1);
  EXPECT_EQ(config.GetValue<std::string>("glob_param_3").value(), "other_value");

  EXPECT_EQ(global_param_1.GetValue(), 100);
  EXPECT_EQ(global_param_2.GetValue(), 1.1);
}

TEST(ParamsWithDefaultValue, Help) {
  decltype(auto) config = Configurator<>::GetInstance();

  auto help_map = config.GetHelp();
  EXPECT_EQ(help_map["glob_param_1"], "ParamHelp1");
  EXPECT_EQ(help_map["glob_param_2"], "");
  EXPECT_EQ(help_map["glob_param_3"], "ParamHelp3");

  EXPECT_EQ(config.GetHelp("glob_param_1"), "ParamHelp1");
}

TEST(ParamsWithDefaultValue, Drop) {
  decltype(auto) config = Configurator<>::GetInstance();

  config.Drop("glob_param_2");

  EXPECT_EQ(config.GetValue<int>("glob_param_1").value(), 100);
  EXPECT_FALSE(config.GetValue<double>("glob_param_2").has_value());
  EXPECT_EQ(config.GetValue<std::string>("glob_param_3").value(), "other_value");

  EXPECT_EQ(global_param_1.GetValue(), 100);
  EXPECT_FALSE(global_param_2.HasValue());
}

struct NoDefaultValuesTag {};

Setting<int, NoDefaultValuesTag> no_def_param_1("no_def_param_1", 10);
Setting<bool, NoDefaultValuesTag> no_def_param_2("no_def_param_2");
Setting<std::string, NoDefaultValuesTag> no_def_param_3("no_def_param_3");

TEST(ParamsWithoutDefaultValue, GetValue) {
  decltype(auto) config = Configurator<NoDefaultValuesTag>::GetInstance();

  EXPECT_TRUE(config.GetValue<int>("no_def_param_1").has_value());
  EXPECT_EQ(config.GetValue<int>("no_def_param_1").value(), 10);
  EXPECT_FALSE(config.GetValue<bool>("no_def_param_2").has_value());
  EXPECT_FALSE(config.GetValue<std::string>("no_def_param_3").has_value());

  EXPECT_THROW(config.GetValue<int>("no_def_param_4"), UnknownParametr);
  EXPECT_THROW(config.GetValue<double>("no_def_param_1"), BadConfigValueType);

  EXPECT_TRUE(no_def_param_1.HasValue());
  EXPECT_FALSE(no_def_param_2.HasValue());
  EXPECT_FALSE(no_def_param_3.HasValue());
}

TEST(ParamsWithoutDefaultValue, GetValueAsString) {
  decltype(auto) config = Configurator<NoDefaultValuesTag>::GetInstance();

  EXPECT_EQ(config.GetValueAsString("no_def_param_1"), "10");
  EXPECT_EQ(config.GetValueAsString("no_def_param_2"), "");
  EXPECT_EQ(config.GetValueAsString("no_def_param_3"), "");

  EXPECT_THROW(config.GetValueAsString("no_def_param_4"), UnknownParametr);
}

TEST(ParamsWithoutDefaultValue, SetValue) {
  decltype(auto) config = Configurator<NoDefaultValuesTag>::GetInstance();

  auto old = config.SetValue("no_def_param_2", false);

  EXPECT_FALSE(old.has_value());
  EXPECT_TRUE(config.GetValue<bool>("no_def_param_2").has_value());
  EXPECT_EQ(config.GetValue<bool>("no_def_param_2").value(), false);
  EXPECT_EQ(config.GetValueAsString("no_def_param_2"), "false");

  EXPECT_FALSE(config.GetValue<std::string>("no_def_param_3").has_value());

  config.Drop("no_def_param_2");
}

TEST(ParamsWithoutDefaultValue, Init) {
  decltype(auto) config = Configurator<NoDefaultValuesTag>::GetInstance();

  std::map<std::string, std::string> conf_values {
    {"no_def_param_1", "20"}, {"no_def_param_2", "true"}
  };

  config.Init(conf_values);

  EXPECT_EQ(config.GetValue<int>("no_def_param_1").value(), 20);
  EXPECT_EQ(config.GetValue<bool>("no_def_param_2").value(), true);
  EXPECT_EQ(config.GetValueAsString("no_def_param_2"), "true");
  EXPECT_FALSE(config.GetValue<std::string>("no_def_param_3").has_value());

  EXPECT_EQ(no_def_param_1.GetValue(), 20);
}

struct NoCopyConstructible {
  NoCopyConstructible() = default;
  NoCopyConstructible(int x) : value(x) {}
  
  NoCopyConstructible(const NoCopyConstructible&) = delete;
  NoCopyConstructible& operator=(const NoCopyConstructible&) = delete;

  NoCopyConstructible(NoCopyConstructible&&) = default;
  NoCopyConstructible& operator=(NoCopyConstructible&&) = default;

  friend bool operator==(const NoCopyConstructible&, const NoCopyConstructible&) = default;

  int value;
};

struct NoDefaultConstructible {
  NoDefaultConstructible() = delete;
  NoDefaultConstructible(int x) : value(x) {}

  NoDefaultConstructible(const NoDefaultConstructible&) = default;
  NoDefaultConstructible& operator=(const NoDefaultConstructible&) = default;

  NoDefaultConstructible(NoDefaultConstructible&&) = default;
  NoDefaultConstructible& operator=(NoDefaultConstructible&&) = default;

  friend bool operator==(const NoDefaultConstructible&, const NoDefaultConstructible&) = default;

  int value;
};

struct NoDefaultNorCopyConstructible {
  NoDefaultNorCopyConstructible() = delete;
  NoDefaultNorCopyConstructible(int x) : value(x) {}

  NoDefaultNorCopyConstructible(const NoDefaultNorCopyConstructible&) = delete;
  NoDefaultNorCopyConstructible& operator=(const NoDefaultNorCopyConstructible&) = delete;

  NoDefaultNorCopyConstructible(NoDefaultNorCopyConstructible&&) = default;
  NoDefaultNorCopyConstructible& operator=(NoDefaultNorCopyConstructible&&) = default;

  friend bool operator==(const NoDefaultNorCopyConstructible&, const NoDefaultNorCopyConstructible&) = default;

  int value;
};

struct CustomIO {
  CustomIO() = default;
  CustomIO(int x) : x(x) {}
  
  int x;

  friend std::ostream& operator<<(std::ostream& out, const CustomIO& custom) {
    out << custom.x << custom.x;
    return out;
  }

  friend std::istream& operator>>(std::istream& in, CustomIO& custom) {
    int x;
    in >> x;
    custom.x = x * 10;
    return in;
  }

  friend bool operator==(const CustomIO& lhs, const CustomIO& rhs) = default;
};

struct DifferentTypesTag {};

Setting<NoDefaultConstructible, DifferentTypesTag> non_def_1("non_def_with_default", NoDefaultConstructible(10));
Setting<NoDefaultConstructible, DifferentTypesTag> non_def_2("non_def_without_default");

Setting<NoCopyConstructible, DifferentTypesTag> non_copy_1("non_copy_with_default", NoCopyConstructible(20));
Setting<NoCopyConstructible, DifferentTypesTag> non_copy_2("non_copy_without_default");

Setting<NoDefaultNorCopyConstructible, DifferentTypesTag> non_both_1("non_both_with_default", NoDefaultNorCopyConstructible(30));
Setting<NoDefaultNorCopyConstructible, DifferentTypesTag> non_both_2("non_both_without_default");

Setting<CustomIO, DifferentTypesTag> custom_1("custom_1", CustomIO{10});
Setting<CustomIO, DifferentTypesTag> custom_2("custom_2");

TEST(DifferentTypes, GetValue) {
  decltype(auto) config = Configurator<DifferentTypesTag>::GetInstance();

  EXPECT_EQ(config.GetValue<NoDefaultConstructible>("non_def_with_default").value(), NoDefaultConstructible(10));
  EXPECT_FALSE(config.GetValue<NoDefaultConstructible>("non_def_without_default").has_value());

  EXPECT_EQ(config.GetValue<NoCopyConstructible>("non_copy_with_default").value(), NoCopyConstructible(20));
  EXPECT_FALSE(config.GetValue<NoCopyConstructible>("non_copy_without_default").has_value());

  EXPECT_EQ(config.GetValue<NoDefaultNorCopyConstructible>("non_both_with_default").value(), NoDefaultNorCopyConstructible(30));
  EXPECT_FALSE(config.GetValue<NoDefaultNorCopyConstructible>("non_both_without_default").has_value());
}

TEST(DifferentTypes, SetValue) {
  decltype(auto) config = Configurator<DifferentTypesTag>::GetInstance();

  auto old_non_def = config.SetValue("non_def_without_default", NoDefaultConstructible(100));
  auto old_non_copy = config.SetValue("non_copy_without_default", NoCopyConstructible(200));
  auto old_non_both = config.SetValue("non_both_without_default", NoDefaultNorCopyConstructible(300));

  EXPECT_FALSE(old_non_def.has_value());
  EXPECT_FALSE(old_non_copy.has_value());
  EXPECT_FALSE(old_non_both.has_value());

  EXPECT_EQ(non_def_2.GetValue(), NoDefaultConstructible(100));
  EXPECT_EQ(non_copy_2.GetValue(), NoCopyConstructible(200));
  EXPECT_EQ(non_both_2.GetValue(), NoDefaultNorCopyConstructible(300));

  config.Drop("non_both_without_default");

  EXPECT_FALSE(non_both_2.HasValue());

  NoCopyConstructible val(2000);
  config.SetValue("non_copy_without_default", std::move(val));
  EXPECT_EQ(config.GetValue<NoCopyConstructible>("non_copy_without_default").value(), 2000);

}

TEST(DifferentTypes, CustomIOCase) {
  decltype(auto) config = Configurator<DifferentTypesTag>::GetInstance();

  EXPECT_EQ(config.GetValueAsString("custom_1"), "1010");

  std::map<std::string, std::string> conf_param {
    {"custom_2", "12"}
  };

  config.Init(conf_param);

  EXPECT_EQ(config.GetValue<CustomIO>("custom_2").value(), 120);
  EXPECT_EQ(config.GetValueAsString("custom_2"), "120120");

  EXPECT_EQ(custom_1.GetValue(), 10);
  EXPECT_EQ(custom_2.GetValue(), 120);
}

struct CallbackTag {};

int callback_test_var = 0;

int data = 0;

struct DataHolder {
  int x = 10;
  int& y = data;
};

DataHolder holder;

Setting<int, CallbackTag> stateless("stateless", 10, "helper",
                                    [](int value){ callback_test_var += value; });
Setting<std::string, CallbackTag> statefull("statefull", "Default", "helper",
                                    [holder = holder](const std::string& s) mutable{
                                        holder.x += s.size();
                                        holder.y += s.size();
                                      });

TEST(Callback, Basic) {
  decltype(auto) config = Configurator<CallbackTag>::GetInstance();
  
  config.SetValue("stateless", 15);
  EXPECT_EQ(callback_test_var, 15);

  config.SetValue("statefull", std::string("Hello"));
  EXPECT_EQ(holder.x, 10);
  EXPECT_EQ(holder.y, 5);

  config.SetValue("statefull", std::string("Hello"));
  EXPECT_EQ(holder.x, 10);
  EXPECT_EQ(holder.y, 10);

  std::map<std::string, std::string> config_vals {
    {"stateless", "30"}, {"statefull", "Wooorld"}
  };

  config.Init(config_vals);

  EXPECT_EQ(callback_test_var, 45);
  EXPECT_EQ(holder.x, 10);
  EXPECT_EQ(holder.y, 17);
  EXPECT_EQ(data, 17);
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
