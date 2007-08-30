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
#include "client.h"
#include "clientmodel.h"

#include <soprano/bindingset.h>
#include <soprano/statement.h>


Soprano::Server::ClientQueryResultIteratorBackend::ClientQueryResultIteratorBackend( int itId, ClientModel* client )
    : m_iteratorId( itId ),
      m_model( client )
{
}


Soprano::Server::ClientQueryResultIteratorBackend::~ClientQueryResultIteratorBackend()
{
    close();
}


bool Soprano::Server::ClientQueryResultIteratorBackend::next()
{
    if ( m_model ) {
        return m_model->client()->iteratorNext( m_iteratorId );
    }
    else {
        return false;
    }
}


Soprano::BindingSet Soprano::Server::ClientQueryResultIteratorBackend::current() const
{
    if ( m_model ) {
        return m_model->client()->queryIteratorCurrent( m_iteratorId );
    }
    else {
        return BindingSet();
    }
}


void Soprano::Server::ClientQueryResultIteratorBackend::close()
{
    if ( m_model ) {
        m_model->closeIterator( m_iteratorId );
    }
}


Soprano::Statement Soprano::Server::ClientQueryResultIteratorBackend::currentStatement() const
{
    if ( m_model ) {
        return m_model->client()->queryIteratorCurrentStatement( m_iteratorId );
    }
    else {
        return Statement();
    }
}


Soprano::Node Soprano::Server::ClientQueryResultIteratorBackend::binding( const QString &name ) const
{
    // FIXME: use an extra method for performance
    return current()[name];
}


Soprano::Node Soprano::Server::ClientQueryResultIteratorBackend::binding( int offset ) const
{
    // FIXME: use an extra method for performance
    return current()[offset];
}


int Soprano::Server::ClientQueryResultIteratorBackend::bindingCount() const
{
    // FIXME: use an extra method for performance
    return current().count();
}


QStringList Soprano::Server::ClientQueryResultIteratorBackend::bindingNames() const
{
    // FIXME: use an extra method for performance
    return current().bindingNames();
}


bool Soprano::Server::ClientQueryResultIteratorBackend::isGraph() const
{
    if ( m_model ) {
        return m_model->client()->queryIteratorType( m_iteratorId ) == 1;
    }
    else {
        return false;
    }
}


bool Soprano::Server::ClientQueryResultIteratorBackend::isBinding() const
{
    if ( m_model ) {
        return m_model->client()->queryIteratorType( m_iteratorId ) == 3;
    }
    else {
        return false;
    }
}


bool Soprano::Server::ClientQueryResultIteratorBackend::isBool() const
{
    if ( m_model ) {
        return m_model->client()->queryIteratorType( m_iteratorId ) == 2;
    }
    else {
        return false;
    }
}


bool Soprano::Server::ClientQueryResultIteratorBackend::boolValue() const
{
    if ( m_model ) {
        return m_model->client()->queryIteratorBoolValue( m_iteratorId );
    }
    else {
        return false;
    }
}
