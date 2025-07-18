#pragma once
#include <iostream>

template <typename T, typename Allocator = std::allocator<T>>
class List {
 private:
  struct BaseNode;
  struct Node;

 public:
  template <bool IsConst>
  struct BaseIterator;
  using iterator = BaseIterator<false>;
  using const_iterator = BaseIterator<true>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;
  using allocator_type = Allocator;
  using alloc_traits = std::allocator_traits<allocator_type>;
  using node_allocator = typename alloc_traits::template rebind_alloc<Node>;
  using node_alloc_traits = std::allocator_traits<node_allocator>;
  using value_type = T;

  List();
  List(size_t /*n*/);
  List(size_t /*n*/, const T& /*value*/);
  List(const Allocator& /*alloc*/);
  List(size_t n, const Allocator& /*alloc*/);
  List(size_t /*n*/, const T& /*value*/, const Allocator& /*alloc*/);
  List(std::initializer_list<T> /*init*/, const Allocator& /*alloc*/);
  List(std::initializer_list<T> /*init*/);

  List(const List& /*other*/);
  List& operator=(const List& /*other*/);
  ~List();

  size_t size() const;
  Allocator get_allocator() const;
  void clear();
  bool empty() const;

  void push_back(const T& /*value*/);
  void push_front(const T& /*value*/);
  void pop_back();
  void pop_front();

  iterator insert(iterator /*pos*/, const T& /*value*/);
  iterator insert(const_iterator /*pos*/, const T& /*value*/);
  iterator erase(iterator /*pos*/);
  iterator erase(const_iterator /*pos*/);

  iterator begin();
  iterator end();
  const_iterator begin() const;
  const_iterator end() const;
  const_iterator cbegin() const;
  const_iterator cend() const;
  reverse_iterator rbegin();
  reverse_iterator rend();
  const_reverse_iterator rbegin() const;
  const_reverse_iterator rend() const;
  const_reverse_iterator crbegin() const;
  const_reverse_iterator crend() const;

 private:
  node_allocator node_alloc_;
  mutable BaseNode base_node_;
  size_t size_;

  void push_back_default(BaseNode* /*node*/);
};

template <typename T, typename Allocator>
template <bool IsConst>
struct List<T, Allocator>::BaseIterator {
 public:
  using iterator_category = std::bidirectional_iterator_tag;
  using difference_type = int;
  using value_type = T;
  using pointer = typename std::conditional<IsConst, const T*, T*>::type;
  using reference = typename std::conditional<IsConst, const T&, T&>::type;

  BaseIterator(BaseNode* node) : node_(node) {}
  BaseIterator(const BaseIterator& other) : node_(other.node_) {}

  operator const_iterator() const { return const_iterator(node_); }

  pointer operator->() { return &(node_->as_node()->value); }
  reference operator*() { return node_->as_node()->value; }

  BaseIterator& operator=(const BaseIterator& other) {
    node_ = other.node_;
    return *this;
  }

  BaseIterator& operator++() {
    node_ = node_->next;
    return *this;
  }
  BaseIterator operator++(int) {
    BaseIterator tmp = *this;
    node_ = node_->next;
    return tmp;
  }

  BaseIterator& operator--() {
    node_ = node_->prev;
    return *this;
  }
  BaseIterator operator--(int) {
    BaseIterator tmp = *this;
    node_ = node_->prev;
    return tmp;
  }

  friend bool operator==(const BaseIterator& it1, const BaseIterator& it2) {
    return (it1.node_ == it2.node_);
  }
  friend bool operator!=(const BaseIterator& it1, const BaseIterator& it2) {
    return !(it1 == it2);
  }

  BaseNode* get_node() { return node_; }

 private:
  BaseNode* node_;
};

template <typename T, typename Allocator>
struct List<T, Allocator>::BaseNode {
 public:
  BaseNode* prev;
  BaseNode* next;
  BaseNode() : prev(this), next(this) {}
  BaseNode(BaseNode* prev, BaseNode* next) : prev(prev), next(next) {}

  Node* as_node() { return static_cast<Node*>(this); }
  const Node* as_node() const { return static_cast<const Node*>(this); }
};
template <typename T, typename Allocator>
struct List<T, Allocator>::Node : public BaseNode {
  T value;
  Node(const T& value, BaseNode* prev, BaseNode* next)
      : BaseNode(prev, next), value(value) {}
  Node(BaseNode* prev, BaseNode* next) : BaseNode(prev, next), value() {}
};

template <typename T, typename Allocator>
List<T, Allocator>::List() : node_alloc_(), base_node_(), size_(0) {}

template <typename T, typename Allocator>
List<T, Allocator>::List(size_t n) : node_alloc_(), base_node_(), size_(0) {
  try {
    for (size_t i = 0; i < n; ++i) {
      push_back_default(end().get_node());
    }
  } catch (...) {
    clear();
    throw;
  }
}

