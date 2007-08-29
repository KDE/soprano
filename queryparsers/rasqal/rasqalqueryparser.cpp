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

#include <soprano/locator.h>
#include <soprano/query/query.h>

#include <rasqal.h>

#include "rasqalqueryparser.h"

Q_EXPORT_PLUGIN2(soprano_rasqalqueryparser, Soprano::Rasqal::QueryParser)

static Soprano::Query::Prefix convert_rasqal_prefix( rasqal_prefix* rprefix )
{
    Soprano::Query::Prefix prefix( (const char*)rprefix->prefix, QUrl());

    return prefix;
}

static Soprano::Query::Expression *convert_rasqal_expression( rasqal_expression* rexp )
{
    return 0;
}

static Soprano::Query::GraphPattern convert_rasqal_graph_pattern( rasqal_graph_pattern* rgp )
{
    return Soprano::Query::GraphPattern();
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
        return Soprano::Query::Query();
    }

    qDebug() << "Query is valid!!";
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
   
    // TODO: QueryTerms (Variables) and Anonymous Variables

    // GraphPattern 

    raptor_sequence* graph_pattern_sequence = rasqal_query_get_graph_pattern_sequence( rq );
    if ( graph_pattern_sequence )
    {
        for (int i=0; i < raptor_sequence_size(graph_pattern_sequence); i++)   
        {
            rasqal_graph_pattern* graph_pattern = (rasqal_graph_pattern*)raptor_sequence_get_at(graph_pattern_sequence, i);
    	    rasqal_graph_pattern_print( graph_pattern, stdout );
        }
    }

    rasqal_query_print( rq, stdout );
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
    emit syntaxError( locator, message );
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
