#include <QDebug>
#include <QtTest>

#include "include/gcodeinstruction.h"
// add necessary includes here

class TestGcodeInstruction : public QObject {
  Q_OBJECT

 public:
  TestGcodeInstruction();
  ~TestGcodeInstruction();

 private slots:
  void test_case1();
};

TestGcodeInstruction::TestGcodeInstruction() {}

TestGcodeInstruction::~TestGcodeInstruction() {}

void TestGcodeInstruction::test_case1() {
  qDebug() << "hi" << c3picko::GcodeInstruction::Init().ToString().c_str();
}

QTEST_APPLESS_MAIN(TestGcodeInstruction)

#include "tst_gcodeinstruction.moc"
