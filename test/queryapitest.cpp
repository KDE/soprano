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

#include "soprano/node.h"
#include "soprano/locator.h"
#include "soprano/pluginmanager.h"
#include "soprano/query/query.h"
#include "soprano/query/queryparser.h"

#include "queryapitest.h"

/*
 * TEST Visitor
 */

class TestVisitor : public Soprano::Query::ExpressionVisitor {
public:
    QString output;    

    void visit( Soprano::Query::Not *expression )
    {
    //    output << " Not ";
    }

    void visit( Soprano::Query::Negate *expression )
    {
    //    output << " -";
    }

    void visit( Soprano::Query::IsBound *expression )
    {
    //    output << " isBound( ";
    //    expression->variable();
    //    output << " ) ";
    }

    void visit( Soprano::Query::IsIRI *expression )
    {
    //    output << " isIRI( ";
    //    expression->rterm();
    }

    void visit( Soprano::Query::IsBlank *expression )
    {
    }

    void visit( Soprano::Query::IsLiteral *expression )
    {
    }

    void visit( Soprano::Query::StringValue *expression )
    {
    }

    void visit( Soprano::Query::LangValue *expression )
    {
    }

    void visit( Soprano::Query::DataTypeValue *expression )
    {
    }

    void visit( Soprano::Query::LogicAnd *expression )
    {
    }

    void visit( Soprano::Query::LogicOr *expression )
    {
    }

    void visit( Soprano::Query::NumericalEqual *expression )
    {
    }

    void visit( Soprano::Query::NumericalNotEqual *expression )
    {
    }
   
    void visit( Soprano::Query::StringEqual *expression )
    {
    }
   
    void visit( Soprano::Query::StringNotEqual *expression )
    {
    }

    void visit( Soprano::Query::DateTimeEqual *expression )
    {
    }

    void visit( Soprano::Query::DateTimeNotEqual *expression )
    {
    }

    void visit( Soprano::Query::Numerical *expression )
    {
    }

    void visit( Soprano::Query::NumericalLessThan *expression )
    {
    }

    void visit( Soprano::Query::NumericalGreaterThan *expression )
    {
    }

    void visit( Soprano::Query::NumericalLessThanEqual *expression )
    {
    }

    void visit( Soprano::Query::NumericalGreaterThanEqual *expression )
    {
    }

    void visit( Soprano::Query::String *expression )
    {
    }

    void visit( Soprano::Query::StringLessThan *expression )
    {
    }

    void visit( Soprano::Query::StringGreaterThan *expression )
    {
    }

    void visit( Soprano::Query::StringLessThanEqual *expression )
    {
    }

    void visit( Soprano::Query::StringGreaterThanEqual *expression )
    {
    }

    void visit( Soprano::Query::DateTimeLessThan *expression )
    {
    }

    void visit( Soprano::Query::DateTimeGreaterThan *expression )
    {
    }

    void visit( Soprano::Query::DateTimeLessThanEqual *expression )
    {
    }

    void visit( Soprano::Query::DateTimeGreaterThanEqual *expression )
    {
    }

    void visit( Soprano::Query::NumericalMultiply *expression )
    {
    }

    void visit( Soprano::Query::NumericalDivide *expression )
    {
    }

    void visit( Soprano::Query::NumericalAdd *expression )
    {
    }

    void visit( Soprano::Query::NumericalSubtract *expression )
    {
    }

    void visit( Soprano::Query::RTermEqual *expression )
    {
    }

    void visit( Soprano::Query::RTermNotEqual *expression )
    {
    }

    void visit( Soprano::Query::LangMatches *expression )
    {
    }

    void visit( Soprano::Query::Regexp *expression )
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

    void visit( Soprano::Query::TriplePattern *triplePattern )
    {
        const Soprano::Query::RTerm* subject = triplePattern->subject();
        const Soprano::Query::RTerm* predicate = triplePattern->predicate();
        const Soprano::Query::RTerm* object = triplePattern->object();

        qDebug() << "TriplePattern:";
    	print( "Subject: ", subject );   
    	print( "Predicate: ", predicate);   
    	print( "Object: ", object);   
    }

    void visit( Soprano::Query::GraphPattern *graphPattern )
    {
        qDebug() << "GraphPattern: " << graphPattern->optional();
        
        QListIterator<Soprano::Query::TriplePattern> tripleiter(graphPattern->triplePatterns());
        while ( tripleiter.hasNext() )
        {
            Soprano::Query::TriplePattern pattern = tripleiter.next();
            pattern.accept( this );    
        }
        
        QListIterator<Soprano::Query::GraphPattern> graphiter(graphPattern->subGraphPatterns());
        while ( graphiter.hasNext() )
        {
            Soprano::Query::GraphPattern pattern = graphiter.next();
            pattern.accept( this );    
        }
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
        
        const Soprano::Query::GraphPattern graphPattern = query->graphPattern();
        
        Q_FOREACH( Soprano::Query::TriplePattern pattern, graphPattern.triplePatterns() )
        {
            pattern.accept( this );
        }

        Q_FOREACH( Soprano::Query::GraphPattern pattern, graphPattern.subGraphPatterns() )
        {
            pattern.accept( this );
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
        "PREFIX foaf:   <http://xmlns.com/foaf/0.1/>\
        SELECT ?x ?name\
        WHERE  {\
            ?x foaf:name ?name .\
            ?x foaf:desc ?anon\
        }" 
        , Soprano::Query::QUERY_LANGUAGE_SPARQL
    );

    TestVisitor visitor( &query );
    
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
