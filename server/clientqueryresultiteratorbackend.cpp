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

#include "clientqueryresultiteratorbackend.h"
#include "clientconnection.h"
#include "clientmodel.h"

#include "bindingset.h"
#include "statement.h"


Soprano::Client::ClientQueryResultIteratorBackend::ClientQueryResultIteratorBackend( int itId, const ClientModelBase* client )
    : ClientIteratorBase( itId, client)
{
}


Soprano::Client::ClientQueryResultIteratorBackend::~ClientQueryResultIteratorBackend()
{
    close();
}


bool Soprano::Client::ClientQueryResultIteratorBackend::next()
{
    if ( modelBase() ) {
        bool r = modelBase()->connection()->iteratorNext( iteratorId() );
        setError( modelBase()->connection()->lastError() );
        return r;
    }
    else {
        setError( "Connection to server closed." );
        return false;
    }
}


Soprano::BindingSet Soprano::Client::ClientQueryResultIteratorBackend::current() const
{
    if ( modelBase() ) {
        BindingSet s = modelBase()->connection()->queryIteratorCurrent( iteratorId() );
        setError( modelBase()->connection()->lastError() );
        return s;
    }
    else {
        setError( "Connection to server closed." );
        return BindingSet();
    }
}


void Soprano::Client::ClientQueryResultIteratorBackend::close()
{
    closeIterator();
    clearError();
}


Soprano::Statement Soprano::Client::ClientQueryResultIteratorBackend::currentStatement() const
{
    if ( modelBase() ) {
        Statement s = modelBase()->connection()->queryIteratorCurrentStatement( iteratorId() );
        setError( modelBase()->connection()->lastError() );
        return s;
    }
    else {
        setError( "Connection to server closed." );
        return Statement();
    }
}


Soprano::Node Soprano::Client::ClientQueryResultIteratorBackend::binding( const QString &name ) const
{
    // FIXME: use an extra method for performance
    return current()[name];
}


Soprano::Node Soprano::Client::ClientQueryResultIteratorBackend::binding( int offset ) const
{
    // FIXME: use an extra method for performance
    return current()[offset];
}


int Soprano::Client::ClientQueryResultIteratorBackend::bindingCount() const
{
    // FIXME: use an extra method for performance
    return current().count();
}


QStringList Soprano::Client::ClientQueryResultIteratorBackend::bindingNames() const
{
    // FIXME: use an extra method for performance
    return current().bindingNames();
}


bool Soprano::Client::ClientQueryResultIteratorBackend::isGraph() const
{
    if ( modelBase() ) {
        bool r = modelBase()->connection()->queryIteratorType( iteratorId() ) == 1;
        setError( modelBase()->connection()->lastError() );
        return r;
    }
    else {
        setError( "Connection to server closed." );
        return false;
    }
}


bool Soprano::Client::ClientQueryResultIteratorBackend::isBinding() const
{
    if ( modelBase() ) {
        bool r = modelBase()->connection()->queryIteratorType( iteratorId() ) == 3;
        setError( modelBase()->connection()->lastError() );
        return r;
    }
    else {
        setError( "Connection to server closed." );
        return false;
    }
}


bool Soprano::Client::ClientQueryResultIteratorBackend::isBool() const
{
    if ( modelBase() ) {
        bool r = modelBase()->connection()->queryIteratorType( iteratorId() ) == 2;
        setError( modelBase()->connection()->lastError() );
        return r;
    }
    else {
        setError( "Connection to server closed." );
        return false;
    }
}


bool Soprano::Client::ClientQueryResultIteratorBackend::boolValue() const
{
    if ( modelBase() ) {
        bool r = modelBase()->connection()->queryIteratorBoolValue( iteratorId() );
        setError( modelBase()->connection()->lastError() );
        return r;
    }
    else {
        setError( "Connection to server closed." );
        return false;
    }
}
