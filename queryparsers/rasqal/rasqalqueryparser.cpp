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

#include <QDebug>
#include <QUrl>
#include <QtCore/QtPlugin>
#include <QtCore/QTextStream>

#include <soprano/node.h>
#include <soprano/locator.h>
#include <soprano/query/query.h>

#include <rasqal.h>

#include "rasqalqueryparser.h"

Q_EXPORT_PLUGIN2(soprano_rasqalqueryparser, Soprano::Rasqal::QueryParser)

static Soprano::Query::Prefix convert_rasqal_prefix( rasqal_prefix* rprefix )
{
    char* uri_value = (char *)raptor_uri_to_string(rprefix->uri);
    Soprano::Query::Prefix prefix( (const char*)rprefix->prefix, QUrl(uri_value));

    free(uri_value);

    return prefix;
}

static Soprano::Query::Expression *convert_rasqal_expression( rasqal_expression* rexp )
{
    // TODO: Implement me
    return 0;
}

static Soprano::Query::RTerm* convert_rasqal_literal( rasqal_literal* rl )
{
    switch( rl->type )
    {
        case RASQAL_LITERAL_UNKNOWN:
            break;
        case RASQAL_LITERAL_BLANK:
        case RASQAL_LITERAL_URI:
        {
            QUrl uri( (const char*)rasqal_literal_as_string( rl ) );
            return new Soprano::Query::Node( Soprano::Node(uri) );
        }
        /*case RASQAL_LITERAL_STRING:
            break;
        case RASQAL_LITERAL_BOOLEAN:
            break;
        case RASQAL_LITERAL_INTEGER:
            break;
        case RASQAL_LITERAL_DOUBLE:
            break;
        case RASQAL_LITERAL_FLOATING:
            break;
        case RASQAL_LITERAL_FLOAT:
            break;
        case RASQAL_LITERAL_DECIMAL:
            break;
  	case RASQAL_LITERAL_DATETIME:
            break;
        case RASQAL_LITERAL_PATTERN:
            break;
        case RASQAL_LITERAL_QNAME:
            break;*/
        case RASQAL_LITERAL_VARIABLE:
        {
            rasqal_variable* variable = rasqal_literal_as_variable( rl );
            Soprano::Query::Variable* var = new Soprano::Query::Variable( (const char*)variable->name );

            return var;
        }
    }

    return 0;
}


static Soprano::Query::TriplePattern* convert_rasqal_triple_pattern( rasqal_triple* rtp )
{
    return new Soprano::Query::TriplePattern( convert_rasqal_literal( rtp->subject ),
                                              convert_rasqal_literal( rtp->predicate ),
                                              convert_rasqal_literal( rtp->object ) );
}

static Soprano::Query::BooleanExpression* convert_rasqal_graph_pattern( rasqal_graph_pattern* rgp )
{
    qDebug() << "Soprano::Query::TriplePattern";

    Soprano::Query::LogicAnd* andExpr = new Soprano::Query::LogicAnd();

    // FIXME: Use rasqal_graph_pattern_get_triple_sequence when rasqal 0.9.15 will be released.
    rasqal_triple* triple;
    int i = 0;
    while ( triple = rasqal_graph_pattern_get_triple( rgp, i ) )
    {
        andExpr->addConditon( convert_rasqal_triple_pattern( triple ) );
        i++;
    }

    // TODO: SubGraphs...

    return andExpr;
}

static Soprano::Query::QueryTerms get_query_terms( rasqal_query *rq )
{
    Soprano::Query::QueryTerms queryTerms;

    raptor_sequence* binding_sequence = rasqal_query_get_bound_variable_sequence( rq );
    if ( binding_sequence )
    {
        for (int i=0; i < raptor_sequence_size(binding_sequence); i++)
        {
            rasqal_variable* variable = (rasqal_variable*)raptor_sequence_get_at(binding_sequence, i);
            Soprano::Query::Variable* var = new Soprano::Query::Variable( (char *)variable->name );
            queryTerms.addVariable( var );
        }
    }

    return queryTerms;
}

Soprano::Rasqal::QueryParser::QueryParser()
    : Soprano::Query::Parser( "rasqal" )
{
    rasqal_init();
}

Soprano::Rasqal::QueryParser::~QueryParser()
{
    rasqal_finish();
}

