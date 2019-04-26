#include <QGuiApplication>
#include <QTest>
#include "tst_colonycollision.h"
#include "tst_platedetection.h"
#include "tst_report.h"

int main(int argc, char** argv) {
  QGuiApplication app(argc, argv);
  int status = 0;
  /*{
		  ColonyCollision tc;
		  status |= QTest::qExec(&tc, argc, argv);
  } {
	PlateDetection tc;
	status |= QTest::qExec(&tc, argc, argv);
  }*/
  {
	ReportTest tc;
	status |= QTest::qExec(&tc, argc, argv);
  }
  return status;
}
