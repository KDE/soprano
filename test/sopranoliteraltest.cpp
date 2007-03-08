/* 
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2007 Sebastian Trueg <trueg@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include <soprano/soprano.h>

#include "sopranoliteraltest.h"

using namespace Soprano;

Q_DECLARE_METATYPE(QVariant)

void SopranoLiteralTest::initTestCase()
{
  m_model = Soprano::createModel();
  QVERIFY( m_model != 0 );
}


void SopranoLiteralTest::cleanupTestCase()
{
  delete m_model;
}


void SopranoLiteralTest::testDataTypes_data()
{
  QTest::addColumn<QString>( "dataType" );
  QTest::addColumn<QVariant>( "value" );

  QTest::newRow( "int" ) << "int" << QVariant( (int)-17 );
  QTest::newRow( "long" ) << "long" << QVariant( (qlonglong)17927948235235LL );
  QTest::newRow( "unsignedInt" ) << "unsignedInt" << QVariant( (unsigned int)17 );
  QTest::newRow( "unsignedLong" ) << "unsignedLong" << QVariant( (qulonglong)17927948235235LL );
  QTest::newRow( "double" ) << "double" << QVariant( (double)325423.43252 );
  QTest::newRow( "string" ) << "string" << QVariant( QString("Hello World") );
  QTest::newRow( "date" ) << "date" << QVariant( QDate::currentDate() );
  QTest::newRow( "time" ) << "time" << QVariant( QTime::currentTime() );
  QTest::newRow( "dateTime" ) << "dateTime" << QVariant( QDateTime::currentDateTime() );
  QTest::newRow( "boolean-false" ) << "boolean" << QVariant( false );
  QTest::newRow( "boolean-true" ) << "boolean" << QVariant( true );
}


void SopranoLiteralTest::testDataTypes()
{
  const QString xmlSchemaNs( "http://www.w3.org/2001/XMLSchema#" );

  QFETCH(QString, dataType);
  QFETCH(QVariant, value);

  Node subject( QUrl("http://iamarandomuri/yessir") );
  Node predicate( QUrl("http://iamarandomuri/yesmadam/" + QString(QTest::currentDataTag())) );

  Node object( value.toString(),
	       Node::Literal,
	       QUrl( xmlSchemaNs + dataType ) );

  Statement s( subject, predicate, object );

  // add the value
  QVERIFY( m_model->add( s ) == 0 );

  // and retrieve it
  StatementIterator it = m_model->listStatements( Statement( subject, predicate, Node() ) );
  QVERIFY( it.isValid() );
  QVERIFY( !it.isEmpty() );
  QVERIFY( it.hasNext() );
  Statement s2 = it.next();
  QCOMPARE( s2.object().literal(), value.toString() );
  QCOMPARE( s2.object().dataType(), object.dataType() );
}


QTEST_MAIN(SopranoLiteralTest)

#include "sopranoliteraltest.moc"
