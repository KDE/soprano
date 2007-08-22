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

#include <QtCore/QtPlugin>
#include <QtCore/QTextStream>

#include <rasqal.h>
#include <raptor.h>

#include "soprano/query/query.h"
#include "rasqalqueryparser.h"

Q_EXPORT_PLUGIN2(soprano_rasqalqueryparser, Soprano::Rasqal::QueryParser)

Soprano::Rasqal::QueryParser::QueryParser()
    : QObject(),
      Soprano::Query::Parser( "rasqal" )
{
}

Soprano::Rasqal::QueryParser::~QueryParser()
{
}

Soprano::Query::Query Soprano::Rasqal::QueryParser::parseQuery( const QString &query, Soprano::Query::QueryLanguage lang, const QString& userQueryLanguage )
{
    return Soprano::Query::Query();
}

Soprano::Query::QueryLanguages Soprano::Rasqal::QueryParser::supportedQueryLanguages() const
{
    return Soprano::Query::QUERY_LANGUAGE_SPARQL;
}

#include "rasqalqueryparser.moc"


/*Soprano::Query::Query *Soprano::Query::Parser::RasqalQueryParser::parseQuery( const QString &query )
{
    qDebug() << query << endl;

    rasqal_init();

    // Create a SPARQL query object
    rasqal_query *rq = rasqal_new_query( "sparql", (const unsigned char*)"http://www.w3.org/TR/rdf-sparql-query/" );
    if ( !rq ) 
    {
        qDebug() << "ERROR!" << rq << endl;
        // TODO: error during query creation
        return NULL;
    }

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
            queryVerb = "UNKNOWN";
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
