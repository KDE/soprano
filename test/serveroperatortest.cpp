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
#include "../server/operators.h"

#include "../soprano/soprano.h"

#include <QtTest>

#include <QtCore/QByteArray>
#include <QtCore/QDataStream>

using namespace Soprano;

Q_DECLARE_METATYPE( Error::Locator )
Q_DECLARE_METATYPE( Error::Error )
Q_DECLARE_METATYPE( Error::ErrorCode )
Q_DECLARE_METATYPE( LiteralValue )
Q_DECLARE_METATYPE( Node )
Q_DECLARE_METATYPE( Statement )
Q_DECLARE_METATYPE( BindingSet )
Q_DECLARE_METATYPE( BackendSetting )


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
    QDataStream s( &data, QIODevice::WriteOnly );

    s << original;

    Error::Locator copy;
    QDataStream ss( data );
    ss >> copy;
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
    QDataStream s( &data, QIODevice::WriteOnly );

    // empty object
    s << error;

    Error::Error copy;
    QDataStream ss( data );
    ss >> copy;
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
    QDataStream s( &data, QIODevice::WriteOnly );

    s << errorCode;

    Error::ErrorCode copy;
    QDataStream ss( data );
    ss >> copy;
    QCOMPARE( errorCode, copy );
}


void ServerOperatorTest::testBackendSetting_data()
{
    QTest::addColumn<BackendSetting>( "setting" );

    QTest::newRow( "empty" ) << BackendSetting();
    QTest::newRow( "memory" ) << BackendSetting( BackendOptionStorageMemory );
    QTest::newRow( "inference" ) << BackendSetting( BackendOptionEnableInference );
    QTest::newRow( "dir" ) << BackendSetting( BackendOptionStorageDir, "/somedir" );
    QTest::newRow( "user-string" ) << BackendSetting( "user1", "user1value" );
    QTest::newRow( "user-int" ) << BackendSetting( "user1", 42 );
    QTest::newRow( "user-double" ) << BackendSetting( "user1", 42.42 );
}


void ServerOperatorTest::testBackendSetting()
{
    QFETCH(BackendSetting, setting);

    QByteArray data;
    QDataStream s( &data, QIODevice::WriteOnly );

    s << setting;

    BackendSetting copy;
    QDataStream ss( data );
    ss >> copy;
    QCOMPARE( setting.option(), copy.option() );
    QCOMPARE( setting.userOptionName(), copy.userOptionName() );
    QCOMPARE( setting.value(), copy.value() );
}


void ServerOperatorTest::testLiteralValue_data()
{
    QTest::addColumn<LiteralValue>( "original" );

    QTest::newRow( "empty" ) << LiteralValue();
    QTest::newRow( "int" ) << LiteralValue( 42 );
    QTest::newRow( "double" ) << LiteralValue( 42.42 );
    QTest::newRow( "string" ) << LiteralValue( "Hello World" );
    QTest::newRow( "userDataType" ) << LiteralValue::fromString( "Hello World", QUrl( "http://soprano.org/mytestType" ) );
}


void ServerOperatorTest::testLiteralValue()
{
    QFETCH(LiteralValue, original);

    QByteArray data;
    QDataStream s( &data, QIODevice::WriteOnly );

    s << original;

    LiteralValue copy;
    QDataStream ss( data );
    ss >> copy;
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
    QDataStream s( &data, QIODevice::WriteOnly );

    s << original;

    Node copy;
    QDataStream ss( data );
    ss >> copy;
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
    QDataStream s( &data, QIODevice::WriteOnly );

    s << original;

    Statement copy;
    QDataStream ss( data );
    ss >> copy;
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
    QDataStream s( &data, QIODevice::WriteOnly );

    s << original;

    BindingSet copy;
    QDataStream ss( data );
    ss >> copy;
    foreach( QString name, original.bindingNames() ) {
        QVERIFY( copy.contains( name ) );
        QCOMPARE( original[name], copy[name] );
    }
}

QTEST_MAIN( ServerOperatorTest )

#include "serveroperatortest.moc"