template <typename T, typename Allocator>
List<T, Allocator>::List(std::initializer_list<T> init)
    : node_alloc_(), base_node_(), size_(0) {
  try {
    for (const T& value : init) {
      push_back(value);
    }
  } catch (...) {
    clear();
    throw;
  }
}

template <typename T, typename Allocator>
List<T, Allocator>::List(size_t n, const T& value)
    : node_alloc_(), base_node_(), size_(0) {
  try {
    for (size_t i = 0; i < n; ++i) {
      push_back(value);
    }
  } catch (...) {
    clear();
    throw;
  }
}

template <typename T, typename Allocator>
List<T, Allocator>::List(const Allocator& alloc)
    : node_alloc_(alloc), base_node_(), size_(0) {}

template <typename T, typename Allocator>
List<T, Allocator>::List(size_t n, const Allocator& alloc)
    : node_alloc_(alloc), base_node_(), size_(0) {
  try {
    for (size_t i = 0; i < n; ++i) {
      push_back_default(end().get_node());
    }
  } catch (...) {
    clear();
    throw;
  }
}

template <typename T, typename Allocator>
List<T, Allocator>::List(size_t n, const T& value, const Allocator& alloc)
    : node_alloc_(alloc), base_node_(), size_(0) {
  try {
    for (size_t i = 0; i < n; ++i) {
      push_back(value);
    }
  } catch (...) {
    clear();
    throw;
  }
}

template <typename T, typename Allocator>
List<T, Allocator>::List(std::initializer_list<T> init, const Allocator& alloc)
    : node_alloc_(alloc), base_node_(), size_(0) {
  try {
    for (const auto& elem : init) {
      push_back(elem);
    }
  } catch (...) {
    clear();
    throw;
  }
}

template <typename T, typename Allocator>
List<T, Allocator>::List(const List& other)
    : node_alloc_(node_alloc_traits::select_on_container_copy_construction(
          other.node_alloc_)),
      base_node_(),
      size_(0) {
  try {
    for (auto it = other.begin(); it != other.end(); ++it) {
      push_back(*it);
    }
  } catch (...) {
    clear();
    throw;
  }
}

template <typename T, typename Allocator>
List<T, Allocator>& List<T, Allocator>::operator=(const List& other) {
  // случай, когда делают: а = а;
  if (this == &other) {
    return *this;
  }

  auto tmp_node_alloc = node_alloc_;
  if (node_alloc_traits::propagate_on_container_copy_assignment::value) {
    node_alloc_ = other.node_alloc_;
  } else if (!alloc_traits::is_always_equal::value &&
             node_alloc_ != other.node_alloc_) {
    node_alloc_ = other.node_alloc_;
  }
  size_t correct_added_cnt = 0;
  size_t old_size = size_;
  try {
    for (auto it = other.begin(); it != other.end(); ++it) {
      push_back(*it);
      ++correct_added_cnt;
    }
  } catch (...) {
    for (size_t i = 0; i < correct_added_cnt; ++i) {
      pop_back();
    }
    node_alloc_ = tmp_node_alloc;
    throw;
  }
  for (size_t i = 0; i < old_size; ++i) {
    pop_front();
  }

  return *this;
}

template <typename T, typename Allocator>
List<T, Allocator>::~List() {
  clear();
}

template <typename T, typename Allocator>
size_t List<T, Allocator>::size() const {
  return size_;
}

template <typename T, typename Allocator>
Allocator List<T, Allocator>::get_allocator() const {
  return allocator_type(node_alloc_);
}

template <typename T, typename Allocator>
void List<T, Allocator>::clear() {
  while (!empty()) {
    pop_back();
  }
}

template <typename T, typename Allocator>
bool List<T, Allocator>::empty() const {
  return size_ == 0;
}

template <typename T, typename Allocator>
void List<T, Allocator>::push_back(const T& value) {
  BaseNode* prev = end().get_node()->prev;
  BaseNode* next = end().get_node();

  Node* new_node = nullptr;

  try {
    new_node = node_alloc_traits::allocate(node_alloc_, 1);

    node_alloc_traits::construct(node_alloc_, new_node, value, prev, next);

    prev->next = new_node;
    next->prev = new_node;

    ++size_;
  } catch (...) {
    if (new_node) {
      node_alloc_traits::deallocate(node_alloc_, new_node, 1);
    }
    throw;
  }
}

template <typename T, typename Allocator>
void List<T, Allocator>::push_front(const T& value) {
  BaseNode* prev = begin().get_node()->prev;
  BaseNode* next = begin().get_node();

  Node* new_node = node_alloc_traits::allocate(node_alloc_, 1);
  node_alloc_traits::construct(node_alloc_, new_node, value, prev, next);
  prev->next = new_node;
  next->prev = new_node;
  ++size_;
}

