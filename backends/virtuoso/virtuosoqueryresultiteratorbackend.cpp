/*
 * This file is part of Soprano Project
 *
 * Copyright (C) 2008 Sebastian Trueg <trueg@kde.org>
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

#include "virtuosoqueryresultiteratorbackend.h"
#include "virtuosoqueryresultiteratorbackend_p.h"
#include "virtuosomodel_p.h"
#include "virtuosotools.h"
#include "odbcqueryresult.h"
#include "statement.h"
#include "statementiterator.h"
#include "nodeiterator.h"
#include "queryresultiterator.h"
#include "literalvalue.h"
#include "node.h"
#include "statementiterator.h"
#include "parser.h"
#include "pluginmanager.h"

#include <QtCore/QHash>
#include <QtCore/QVector>
#include <QtCore/QBitArray>
#include <QtCore/QStringList>
#include <QtCore/QPointer>
#include <QtCore/QDebug>


Soprano::Virtuoso::QueryResultIteratorBackend::QueryResultIteratorBackend( ODBC::QueryResult* result )
    : Soprano::QueryResultIteratorBackend(),
      d( new QueryResultIteratorBackendPrivate() )
{
    // cache binding names
    d->m_queryResult = result;
    d->m_resultType = QueryResultIteratorBackendPrivate::UnknownResult;
    d->bindingNames = d->m_queryResult->resultColumns();
    for ( int i = 0; i < d->bindingNames.count(); ++i ) {
        d->bindingIndexHash.insert( d->bindingNames[i], i );
    }
    d->bindingCachedFlags = QBitArray( d->bindingNames.count(), false );
    d->bindingCache.resize( d->bindingNames.count() );

    // ASK queries are rather easy to detect
    // =====================================
    if ( d->bindingNames.count() == 1 &&
         d->bindingNames[0] == QLatin1String( "__ASK_RETVAL" ) ) {
        d->m_resultType = QueryResultIteratorBackendPrivate::AskResult;
        // cache the result
        // virtuoso returns an empty result set for false boolean results
        // otherwise a single row is returned
        if ( d->m_queryResult->fetchRow() ) {
            Node askVal = d->m_queryResult->getData( 1 );
//            qDebug() << Q_FUNC_INFO << d->m_queryResult->resultColumns() << askVal;
            d->askResult = askVal.literal().toInt() != 0;
        }
        else {
            d->askResult = false;
        }
    }

    // Graph queries are a little trickier
    // =====================================
    else if ( d->bindingNames.count() == 1 &&
              ( d->bindingNames[0] == QLatin1String( "callret-0" ) ||
                d->bindingNames[0] == QLatin1String( "fmtaggret-" ) ) ) { // try to be 6.1.1 compatible
        if ( d->m_queryResult->fetchRow() ) {
            Node node = d->m_queryResult->getData( 1 );
            if ( !d->m_queryResult->lastError() ) {
                // FIXME: This is hacky. Better check if the value is a BLOB
                if ( node.literal().isString() ) {
                    d->m_resultType = QueryResultIteratorBackendPrivate::GraphResult;
                    if ( const Parser* parser = PluginManager::instance()->discoverParserForSerialization( SerializationTurtle ) ) {
                        QString data = node.toString();
                        d->graphIterator = parser->parseString( data, QUrl(), SerializationTurtle );
                        setError( parser->lastError() );
                    }
                    else {
                        setError( "Failed to load Turtle parser for graph data from Virtuoso server" );
                    }
                }
                else {
                    d->m_resultType = QueryResultIteratorBackendPrivate::MethodCallResult;
                    d->m_methodCallResultIterated = false;
                    d->bindingCache[0] = node;
                    d->bindingCachedFlags.setBit( 0 );
                }
            }
            else {
                setError( d->m_queryResult->lastError() );
            }
        }
        else {
            setError( d->m_queryResult->lastError() );
        }
    }

    else {
        d->m_resultType = QueryResultIteratorBackendPrivate::BindingResult;
    }
}


Soprano::Virtuoso::QueryResultIteratorBackend::~QueryResultIteratorBackend()
{
    close();
    delete d;
}


bool Soprano::Virtuoso::QueryResultIteratorBackend::next()
{
    switch( d->m_resultType ) {
    case QueryResultIteratorBackendPrivate::AskResult:
        return d->m_queryResult != 0;

    case QueryResultIteratorBackendPrivate::GraphResult:
        return d->graphIterator.next();

    case QueryResultIteratorBackendPrivate::BindingResult:
        // reset cache
        d->bindingCachedFlags.fill( false );

        // ask statement handler for cursor scroll
        if ( d->m_queryResult && d->m_queryResult->fetchRow() ) {
            // we need to cache the values already here since there are situations where
            // the query succeeds but getting values fails
            for ( int i = 0; i < bindingCount(); ++i ) {
                binding( i );
                if ( lastError() ) {
                    return false;
                }
            }
            return true;
        }
        else {
            return false;
        }

    case QueryResultIteratorBackendPrivate::MethodCallResult:
        if ( d->m_methodCallResultIterated ) {
            return false;
        }
        else {
            d->m_methodCallResultIterated = true;
            return true;
        }

    default:
        return false;
    }
}


Soprano::Statement Soprano::Virtuoso::QueryResultIteratorBackend::currentStatement() const
{
    return d->graphIterator.current();
}


Soprano::Node Soprano::Virtuoso::QueryResultIteratorBackend::binding( const QString& name ) const
{
    if ( d->bindingIndexHash.contains( name ) ) {
        return binding( d->bindingIndexHash[name] );
    }
    else {
        setError( QString( "Invalid binding name: %1" ).arg( name ), Error::ErrorInvalidArgument );
        return Node();
    }
}


Soprano::Node Soprano::Virtuoso::QueryResultIteratorBackend::binding( int offset ) const
{
    if ( d->m_queryResult && offset < bindingCount() && offset >= 0 ) {
        if ( !d->bindingCachedFlags[offset] ) {
            Node node = d->m_queryResult->getData( offset+1 );
            setError( d->m_queryResult->lastError() );

            // convert the default graph back to the empty graph (hacky but should work in most situations)
            if ( node == Virtuoso::defaultGraph() )
                node = Node();

            d->bindingCache[offset] = node;
            d->bindingCachedFlags.setBit( offset );

            return node;
        }
        else {
            return d->bindingCache[offset];
        }
    }
    return Node();
}


int Soprano::Virtuoso::QueryResultIteratorBackend::bindingCount() const
{
    return d->bindingNames.count();
}


QStringList Soprano::Virtuoso::QueryResultIteratorBackend::bindingNames() const
{
    return d->bindingNames;
}


bool Soprano::Virtuoso::QueryResultIteratorBackend::isGraph() const
{
    return d->m_resultType == QueryResultIteratorBackendPrivate::GraphResult;
}


bool Soprano::Virtuoso::QueryResultIteratorBackend::isBinding() const
{
    return( d->m_resultType == QueryResultIteratorBackendPrivate::BindingResult ||
            d->m_resultType == QueryResultIteratorBackendPrivate::MethodCallResult );
}


bool Soprano::Virtuoso::QueryResultIteratorBackend::isBool() const
{
    return d->m_resultType == QueryResultIteratorBackendPrivate::AskResult;
}


bool Soprano::Virtuoso::QueryResultIteratorBackend::boolValue() const
{
    return d->askResult;
}


void Soprano::Virtuoso::QueryResultIteratorBackend::close()
{
    //
    // while an iterator must not be used in two differnt threads,
    // the VirtuosoModel destructor will close all open iterators
    // which may happen in a different thread indeed.
    //
    d->m_closeMutex.lock();
    if ( d->m_model ) {
        d->m_model->removeIterator( this );
        d->m_model = 0;
        d->graphIterator.close();
        delete d->m_queryResult;
        d->m_queryResult = 0;
    }
    d->m_closeMutex.unlock();
}
