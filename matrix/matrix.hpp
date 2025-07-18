#pragma once

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <vector>

#define STATIC_ASSERT(condition) \
  typedef char STATIC_ASSERTION[(condition) ? 1 : -10]

template <size_t N, size_t M, typename T = int64_t>
class Matrix {
 public:
  Matrix() : data_(N, std::vector<T>(M, T())) {}

  Matrix(const std::vector<std::vector<T>>& vec) {
    if (vec.size() != N || (N > 0 && vec[0].size() != M)) {
      throw std::invalid_argument("size error");
    }
    data_ = vec;
  }

  Matrix(const T& elem) : data_(N, std::vector<T>(M, elem)) {}

  T& operator()(size_t line, size_t column) { return data_[line][column]; }

  const T& operator()(size_t line, size_t column) const {
    return data_[line][column];
  }

  Matrix<N, M, T> operator+(const Matrix<N, M, T>& other) const {
    Matrix<N, M, T> result;
    for (size_t i = 0; i < N; ++i) {
      for (size_t j = 0; j < M; ++j) {
        result(i, j) = data_[i][j] + other(i, j);
      }
    }
    return result;
  }

  Matrix<N, M, T> operator-(const Matrix<N, M, T>& other) const {
    Matrix<N, M, T> result;
    for (size_t i = 0; i < N; ++i) {
      for (size_t j = 0; j < M; ++j) {
        result(i, j) = data_[i][j] - other(i, j);
      }
    }
    return result;
  }

  Matrix<N, M, T>& operator+=(const Matrix<N, M, T>& other) {
    for (size_t i = 0; i < N; ++i) {
      for (size_t j = 0; j < M; ++j) {
        data_[i][j] += other(i, j);
      }
    }
    return *this;
  }

  Matrix<N, M, T>& operator-=(const Matrix<N, M, T>& other) {
    for (size_t i = 0; i < N; ++i) {
      for (size_t j = 0; j < M; ++j) {
        data_[i][j] -= other(i, j);
      }
    }
    return *this;
  }

  Matrix<N, M, T> operator*(const T kScalar) const {
    Matrix<N, M, T> result;
    for (size_t i = 0; i < N; ++i) {
      for (size_t j = 0; j < M; ++j) {
        result(i, j) = data_[i][j] * kScalar;
      }
    }
    return result;
  }

  template <size_t K>
  Matrix<N, K, T> operator*(const Matrix<M, K, T>& other) const {
    Matrix<N, K, T> result;
    for (size_t i = 0; i < N; ++i) {
      for (size_t j = 0; j < K; ++j) {
        for (size_t k = 0; k < M; ++k) {
          result(i, j) += data_[i][k] * other(k, j);
        }
      }
    }
    return result;
  }

  Matrix<M, N, T> Transposed() const {
    Matrix<M, N, T> result;
    for (size_t i = 0; i < N; ++i) {
      for (size_t j = 0; j < M; ++j) {
        result(j, i) = data_[i][j];
      }
    }
    return result;
  }

  T Trace() const {
    STATIC_ASSERT(N == M);

    T trace = 0;
    for (size_t i = 0; i < N; ++i) {
      trace += data_[i][i];
    }
    return trace;
  }

  bool operator==(const Matrix<N, M, T>& other) const = default;

 private:
  std::vector<std::vector<T>> data_;
};
