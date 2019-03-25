#include <QTest>
#include "tst_colonycollision.h"
#include "tst_platedetection.h"

int main(int argc, char **argv) {
  int status = 0;
  {
      // ColonyCollision tc;
      // status |= QTest::qExec(&tc, argc, argv);
  } {
    PlateDetection tc;
    status |= QTest::qExec(&tc, argc, argv);
  }
  return status;
}
