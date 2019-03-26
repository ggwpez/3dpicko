#include "tst_report.h"
#include "include/database.hpp"
#include <QPdfWriter>
#include <QTextDocument>
#include <QtTest>

using namespace c3picko;

ReportTest::ReportTest() {}

ReportTest::~ReportTest() {}

void ReportTest::initTestCase() {}

void ReportTest::test_case1()
{
	std::vector<std::pair<Colony::ID, Well>> pseq;

	for (int i = 0; i < 300; ++i)
		pseq.emplace_back(i, Well{1, 1});

	QSettings settings("/home/vados/Code/Projects/3cpicko/GUI/serverconfig.ini", QSettings::IniFormat);
	settings.beginGroup("database");
	// Database* db = new Database(settings, this);

	QPdfWriter pdf("out.pdf");
	// report.writePdfReport(&pdf);
}

void ReportTest::cleanupTestCase() {}
