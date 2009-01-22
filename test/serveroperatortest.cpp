/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2008 Sebastian Trueg <trueg@kde.org>
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

#include "serveroperatortest.h"
#include "../server/datastream.h"

#include "../soprano/soprano.h"

#include <QtTest/QtTest>

#include <QtCore/QByteArray>
#include <QtCore/QBuffer>

using namespace Soprano;

Q_DECLARE_METATYPE( Error::Locator )
Q_DECLARE_METATYPE( Error::Error )
Q_DECLARE_METATYPE( Error::ErrorCode )
Q_DECLARE_METATYPE( LiteralValue )
Q_DECLARE_METATYPE( Node )
Q_DECLARE_METATYPE( Statement )
Q_DECLARE_METATYPE( BindingSet )
Q_DECLARE_METATYPE( BackendSetting )



void ServerOperatorTest::testInt32_data()
{
    QTest::addColumn<qint32>( "original" );

    QTest::newRow( "0" ) << qint32( 0 );
    QTest::newRow( "42" ) << qint32( 42 );
    QTest::newRow( "-42" ) << qint32( -42 );
    QTest::newRow( "97876999" ) << qint32( 97876999 );
    QTest::newRow( "-97876999" ) << qint32( -97876999 );
}


void ServerOperatorTest::testInt32()
{
    QFETCH(qint32, original);

    QByteArray data;
    QBuffer buffer( &data );
    buffer.open( QIODevice::ReadWrite );
    DataStream s( &buffer );

    s.writeInt32( original );

    qint32 copy;
    buffer.seek( 0 );
    QVERIFY( s.readInt32( copy ) );

    QCOMPARE( original, copy );
}


void ServerOperatorTest::testUnsignedInt32_data()
{
    QTest::addColumn<quint32>( "original" );

    QTest::newRow( "0" ) << quint32( 0 );
    QTest::newRow( "42" ) << quint32( 42 );
    QTest::newRow( "97876999" ) << quint32( 97876999 );
}


void ServerOperatorTest::testUnsignedInt32()
{
    QFETCH(quint32, original);

    QByteArray data;
    QBuffer buffer( &data );
    buffer.open( QIODevice::ReadWrite );
    DataStream s( &buffer );

    s.writeUnsignedInt32( original );

    quint32 copy;
    buffer.seek( 0 );
    QVERIFY( s.readUnsignedInt32( copy ) );

    QCOMPARE( original, copy );
}


void ServerOperatorTest::testUnsignedInt16_data()
{
    QTest::addColumn<quint16>( "original" );

    QTest::newRow( "0" ) << quint16( 0 );
    QTest::newRow( "42" ) << quint16( 42 );
    QTest::newRow( "97876999" ) << quint16( 97876999 );
}


void ServerOperatorTest::testUnsignedInt16()
{
    QFETCH(quint16, original);

    QByteArray data;
    QBuffer buffer( &data );
    buffer.open( QIODevice::ReadWrite );
    DataStream s( &buffer );

    s.writeUnsignedInt16( original );

    quint16 copy;
    buffer.seek( 0 );
    QVERIFY( s.readUnsignedInt16( copy ) );

    QCOMPARE( original, copy );
}


void ServerOperatorTest::testUnsignedInt8_data()
{
    QTest::addColumn<quint8>( "original" );

    QTest::newRow( "0" ) << quint8( 0 );
    QTest::newRow( "42" ) << quint8( 42 );
}


void ServerOperatorTest::testUnsignedInt8()
{
    QFETCH(quint8, original);

    QByteArray data;
    QBuffer buffer( &data );
    buffer.open( QIODevice::ReadWrite );
    DataStream s( &buffer );

    s.writeUnsignedInt8( original );

    quint8 copy;
    buffer.seek( 0 );
    QVERIFY( s.readUnsignedInt8( copy ) );

    QCOMPARE( original, copy );
}


void ServerOperatorTest::testBool_data()
{
    QTest::addColumn<bool>( "original" );

    QTest::newRow( "true" ) << true;
    QTest::newRow( "false" ) << false;
}


void ServerOperatorTest::testBool()
{
    QFETCH(bool, original);

    QByteArray data;
    QBuffer buffer( &data );
    buffer.open( QIODevice::ReadWrite );
    DataStream s( &buffer );

    s.writeBool( original );

    bool copy;
    buffer.seek( 0 );
    QVERIFY( s.readBool( copy ) );

    QCOMPARE( original, copy );
}


void ServerOperatorTest::testString_data()
{
    QTest::addColumn<QString>( "original" );

    QTest::newRow( "empty" ) << QString();
    QTest::newRow( "length 0" ) << QString("");
    QTest::newRow( "Hello World" ) << QString( "Hello World" );
}


void ServerOperatorTest::testString()
{
    QFETCH(QString, original);

    QByteArray data;
    QBuffer buffer( &data );
    buffer.open( QIODevice::ReadWrite );
    DataStream s( &buffer );

    s.writeString( original );

    QString copy;
    buffer.seek( 0 );
    QVERIFY( s.readString( copy ) );

    QCOMPARE( original, copy );
}


