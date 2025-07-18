
#pragma once
#include <memory>
#include <type_traits>

template <typename T>
class SharedPtr;

template <typename T>
class WeakPtr;

template <typename T>
class EnableSharedFromThis;

struct BaseControlBlock {
  size_t shared_cnt;
  size_t weak_cnt;

  BaseControlBlock(size_t scount, size_t wcount)
      : shared_cnt(scount), weak_cnt(wcount) {}

  virtual void destroy_object() = 0;
  virtual void deallocate_control_block() = 0;

  virtual ~BaseControlBlock() = default;
};

template <typename T, typename Deleter, typename Alloc>
struct RegularControlBlock : BaseControlBlock {
  Deleter del;
  Alloc alloc;

  using alloc_traits = typename std::allocator_traits<Alloc>;
  using block_alloc = typename alloc_traits::template rebind_alloc<
      RegularControlBlock<T, Deleter, Alloc>>;
  using block_alloc_traits = typename alloc_traits::template rebind_traits<
      RegularControlBlock<T, Deleter, Alloc>>;

  T* managed_object;

  RegularControlBlock(T* ptr, const Deleter& dell, Alloc allc)
      : BaseControlBlock(1, 0), del(dell), alloc(allc), managed_object(ptr) {}

  void destroy_object() override { del(managed_object); }
  void deallocate_control_block() override {
    block_alloc cb_alloc = alloc;
    block_alloc_traits::deallocate(cb_alloc, this, 1);
  }

  ~RegularControlBlock() override = default;
};

template <typename T, typename Alloc = std::allocator<T>>
struct MakeSharedControlBlock : BaseControlBlock {
  Alloc alloc;
  T object;

  using alloc_traits = typename std::allocator_traits<Alloc>;
  using block_alloc = typename alloc_traits::template rebind_alloc<
      MakeSharedControlBlock<T, Alloc>>;
  using block_alloc_traits = typename alloc_traits::template rebind_traits<
      MakeSharedControlBlock<T, Alloc>>;
  using obj_alloc = typename alloc_traits::template rebind_alloc<T>;
  using obj_alloc_traits = typename alloc_traits::template rebind_traits<T>;

  template <typename... Args>
  MakeSharedControlBlock(size_t scount, size_t wcount, Alloc allc,
                         Args&&... args)
      : BaseControlBlock(scount, wcount),
        alloc(allc),
        object(std::forward<Args>(args)...) {}

  void destroy_object() override {
    obj_alloc obj_alloc = alloc;
    obj_alloc_traits::destroy(obj_alloc, &object);
  }
  void deallocate_control_block() override {
    block_alloc cb_alloc = alloc;
    block_alloc_traits::deallocate(cb_alloc, this, 1);
  }

  ~MakeSharedControlBlock() override = default;
};

template <typename T>
class SharedPtr {
 public:
  SharedPtr() : object_(nullptr), control_block_(nullptr) {}

  template <typename Deleter = std::default_delete<T>,
            typename Alloc = std::allocator<T>>
  SharedPtr(T* ptr, const Deleter& del = Deleter(), Alloc alloc = Alloc())
      : object_(ptr) {
    if (!ptr) {
      control_block_ = nullptr;
      return;
    }

    update_enable_shared_from_this();

    using regular_control_block_type = RegularControlBlock<T, Deleter, Alloc>;
    using block_alloc = typename std::allocator_traits<
        Alloc>::template rebind_alloc<regular_control_block_type>;

    block_alloc cb_alloc(alloc);
    auto* regular_cb =
        std::allocator_traits<block_alloc>::allocate(cb_alloc, 1);
    new (regular_cb) regular_control_block_type(ptr, del, alloc);

    control_block_ = regular_cb;
  }

  template <typename Y, typename Deleter = std::default_delete<Y>,
            typename Alloc = std::allocator<Y>>
    requires std::is_convertible_v<Y*, T*>
  SharedPtr(Y* ptr, const Deleter& del = Deleter(), Alloc alloc = Alloc())
      : object_(ptr) {
    if (!ptr) {
      control_block_ = nullptr;
      return;
    }

    update_enable_shared_from_this();

    using regular_control_block_type = RegularControlBlock<Y, Deleter, Alloc>;
    using block_alloc = typename std::allocator_traits<
        Alloc>::template rebind_alloc<regular_control_block_type>;

    block_alloc cb_alloc(alloc);
    auto* regular_cb =
        std::allocator_traits<block_alloc>::allocate(cb_alloc, 1);
    new (regular_cb) regular_control_block_type(ptr, del, alloc);
    control_block_ = regular_cb;
  }

