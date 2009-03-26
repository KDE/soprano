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
    d->bindingNames = d->m_queryResult->resultColumns();
    for ( int i = 0; i < d->bindingNames.count(); ++i ) {
        d->bindingIndexHash.insert( d->bindingNames[i], i );
    }
    d->bindingCachedFlags = QBitArray( d->bindingNames.count(), false );
    d->bindingCache.resize( d->bindingNames.count() );

    // handle ask and graph results the hacky way
    d->isGraphResult = ( d->bindingNames.count() == 1 &&
                         d->bindingNames[0] == "callret-0" );
    d->askResult = false;
    d->isAskQueryResult = ( d->bindingNames.count() == 1 &&
                            d->bindingNames[0] == "__ASK_RETVAL" );
    if ( d->isAskQueryResult ) {
        // cache the result
        // virtuoso returns an empty result set for false boolean results
        // otherwise a single row is returned
        d->askResult = d->m_queryResult->fetchScroll();
    }
    else if ( d->isGraphResult ) {
        // parse the data
        if ( d->m_queryResult->fetchScroll() ) {
            if ( const Parser* parser = PluginManager::instance()->discoverParserForSerialization( SerializationTurtle ) ) {
                QString data = d->m_queryResult->getData( 1 ).toString();
                d->graphIterator = parser->parseString( data, QUrl(), SerializationTurtle );
            }
            else {
                setError( "Failed to load Turtle parser for graph data from Virtuoso server" );
            }
        }
    }
}


Soprano::Virtuoso::QueryResultIteratorBackend::~QueryResultIteratorBackend()
{
    d->m_model->m_openIterators.removeAll( this );
    delete d->m_queryResult;
    delete d;
}


bool Soprano::Virtuoso::QueryResultIteratorBackend::next()
{
    if ( d->isAskQueryResult ) {
        return d->m_queryResult != 0;
    }
    else if ( d->isGraphResult ) {
        return d->graphIterator.next();
    }
    else {
        // reset cache
        d->bindingCachedFlags.fill( false );

        // ask statement handler for cursor scroll
        return d->m_queryResult ? d->m_queryResult->fetchScroll() : false;
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
            d->bindingCache[offset] = d->m_queryResult->getData( offset+1 );
            // convert the default graph back to the empty graph (hacky but should work in most situations)
            if ( d->bindingCache[offset] == Virtuoso::defaultGraph() )
                d->bindingCache[offset] = Node();
            d->bindingCachedFlags.setBit( offset );
        }
        return d->bindingCache[offset];
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
    return d->isGraphResult;
}


bool Soprano::Virtuoso::QueryResultIteratorBackend::isBinding() const
{
    return !d->isAskQueryResult && !d->isGraphResult;
}


bool Soprano::Virtuoso::QueryResultIteratorBackend::isBool() const
{
    return d->isAskQueryResult;
}


bool Soprano::Virtuoso::QueryResultIteratorBackend::boolValue() const
{
    return d->askResult;
}


void Soprano::Virtuoso::QueryResultIteratorBackend::close()
{
    d->graphIterator.close();
    delete d->m_queryResult;
    d->m_queryResult = 0;
}
