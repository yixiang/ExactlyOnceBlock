#import <XCTest/XCTest.h>

#import "ExactlyOnceBlock.h"

@interface ExactlyOnceBlockTests : XCTestCase

@end

@implementation ExactlyOnceBlockTests

- (void)setUp {
  [super setUp];
}

- (void)tearDown {
  [super tearDown];
}

static int gAssertCount = 0;
struct TestAssert {
  void operator()() {
    ++gAssertCount;
  }
};

- (void)testAssertOverRelease {
  __block int completed = 0;
  auto block = ^int(int i) {
    ++completed;
    return i + 1;
  };
  gAssertCount = 0;
  XCTAssertEqual(completed, 0);
  auto onceBlock = onceblock::Maker<TestAssert>::Make(block);

  XCTAssertEqual(onceBlock.CallAndRelease(3), 4);
  XCTAssertEqual(completed, 1);
  XCTAssertEqual(gAssertCount, 0);

  XCTAssertEqual(onceBlock.CallAndRelease(3), 0);
  XCTAssertEqual(completed, 1);
  XCTAssertEqual(gAssertCount, 1);
}

- (void)testAssertOnUnderRelease {
  __block int completed = 0;
  auto block = ^int(int i) {
    ++completed;
    return i + 1;
  };
  gAssertCount = 0;
  {
    XCTAssertEqual(completed, 0);
    auto onceBlock = onceblock::Maker<TestAssert>::Make(block);
  }
  XCTAssertEqual(completed, 0);
  XCTAssertEqual(gAssertCount, 1);
}

- (void)test1 {
  __block int completed = 0;
  dispatch_block_t block = ^{
    ++completed;
  };
  XCTAssertEqual(completed, 0);
  auto onceBlock = onceblock::Make(block);
  onceBlock.CallAndRelease();
  XCTAssertEqual(completed, 1);
}

- (void)test2 {
  __block int completed = 0;
  auto block = ^int(int i) {
    ++completed;
    return i + 1;
  };
  XCTAssertEqual(completed, 0);
  auto onceBlock = onceblock::Make(block);
  XCTAssertEqual(onceBlock.CallAndRelease(2), 3);
  XCTAssertEqual(completed, 1);

  XCTAssertEqual(onceBlock.CallAndRelease(2), 0);
  XCTAssertEqual(completed, 1);
}

- (void)test3 {
  __block int completed = 0;
  auto block = ^double(int i, short s) {
    ++completed;
    return i + s;
  };
  XCTAssertEqual(completed, 0);
  auto onceBlock = onceblock::Make(block);
  XCTAssertEqual(onceBlock.CallAndRelease(2, 3), 5);
  XCTAssertEqual(completed, 1);

  XCTAssertEqual(onceBlock.CallAndRelease(2, 3), 0);
  XCTAssertEqual(completed, 1);
}

@end