  SharedPtr(const SharedPtr& shptr) noexcept
      : object_(shptr.object_), control_block_(shptr.control_block_) {
    increment_shared_count();
  }

  template <typename Y>
    requires std::is_convertible_v<Y, T>
  SharedPtr(const SharedPtr<Y>& shptr) noexcept
      : object_(shptr.object_), control_block_(shptr.control_block_) {
    increment_shared_count();
  }

  SharedPtr(SharedPtr&& shptr) noexcept
      : object_(shptr.object_), control_block_(shptr.control_block_) {
    shptr.object_ = nullptr;
    shptr.control_block_ = nullptr;
  }

  template <typename Y>
    requires std::is_convertible_v<Y, T>
  SharedPtr(SharedPtr<Y>&& shptr) noexcept
      : object_(shptr.object_), control_block_(shptr.control_block_) {
    shptr.object_ = nullptr;
    shptr.control_block_ = nullptr;
  }

  SharedPtr<T>& operator=(const SharedPtr<T>& shptr) {
    if (this != &shptr) {
      SharedPtr<T> tmp(shptr);
      swap(tmp);
    }
    return *this;
  }

  template <typename Y>
    requires std::is_convertible_v<Y, T>
  SharedPtr& operator=(const SharedPtr<Y>& shptr) noexcept {
    SharedPtr tmp(shptr);
    swap(tmp);
    return *this;
  }

  SharedPtr& operator=(SharedPtr&& shptr) noexcept {
    swap(shptr);
    return *this;
  }

  template <typename Y>
    requires std::is_convertible_v<Y, T>
  SharedPtr& operator=(SharedPtr<Y>&& shptr) noexcept {
    swap(shptr);
    return *this;
  }

  T& operator*() const {
    if (object_) {
      return *object_;
    }
    return static_cast<MakeSharedControlBlock<T>*>(control_block_)->object;
  }

  T* operator->() const {
    if (object_) {
      return object_;
    }
    if (control_block_ == nullptr) {
      return nullptr;
    }
    return &(static_cast<MakeSharedControlBlock<T>*>(control_block_)->object);
  }

  ~SharedPtr() { clear(); }

  size_t use_count() const {
    if (control_block_ == nullptr) {
      return 0;
    }
    return control_block_->shared_cnt;
  }

  T* get() const { return object_; }

  void reset() { SharedPtr<T>().swap(*this); }

  template <typename Y, typename Deleter = std::default_delete<T>,
            typename Alloc = std::allocator<T>>
    requires std::is_convertible_v<Y, T>
  void reset(Y* ptr, Deleter del = Deleter(), Alloc alloc = Alloc()) {
    SharedPtr<T>(ptr, del, alloc).swap(*this);
  }

  void swap(SharedPtr& other) noexcept {
    std::swap(object_, other.object_);
    std::swap(control_block_, other.control_block_);
  }

 private:
  template <typename Alloc>
  SharedPtr(MakeSharedControlBlock<T, Alloc>* make_shared_cb)
      : object_(nullptr), control_block_(make_shared_cb) {
    update_enable_shared_from_this();
  }

  SharedPtr(WeakPtr<T> wptr) {
    if (wptr.expired()) {
      object_ = nullptr;
      control_block_ = nullptr;
      return;
    }
    object_ = wptr.object_;
    control_block_ = wptr.control_block_;
    increment_shared_count();
    update_enable_shared_from_this();
  }

  void clear() {
    if (control_block_ == nullptr) {
      return;
    }
    --control_block_->shared_cnt;
    if (control_block_->shared_cnt == 0) {
      control_block_->destroy_object();
      if (control_block_->weak_cnt == 0) {
        control_block_->deallocate_control_block();
      }
    }
    control_block_ = nullptr;
    object_ = nullptr;
  }

  void update_enable_shared_from_this() {
    if (control_block_ == nullptr) {
      return;
    }
    if constexpr (std::is_base_of_v<EnableSharedFromThis<T>, T>) {
      if (object_) {
        object_->wptr_ = *this;
      } else {
        MakeSharedControlBlock<T>* make_shared_cb =
            static_cast<MakeSharedControlBlock<T>*>(control_block_);
        if (make_shared_cb) {
          make_shared_cb->object.wptr_ = *this;
        }
      }
    }
  }

  void increment_shared_count() {
    if (control_block_ != nullptr) {
      ++control_block_->shared_cnt;
    }
  }

  T* object_ = nullptr;
  BaseControlBlock* control_block_ = nullptr;

  template <typename Y>
  friend class WeakPtr;

  template <typename Y>
  friend class SharedPtr;

