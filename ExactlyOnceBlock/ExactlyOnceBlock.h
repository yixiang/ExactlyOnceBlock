namespace onceblock {

/**
 * ReturnType must be one of the following:
 *   * a pirimitive type.
 *   * a C++/Objective-C pointer.
 *   * a C++ class with a public prameterless constructor.
 */
template <typename AssertFunc, typename ReturnType, typename... Args>
class ExactlyOnceBlock {
 public:
  using BlockType = ReturnType (^)(Args...);
  using AssertType = AssertFunc;
  explicit ExactlyOnceBlock(BlockType block)
      : block_(block), is_block_nullable_(block == nil) {}
  ExactlyOnceBlock(ExactlyOnceBlock &&other)
      : block_(other.Release()), is_block_nullable_(other.is_block_nullable_) {}
  ExactlyOnceBlock &operator=(ExactlyOnceBlock &&other) {
    block_ = other.Release();
  }
  ~ExactlyOnceBlock() {
    if (block_) {
      AssertFunc()();
    }
  }
  BlockType Release() {
    if (!is_block_nullable_ && !block_) {
      AssertFunc()();
    }
    BlockType block = block_;
    block_ = nil;
    return block;
  }
  ReturnType CallAndRelease(Args... args) {
    BlockType block = Release();
    if (block) {
      return block(args...);
    } else {
      return ReturnType();
    }
  }

 private:
  BlockType block_;
  bool is_block_nullable_;
};

struct NoAssert {
  void operator()() {}
};

template <typename AssertFunc>
struct Maker {
  template <typename ReturnType, typename... Args>
  static ExactlyOnceBlock<AssertFunc, ReturnType, Args...> Make(
      ReturnType (^block)(Args...)) {
    return ExactlyOnceBlock<AssertFunc, ReturnType, Args...>(block);
  }
};

template <typename ReturnType, typename... Args>
ExactlyOnceBlock<NoAssert, ReturnType, Args...> Make(
    ReturnType (^block)(Args...)) {
  return Maker<NoAssert>::Make(block);
}
}
