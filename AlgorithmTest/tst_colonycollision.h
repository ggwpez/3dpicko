#pragma once

#include <vector>

#include <QObject>
#include "include/colony.hpp"

struct TestData {
  std::vector<c3picko::Colony> colonies_;
  const int r;
  const int d;
  const int min_num_;
};

class ColonyCollision : public QObject {
  Q_OBJECT

 public:
  ColonyCollision();
  ~ColonyCollision();

 private slots:
  void initTestCase();
  void cleanupTestCase();
  void test_case1();

 private:
  std::vector<TestData> solutions_;
};
