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

#include "clientmodel.h"
#include "client.h"
#include "clientnodeiteratorbackend.h"
#include "clientstatementiteratorbackend.h"
#include "clientqueryresultiteratorbackend.h"

#include <soprano/backend.h>
#include <soprano/error.h>
#include <soprano/nodeiterator.h>
#include <soprano/statementiterator.h>
#include <soprano/queryresultiterator.h>


Soprano::Server::ClientModel::ClientModel( const Backend* backend, int modelId, Client* client )
    : StorageModel( backend ),
      m_modelId( modelId ),
      m_client( client )
{
}


Soprano::Server::ClientModel::~ClientModel()
{
    for ( int i = 0; i < m_openIterators.count(); ++i ) {
        m_client->iteratorClose( m_openIterators[i] );
    }
}


Soprano::Error::ErrorCode Soprano::Server::ClientModel::addStatement( const Statement &statement )
{
    Error::ErrorCode c = m_client->addStatement( m_modelId, statement );
    setError( m_client->lastError() );
    return c;
}


Soprano::NodeIterator Soprano::Server::ClientModel::listContexts() const
{
    int itId = m_client->listContexts( m_modelId );
    if ( itId > 0 ) {
        m_openIterators.append( itId );
    }
    setError( m_client->lastError() );
    if ( lastError() ) {
        return NodeIterator();
    }
    else {
        return new ClientNodeIteratorBackend( itId, const_cast<ClientModel*>( this ) );
    }
}


Soprano::QueryResultIterator Soprano::Server::ClientModel::executeQuery( const QueryLegacy &query ) const
{
    int itId = m_client->executeQuery( m_modelId, query );
    if ( itId > 0 ) {
        m_openIterators.append( itId );
    }
    setError( m_client->lastError() );
    if ( lastError() ) {
        return QueryResultIterator();
    }
    else {
        return new ClientQueryResultIteratorBackend( itId, const_cast<ClientModel*>( this ) );
    }
}


Soprano::StatementIterator Soprano::Server::ClientModel::listStatements( const Statement &partial ) const
{
    int itId = m_client->listStatements( m_modelId, partial );
    if ( itId > 0 ) {
        m_openIterators.append( itId );
    }
    setError( m_client->lastError() );
    if ( lastError() ) {
        return StatementIterator();
    }
    else {
        return new ClientStatementIteratorBackend( itId, const_cast<ClientModel*>( this ) );
    }
}


Soprano::Error::ErrorCode Soprano::Server::ClientModel::removeStatements( const Statement &statement )
{
    Error::ErrorCode c = m_client->removeStatements( m_modelId, statement );
    setError( m_client->lastError() );
    return c;
}


int Soprano::Server::ClientModel::statementCount() const
{
    int cnt = m_client->statementCount( m_modelId );
    setError( m_client->lastError() );
    return cnt;
}


bool Soprano::Server::ClientModel::containsStatements( const Statement &statement ) const
{
    bool c = m_client->containsStatements( m_modelId, statement );
    setError( m_client->lastError() );
    return c;
}


void Soprano::Server::ClientModel::closeIterator( int id ) const
{
    m_client->iteratorClose( id );
    m_openIterators.removeAll( id );
}

#include "clientmodel.moc"
