#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

class BigInt {
 public:
  enum Sign { Plus = 1, Minus = -1 };

  // constructors
  BigInt() = default;

  explicit BigInt(std::string str);

  BigInt(const int64_t kInteger) : BigInt(std::to_string(kInteger)) {}

  BigInt(const BigInt& integer) = default;

  BigInt& operator=(const BigInt& integer) = default;
  BigInt operator-() const;
  BigInt& operator+=(const BigInt&);
  BigInt& operator-=(const BigInt&);
  BigInt& operator*=(const BigInt&);
  BigInt& operator/=(const BigInt&);
  BigInt& operator%=(const BigInt&);
  BigInt& operator++();
  BigInt& operator--();

  BigInt operator++(int);
  BigInt operator--(int);
  explicit operator bool() const;

  friend BigInt operator*(const BigInt&, const BigInt&);
  friend BigInt operator/(const BigInt&, const BigInt&);
  friend BigInt operator%(const BigInt&, const BigInt&);
  friend BigInt operator+(const BigInt&, const BigInt&);
  friend BigInt operator-(const BigInt&, const BigInt&);
  bool operator==(const BigInt&) const = default;

  friend bool operator<(const BigInt&, const BigInt&);
  friend bool operator>(const BigInt&, const BigInt&);

  friend std::ostream& operator<<(std::ostream& os, const BigInt& integer);
  friend std::istream& operator>>(std::istream& in, BigInt& integer);

 private:
  Sign sign_ = Plus;
  int size_ = 0;
  int count_digit_ = 0;
  std::vector<int64_t> value_;

  static const int64_t kBaseDigit = 1'000'000'000;
  static const int kDim = 9;

  void SetAnotherSign() { sign_ = (sign_ == Plus) ? Minus : Plus; }
  void SetSign(Sign sign_new) { sign_ = sign_new; }
  void DetermineSignsAndSizes(const BigInt& a, std::vector<int64_t>&,
                              std::vector<int64_t>&);
  static void PerformSubtraction(std::vector<int64_t>&,
                                 const std::vector<int64_t>&);
  void CleanUp(const std::vector<int64_t>&);
  BigInt& HandleComparison(const BigInt&);
  int64_t GetNextQuotientDigit(BigInt&, const BigInt&);
  std::string ToString() const;
  std::string ToStringWithoutSign() const;
  void SimpleNull();
  void PrepareForAddition(std::size_t index);
  void UpdateCountDigits();
  static int64_t BinSearch(BigInt&, BigInt&);
  BigInt Multiple(int64_t number);
  void AdjustSize();
};