template <typename T, typename Allocator>
void List<T, Allocator>::pop_back() {
  BaseNode* delete_node = end().get_node()->prev;
  delete_node->prev->next = delete_node->next;
  delete_node->next->prev = delete_node->prev;
  --size_;

  node_alloc_traits::destroy(node_alloc_, reinterpret_cast<Node*>(delete_node));
  node_alloc_traits::deallocate(node_alloc_,
                                reinterpret_cast<Node*>(delete_node), 1);
}

template <typename T, typename Allocator>
void List<T, Allocator>::pop_front() {
  BaseNode* delete_node = begin().get_node();
  delete_node->prev->next = delete_node->next;
  delete_node->next->prev = delete_node->prev;
  --size_;

  node_alloc_traits::destroy(node_alloc_, reinterpret_cast<Node*>(delete_node));
  node_alloc_traits::deallocate(node_alloc_,
                                reinterpret_cast<Node*>(delete_node), 1);
}

template <typename T, typename Allocator>
typename List<T, Allocator>::iterator List<T, Allocator>::insert(
    List::iterator pos, const T& value) {
  return insert(const_iterator(pos), value);
}

template <typename T, typename Allocator>
typename List<T, Allocator>::iterator List<T, Allocator>::insert(
    List::const_iterator pos, const T& value) {
  BaseNode* prev = pos.get_node()->prev;
  BaseNode* next = pos.get_node();

  Node* new_node = node_alloc_traits::allocate(node_alloc_, 1);
  node_alloc_traits::construct(node_alloc_, new_node, value, prev, next);
  prev->next = new_node;
  next->prev = new_node;
  ++size_;
  return iterator(new_node);
}

template <typename T, typename Allocator>
typename List<T, Allocator>::iterator List<T, Allocator>::erase(
    List::iterator pos) {
  return erase(const_iterator(pos));
}

template <typename T, typename Allocator>
typename List<T, Allocator>::iterator List<T, Allocator>::erase(
    List::const_iterator pos) {
  BaseNode* delete_node = pos.get_node();
  delete_node->prev->next = delete_node->next;
  delete_node->next->prev = delete_node->prev;
  --size_;

  BaseNode* return_node = delete_node->next;
  node_alloc_traits::destroy(node_alloc_, reinterpret_cast<Node*>(delete_node));
  node_alloc_traits::deallocate(node_alloc_,
                                reinterpret_cast<Node*>(delete_node), 1);
  return iterator(return_node);
}

template <typename T, typename Allocator>
typename List<T, Allocator>::iterator List<T, Allocator>::begin() {
  return iterator(base_node_.next);
}

template <typename T, typename Allocator>
typename List<T, Allocator>::iterator List<T, Allocator>::end() {
  return iterator(&base_node_);
}

template <typename T, typename Allocator>
typename List<T, Allocator>::const_iterator List<T, Allocator>::begin() const {
  return cbegin();
}

template <typename T, typename Allocator>
typename List<T, Allocator>::const_iterator List<T, Allocator>::end() const {
  return cend();
}

template <typename T, typename Allocator>
typename List<T, Allocator>::const_iterator List<T, Allocator>::cbegin() const {
  return const_iterator(base_node_.next);
}

template <typename T, typename Allocator>
typename List<T, Allocator>::const_iterator List<T, Allocator>::cend() const {
  return const_iterator(&base_node_);
}

template <typename T, typename Allocator>
typename List<T, Allocator>::reverse_iterator List<T, Allocator>::rbegin() {
  return reverse_iterator(base_node_.prev);
}

template <typename T, typename Allocator>
typename List<T, Allocator>::reverse_iterator List<T, Allocator>::rend() {
  return reverce_iterator(&base_node_);
}

template <typename T, typename Allocator>
typename List<T, Allocator>::const_reverse_iterator List<T, Allocator>::rbegin()
    const {
  return crbegin();
}

template <typename T, typename Allocator>
typename List<T, Allocator>::const_reverse_iterator List<T, Allocator>::rend()
    const {
  return crend();
}

template <typename T, typename Allocator>
typename List<T, Allocator>::const_reverse_iterator
List<T, Allocator>::crbegin() const {
  return const_reverse_iterator(base_node_.prev);
}

template <typename T, typename Allocator>
typename List<T, Allocator>::const_reverse_iterator List<T, Allocator>::crend()
    const {
  return const_reverse_iterator(base_node_);
}

template <typename T, typename Allocator>
void List<T, Allocator>::push_back_default(BaseNode* node) {
  BaseNode* prev = node->prev;
  BaseNode* next = node;

  Node* new_node = node_alloc_traits::allocate(node_alloc_, 1);

  try {
    node_alloc_traits::construct(node_alloc_, new_node, prev, next);

    prev->next = new_node;
    next->prev = new_node;

    ++size_;
  } catch (...) {
    node_alloc_traits::deallocate(node_alloc_, new_node, 1);
    throw;
  }
}