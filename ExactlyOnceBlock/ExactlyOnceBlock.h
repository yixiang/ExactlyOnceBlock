namespace onceblock {

/**
 * ReturnType must be one of the following:
 *   * a pirimitive type.
 *   * a C++/Objective-C pointer.
 *   * a C++ class with a public prameterless constructor.
 */
template <typename AssertFunc, typename ReturnType, typename... Args>
class RunOnceBlock {
 public:
  using BlockType = ReturnType (^)(Args...);
  using AssertType = AssertFunc;
  explicit RunOnceBlock(BlockType block)
      : block_(block), is_block_nullable_(block == nil) {}
  RunOnceBlock(RunOnceBlock &&other)
      : block_(other.Release()), is_block_nullable_(other.is_block_nullable_) {}
  RunOnceBlock &operator=(RunOnceBlock &&other) {
    block_ = other.Release();
  }
  ~RunOnceBlock() {
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
  static RunOnceBlock<AssertFunc, ReturnType, Args...> Make(
      ReturnType (^block)(Args...)) {
    return RunOnceBlock<AssertFunc, ReturnType, Args...>(block);
  }
};

template <typename ReturnType, typename... Args>
RunOnceBlock<NoAssert, ReturnType, Args...> Make(ReturnType (^block)(Args...)) {
  return Maker<NoAssert>::Make(block);
}

}
