#include <QtTest>
#include <QCoreApplication>

// add necessary includes here

class ColonyCollision : public QObject
{
	Q_OBJECT

public:
	ColonyCollision();
	~ColonyCollision();

private slots:
	void initTestCase();
	void cleanupTestCase();
	void test_case1();

};

ColonyCollision::ColonyCollision()
{

}

ColonyCollision::~ColonyCollision()
{

}

void ColonyCollision::initTestCase()
{

}

void ColonyCollision::cleanupTestCase()
{

}

void ColonyCollision::test_case1()
{

}

QTEST_MAIN(ColonyCollision)

#include "tst_colonycollision.moc"