Soprano::Query::Query Soprano::Rasqal::QueryParser::parseQuery( const QString &query, Soprano::Query::QueryLanguage lang, const QString& userQueryLanguage ) const
{
    rasqal_query *rq = rasqal_new_query( "sparql", (const unsigned char*)"http://www.w3.org/TR/rdf-sparql-query/" );
    if ( !rq )
    {
        return Soprano::Query::Query();
    }

    rasqal_query_set_fatal_error_handler( rq, const_cast<QueryParser*>( this ), raptor_message_handler );
    rasqal_query_set_error_handler( rq, const_cast<QueryParser*>( this ), raptor_message_handler );

    if ( rasqal_query_prepare( rq, (unsigned char *)query.toLatin1().data(), NULL ) )
    {
    	rasqal_free_query( rq );
        return Soprano::Query::Query();
    }
    Soprano::Query::Query queryobj;

    // TODO: GROUP and ORDER are supported only by http://www.dajobe.org/2007/04/laqrs/

    // Prefix

    raptor_sequence* prefix_sequence = rasqal_query_get_prefix_sequence( rq );
    if ( prefix_sequence )
    {
        for (int i=0; i < raptor_sequence_size(prefix_sequence); i++)
        {
            rasqal_prefix *prefix = (rasqal_prefix*)raptor_sequence_get_at(prefix_sequence, i);
    	    queryobj.addPrefix( convert_rasqal_prefix( prefix ) );
        }
    }

    // Query Terms

    queryobj.setQueryTerms( get_query_terms( rq ) );

    // Top level GraphPattern

    rasqal_graph_pattern* graph_pattern = (rasqal_graph_pattern*)rasqal_query_get_query_graph_pattern( rq );
    queryobj.setCondition( convert_rasqal_graph_pattern(graph_pattern) );

    //rasqal_query_print( rq, stdout );
    rasqal_free_query( rq );

    return queryobj;
}

Soprano::Query::QueryLanguages Soprano::Rasqal::QueryParser::supportedQueryLanguages() const
{
    return Soprano::Query::QUERY_LANGUAGE_SPARQL;
}

void Soprano::Rasqal::QueryParser::raptor_message_handler( void *query_parser, raptor_locator *rl, const char *msg )
{
    QueryParser* parser = static_cast<QueryParser *>(query_parser);

    Soprano::Error::Locator locator;
    locator.setLine( raptor_locator_line(rl) );
    locator.setColumn( raptor_locator_column(rl) );

    parser->emitSyntaxError( locator , msg );
}

void Soprano::Rasqal::QueryParser::emitSyntaxError( const Error::Locator& locator, const QString& message )
{
    setError( Error::ParserError( locator, message ) );
}

#include "rasqalqueryparser.moc"

/*Soprano::Query::Query *Soprano::Query::Parser::RasqalQueryParser::parseQuery( const QString &query )
{
    if ( rasqal_query_prepare( rq, (unsigned char *)query.toLatin1().data(), NULL ) )
    {
        qDebug() << "SYNTAX ERROR!" << endl;
        // TODO: error during query creation
        return NULL;
    }

    // FIXME: set the right QueryObjects' PREFIX
    //raptor_sequence* prefixes = rasqal_query_get_prefix_sequence( rq );

    // FIXME: set the QueryObject's query verb
    rasqal_query_verb query_verb = rasqal_query_get_verb( rq );

    QString queryVerb;
    switch ( query_verb )
    {
        case RASQAL_QUERY_VERB_SELECT:
            queryVerb = "SELECT";
            break;
        case RASQAL_QUERY_VERB_CONSTRUCT:
            queryVerb = "CONSTRUCT";
            break;
        case RASQAL_QUERY_VERB_DESCRIBE:
            queryVerb = "DESCRIBE";
            break;
        case RASQAL_QUERY_VERB_ASK:
            queryVerb = "ASK";
            break;
        case RASQAL_QUERY_VERB_DELETE:
            queryVerb = "DELETE";
            break;
        case RASQAL_QUERY_VERB_INSERT:
            queryVerb = "INSERT";
            break;
        default:
            queryVerb = "UNKNOWNN";
    };

    // The top query graph pattern
    rasqal_graph_pattern* graph_pattern = rasqal_query_get_query_graph_pattern( rq );

    rasqal_triple *triple = rasqal_graph_pattern_get_triple( graph_pattern, 0 );

    qDebug() << triple->subject->type << endl;
    qDebug() << triple->predicate->type << endl;
    qDebug() << triple->object->type << RASQAL_LITERAL_VARIABLE << endl;

    rasqal_graph_pattern_operator op = rasqal_graph_pattern_get_operator( graph_pattern );
    qDebug() << op << endl;

    rasqal_finish();

    Query *qo = new Query();
    //qo->set( queryVerb );

    //qDebug() << qo->queryVerb() << endl;

    return qo;
}*/
