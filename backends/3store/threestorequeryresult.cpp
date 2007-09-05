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

#include "threestorequeryresult.h"

#include "statement.h"

#include <QtCore/QDebug>

extern "C" {
#include <rasqal.h>
#include <3store3/datatypes.h>
#include <3store3/hash.h>
}


class Soprano::ThreeStore::QueryResult::Private
{
public:
    Private()
        : result( 0 ),
          currentRow( 0 ) {
    }

    ts_result* result;

    ts_row* currentRow;
    int rowIndex;

    QStringList bindingNames;

    Soprano::Node tsBindingToNode( ts_binding* binding ) {
        if ( binding->lexical ) {
            int datatypeId = binding->datatype ? atoi( binding->datatype ) : result->con->uri_dt;
            int langId = binding->lang ? atoi( binding->lang ) : 0;

            if ( datatypeId == result->con->uri_dt ) {
                if ( *binding->lexical == '_' ) {
                    return Soprano::Node( QString::fromAscii( binding->lexical+2 ) );
                }
                else {
                    return Soprano::Node( QUrl::fromEncoded( binding->lexical ) );
                }
            }
            else {
                QString lang;
                if ( langId ) {
                    char* ls = ts_get_language( result->con, langId );
                    lang = QString::fromUtf8( ls );
                    free( ls );
                }

                QUrl datatype;
                if ( datatypeId ) {
                    char* ds = ts_get_datatype( result->con, datatypeId );
                    datatype = QUrl::fromEncoded( ds );
                    free( ds );
                }

                return Soprano::Node( Soprano::LiteralValue::fromString( QString::fromUtf8( binding->lexical ), datatype ), lang );
            }
        }
        else {
            return Soprano::Node();
        }
    }

    Soprano::Node literalToNode( rasqal_literal* l, ts_row* row ) {
        QString literalString = QString::fromUtf8( ( const char* )rasqal_literal_as_string( l ) );
        switch( l->type ) {
        case RASQAL_LITERAL_URI:
//            qDebug() << "(Soprano::ThreeStore::QueryResult) creating node from uri" << literalString;
            return Node( QUrl( literalString ) );

        case RASQAL_LITERAL_BLANK:
            return Node( literalString );

        case RASQAL_LITERAL_STRING:
            return Node( LiteralValue::fromString( literalString, QVariant::String ) );
        case RASQAL_LITERAL_BOOLEAN:
            return Node( LiteralValue::fromString( literalString, QVariant::Bool ) );
        case RASQAL_LITERAL_INTEGER:
            return Node( LiteralValue::fromString( literalString, QVariant::Int ) );
        case RASQAL_LITERAL_DOUBLE:
            return Node( LiteralValue::fromString( literalString, QVariant::Double ) );
        case RASQAL_LITERAL_FLOAT:
            return Node( LiteralValue::fromString( literalString, QVariant::Double ) );
        case RASQAL_LITERAL_DECIMAL:
            return Node( LiteralValue::fromString( literalString, QVariant::Int ) );
        case RASQAL_LITERAL_DATETIME:
            return Node( LiteralValue::fromString( literalString, QVariant::DateTime ) );

        case RASQAL_LITERAL_VARIABLE:
//            qDebug() << "(Soprano::ThreeStore::QueryResult) creating node from variable" << (char *)l->value.variable->name;
            return tsBindingToNode( ts_row_get_binding_by_name( row, (char *)l->value.variable->name ) );

        default:
            qDebug() << "(Soprano::ThreeStore::QueryResult) invalid rasqal literal of type " << l->type;
            return Node();
        }
    }