  template <typename Y, typename... Args>
  friend SharedPtr<Y> MakeShared(Args&&...);

  template <typename Y, typename Alloc, typename... Args>
  friend SharedPtr<Y> AllocateShared(Alloc, Args&&...);
};

template <typename T, typename Alloc, typename... Args>
SharedPtr<T> AllocateShared(Alloc alloc, Args&&... args) {
  using alloc_traits = typename std::allocator_traits<Alloc>;
  using make_shared_block_alloc = typename alloc_traits::template rebind_alloc<
      MakeSharedControlBlock<T, Alloc>>;
  using make_shared_block_alloc_traits =
      typename alloc_traits::template rebind_traits<
          MakeSharedControlBlock<T, Alloc>>;
  make_shared_block_alloc cb_alloc = alloc;

  auto cblk = make_shared_block_alloc_traits::allocate(cb_alloc, 1);

  make_shared_block_alloc_traits::construct(cb_alloc, cblk, 1, 0, alloc,
                                            std::forward<Args>(args)...);
  return SharedPtr<T>(cblk);
}

template <typename T, typename... Args>
SharedPtr<T> MakeShared(Args&&... args) {
  return AllocateShared<T>(std::allocator<T>(), std::forward<Args>(args)...);
}

template <typename T>
class WeakPtr {
 public:
  WeakPtr() : object_(nullptr), control_block_(nullptr) {}

  WeakPtr(const SharedPtr<T>& sptr)
      : object_(sptr.object_), control_block_(sptr.control_block_) {
    increment_weak_count();
  }

  template <typename Y>
    requires std::is_convertible_v<Y, T>
  WeakPtr(const SharedPtr<Y>& sptr)
      : object_(sptr.object_), control_block_(sptr.control_block_) {
    increment_weak_count();
  }

  WeakPtr(const WeakPtr& other)
      : object_(other.object_), control_block_(other.control_block_) {
    increment_weak_count();
  }

  template <typename Y>
    requires std::is_convertible_v<Y, T>
  WeakPtr(const WeakPtr<Y>& other)
      : object_(other.object_), control_block_(other.control_block_) {
    increment_weak_count();
  }

  WeakPtr(WeakPtr&& other)
      : object_(other.object_), control_block_(other.control_block_) {
    other.object_ = nullptr;
    other.control_block_ = nullptr;
  }

  template <typename Y>
    requires std::is_convertible_v<Y, T>
  WeakPtr(WeakPtr<Y>&& other)
      : object_(other.object_), control_block_(other.control_block_) {
    other.object_ = nullptr;
    other.control_block_ = nullptr;
  }

  WeakPtr& operator=(const WeakPtr& other) {
    if (this != &other) {
      WeakPtr tmp(other);
      swap(tmp);
    }
    return *this;
  }

  template <typename Y>
    requires std::is_convertible_v<Y, T>
  WeakPtr& operator=(const WeakPtr<Y>& other) {
    WeakPtr tmp(other);
    swap(tmp);
    return *this;
  }

  WeakPtr& operator=(WeakPtr&& other) {
    swap(other);
    return *this;
  }

  template <typename Y>
    requires std::is_convertible_v<Y, T>
  WeakPtr& operator=(WeakPtr<Y>&& other) {
    swap(other);
    return *this;
  }

  ~WeakPtr() { clear(); }

  size_t use_count() const noexcept {
    if (control_block_ == nullptr) {
      return 0;
    }
    return control_block_->shared_cnt;
  }

  bool expired() const noexcept { return use_count() == 0; }

  SharedPtr<T> lock() const noexcept {
    if (expired() || (control_block_ == nullptr)) {
      return SharedPtr<T>();
    }
    return SharedPtr<T>(*this);
  }

 private:
  void swap(WeakPtr& other) {
    std::swap(object_, other.object_);
    std::swap(control_block_, other.control_block_);
  }

  void clear() {
    if (control_block_ == nullptr) {
      return;
    }
    --control_block_->weak_cnt;
    if (control_block_->shared_cnt == 0 && control_block_->weak_cnt == 0) {
      control_block_->deallocate_control_block();
    }
  }

  void increment_weak_count() {
    if (control_block_ != nullptr) {
      ++control_block_->weak_cnt;
    }
  }

  T* object_ = nullptr;
  BaseControlBlock* control_block_ = nullptr;

  template <typename Y>
  friend class WeakPtr;

  template <typename Y>
  friend class SharedPtr;
};

template <typename T>
class EnableSharedFromThis {
 public:
  SharedPtr<T> shared_from_this() const noexcept { return wptr_.lock(); }

 private:
  WeakPtr<T> wptr_;
};