void ServerOperatorTest::testUrl_data()
{
    QTest::addColumn<QUrl>( "original" );

    QTest::newRow( "empty" ) << QUrl();
    QTest::newRow( "http" ) << QUrl("http://soprano.sf.net");
    QTest::newRow( "with spaces" ) << QUrl( "http://soprano.net/Hallo World" );
    QTest::newRow( "invalid" ) << QUrl("I am not a URL");
}


void ServerOperatorTest::testUrl()
{
    QFETCH(QUrl, original);

    QByteArray data;
    QBuffer buffer( &data );
    buffer.open( QIODevice::ReadWrite );
    DataStream s( &buffer );

    s.writeUrl( original );

    QUrl copy;
    buffer.seek( 0 );
    QVERIFY( s.readUrl( copy ) );

    QCOMPARE( original, copy );
}


void ServerOperatorTest::testLocator_data()
{
    QTest::addColumn<Error::Locator>( "original" );

    QTest::newRow( "empty" ) << Error::Locator();
    QTest::newRow( "simple" ) << Error::Locator( 10, 42 );
    QTest::newRow( "full" ) << Error::Locator( 10, 42, 7, "testfile" );
}


void ServerOperatorTest::testLocator()
{
    QFETCH(Error::Locator, original);

    QByteArray data;
    QBuffer buffer( &data );
    buffer.open( QIODevice::ReadWrite );
    DataStream s( &buffer );

    s.writeLocator( original );

    Error::Locator copy;
    buffer.seek( 0 );
    QVERIFY( s.readLocator( copy ) );

    QCOMPARE( original.line(), copy.line() );
    QCOMPARE( original.column(), copy.column() );
    QCOMPARE( original.byte(), copy.byte() );
    QCOMPARE( original.fileName(), copy.fileName() );
}


void ServerOperatorTest::testError_data()
{
    QTest::addColumn<Error::Error>( "error" );

    QTest::newRow( "empty" ) << Error::Error();
    QTest::newRow( "invalidArg" ) << Error::Error( "Some invalid arg", Error::ErrorInvalidArgument );
    QTest::newRow( "notSupported" ) << Error::Error( "Unsupported method", Error::ErrorNotSupported );
    QTest::newRow( "permission" ) << Error::Error( "Permission denied", Error::ErrorPermissionDenied );
    QTest::newRow( "parser" ) << Error::Error( Error::ParserError( Error::Locator( 10, 42, 7, "testfile" ) ) );
    QTest::newRow( "parser2" ) << Error::Error( Error::ParserError( Error::Locator( 10, 42 ) ) );
}


void ServerOperatorTest::testError()
{
    QFETCH(Error::Error, error);

    QByteArray data;
    QBuffer buffer( &data );
    buffer.open( QIODevice::ReadWrite );
    DataStream s( &buffer );

    // empty object
    s.writeError( error );

    Error::Error copy;
    buffer.seek( 0 );
    QVERIFY( s.readError( copy ) );

    QCOMPARE( error, copy );
}


void ServerOperatorTest::testErrorCode_data()
{
    QTest::addColumn<Error::ErrorCode>( "errorCode" );

    QTest::newRow( "noError" ) << Error::ErrorNone;
    QTest::newRow( "invalidArg" ) << Error::ErrorInvalidArgument;
    QTest::newRow( "notSupported" ) << Error::ErrorNotSupported;
    QTest::newRow( "permission" ) << Error::ErrorPermissionDenied;
    QTest::newRow( "parser" ) << Error::ErrorParsingFailed;
}


void ServerOperatorTest::testErrorCode()
{
    QFETCH(Error::ErrorCode, errorCode);

    QByteArray data;
    QBuffer buffer( &data );
    buffer.open( QIODevice::ReadWrite );
    DataStream s( &buffer );

    QVERIFY( s.writeErrorCode( errorCode ) );

    Error::ErrorCode copy;
    buffer.seek( 0 );
    QVERIFY( s.readErrorCode( copy ) );

    QCOMPARE( errorCode, copy );
}


// void ServerOperatorTest::testBackendSetting_data()
// {
//     QTest::addColumn<BackendSetting>( "setting" );

//     QTest::newRow( "empty" ) << BackendSetting();
//     QTest::newRow( "memory" ) << BackendSetting( BackendOptionStorageMemory );
//     QTest::newRow( "inference" ) << BackendSetting( BackendOptionEnableInference );
//     QTest::newRow( "dir" ) << BackendSetting( BackendOptionStorageDir, "/somedir" );
//     QTest::newRow( "user-string" ) << BackendSetting( "user1", "user1value" );
//     QTest::newRow( "user-int" ) << BackendSetting( "user1", 42 );
//     QTest::newRow( "user-double" ) << BackendSetting( "user1", 42.42 );
// }


// void ServerOperatorTest::testBackendSetting()
// {
//     QFETCH(BackendSetting, setting);

//     QByteArray data;
//     QBuffer buffer( &data );
//     buffer.open( QIODevice::ReadWrite );
//     DataStream s( &buffer );

//     s.writeBackendSetting( setting );

