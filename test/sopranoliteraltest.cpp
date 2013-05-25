/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2007-2011 Sebastian Trueg <trueg@kde.org>
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
using namespace Soprano::Vocabulary;

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
    QTest::newRow( "binaryData (from constructor)" ) << LiteralValue( QByteArray( "Hello World" ) ) << QString::fromLatin1( QByteArray( "Hello World" ).toBase64() );

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
    QTest::newRow( "binaryData (from operator=)" ) << ( LiteralValue() = QByteArray( "Hello World" ) ) << QString::fromLatin1( QByteArray( "Hello World" ).toBase64() );
}


void SopranoLiteralTest::testToString()
{
    QFETCH( LiteralValue, value );
    QFETCH( QString, stringValue );

    QCOMPARE( value.toString(), stringValue );
}


void SopranoLiteralTest::testFromVariant_data()
{
    QTest::addColumn<QVariant>( "value" );
    QTest::addColumn<QUrl>( "dataType" );
    QTest::addColumn<QVariant>( "convertedValue" );

    const QDateTime dt = QDateTime::currentDateTime();

    // simple conversions 1-to-1
    QTest::newRow( "simple - xsd:int" ) << QVariant(42) << XMLSchema::xsdInt() << QVariant(42);
    QTest::newRow( "simple - xsd:integer" ) << QVariant(42) << XMLSchema::integer() << QVariant(42);
    QTest::newRow( "simple - xsd:negativeInteger" ) << QVariant(-42) << XMLSchema::negativeInteger() << QVariant(-42);
    QTest::newRow( "simple - xsd:decimal" ) << QVariant(42) << XMLSchema::decimal() << QVariant(42);
    QTest::newRow( "simple - xsd:short" ) << QVariant(42) << XMLSchema::xsdShort() << QVariant(42);
    QTest::newRow( "simple - xsd:nonNegativeInteger" ) << QVariant(uint(42)) << XMLSchema::nonNegativeInteger() << QVariant(uint(42));
    QTest::newRow( "simple - xsd:unsignedInt" ) << QVariant(uint(42)) << XMLSchema::unsignedInt() << QVariant(uint(42));
    QTest::newRow( "simple - xsd:unsignedShort" ) << QVariant(uint(42)) << XMLSchema::unsignedShort() << QVariant(uint(42));
    QTest::newRow( "simple - xsd:long" ) << QVariant(42) << XMLSchema::xsdLong() << QVariant(42);
    QTest::newRow( "simple - xsd:unsignedLong" ) << QVariant(uint(42)) << XMLSchema::unsignedLong() << QVariant(uint(42));
    QTest::newRow( "simple - xsd:boolean (true)" ) << QVariant(true) << XMLSchema::boolean() << QVariant(true);
    QTest::newRow( "simple - xsd:boolean (false)" ) << QVariant(false) << XMLSchema::boolean() << QVariant(false);
    QTest::newRow( "simple - xsd:double" ) << QVariant(double(42.5)) << XMLSchema::xsdDouble() << QVariant(double(42.5));
    QTest::newRow( "simple - xsd:float" ) << QVariant(float(42.5)) << XMLSchema::xsdFloat() << QVariant(float(42.5));
    QTest::newRow( "simple - xsd:string" ) << QVariant(QLatin1String("foobar")) << XMLSchema::string() << QVariant(QLatin1String("foobar"));
    QTest::newRow( "simple - xsd:date" ) << QVariant(dt.date()) << XMLSchema::date() << QVariant(dt.date());
    QTest::newRow( "simple - xsd:time" ) << QVariant(dt.time()) << XMLSchema::time() << QVariant(dt.time());
    QTest::newRow( "simple - xsd:dateTime" ) << QVariant(dt) << XMLSchema::dateTime() << QVariant(dt);

    // anything to int
    QTest::newRow( "uint -> xsd:int" ) << QVariant(42) << XMLSchema::xsdInt() << QVariant(42);
    QTest::newRow( "string -> xsd:int" ) << QVariant("42") << XMLSchema::xsdInt() << QVariant(42);

    // anything to integer
    QTest::newRow( "uint -> xsd:integer" ) << QVariant(42) << XMLSchema::integer() << QVariant(42);
    QTest::newRow( "string -> xsd:integer" ) << QVariant("42") << XMLSchema::integer() << QVariant(42);

    // anything to negativeInteger
    QTest::newRow( "string -> xsd:integer" ) << QVariant("-42") << XMLSchema::negativeInteger() << QVariant(-42);

    // anything to decimal
    QTest::newRow( "uint -> xsd:decimal" ) << QVariant(42) << XMLSchema::decimal() << QVariant(42);
    QTest::newRow( "string -> xsd:decimal" ) << QVariant("42") << XMLSchema::decimal() << QVariant(42);

    // anything to short
    QTest::newRow( "uint -> xsd:short" ) << QVariant(42) << XMLSchema::xsdShort() << QVariant(42);
    QTest::newRow( "string -> xsd:short" ) << QVariant("42") << XMLSchema::xsdShort() << QVariant(42);

    // anything to unsignedInteger
    QTest::newRow( "int -> xsd:nonNegativeInteger" ) << QVariant(42) << XMLSchema::nonNegativeInteger() << QVariant(uint(42));
    QTest::newRow( "string -> xsd:nonNegativeInteger" ) << QVariant("42") << XMLSchema::nonNegativeInteger() << QVariant(uint(42));

    // anything to unsignedInt
    QTest::newRow( "int -> xsd:unsignedInt" ) << QVariant(42) << XMLSchema::unsignedInt() << QVariant(uint(42));
    QTest::newRow( "string -> xsd:unsignedInt" ) << QVariant("42") << XMLSchema::unsignedInt() << QVariant(uint(42));

    // anything to unsignedShort
    QTest::newRow( "int -> xsd:unsignedShort" ) << QVariant(42) << XMLSchema::unsignedShort() << QVariant(uint(42));
    QTest::newRow( "string -> xsd:unsignedShort" ) << QVariant("42") << XMLSchema::unsignedShort() << QVariant(uint(42));

    // anything to long
    QTest::newRow( "int -> xsd:long" ) << QVariant(42) << XMLSchema::xsdLong() << QVariant(int(42));
    QTest::newRow( "uint -> xsd:long" ) << QVariant(uint(42)) << XMLSchema::xsdLong() << QVariant(int(42));
    QTest::newRow( "long -> xsd:long" ) << QVariant(int(42)) << XMLSchema::xsdLong() << QVariant(int(42));
    QTest::newRow( "string -> xsd:long" ) << QVariant("42") << XMLSchema::xsdLong() << QVariant(int(42));

    // anything to unsignedLong
    QTest::newRow( "int -> xsd:unsignedLong" ) << QVariant(42) << XMLSchema::unsignedLong() << QVariant(42);
    QTest::newRow( "long -> xsd:unsignedLong" ) << QVariant(int(42)) << XMLSchema::unsignedLong() << QVariant(42);
    QTest::newRow( "uint -> xsd:unsignedLong" ) << QVariant(uint(42)) << XMLSchema::unsignedLong() << QVariant(42);
    QTest::newRow( "string -> xsd:unsignedLong" ) << QVariant("42") << XMLSchema::unsignedLong() << QVariant(42);

    // anything to boolean
    QTest::newRow( "int -> xsd:boolean (true)" ) << QVariant(1) << XMLSchema::boolean() << QVariant(true);
    QTest::newRow( "int -> xsd:boolean (false)" ) << QVariant(0) << XMLSchema::boolean() << QVariant(false);
    QTest::newRow( "uint -> xsd:boolean (true)" ) << QVariant(uint(1)) << XMLSchema::boolean() << QVariant(true);
    QTest::newRow( "uint -> xsd:boolean (false)" ) << QVariant(uint(0)) << XMLSchema::boolean() << QVariant(false);
    QTest::newRow( "string -> xsd:boolean (true)" ) << QVariant("true") << XMLSchema::boolean() << QVariant(true);
    QTest::newRow( "string -> xsd:boolean (false)" ) << QVariant("false") << XMLSchema::boolean() << QVariant(false);
    QTest::newRow( "string -> xsd:boolean (1)" ) << QVariant("1") << XMLSchema::boolean() << QVariant(true);
    QTest::newRow( "string -> xsd:boolean (0)" ) << QVariant("0") << XMLSchema::boolean() << QVariant(false);

    // anything to double
    QTest::newRow( "float -> xsd:double" ) << QVariant(float(42.5)) << XMLSchema::xsdDouble() << QVariant(double(42.5));
    QTest::newRow( "string -> xsd:double" ) << QVariant("42.5") << XMLSchema::xsdDouble() << QVariant(double(42.5));

    // anything to float
    QTest::newRow( "double -> xsd:float" ) << QVariant(double(42.5)) << XMLSchema::xsdFloat() << QVariant(double(42.5));
    QTest::newRow( "string -> xsd:float" ) << QVariant("42.5") << XMLSchema::xsdFloat() << QVariant(double(42.5));

    // anything to dateTime (we loose accuracy in toTIme_t(), thus, the weird conversion at the end)
    QTest::newRow( "int -> xsd:dateTime" ) << QVariant(dt.toTime_t()) << XMLSchema::dateTime() << QVariant(QDateTime::fromTime_t(dt.toTime_t()));

    // the special case - rdfs:Literal
    QTest::newRow("rdfs:Literal") << QVariant(QLatin1String("foobar")) << RDFS::Literal() << QVariant(QLatin1String("foobar"));

    // conversions which should fail and result in invalid LiteralValues
    QTest::newRow( "from-string-invalid - xsd:int" ) << QVariant(QLatin1String("foobar")) << XMLSchema::xsdInt() << QVariant();
    QTest::newRow( "from-string-invalid - xsd:integer" ) << QVariant(QLatin1String("foobar")) << XMLSchema::integer() << QVariant();
    QTest::newRow( "from-string-invalid - xsd:negativeInteger" ) << QVariant(QLatin1String("foobar")) << XMLSchema::negativeInteger() << QVariant();
    QTest::newRow( "from-string-invalid - xsd:decimal" ) << QVariant(QLatin1String("foobar")) << XMLSchema::decimal() << QVariant();
    QTest::newRow( "from-string-invalid - xsd:short" ) << QVariant(QLatin1String("foobar")) << XMLSchema::xsdShort() << QVariant();
    QTest::newRow( "from-string-invalid - xsd:nonNegativeInteger" ) << QVariant(QLatin1String("foobar")) << XMLSchema::nonNegativeInteger() << QVariant();
    QTest::newRow( "from-string-invalid - xsd:unsignedInt" ) << QVariant(QLatin1String("foobar")) << XMLSchema::unsignedInt() << QVariant();
    QTest::newRow( "from-string-invalid - xsd:unsignedShort" ) << QVariant(QLatin1String("foobar")) << XMLSchema::unsignedShort() << QVariant();
    QTest::newRow( "from-string-invalid - xsd:long" ) << QVariant(QLatin1String("foobar")) << XMLSchema::xsdLong() << QVariant();
    QTest::newRow( "from-string-invalid - xsd:unsignedLong" ) << QVariant(QLatin1String("foobar")) << XMLSchema::unsignedLong() << QVariant();
    QTest::newRow( "from-string-invalid - xsd:boolean (true)" ) << QVariant(QLatin1String("foobar")) << XMLSchema::boolean() << QVariant();
    QTest::newRow( "from-string-invalid - xsd:boolean (false)" ) << QVariant(QLatin1String("foobar")) << XMLSchema::boolean() << QVariant();
    QTest::newRow( "from-string-invalid - xsd:double" ) << QVariant(QLatin1String("foobar")) << XMLSchema::xsdDouble() << QVariant();
    QTest::newRow( "from-string-invalid - xsd:float" ) << QVariant(QLatin1String("foobar")) << XMLSchema::xsdFloat() << QVariant();
    QTest::newRow( "from-string-invalid - xsd:date" ) << QVariant(QLatin1String("foobar")) << XMLSchema::date() << QVariant();
    QTest::newRow( "from-string-invalid - xsd:time" ) << QVariant(QLatin1String("foobar")) << XMLSchema::time() << QVariant();
    QTest::newRow( "from-string-invalid - xsd:dateTime" ) << QVariant(QLatin1String("foobar")) << XMLSchema::dateTime() << QVariant();

    QTest::newRow( "invalid-type" ) << QVariant(QLatin1String("foobar")) << QUrl() << QVariant(QLatin1String("foobar"));
    QTest::newRow( "invalid-value" ) << QVariant() << XMLSchema::integer() << QVariant();
}


void SopranoLiteralTest::testFromVariant()
{
    QFETCH( QVariant, value );
    QFETCH( QUrl, dataType );
    QFETCH( QVariant, convertedValue );

    const LiteralValue v = LiteralValue::fromVariant( value, dataType );
    if(convertedValue.isValid()) {
        QCOMPARE(v.variant(), convertedValue);
        if(dataType.isValid())
            QCOMPARE(v.dataTypeUri(), dataType);
    }
    else {
        QVERIFY(!v.isValid());
    }
}

QTEST_MAIN(SopranoLiteralTest)

