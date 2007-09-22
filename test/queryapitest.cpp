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

#include <QString>
#include <QtTest>
#include <QDebug>

#include "soprano/soprano.h"
#include "soprano/locator.h"
#include "soprano/pluginmanager.h"
#include "soprano/query/query.h"
#include "soprano/query/queryparser.h"

#include "queryapitest.h"

using namespace Soprano;


/*
 * TEST Visitor
 */

class TestVisitor : public Soprano::Query::ExpressionVisitor {
public:
    QString output;

    void visit( const Soprano::Query::Not *expression )
    {
    //    output << " Not ";
    }

    void visit( const Soprano::Query::Negate *expression )
    {
    //    output << " -";
    }

    void visit( const Soprano::Query::IsBound *expression )
    {
    //    output << " isBound( ";
    //    expression->variable();
    //    output << " ) ";
    }

    void visit( const Soprano::Query::IsIRI *expression )
    {
    //    output << " isIRI( ";
    //    expression->rterm();
    }

    void visit( const Soprano::Query::IsBlank *expression )
    {
    }

    void visit( const Soprano::Query::IsLiteral *expression )
    {
    }

    void visit( const Soprano::Query::StringValue *expression )
    {
    }

    void visit( const Soprano::Query::NumericalValue *expression )
    {
    }

    void visit( const Soprano::Query::LangValue *expression )
    {
    }

    void visit( const Soprano::Query::DataTypeValue *expression )
    {
    }

    void visit( const Soprano::Query::LogicAnd *expression )
    {
        qDebug() << "AND {";
        for ( int i = 0; i < expression->count(); ++i ) {
            expression->condition( i )->accept( this );
        }
        qDebug() << "}";
    }

    void visit( const Soprano::Query::LogicOr *expression )
    {
        qDebug() << "OR {";
        for ( int i = 0; i < expression->count(); ++i ) {
            expression->condition( i )->accept( this );
        }
        qDebug() << "}";
    }

    void visit( const Soprano::Query::NumericalEqual *expression )
    {
        qDebug() << "(";
        expression->first()->accept( this );
        qDebug() << "==";
        expression->second()->accept( this );
        qDebug() << ")";
    }

    void visit( const Soprano::Query::NumericalNotEqual *expression )
    {
    }

    void visit( const Soprano::Query::StringEqual *expression )
    {
    }

    void visit( const Soprano::Query::StringNotEqual *expression )
    {
    }

    void visit( const Soprano::Query::DateTimeEqual *expression )
    {
    }

    void visit( const Soprano::Query::DateTimeNotEqual *expression )
    {
    }

    void visit( const Soprano::Query::Numerical *expression )
    {
    }

    void visit( const Soprano::Query::NumericalLessThan *expression )
    {
    }

    void visit( const Soprano::Query::NumericalGreaterThan *expression )
    {
    }

    void visit( const Soprano::Query::NumericalLessThanEqual *expression )
    {
    }

    void visit( const Soprano::Query::NumericalGreaterThanEqual *expression )
    {
    }

    void visit( const Soprano::Query::String *expression )
    {
    }

    void visit( const Soprano::Query::StringLessThan *expression )
    {
    }

    void visit( const Soprano::Query::StringGreaterThan *expression )
    {
    }

    void visit( const Soprano::Query::StringLessThanEqual *expression )
    {
    }

    void visit( const Soprano::Query::StringGreaterThanEqual *expression )
    {
    }

    void visit( const Soprano::Query::DateTimeLessThan *expression )
    {
    }

    void visit( const Soprano::Query::DateTimeGreaterThan *expression )
    {
    }

    void visit( const Soprano::Query::DateTimeLessThanEqual *expression )
    {
    }

    void visit( const Soprano::Query::DateTimeGreaterThanEqual *expression )
    {
    }

    void visit( const Soprano::Query::NumericalMultiply *expression )
    {
    }

    void visit( const Soprano::Query::NumericalDivide *expression )
    {
    }

    void visit( const Soprano::Query::NumericalAdd *expression )
    {
    }

