#include "big_integer.hpp"

BigInt::BigInt(std::string str) {
  sign_ = Plus;
  size_ = str.size();
  value_ = {};
  int ind_start = 0;
  if (str[0] == '-') {
    sign_ = Minus;
    ind_start++;
  }
  for (int64_t i = 0;
       i < ((size_ - ind_start) / kDim) + int((size_ - ind_start) % kDim != 0);
       ++i) {
    if ((size_ - kDim * (i + 1)) < 0) {
      value_.push_back(
          std::stoi(str.substr(ind_start, (size_ - ind_start) % kDim)));
      break;
    }
    value_.push_back(std::stoi(str.substr(size_ - kDim * (i + 1), kDim)));
  }
  if (sign_ == Minus) {
    size_--;
  }
  count_digit_ = size_ / kDim + int(size_ % kDim != 0);
  if (size_ == 1 and value_[0] == 0) {
    sign_ = Plus;
  }
}

std::ostream& operator<<(std::ostream& os, const BigInt& integer) {
  os << integer.ToString();
  return os;
}

std::istream& operator>>(std::istream& in, BigInt& integer) {
  std::string number_string;
  in >> number_string;
  integer = BigInt(number_string);
  return in;
}

bool operator<(const BigInt& first, const BigInt& second) {
  if (first.sign_ != second.sign_) {
    return first.sign_ < second.sign_;
  }
  bool is_first_positive = first.sign_ > 0;
  if (first.count_digit_ != second.count_digit_) {
    return is_first_positive ? (first.count_digit_ < second.count_digit_)
                             : (first.count_digit_ > second.count_digit_);
  }
  for (int i = first.count_digit_ - 1; i >= 0; --i) {
    if (first.value_[i] < second.value_[i]) {
      return is_first_positive;
    }
    if (first.value_[i] > second.value_[i]) {
      return !is_first_positive;
    }
  }
  return false;
}

bool operator>(const BigInt& first, const BigInt& second) {
  return second < first;
}

bool operator>=(const BigInt& first, const BigInt& second) {
  return !(first < second);
}

bool operator<=(const BigInt& first, const BigInt& second) {
  return !(first > second);
}

BigInt operator+(const BigInt& first, const BigInt& second) {
  BigInt result = first;
  result += second;
  return result;
}

BigInt operator-(const BigInt& first, const BigInt& second) {
  BigInt result = first;
  result -= second;
  return result;
}

BigInt operator*(const BigInt& first, const BigInt& second) {
  BigInt result = first;
  result *= second;
  return result;
}

BigInt operator/(const BigInt& first, const BigInt& second) {
  BigInt result = first;
  result /= second;
  return result;
}

BigInt operator%(const BigInt& first, const BigInt& second) {
  BigInt result = first;
  result %= second;
  return result;
}

BigInt::operator bool() const { return this->size_ > 0 || this->sign_ != 0; }

void BigInt::SimpleNull() {
  while (count_digit_ > 1 && value_.back() == 0) {
    --count_digit_;
    value_.pop_back();
  }

  if (count_digit_ == 1 && value_[0] == 0) {
    sign_ = Plus;
    size_ = 1;
  }
}

BigInt BigInt::Multiple(int64_t number) {
  BigInt result;
  int64_t remainder = 0;
  for (std::size_t i = 0; i < (std::size_t)count_digit_; ++i) {
    result.value_.push_back((value_[i] * number + remainder) % kBaseDigit);
    remainder = (value_[i] * number + remainder) / kBaseDigit;
  }
  if (remainder != 0) {
    result.value_.push_back(remainder);
  }
  result.count_digit_ = result.value_.size();
  result.size_ = (result.count_digit_ - 1) * kDim +
                 std::to_string(result.value_.back()).size();
  result.SimpleNull();
  return result;
}

BigInt& BigInt::operator%=(const BigInt& number) {
  (*this) -= (number * (*this / number));
  SimpleNull();

  return *this;
}

std::string BigInt::ToString() const {
  std::string result = ToStringWithoutSign();
  if (sign_ == Minus) {
    result = '-' + result;
  }
  return result;
}

BigInt BigInt::operator-() const {
  BigInt result = *this;
  result.SetAnotherSign();
  result.SimpleNull();
  return result;
}

BigInt& BigInt::operator++() {
  *this += BigInt(1);
  return *this;
}

BigInt& BigInt::operator--() {
  *this -= BigInt(1);
  return *this;
}

BigInt BigInt::operator--(int) {
  BigInt new_int(*this);
  *this -= BigInt(1);
  return new_int;
}

BigInt BigInt::operator++(int) {
  BigInt new_int(*this);
  *this += BigInt(1);
  return new_int;
}

void BigInt::DetermineSignsAndSizes(const BigInt& a,
                                    std::vector<int64_t>& smallest,
                                    std::vector<int64_t>& biggest) {
  if ((size_ == a.size_) &&
      (this->ToStringWithoutSign() < a.ToStringWithoutSign())) {
    sign_ = a.sign_;
    std::swap(smallest, biggest);
  }
  if (size_ < a.size_) {
    sign_ = a.sign_;
    std::swap(smallest, biggest);
  }
}

