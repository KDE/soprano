/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2007 Daniele Galdi <daniele.galdi@gmail.com>
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

#include <QtTest>
#include <QDebug>

#include "soprano/locator.h"
#include "soprano/pluginmanager.h"
#include "soprano/query/query.h"
#include "soprano/query/queryparser.h"

#include "queryapitest.h"

void QueryAPITest::initTestCase()
{
    m_manager = Soprano::PluginManager::instance();
}

void QueryAPITest::cleanupTestCase()
{
    delete m_manager;
}

void QueryAPITest::syntaxErrorFounded( const Soprano::Error::Locator &locator, const QString& message )
{
    qDebug() << "(Soprano::Query::Parser)" << message << locator.line() << locator.column();
}

void QueryAPITest::testQuery_1()
{
    const Soprano::Query::Parser *parser = m_manager->discoverQueryParserByName( "rasqal" );
    connect( parser, SIGNAL( syntaxError(const Soprano::Error::Locator &, const QString&) ),
             this, SLOT( syntaxErrorFounded(const Soprano::Error::Locator &, const QString& )));

    Soprano::Query::Query query = parser->parseQuery(
         "SELECT ?title WHERE\
         {\
            ?title <test> ?c .\
            ?t <test2> \"ciao\" .\
         }"
         , Soprano::Query::QUERY_LANGUAGE_SPARQL
    );
    
    disconnect( parser, SIGNAL( syntaxError(const Soprano::Error::Locator &, const QString&) ),
             this, SLOT( syntaxErrorFounded(const Soprano::Error::Locator &, const QString& )));
}

void QueryAPITest::testQuery_2()
{
    const Soprano::Query::Parser *parser = m_manager->discoverQueryParserByName( "rasqal" );
    connect( parser, SIGNAL( syntaxError(const Soprano::Error::Locator &, const QString&) ),
             this, SLOT( syntaxErrorFounded(const Soprano::Error::Locator &, const QString& )));

    Soprano::Query::Query query = parser->parseQuery(
        "PREFIX  dc:\
        SELECT  ?title\
        WHERE   {  dc:title ?title }"
        , Soprano::Query::QUERY_LANGUAGE_SPARQL
    );
    
    disconnect( parser, SIGNAL( syntaxError(const Soprano::Error::Locator &, const QString&) ),
             this, SLOT( syntaxErrorFounded(const Soprano::Error::Locator &, const QString& )));
}

void QueryAPITest::testQuery_3()
{
    const Soprano::Query::Parser *parser = m_manager->discoverQueryParserByName( "rasqal" );
    connect( parser, SIGNAL( syntaxError(const Soprano::Error::Locator &, const QString&) ),
             this, SLOT( syntaxErrorFounded(const Soprano::Error::Locator &, const QString& )));

    Soprano::Query::Query query = parser->parseQuery(
        "PREFIX  dc:\
         PREFIX  :\
         SELECT  $title\
         WHERE   { :book1  dc:title  $title }"
         , Soprano::Query::QUERY_LANGUAGE_SPARQL
    );
    
    disconnect( parser, SIGNAL( syntaxError(const Soprano::Error::Locator &, const QString&) ),
             this, SLOT( syntaxErrorFounded(const Soprano::Error::Locator &, const QString& )));
}

void QueryAPITest::testQuery_4()
{
    const Soprano::Query::Parser *parser = m_manager->discoverQueryParserByName( "rasqal" );
    connect( parser, SIGNAL( syntaxError(const Soprano::Error::Locator &, const QString&) ),
             this, SLOT( syntaxErrorFounded(const Soprano::Error::Locator &, const QString& )));

    Soprano::Query::Query query = parser->parseQuery(
        "BASE\
        PREFIX  dcore:\
        SELECT  ?title\
        WHERE   {  dcore:title ?title }"
        , Soprano::Query::QUERY_LANGUAGE_SPARQL
    );
    
    disconnect( parser, SIGNAL( syntaxError(const Soprano::Error::Locator &, const QString&) ),
             this, SLOT( syntaxErrorFounded(const Soprano::Error::Locator &, const QString& )));
}

void QueryAPITest::testSyntaxError()
{
    const Soprano::Query::Parser *parser = m_manager->discoverQueryParserByName( "rasqal" );
    connect( parser, SIGNAL( syntaxError(const Soprano::Error::Locator &, const QString&) ),
             this, SLOT( syntaxErrorFounded(const Soprano::Error::Locator &, const QString& )));

    parser->parseQuery( " select ?a where ; broken query ", Soprano::Query::QUERY_LANGUAGE_SPARQL );

    disconnect( parser, SIGNAL( syntaxError(const Soprano::Error::Locator &, const QString&) ),
             this, SLOT( syntaxErrorFounded(const Soprano::Error::Locator &, const QString& )));
}

QTEST_MAIN(QueryAPITest)

#include "queryapitest.moc"
