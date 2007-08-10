/*
 *
 * $Id: sourceheader 511311 2006-02-19 14:51:05Z trueg $
 *
 * This file is part of the Nepomuk KDE project.
 * Copyright (C) 2006-2007 Sebastian Trueg <trueg@kde.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * See the file "COPYING" for the exact licensing terms.
 */

#include "wstringtest.h"
#include "../soprano/index/wstring.h"

#include <wchar.h>

#include <QtTest/QTest>
#include <QtCore/QDebug>



void WStringTest::testConversion()
{
    QString qs1( "Hello World" );
    WString ws1( L"Hello World" );
    WString ws2( qs1 );

    qDebug() << "qs1: " << qs1;
    qDebug() << "ws1( L\"Hello World\" ).toQString(): " << ws1.toQString() << " (length: " << ws1.length() << ")";
    qDebug() << "ws2( qs1 ).toQString(): " << ws2.toQString() << " (length: " << ws2.length() << ")";

    QCOMPARE( ws1.toQString(), qs1 );
    QCOMPARE( ws2.toQString(), qs1 );
    QVERIFY( !wcscmp( ws1.data(), ws2.data() ) );
    QCOMPARE( ws1.length(), qs1.length() );
    QCOMPARE( ws2.length(), qs1.length() );

    for ( int i = 0; i < 100; ++i ) {
        QString s = QString::number( i ).rightJustified( 10, 'x' );
        if ( s != WString( s ).toQString() ) {
            qDebug() << "strings differ: " << s << "(QString length" << s.length() << "WString length" << WString( s ).length() << ")";
        }
        QCOMPARE( s, WString( s ).toQString() );
    }
}

QTEST_MAIN( WStringTest );

#include "wstringtest.moc"
