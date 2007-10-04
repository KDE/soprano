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

#include "cluceneindextest.h"
#include "stringpool.h"

#include <QtTest/QTest>
#include <QtCore/QDebug>
#include <QtCore/QProcess>
#include <QtCore/QDir>

#include "../soprano/soprano.h"
#include "../index/indexfiltermodel.h"
#include "../index/cluceneindex.h"
#include "../index/tstring.h"

#include <CLucene.h>

using namespace Soprano;
using namespace Soprano::Index;


void IndexTest::init()
{
    // create any memory model
    QList<BackendSetting> settings;
    settings.append( BackendSetting( BackendOptionStorageMemory ) );
    m_model = createModel( settings );

    QVERIFY( m_model != 0 );

    // remove previous testdata
    QProcess::execute( "rm", QString( "-rf,/tmp/sopranoindextest" ).split( "," ) );
    QDir( "/tmp" ).mkdir( "sopranoindextest" );

    m_indexModel = new IndexFilterModel( "/tmp/sopranoindextest", m_model );
}


void IndexTest::cleanup()
{
    delete m_indexModel;
    delete m_model;
}


void IndexTest::testAddStatement()
{
    Statement s1( QUrl( "http://soprano.sf.net/test#A" ),
                  QUrl( "http://soprano.sf.net/test#valueX" ),
                  LiteralValue( "Hello World" ) );
    Statement s2( QUrl( "http://soprano.sf.net/test#A" ),
                  QUrl( "http://soprano.sf.net/test#valueY" ),
                  LiteralValue( "Wurst" ) );

    QVERIFY( m_indexModel->addStatement( s1 ) == Error::ErrorNone );
    QVERIFY( m_indexModel->containsAnyStatement( s1 ) );

    // now check if the statement was properly indexed
    Iterator<QueryHit> hits = m_indexModel->index()->search( "Hello World" );
    QVERIFY( hits.next() );
    QCOMPARE( hits.current().resource(), s1.subject() );
    QVERIFY( !hits.next() );
    hits.close();

    QVERIFY( m_indexModel->addStatement( s2 ) == Error::ErrorNone );
    QVERIFY( m_indexModel->containsAnyStatement( s2 ) );

    // check the previous one again to make sure the index is not borked
    hits = m_indexModel->index()->search( "Hello World" );
    QVERIFY( hits.next() );
    QCOMPARE( hits.current().resource(), s1.subject() );
    QVERIFY( !hits.next() );
    hits.close();

    // and check for the second statement
    hits = m_indexModel->index()->search( "Wurst" );
    QVERIFY( hits.next() );
    QCOMPARE( hits.current().resource(), s2.subject() );
    QVERIFY( !hits.next() );
    hits.close();
}


void IndexTest::testRemoveStatement()
{
    QTextStream s( stderr );

    // first add something
    Statement s1( QUrl( "http://soprano.sf.net/test#A" ),
                  QUrl( "http://soprano.sf.net/test#valueX" ),
                  LiteralValue( "Hello World" ) );
    Statement s2( QUrl( "http://soprano.sf.net/test#A" ),
                  QUrl( "http://soprano.sf.net/test#valueY" ),
                  LiteralValue( "Wurst" ) );

    QVERIFY( m_indexModel->addStatement( s1 ) == Error::ErrorNone );
    QVERIFY( m_indexModel->containsAnyStatement( s1 ) );
    m_indexModel->index()->dump( s );
    QVERIFY( m_indexModel->addStatement( s2 ) == Error::ErrorNone );
    QVERIFY( m_indexModel->containsAnyStatement( s2 ) );

    m_indexModel->index()->dump( s );

    // now remove one of them
    QVERIFY( m_indexModel->removeStatement( s1 ) == Error::ErrorNone );

    m_indexModel->index()->dump( s );

    // now make sure that the index does not contain it anymore
    Iterator<QueryHit> hits = m_indexModel->index()->search( "Hello World" );
    QVERIFY( !hits.next() );
}


void IndexTest::testUriEncoding_data()
{
    QTest::addColumn<QUrl>( "uri" );
    QTest::addColumn<QUrl>( "predicate" );
    QTest::addColumn<QString>( "value" );

    QString ns = "http://soprano.org/uriEncodinglTest#";

    // a simple URI
    QTest::newRow("plain") << QUrl(ns + "simple") << QUrl(ns + "plain") << QString( QLatin1String( "plain" ) );

    // with some white space
    QTest::newRow("withSpace") << QUrl(ns + "URI with space") << QUrl(ns + "withSpace") << QString( QLatin1String( "withSpace" ) );

    // unicode
    QTest::newRow("germanUmlauts") << QUrl( ns + StringPool::germanUmlauts() ) << QUrl(ns + "germanUmlauts") << StringPool::germanUmlauts();
    QTest::newRow("frenchAccents") << QUrl( ns + StringPool::frenchAccents() ) << QUrl(ns + "frenchAccents") << StringPool::frenchAccents();
    QTest::newRow("russianChars") << QUrl( ns + StringPool::russianChars() ) << QUrl(ns + "russianChars") << StringPool::russianChars();
}

void IndexTest::testUriEncoding()
{
    QFETCH( QUrl, uri );
    QFETCH( QUrl, predicate );
    QFETCH( QString, value );

    Statement s( uri, predicate, LiteralValue( value ) );

    QVERIFY( m_indexModel->index()->addStatement( s ) == Error::ErrorNone );

    // check if a search gets the proper URI back
    Iterator<QueryHit> it = m_indexModel->index()->search( value );
    QVERIFY( it.next() );
    QCOMPARE( uri, it.current().resource().uri() );
    QVERIFY( !it.next() );
}

QTEST_MAIN( IndexTest )

#include "cluceneindextest.moc"
