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


Soprano::Server::ClientQueryResultIteratorBackend::ClientQueryResultIteratorBackend( int itId, const ClientModel* client )
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
    return m_model->client()->iteratorNext( m_iteratorId );
}


Soprano::BindingSet Soprano::Server::ClientQueryResultIteratorBackend::current() const
{
    return m_model->client()->queryIteratorCurrent( m_iteratorId );
}


void Soprano::Server::ClientQueryResultIteratorBackend::close()
{
    m_model->closeIterator( m_iteratorId );
}


Soprano::Statement Soprano::Server::ClientQueryResultIteratorBackend::currentStatement() const
{
    return m_model->client()->queryIteratorCurrentStatement( m_iteratorId );
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
    return m_model->client()->queryIteratorType( m_iteratorId ) == 1;
}


bool Soprano::Server::ClientQueryResultIteratorBackend::isBinding() const
{
    return m_model->client()->queryIteratorType( m_iteratorId ) == 3;
}


bool Soprano::Server::ClientQueryResultIteratorBackend::isBool() const
{
    return m_model->client()->queryIteratorType( m_iteratorId ) == 2;
}


bool Soprano::Server::ClientQueryResultIteratorBackend::boolValue() const
{
    return m_model->client()->queryIteratorBoolValue( m_iteratorId );
}
