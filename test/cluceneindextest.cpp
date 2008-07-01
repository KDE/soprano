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
#include <QtCore/QUuid>

#include "../soprano/soprano.h"
#include "../index/indexfiltermodel.h"
#include "../index/cluceneindex.h"
#include "../index/tstring.h"

#include <CLucene.h>

using namespace Soprano;
using namespace Soprano::Index;


static QUrl createRandomUri()
{
    // FIXME: check if the uri already exists
    QString uid = QUuid::createUuid().toString();
    uid = uid.mid( 1, uid.length()-2 );
    return QUrl( "http://soprano.org/test#" + uid );
}


static QList<Statement> createTestData( const Statement& s, int num )
{
    QList<Statement> sl;
    for( int i = 0; i < num; ++i ) {
        sl.append( Statement( s.subject().isEmpty() ? Node( createRandomUri() ) : s.subject(),
                              s.predicate().isEmpty() ? Node( createRandomUri() ) : s.predicate(),
                              s.object().isEmpty() ? Node( LiteralValue( createRandomUri().toString() ) ) : s.object(),
                              s.context() ) );
    }
    return sl;
}


void IndexTest::init()
{
    // remove previous testdata
    QProcess::execute( "rm", QString( "-rf,/tmp/sopranoindextest" ).split( "," ) );
    QDir( "/tmp" ).mkdir( "sopranoindextest" );
    QDir( "/tmp" ).mkpath( "sopranoindextest/index" );

    // create any memory model
    QList<BackendSetting> settings;
    settings.append( BackendSetting( BackendOptionStorageDir, "/tmp/sopranoindextest" ) );
    m_model = createModel( settings );

    QVERIFY( m_model != 0 );


    m_index = new CLuceneIndex();
    QVERIFY( m_index->open( "/tmp/sopranoindextest/index", true ) );
    m_indexModel = new IndexFilterModel( m_index, m_model );
}


void IndexTest::cleanup()
{
    delete m_indexModel;
    delete m_index;
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
    Statement s3( QUrl( "http://soprano.sf.net/test#B" ),
                  QUrl( "http://soprano.sf.net/test#valueX" ),
                  LiteralValue( "Hello World" ) );
    Statement s4( QUrl( "http://soprano.sf.net/test#B" ),
                  QUrl( "http://soprano.sf.net/test#valueY" ),
                  LiteralValue( "Wurst" ) );
    Statement s5( QUrl( "http://soprano.sf.net/test#B" ),
                  QUrl( "http://soprano.sf.net/test#valueZ" ),
                  LiteralValue( 42 ) );

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


    // now add a new resource
    QVERIFY( m_indexModel->addStatement( s3 ) == Error::ErrorNone );
    QVERIFY( m_indexModel->addStatement( s4 ) == Error::ErrorNone );
    QVERIFY( m_indexModel->containsStatement( s3 ) );
    QVERIFY( m_indexModel->containsStatement( s4 ) );

    // and make sure we get both resources in a query
    hits = m_indexModel->index()->search( "Wurst" );
    QVERIFY( hits.next() );
    QVERIFY( hits.next() );
    QVERIFY( !hits.next() );

    QVERIFY( m_indexModel->addStatement( s5 ) == Error::ErrorNone );

    QTextStream s( stderr );
    m_indexModel->index()->dump(s);

    hits = m_indexModel->index()->search( "42" );
    QVERIFY( hits.next() );
    QCOMPARE( hits.current().resource(), s5.subject() );
    QVERIFY( !hits.next() );

    m_indexModel->index()->clear();
    QCOMPARE( m_indexModel->index()->resourceCount(), 0 );

    m_indexModel->rebuildIndex();

    hits = m_indexModel->index()->search( "42" );
    QVERIFY( hits.next() );
    QCOMPARE( hits.current().resource(), s5.subject() );
    QVERIFY( !hits.next() );
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
//    m_indexModel->index()->dump( s );
    QVERIFY( m_indexModel->addStatement( s2 ) == Error::ErrorNone );
    QVERIFY( m_indexModel->containsAnyStatement( s2 ) );

//    m_indexModel->index()->dump( s );

    // now remove one of them
    QVERIFY( m_indexModel->removeStatement( s1 ) == Error::ErrorNone );

//    m_indexModel->index()->dump( s );

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

void IndexTest::testMassAddStatement()
{
//     QList<Statement> sl = createTestData( Statement( createRandomUri(), Node(), Node() ), 100 );
//     sl += createTestData( Statement( createRandomUri(), Node(), Node() ), 100 );
//     sl += createTestData( Statement( createRandomUri(), Node(), Node() ), 100 );

//     Q_FOREACH( Statement s, sl ) {
//         QVERIFY( m_indexModel->addStatement( s ) == Error::ErrorNone );
//     }
}


QTEST_MAIN( IndexTest )

#include "cluceneindextest.moc"
