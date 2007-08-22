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

#include "soprano/query/query.h"
#include "soprano/query/queryparser.h"
#include "soprano/query/rasqalqueryparser.h"

#include "queryapitest.h"

void QueryAPITest::testQuery_1()
{
    Soprano::Query::Parser::QueryParser *parser = new Soprano::Query::Parser::RasqalQueryParser();

    Soprano::Query::QueryObject *obj = parser->parseQuery(
         "SELECT ?title WHERE\
         {\
            ?title .\
         }"
    );

    delete parser;
    delete obj;
}

void QueryAPITest::testQuery_2()
{
    Soprano::Query::Parser::QueryParser *parser = new Soprano::Query::Parser::RasqalQueryParser();

    Soprano::Query::QueryObject *obj = parser->parseQuery(
        "PREFIX  dc:\ 
        SELECT  ?title\
        WHERE   {  dc:title ?title }"
    );
    
    delete parser;
    delete obj;
}

void QueryAPITest::testQuery_3()
{
    Soprano::Query::Parser::QueryParser *parser = new Soprano::Query::Parser::RasqalQueryParser();

    Soprano::Query::QueryObject *obj = parser->parseQuery(
        "PREFIX  dc:\
         PREFIX  :\
         SELECT  $title\
         WHERE   { :book1  dc:title  $title }"
    );
    
    delete parser;
    delete obj;
}

void QueryAPITest::testQuery_4()
{
    Soprano::Query::Parser::QueryParser *parser = new Soprano::Query::Parser::RasqalQueryParser();

    Soprano::Query::QueryObject *obj = parser->parseQuery(
        "BASE\    
        PREFIX  dcore:\  
        SELECT  ?title\
        WHERE   {  dcore:title ?title }"
    );

    delete parser;
    delete obj;
}

QTEST_MAIN(QueryAPITest)

#include "queryapitest.moc"
