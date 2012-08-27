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


Soprano::Client::ClientQueryResultIteratorBackend::ClientQueryResultIteratorBackend( int itId, ClientModel* client )
    : m_iteratorId( itId ),
      m_model( client )
{
}


Soprano::Client::ClientQueryResultIteratorBackend::~ClientQueryResultIteratorBackend()
{
    close();
}


bool Soprano::Client::ClientQueryResultIteratorBackend::next()
{
    if ( m_model ) {
        bool r = m_model->client()->iteratorNext( m_iteratorId );
        setError( m_model->client()->lastError() );
        if( r ) {
            m_currentBinding = m_model->client()->queryIteratorCurrent( m_iteratorId );
            setError( m_model->client()->lastError() );
        }
        else {
            m_currentBinding = Soprano::BindingSet();
        }
        return r;
    }
    else {
        setError( "Connection to server closed." );
        return false;
    }
}


Soprano::BindingSet Soprano::Client::ClientQueryResultIteratorBackend::current() const
{
    if ( m_model ) {
        return m_currentBinding;
    }
    else {
        setError( "Connection to server closed." );
        return BindingSet();
    }
}


void Soprano::Client::ClientQueryResultIteratorBackend::close()
{
    if ( m_model ) {
        m_model->closeIterator( m_iteratorId );
        setError( m_model->client()->lastError() );
    }
    else {
        setError( "Connection to server closed." );
    }
}


Soprano::Statement Soprano::Client::ClientQueryResultIteratorBackend::currentStatement() const
{
    if ( m_model ) {
        Statement s = m_model->client()->queryIteratorCurrentStatement( m_iteratorId );
        setError( m_model->client()->lastError() );
        return s;
    }
    else {
        setError( "Connection to server closed." );
        return Statement();
    }
}


Soprano::Node Soprano::Client::ClientQueryResultIteratorBackend::binding( const QString &name ) const
{
    return current()[name];
}


Soprano::Node Soprano::Client::ClientQueryResultIteratorBackend::binding( int offset ) const
{
    return current()[offset];
}


int Soprano::Client::ClientQueryResultIteratorBackend::bindingCount() const
{
    return current().count();
}


QStringList Soprano::Client::ClientQueryResultIteratorBackend::bindingNames() const
{
    return current().bindingNames();
}


bool Soprano::Client::ClientQueryResultIteratorBackend::isGraph() const
{
    if ( m_model ) {
        bool r = m_model->client()->queryIteratorType( m_iteratorId ) == 1;
        setError( m_model->client()->lastError() );
        return r;
    }
    else {
        setError( "Connection to server closed." );
        return false;
    }
}


bool Soprano::Client::ClientQueryResultIteratorBackend::isBinding() const
{
    if ( m_model ) {
        bool r = m_model->client()->queryIteratorType( m_iteratorId ) == 3;
        setError( m_model->client()->lastError() );
        return r;
    }
    else {
        setError( "Connection to server closed." );
        return false;
    }
}


bool Soprano::Client::ClientQueryResultIteratorBackend::isBool() const
{
    if ( m_model ) {
        bool r = m_model->client()->queryIteratorType( m_iteratorId ) == 2;
        setError( m_model->client()->lastError() );
        return r;
    }
    else {
        setError( "Connection to server closed." );
        return false;
    }
}


bool Soprano::Client::ClientQueryResultIteratorBackend::boolValue() const
{
    if ( m_model ) {
        bool r = m_model->client()->queryIteratorBoolValue( m_iteratorId );
        setError( m_model->lastError() );
        return r;
    }
    else {
        setError( "Connection to server closed." );
        return false;
    }
}
