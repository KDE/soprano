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

#include "soprano.h"

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
    QTest::newRow( "binaryData" ) << "base64Binary" << LiteralValue( QByteArray( "Hello World" ) );
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

void SopranoLiteralTest::testPlainLiterals_data()
{
    QTest::addColumn<QString>( "str" );
    QTest::addColumn<QString>( "lang" );
    QTest::addColumn<LiteralValue>( "value" );

    QTest::newRow( "char-en" ) << "Some text" << "en" << LiteralValue::createPlainLiteral( "Some text", "en" );
    QTest::newRow( "QLatin1String-en" ) << "Some text" << "en" << LiteralValue::createPlainLiteral( QLatin1String("Some text"), "en" );
    QTest::newRow( "QString-en" ) << "Some text" << "en" << LiteralValue::createPlainLiteral( QString("Some text"), "en" );
    QTest::newRow( "en-us" ) << "Some text" << "en-us" << LiteralValue::createPlainLiteral( "Some text", "en-us" );
    QTest::newRow( "en-US" ) << "Some text" << "en-us" << LiteralValue::createPlainLiteral( "Some text", "en-US" );
    QTest::newRow( "en-GB" ) << "Some text" << "en-GB" << LiteralValue::createPlainLiteral( "Some text", "en-GB" );
    QTest::newRow( "plain" ) << "Some text" << QString() << LiteralValue::createPlainLiteral( "Some text" );
}

void SopranoLiteralTest::testPlainLiterals()
{
    QFETCH(QString, str);
    QFETCH(QString, lang);
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
    QCOMPARE( s2.object().literal().toString(), str );
    QCOMPARE( s2.object().literal().language(), object.literal().language() );
    QCOMPARE( s2.object().literal().language(), LanguageTag(lang) );
}

void SopranoLiteralTest::testToString_data()
{
    QTest::addColumn<LiteralValue>( "value" );
    QTest::addColumn<QString>( "stringValue" );

    QTest::newRow( "int (from constructor)" ) << LiteralValue( (int)-17 ) << QString( "-17" );
    QTest::newRow( "long (from constructor)" ) << LiteralValue( (qlonglong)17927948235235LL ) << QString("17927948235235");
    QTest::newRow( "unsignedInt (from constructor)" ) << LiteralValue( (unsigned int)17 ) << QString( "17" );
    QTest::newRow( "unsignedLong (from constructor)" ) << LiteralValue( (qulonglong)17927948235235LL ) << QString( "17927948235235" );
//    QTest::newRow( "double (from constructor)" ) << LiteralValue( (double)325423.43252 );
    QTest::newRow( "string (from constructor)" ) << LiteralValue( QString("Hello World") ) << QString( "Hello World" );
    QTest::newRow( "date (from constructor)" ) << LiteralValue( QDate( 1977, 8, 9 ) ) << QString( "1977-08-09" );
    QTest::newRow( "time (from constructor)" ) << LiteralValue( QTime( 17, 38, 2 ) ) << QString( "17:38:02Z" );
    QTest::newRow( "dateTime (from constructor)" ) << LiteralValue( QDateTime( QDate( 1977, 8, 9 ), QTime( 17, 38, 2 ), Qt::UTC ) ) << QString( "1977-08-09T17:38:02Z" );
    QTest::newRow( "dateTime2 (from constructor)" ) << LiteralValue( QDateTime( QDate( 1977, 8, 9 ), QTime( 17, 38, 2, 23 ), Qt::UTC ) ) << QString( "1977-08-09T17:38:02.023Z" );
    QTest::newRow( "boolean-false (from constructor)" ) << LiteralValue( false ) << QString( "false" );
    QTest::newRow( "boolean-true (from constructor)" ) << LiteralValue( true ) << QString( "true" );
    QTest::newRow( "binaryData (from constructor)" ) << LiteralValue( QByteArray( "Hello World" ) ) << QString::fromAscii( QByteArray( "Hello World" ).toBase64() );

    QTest::newRow( "int (from operator=)" ) << ( LiteralValue() = (int)-17 ) << QString( "-17" );
    QTest::newRow( "long (from operator=)" ) << ( LiteralValue() = (qlonglong)17927948235235LL ) << QString("17927948235235");
    QTest::newRow( "unsignedInt (from operator=)" ) << ( LiteralValue() = (unsigned int)17 ) << QString( "17" );
    QTest::newRow( "unsignedLong (from operator=)" ) << ( LiteralValue() = (qulonglong)17927948235235LL ) << QString( "17927948235235" );
//    QTest::newRow( "double (from operator=)" ) << ( LiteralValue() = (double)325423.43252 );
    QTest::newRow( "string (from operator=)" ) << ( LiteralValue() = QString("Hello World") ) << QString( "Hello World" );
    QTest::newRow( "date (from operator=)" ) << ( LiteralValue() = QDate( 1977, 8, 9 ) ) << QString( "1977-08-09" );
    QTest::newRow( "time (from operator=)" ) << ( LiteralValue() = QTime( 17, 38, 2 ) ) << QString( "17:38:02Z" );
    QTest::newRow( "dateTime (from operator=)" ) << ( LiteralValue() = QDateTime( QDate( 1977, 8, 9 ), QTime( 17, 38, 2 ), Qt::UTC ) ) << QString( "1977-08-09T17:38:02Z" );
    QTest::newRow( "dateTime2 (from constructor)" ) << ( LiteralValue() = QDateTime( QDate( 1977, 8, 9 ), QTime( 17, 38, 2, 23 ), Qt::UTC ) ) << QString( "1977-08-09T17:38:02.023Z" );
    QTest::newRow( "boolean-false (from operator=)" ) << ( LiteralValue() = false ) << QString( "false" );
    QTest::newRow( "boolean-true (from operator=)" ) << ( LiteralValue() = true ) << QString( "true" );
    QTest::newRow( "binaryData (from operator=)" ) << ( LiteralValue() = QByteArray( "Hello World" ) ) << QString::fromAscii( QByteArray( "Hello World" ).toBase64() );
}


void SopranoLiteralTest::testToString()
{
    QFETCH( LiteralValue, value );
    QFETCH( QString, stringValue );

    QCOMPARE( value.toString(), stringValue );
}


QTEST_MAIN(SopranoLiteralTest)

#include "sopranoliteraltest.moc"
