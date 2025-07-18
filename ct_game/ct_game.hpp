#pragma once
#include <initializer_list>
#include <vector>

namespace ct {
inline constexpr std::size_t BOARD_DIM = 3;
inline constexpr std::size_t TILE_COUNT = BOARD_DIM * BOARD_DIM;
inline constexpr int DIRECTION_COUNT = 4;
inline constexpr int MAX_BOUND = 1000000000;

inline constexpr int row_shift[DIRECTION_COUNT] = {-1, 1, 0, 0};
inline constexpr int col_shift[DIRECTION_COUNT] = {0, 0, -1, 1};

constexpr int abs_diff(int x);
constexpr int compute_heuristic(const std::vector<int> &state);
constexpr bool is_board_valid(const std::vector<int> &state);
constexpr bool is_state_solvable(const std::vector<int> &state);
constexpr bool are_moves_opposite(int prev, int curr);
constexpr bool is_target_reached(int h);
constexpr int adjust_bound(int f, int limit);
constexpr bool is_move_valid(int r, int c);
constexpr int get_new_empty_pos(int empty_pos, int dir);
constexpr std::vector<int> apply_move(const std::vector<int> &state, int from,
                                      int to);
constexpr int explore_and_search(const std::vector<int> &state, int cost,
                                 int limit, int prev_move, int empty_pos);
constexpr int perform_search(std::vector<int> state, int cost, int limit,
                             int prev_move, int empty_pos);
constexpr int compute_solution_length_impl(const std::vector<int> &state);
} // namespace ct

constexpr int ct::abs_diff(int x) { return x < 0 ? -x : x; }

constexpr int ct::compute_heuristic(const std::vector<int> &state) {
  int total = 0;
  for (std::size_t i = 0; i < TILE_COUNT; ++i) {
    int tile = state[i];
    if (tile == 0)
      continue;
    int target = tile - 1;
    int cur_row = int(i) / BOARD_DIM;
    int cur_col = int(i) % BOARD_DIM;
    int tgt_row = target / BOARD_DIM;
    int tgt_col = target % BOARD_DIM;
    total += abs_diff(cur_row - tgt_row) + abs_diff(cur_col - tgt_col);
  }
  return total;
}

constexpr bool ct::is_board_valid(const std::vector<int> &state) {
  if (state.size() != TILE_COUNT)
    return false;
  bool seen[TILE_COUNT] = {};
  for (std::size_t i = 0; i < TILE_COUNT; ++i) {
    int v = state[i];
    if (v < 0 || v >= int(TILE_COUNT) || seen[v])
      return false;
    seen[v] = true;
  }
  return true;
}

constexpr bool ct::is_state_solvable(const std::vector<int> &state) {
  int inversions = 0;
  for (std::size_t i = 0; i < TILE_COUNT; ++i) {
    if (state[i] == 0)
      continue;
    for (std::size_t j = i + 1; j < TILE_COUNT; ++j) {
      if (state[j] == 0)
        continue;
      if (state[i] > state[j])
        ++inversions;
    }
  }
  return (inversions & 1) == 0;
}

constexpr bool ct::are_moves_opposite(int prev, int curr) {
  return ((prev == 0 && curr == 1) || (prev == 1 && curr == 0) ||
          (prev == 2 && curr == 3) || (prev == 3 && curr == 2));
}

constexpr bool ct::is_target_reached(int h) { return h == 0; }

constexpr int ct::adjust_bound(int f, int limit) { return f > limit ? f : -1; }

constexpr bool ct::is_move_valid(int r, int c) {
  return r >= 0 && r < int(BOARD_DIM) && c >= 0 && c < int(BOARD_DIM);
}

constexpr int ct::get_new_empty_pos(int empty_pos, int dir) {
  int row = empty_pos / int(BOARD_DIM);
  int col = empty_pos % int(BOARD_DIM);
  return (row + row_shift[dir]) * int(BOARD_DIM) + (col + col_shift[dir]);
}

constexpr std::vector<int> ct::apply_move(const std::vector<int> &state,
                                          int from, int to) {
  auto next = state;
  next[from] = next[to];
  next[to] = 0;
  return next;
}

constexpr int ct::explore_and_search(const std::vector<int> &state, int cost,
                                     int limit, int prev_move, int empty_pos) {
  int min_limit = MAX_BOUND;
  for (int d = 0; d < DIRECTION_COUNT; ++d) {
    if (prev_move != -1 && are_moves_opposite(prev_move, d))
      continue;
    int row = empty_pos / int(BOARD_DIM);
    int col = empty_pos % int(BOARD_DIM);
    int new_row = row + row_shift[d];
    int new_col = col + col_shift[d];
    if (!is_move_valid(new_row, new_col))
      continue;
    int new_pos = new_row * int(BOARD_DIM) + new_col;
    auto next = apply_move(state, empty_pos, new_pos);
    int t = perform_search(next, cost + 1, limit, d, new_pos);
    if (t <= limit)
      return t;
    if (t < min_limit)
      min_limit = t;
  }
  return min_limit;
}

constexpr int ct::perform_search(std::vector<int> state, int cost, int limit,
                                 int prev_move, int empty_pos) {
  int h = compute_heuristic(state);
  int f = cost + h;
  int pr = adjust_bound(f, limit);
  if (pr != -1)
    return pr;
  if (is_target_reached(h))
    return cost;
  return explore_and_search(state, cost, limit, prev_move, empty_pos);
}

constexpr int ct::compute_solution_length_impl(const std::vector<int> &state) {
  int limit = compute_heuristic(state);
  int empty_pos = 0;
  for (; empty_pos < int(TILE_COUNT); ++empty_pos) {
    if (state[empty_pos] == 0)
      break;
  }
  while (true) {
    int t = perform_search(state, 0, limit, -1, empty_pos);
    if (t <= limit)
      return t;
    if (t == MAX_BOUND)
      return -1;
    limit = t;
  }
}

constexpr int compute_solution_length(std::initializer_list<int> list) {
  if (list.size() != ct::TILE_COUNT)
    return -2;
  std::vector<int> state(list.begin(), list.end());
  if (!ct::is_board_valid(state))
    return -2;
  if (!ct::is_state_solvable(state))
    return -1;
  return ct::compute_solution_length_impl(state);
}

#define SOLVE(...) compute_solution_length({__VA_ARGS__})