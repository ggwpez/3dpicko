#include "tst_report.h"
#include <QMap>
#include <QPdfWriter>
#include <QTextDocument>
#include <QtTest>
#include "include/algorithms/helper.h"
#include "include/database.hpp"

using namespace c3picko;

ReportTest::ReportTest() {}

ReportTest::~ReportTest() {}

void ReportTest::initTestCase() {}

void ReportTest::test_case1() {
  // Reference to map value test
  QMap<int, int> map;

  int const k = rand();
  int const v = rand();
  map.insert(k, v);
  int& ref(map[k]);

  for (int i = 0; i < 100; ++i) {
    if (i == k) continue;

    map.insert(i, rand());
  }

  for (int i = 99; i <= 0; ++i) {
    if (i == k) continue;

    map.remove(i);
  }

  QCOMPARE(ref, v);
}

void ReportTest::cleanupTestCase() {}