//     BackendSetting copy;
//     buffer.seek( 0 );
//    QVERIFY( s.readBackendSetting( copy ) );

//     QCOMPARE( setting.option(), copy.option() );
//     QCOMPARE( setting.userOptionName(), copy.userOptionName() );
//     QCOMPARE( setting.value(), copy.value() );
// }


void ServerOperatorTest::testLiteralValue_data()
{
    QTest::addColumn<LiteralValue>( "original" );

    QTest::newRow( "empty" ) << LiteralValue();
    QTest::newRow( "int" ) << LiteralValue( 42 );
    QTest::newRow( "double" ) << LiteralValue( 42.42 );
    QTest::newRow( "string" ) << LiteralValue( "Hello World" );
    QTest::newRow( "empty-string" ) << LiteralValue( QString() );
    QTest::newRow( "big-string" ) << LiteralValue( QString( 1000000, 'X' ) );
    QTest::newRow( "userDataType" ) << LiteralValue::fromString( "Hello World", QUrl( "http://soprano.org/mytestType" ) );
}


void ServerOperatorTest::testLiteralValue()
{
    QFETCH(LiteralValue, original);

    QByteArray data;
    QBuffer buffer( &data );
    buffer.open( QIODevice::ReadWrite );
    DataStream s( &buffer );

    QVERIFY( s.writeLiteralValue( original ) );

    LiteralValue copy;
    buffer.seek( 0 );
    QVERIFY( s.readLiteralValue( copy ) );

    QCOMPARE( original, copy );
}


void ServerOperatorTest::testNode_data()
{
    QTest::addColumn<Node>( "original" );

    QTest::newRow( "empty" ) << Node();
    QTest::newRow( "blank" ) << Node( QString( "nodeId" ) );
    QTest::newRow( "resource" ) << Node( QUrl( "http://soprano.org/mytestresource" ));
    QTest::newRow( "literal" ) << Node( LiteralValue( "Hello World" ) );
    QTest::newRow( "literal-with-lang" ) << Node( LiteralValue( "Hallo Welt" ), "de" );
}


void ServerOperatorTest::testNode()
{
    QFETCH(Node, original);

    QByteArray data;
    QBuffer buffer( &data );
    buffer.open( QIODevice::ReadWrite );
    DataStream s( &buffer );

    QVERIFY( s.writeNode( original ) );

    Node copy;
    buffer.seek( 0 );
    QVERIFY( s.readNode( copy ) );

    QCOMPARE( original, copy );
}


void ServerOperatorTest::testStatement_data()
{
    QTest::addColumn<Statement>( "original" );

    QTest::newRow( "empty" ) << Statement();
    QTest::newRow( "full" ) << Statement( QUrl( "http://soprano.org/mytestresource" ),
                                          Vocabulary::RDFS::label(),
                                          LiteralValue( "Hello World" ),
                                          QUrl( "http://soprano.org/mytestcontext" ) );
    QTest::newRow( "no-context" ) << Statement( QUrl( "http://soprano.org/mytestresource" ),
                                                Vocabulary::RDFS::label(),
                                                LiteralValue( "Hello World" ) );
    QTest::newRow( "subject" ) << Statement( QUrl( "http://soprano.org/mytestresource" ), Node(), Node() );
    QTest::newRow( "predicate" ) << Statement( Node(), Vocabulary::RDFS::label(), Node() );
    QTest::newRow( "object" ) << Statement( Node(), Node(), QUrl( "http://soprano.org/mytestresource" ) );
    QTest::newRow( "context" ) << Statement( Node(), Node(), Node(), QUrl( "http://soprano.org/mytestcontext" ) );
}


void ServerOperatorTest::testStatement()
{
    QFETCH(Statement, original);

    QByteArray data;
    QBuffer buffer( &data );
    buffer.open( QIODevice::ReadWrite );
    DataStream s( &buffer );

    QVERIFY( s.writeStatement( original ) );

    Statement copy;
    buffer.seek( 0 );
    QVERIFY( s.readStatement( copy ) );

    QCOMPARE( original, copy );
}


void ServerOperatorTest::testBinding_data()
{
    QTest::addColumn<BindingSet>( "original" );

    QTest::newRow( "empty" ) << BindingSet();
    BindingSet set;
    set.insert( "val1", QUrl( "http://soprano.org/mytestresource" ) );
    set.insert( "val2", Vocabulary::RDFS::label() );
    set.insert( "val3", LiteralValue( "Hello World" ) );
    QTest::newRow( "full" ) << set;
}


void ServerOperatorTest::testBinding()
{
    QFETCH(BindingSet, original);

    QByteArray data;
    QBuffer buffer( &data );
    buffer.open( QIODevice::ReadWrite );
    DataStream s( &buffer );

    QVERIFY( s.writeBindingSet( original ) );

    BindingSet copy;
    buffer.seek( 0 );
    QVERIFY( s.readBindingSet( copy ) );

    foreach( QString name, original.bindingNames() ) {
        QVERIFY( copy.contains( name ) );
        QCOMPARE( original[name], copy[name] );
    }
}

QTEST_MAIN( ServerOperatorTest )

#include "serveroperatortest.moc"
