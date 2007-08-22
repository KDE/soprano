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

#include "sopranoliteraltest.h"

#include <soprano/soprano.h>

#include <QtTest/QTest>
#include <QtCore/QVariant>
#include <QtCore/QCoreApplication>


using namespace Soprano;

Q_DECLARE_METATYPE(Soprano::LiteralValue)

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
  QTest::addColumn<LiteralValue>( "value" );

  QTest::newRow( "int" ) << "int" << LiteralValue( (int)-17 );
  QTest::newRow( "long" ) << "long" << LiteralValue( (qlonglong)17927948235235LL );
  QTest::newRow( "unsignedInt" ) << "unsignedInt" << LiteralValue( (unsigned int)17 );
  QTest::newRow( "unsignedLong" ) << "unsignedLong" << LiteralValue( (qulonglong)17927948235235LL );
  QTest::newRow( "double" ) << "double" << LiteralValue( (double)325423.43252 );
  QTest::newRow( "string" ) << "string" << LiteralValue( QString("Hello World") );
  QTest::newRow( "date" ) << "date" << LiteralValue( QDate::currentDate() );
  QTest::newRow( "time" ) << "time" << LiteralValue( QTime::currentTime() );
  QTest::newRow( "dateTime" ) << "dateTime" << LiteralValue( QDateTime::currentDateTime() );
  QTest::newRow( "boolean-false" ) << "boolean" << LiteralValue( false );
  QTest::newRow( "boolean-true" ) << "boolean" << LiteralValue( true );
}


void SopranoLiteralTest::testDataTypes()
{
  const QString xmlSchemaNs( "http://www.w3.org/2001/XMLSchema#" );

  QFETCH(QString, dataType);
  QFETCH(LiteralValue, value);

  Node subject( QUrl("http://iamarandomuri/yessir") );
  Node predicate( QUrl("http://iamarandomuri/yesmadam/" + QString(QTest::currentDataTag())) );

  Node object( value );

  Statement s( subject, predicate, object );

  // add the value
  QVERIFY( m_model->addStatement( s ) == 0 );

  // and retrieve it
  StatementIterator it = m_model->listStatements( Statement( subject, predicate, Node() ) );
  QVERIFY( it.isValid() );
  QVERIFY( it.next() );
  Statement s2 = *it;
  QCOMPARE( s2.object().literal().toString(), value.toString() );
  QCOMPARE( s2.object().dataType(), object.dataType() );
  QCOMPARE( s2.object().literal().dataTypeUri().toString(), xmlSchemaNs + dataType );
}


QTEST_MAIN(SopranoLiteralTest)

#include "sopranoliteraltest.moc"
