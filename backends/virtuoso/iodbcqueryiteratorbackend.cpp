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

#include "iodbcqueryiteratorbackend.h"
#include "iodbcstatementhandler.h"
#include "iodbctools.h"
#include "statement.h"
#include "statementiterator.h"
#include "nodeiterator.h"
#include "queryresultiterator.h"
#include "literalvalue.h"
#include "node.h"

#include <QtCore/QHash>
#include <QtCore/QVector>
#include <QtCore/QBitArray>
#include <QtCore/QStringList>
#include <QtCore/QPointer>


class Soprano::IODBCQueryResultIteratorBackend::Private
{
public:
    QPointer<IODBCStatementHandler> handler;
    QStringList bindingNames;
    QHash<QString, int> bindingIndexHash;

    QVector<Node> bindingCache;
    QBitArray bindingCachedFlags;
};


Soprano::IODBCQueryResultIteratorBackend::IODBCQueryResultIteratorBackend( IODBCStatementHandler* hdl )
    : QueryResultIteratorBackend(),
      d( new Private() )
{
    d->handler = hdl;
    d->bindingNames = hdl->resultColumns();
    for ( int i = 0; i < d->bindingNames.count(); ++i ) {
        d->bindingIndexHash.insert( d->bindingNames[i], i );
    }
    d->bindingCachedFlags = QBitArray( d->bindingNames.count(), false );
    d->bindingCache.resize( d->bindingNames.count() );
}


Soprano::IODBCQueryResultIteratorBackend::~IODBCQueryResultIteratorBackend()
{
    delete d->handler;
    delete d;
}


bool Soprano::IODBCQueryResultIteratorBackend::next()
{
    // reset cache
    d->bindingCachedFlags.fill( false );

    // ask statement handler for cursor scroll
    return d->handler ? d->handler->fetchScroll() : false;
}


Soprano::Statement Soprano::IODBCQueryResultIteratorBackend::currentStatement() const
{
#warning FIXME: IODBCQueryResultIteratorBackend::currentStatement
    return Statement();
}


Soprano::Node Soprano::IODBCQueryResultIteratorBackend::binding( const QString& name ) const
{
    if ( d->bindingIndexHash.contains( name ) ) {
        return binding( d->bindingIndexHash[name] );
    }
    else {
        setError( QString( "Invalid binding name: %1" ).arg( name ), Error::ErrorInvalidArgument );
        return Node();
    }
}


Soprano::Node Soprano::IODBCQueryResultIteratorBackend::binding( int offset ) const
{
    if ( d->handler && offset < bindingCount() && offset >= 0 ) {
        if ( !d->bindingCachedFlags[offset] ) {
            d->bindingCache[offset] = d->handler->getData( offset+1 );
            d->bindingCachedFlags.setBit( offset );
        }
        return d->bindingCache[offset];
    }
    return Node();
}


int Soprano::IODBCQueryResultIteratorBackend::bindingCount() const
{
    return d->bindingNames.count();
}


QStringList Soprano::IODBCQueryResultIteratorBackend::bindingNames() const
{
    return d->bindingNames;
}


bool Soprano::IODBCQueryResultIteratorBackend::isGraph() const
{
#warning FIXME: IODBCQueryResultIteratorBackend::isGraph
    return false;
}


bool Soprano::IODBCQueryResultIteratorBackend::isBinding() const
{
#warning FIXME: IODBCQueryResultIteratorBackend::isBinding
    return true;
}


bool Soprano::IODBCQueryResultIteratorBackend::isBool() const
{
#warning FIXME: IODBCQueryResultIteratorBackend::isBool
    return false;
}


bool Soprano::IODBCQueryResultIteratorBackend::boolValue() const
{
#warning FIXME: IODBCQueryResultIteratorBackend::boolValue
    return false;
}


void Soprano::IODBCQueryResultIteratorBackend::close()
{
    delete d->handler;
    d->handler = 0;
}
