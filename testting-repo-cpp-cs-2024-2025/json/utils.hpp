#pragma once

#include <string>
#include <fstream>
#include <string_view>
#include <algorithm>
#include <cstdlib>
#include <format>

#include "json.hpp"

template <typename T>
bool CompareWithFile(const T& value, const std::string& result_file_name) {
  {
    std::ofstream f("user.json", std::ios::out | std::ios::trunc);
    std::string json = ConvertToJson(value);
    f << json;
  }
  {
    int result = std::system("jq -S . user.json > user_sorted.json");
    if (result != 0) {
      return false;
    }
    result = std::system(std::format("jq -S . {} > answer_sorted.json", result_file_name).data());
    if (result != 0) {
      return false;
    }
  }
  {
    std::ifstream user_f("user_sorted.json");
    std::ifstream answer_f("answer_sorted.json");
    std::string user_s;
    std::string answer_s;
    user_f >> user_s;
    answer_f >> answer_s;
    return user_s == answer_s;
  }
}

inline bool SpacesToEnd(char a, char b) {
  if (std::isspace(a) && std::isspace(b))  {
    return false;
  }
  if (std::isspace(a)) {
    return false;
  }
  if (std::isspace(b)) {
    return true;
  }
  return false;
}

inline void Trim(std::string& str) {
  std::stable_sort(str.begin(), str.end(), SpacesToEnd);
  std::size_t space_length = std::distance(
      str.rbegin(),
      std::find_if(
        str.rbegin(), str.rend(),
        +[](char symb) -> bool { return !std::isspace(symb); }
      )
  );
  str.resize(str.size() - space_length);
}