    void visit( const Soprano::Query::NumericalSubtract *expression )
    {
    }

    void visit( const Soprano::Query::RTermEqual *expression )
    {
    }

    void visit( const Soprano::Query::RTermNotEqual *expression )
    {
    }

    void visit( const Soprano::Query::LangMatches *expression )
    {
    }

    void visit( const Soprano::Query::Regexp *expression )
    {
    }

    void print( const QString& label, const Soprano::Query::RTerm* rterm )
    {
       if ( rterm->isNode() )
        {
            const Soprano::Query::Node* node = dynamic_cast<const Soprano::Query::Node*>(rterm);
            qDebug() << label << node->node().toString() << "NODE";
        }
        else
        {
            const Soprano::Query::Variable* var = dynamic_cast<const Soprano::Query::Variable*>(rterm);
            qDebug() << label << var->name() << "VARIABLE";
        }
    }

    void visit( const Soprano::Query::TriplePattern *triplePattern )
    {
        const Soprano::Query::RTerm* subject = triplePattern->subject();
        const Soprano::Query::RTerm* predicate = triplePattern->predicate();
        const Soprano::Query::RTerm* object = triplePattern->object();

        qDebug() << "TriplePattern:";
    	print( "Subject: ", subject );
    	print( "Predicate: ", predicate);
    	print( "Object: ", object);
    }

    TestVisitor( Soprano::Query::Query* query )
    {
        qDebug() << "Query: ";

        QListIterator<Soprano::Query::Prefix> prefixiter(query->prefixes());
        while ( prefixiter.hasNext() )
        {
            Soprano::Query::Prefix prefix = prefixiter.next();
            qDebug() << "\tPrefix:" << prefix.prefix() << prefix.uri().toString();
        }

        Soprano::Query::QueryTerms queryTerms = query->queryTerms();
        Q_FOREACH( const Soprano::Query::Variable *variable, queryTerms.variables() )
        {
            qDebug() << "\tVariable:" << variable->name();
        }

        if ( query->condition() ) {
            query->condition()->accept( this );
        }
    }
};

/*
 * END
 */

void QueryAPITest::initTestCase()
{
    m_manager = Soprano::PluginManager::instance();
}

void QueryAPITest::cleanupTestCase()
{
    delete m_manager;
}

void QueryAPITest::testQuery_1()
{
    const Soprano::Query::Parser *parser = m_manager->discoverQueryParserByName( "rasqal" );

    Soprano::Query::Query query = parser->parseQuery(
        "PREFIX foaf:   <http://xmlns.com/foaf/0.1/>\
        SELECT ?x ?name\
        WHERE  {\
            ?x foaf:name ?name .\
            ?x foaf:desc ?anon\
        }"
        , Soprano::Query::QUERY_LANGUAGE_SPARQL
    );

    QVERIFY( !parser->lastError() );

    TestVisitor visitor( &query );
}

void QueryAPITest::testSyntaxError()
{
    const Soprano::Query::Parser *parser = m_manager->discoverQueryParserByName( "rasqal" );

    parser->parseQuery( " select ?a where ; broken query ", Soprano::Query::QUERY_LANGUAGE_SPARQL );
    QVERIFY( parser->lastError() );
    qDebug() << parser->lastError();
}


void QueryAPITest::testQueryCreation()
{
    Query::Query query( Query::Query::SELECT_QUERY );
    Query::Variable* vS = new Query::Variable( "r" );
    Query::Node* npP = new Query::Node( Vocabulary::RDF::type() );
    Query::Variable* vO = new Query::Variable( "p" );

    Query::TriplePattern* tp1 = new Query::TriplePattern( vS, npP, vO );
    query.setCondition( tp1 );

    Query::QueryTerms terms;
    // FIXME: problem: one would think that one could reuse the Variable
    // instances here. Well, one cannot. It leads to a crash!
    terms.addVariable( vS->clone() );
    terms.addVariable( vO->clone() );

    query.setQueryTerms( terms );

    TestVisitor visitor( &query );
}

QTEST_MAIN(QueryAPITest)

#include "queryapitest.moc"
