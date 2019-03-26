#pragma once

#include <vector>

#include "include/types/report.h"
#include <QObject>

class ReportTest : public QObject
{
	Q_OBJECT

  public:
	ReportTest();
	~ReportTest();

  private slots:
	void initTestCase();
	void cleanupTestCase();
	void test_case1();

  private:
};
