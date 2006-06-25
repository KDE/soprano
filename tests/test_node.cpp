#include <QtTest>
#include <QtCore>

#include "node.h"

using namespace RDF;

class TestNode: public QObject
{
  Q_OBJECT
  private slots:
  void testBlah();
};

void TestNode::testBlah()
{
  QVERIFY( 1 == 1 );
}

QTEST_MAIN(TestNode)

#include "test_node.moc"