void BigInt::PerformSubtraction(std::vector<int64_t>& biggest,
                                const std::vector<int64_t>& smallest) {
  int64_t summary;
  int remainder = 0;

  int max_size = std::max(biggest.size(), smallest.size());
  for (int i = 0; i < max_size || remainder != 0; ++i) {
    if (i == static_cast<int>(biggest.size()) && remainder != 0) {
      biggest.push_back(1);
      break;
    }

    summary = remainder + biggest[i];
    remainder = 0;

    if (i < static_cast<int>(smallest.size())) {
      summary -= smallest[i];
    }

    if (summary < 0) {
      summary += kBaseDigit;
      remainder = -1;
    }

    biggest[i] = summary;
  }
}

void BigInt::CleanUp(const std::vector<int64_t>& biggest) {
  value_ = biggest;

  count_digit_ = value_.size();
  while (value_.back() == 0 && count_digit_ != 1) {
    value_.pop_back();
    count_digit_--;
  }
  size_ = (count_digit_ - 1) * kDim + std::to_string(value_.back()).size();
  SimpleNull();
}

BigInt& BigInt::HandleComparison(const BigInt& current) {
  std::vector<int64_t> biggest = value_;
  std::vector<int64_t> smallest = current.value_;

  DetermineSignsAndSizes(current, smallest, biggest);
  PerformSubtraction(biggest, smallest);
  CleanUp(biggest);

  return *this;
}

void BigInt::PrepareForAddition(std::size_t index) {
  if (index >= (std::size_t)count_digit_) {
    value_.push_back(0);
  }
}

void BigInt::UpdateCountDigits() {
  count_digit_ = value_.size();
  while (value_.back() == 0 && count_digit_ != 1) {
    value_.pop_back();
    count_digit_--;
  }
}

void BigInt::AdjustSize() {
  size_ = (count_digit_ - 1) * kDim + std::to_string(value_.back()).size();
}

BigInt& BigInt::operator+=(const BigInt& current) {
  if (sign_ * current.sign_ == -1) {
    return HandleComparison(current);
  }
  int64_t summary;
  int remainder = 0;
  std::size_t max_digits = std::max(count_digit_, current.count_digit_);
  for (std::size_t i = 0; i < max_digits || remainder != 0; ++i) {
    PrepareForAddition(i);
    summary = value_[i] + remainder;
    remainder = 0;
    if (i < (std::size_t)current.count_digit_) {
      summary += current.value_[i];
    }
    if (summary >= kBaseDigit) {
      summary -= kBaseDigit;
      remainder = 1;
    }
    value_[i] = summary;
  }
  UpdateCountDigits();
  AdjustSize();
  SimpleNull();
  return *this;
}

BigInt& BigInt::operator-=(const BigInt& second) { return *this += -second; }

BigInt& BigInt::operator*=(const BigInt& second) {
  Sign new_sign = static_cast<Sign>(sign_ * second.sign_);
  BigInt result = Multiple(second.value_[0]);
  sign_ = Plus;

  for (std::size_t i = 1; i < (std::size_t)second.count_digit_; ++i) {
    BigInt elem = Multiple(second.value_[i]);

    std::reverse(elem.value_.begin(), elem.value_.end());

    for (std::size_t j = 0; j < i; ++j) {
      elem.value_.push_back(0);
      elem.count_digit_++;
      elem.size_ += kDim;
    }

    std::reverse(elem.value_.begin(), elem.value_.end());
    result += elem;
  }

  result.SetSign(new_sign);
  result.count_digit_ = result.value_.size();
  result.size_ = (result.count_digit_ - 1) * kDim +
                 std::to_string(result.value_.back()).size();

  *this = result;
  SimpleNull();

  return *this;
}

int64_t BigInt::BinSearch(BigInt& current, BigInt& second_number) {
  int64_t start = -1;
  int64_t end = kBaseDigit;
  int64_t middle;
  while (start <= end) {
    middle = (start + end) / 2;
    BigInt elem = second_number * middle;
    if (elem < current) {
      if (current - elem < second_number) {
        break;
      }
      start = middle + 1;
    } else if (elem == current) {
      break;
    } else {
      end = middle - 1;
    }
  }
  return middle;
}

BigInt& BigInt::operator/=(const BigInt& second) {
  std::vector<int64_t> result;
  Sign new_sign = static_cast<Sign>(sign_ * second.sign_);
  sign_ = Plus;

  BigInt second_number = second;
  second_number.SetSign(Plus);
  if ((*this) < second_number) {
    *this = BigInt(0);
    return *this;
  }
  BigInt current(0);
  for (std::size_t i = 0; i < (std::size_t)count_digit_; ++i) {
    current =
        current * BigInt(kBaseDigit) + BigInt(value_[count_digit_ - 1 - i]);
    if (current < second_number) {
      if (current == 0) {
        result.insert(result.begin(), 0);
      }
      continue;
    }
    int64_t middle = BinSearch(current, second_number);
    result.insert(result.begin(), middle);
    current = current - (middle * second_number);
  }
  value_ = result;
  count_digit_ = result.size();
  size_ = kDim * (count_digit_ - 1) + std::to_string(value_.back()).size();
  sign_ = new_sign;
  return *this;
}

std::string BigInt::ToStringWithoutSign() const {
  std::string result;
  for (std::size_t i = 0; i < (std::size_t)count_digit_; ++i) {
    std::string el = std::to_string(value_[count_digit_ - 1 - i]);
    if (i != 0) {
      for (size_t j = 0; j < kDim - el.size(); ++j) {
        result += "0";
      }
    }
    result += el;
  }
  return result;
}