    Soprano::Statement tsRowToStatement( ts_row* row ) {
        // FIXME: from 3store's result.c it looks as if one row could include multiple statements. Is that really the case?
        rasqal_triple* rTriple = rasqal_query_get_construct_triple( result->query->rquery, 0 );
        if ( !rTriple ) {
            qDebug() << "(Soprano::ThreeStore::QueryResult) no triple at query result row index" << rowIndex;
            return Statement();
        }

        // FIXME: handle the context
        return Statement( literalToNode( rTriple->subject, row ),
                          literalToNode( rTriple->predicate, row ),
                          literalToNode( rTriple->object, row ) );
    }
};


Soprano::ThreeStore::QueryResult::QueryResult( ts_result* r )
    : Soprano::QueryResultIteratorBackend(),
      d(  new Private() )
{
    d->result = r;
    d->rowIndex = -1;

    // cache column names
    if ( d->bindingNames.isEmpty() ) {
        for ( int i = 0; i < d->result->row.cols; ++i ) {
            d->bindingNames.append( QString::fromUtf8( d->result->row.col_desc[i].sym ) );
        }
    }
}


Soprano::ThreeStore::QueryResult::~QueryResult()
{
    close();
    delete d;
}


bool Soprano::ThreeStore::QueryResult::next()
{
    d->currentRow = ts_result_fetch_row( d->result );
    ++d->rowIndex;
    return ( d->currentRow != 0 );
}


Soprano::Statement Soprano::ThreeStore::QueryResult::currentStatement() const
{
    if ( d->currentRow ) {
        return d->tsRowToStatement( d->currentRow );
    }
    else {
        qDebug() << "(Soprano::ThreeStore::QueryResult) called currentStatement on invalid row.";
        return Soprano::Statement();
    }
}


Soprano::Node Soprano::ThreeStore::QueryResult::binding( const QString &name ) const
{
    if ( d->currentRow ) {
        return d->tsBindingToNode( ts_row_get_binding_by_name( d->currentRow, name.toUtf8().data() ) );
    }
    else {
        qDebug() << "(Soprano::ThreeStore::QueryResult) called binding on invalid row.";
        return Soprano::Node();
    }
}


Soprano::Node Soprano::ThreeStore::QueryResult::binding( int offset ) const
{
    if ( d->currentRow ) {
        return d->tsBindingToNode( ts_row_get_binding( d->currentRow, offset ) );
    }
    else {
        qDebug() << "(Soprano::ThreeStore::QueryResult) called binding on invalid row.";
        return Soprano::Node();
    }
}


int Soprano::ThreeStore::QueryResult::bindingCount() const
{
    // FIXME: unsure if both numbers are always the same
    if ( d->currentRow ) {
        return d->currentRow->cols;
    }
    else {
        return d->result->query->cols;
    }
}


QStringList Soprano::ThreeStore::QueryResult::bindingNames() const
{
    return d->bindingNames;
}


bool Soprano::ThreeStore::QueryResult::isGraph() const
{
    return ( rasqal_query_get_verb( d->result->query->rquery ) == RASQAL_QUERY_VERB_CONSTRUCT ||
             rasqal_query_get_verb( d->result->query->rquery ) == RASQAL_QUERY_VERB_DESCRIBE );
}


bool Soprano::ThreeStore::QueryResult::isBinding() const
{
    return ( rasqal_query_get_verb( d->result->query->rquery ) != RASQAL_QUERY_VERB_CONSTRUCT &&
             rasqal_query_get_verb( d->result->query->rquery ) != RASQAL_QUERY_VERB_DESCRIBE &&
             rasqal_query_get_verb( d->result->query->rquery ) != RASQAL_QUERY_VERB_ASK );
}


bool Soprano::ThreeStore::QueryResult::isBool() const
{
    return ( rasqal_query_get_verb( d->result->query->rquery ) == RASQAL_QUERY_VERB_ASK );
}


bool Soprano::ThreeStore::QueryResult::boolValue() const
{
    // FIXME: do we need to cache this value or is it returned multiple times?
    return ts_result_fetch_row( d->result );
}


void Soprano::ThreeStore::QueryResult::close()
{
    ts_query_free( d->result->query );
}
