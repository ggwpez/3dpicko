#include "tst_colonycollision.h"
#include <QtTest>
#include "include/algorithms/helper.h"
#include "include/colony.hpp"

using namespace c3picko;

ColonyCollision::ColonyCollision() {}

ColonyCollision::~ColonyCollision() {}

static Colony makeColony(int x, int y) {
  return Colony(x, y, 0, 0, 0, 0, 0, "");
}

void ColonyCollision::initTestCase() {
  int r = 10, d = 5, p = 4;
  /*
   *    O O
   *     O
   *    O O
   */
  solutions_.push_back(
      {{makeColony(0, 0), makeColony(r * 2 + p, 0), makeColony(0, r * 2 + p),
        makeColony(r * 2 + p, r * 2 + p), makeColony(r + p / 2, r + p / 2)},
       r,
       d,
       2});

  /*
   *    O O
   *
   *    O O
   */
  solutions_.push_back(
      {{makeColony(0, 0), makeColony(r * 2 + p, 0), makeColony(0, r * 2 + p),
        makeColony(r * 2 + p, r * 2 + p)},
       r,
       d,
       2});

  // Now we create some more colonies
  const int min_n = 2, max_n = 10;

  for (int n = min_n; n <= max_n; ++n) {
    int const nsq = n * n;
    std::vector<Colony> colonies;

    for (int x = 0; x < n; ++x) {
      for (int y = 0; y < n; ++y) {
        colonies.push_back(makeColony(2 * r * x + p, 2 * r * y + p));
      }
    }

    solutions_.push_back({{colonies}, r, d, nsq / 2 + (nsq % 2)});
  }
}

void ColonyCollision::cleanupTestCase() {}

void ColonyCollision::test_case1() {
  for (std::size_t i = 0; i < solutions_.size(); ++i) {
    std::vector<Colony> result =
        math::filterByMinDistanceSlow(solutions_[i].colonies_, solutions_[i].r,
                                      solutions_[i].d, solutions_[i].min_num_);

    qDebug().nospace() << "TEST #" << i
                       << "  (n=" << solutions_[i].colonies_.size() << ")";
    QCOMPARE(result.size(), solutions_[i].min_num_);
  }
}
