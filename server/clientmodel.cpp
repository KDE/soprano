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
#include "clientconnection.h"
#include "clientnodeiteratorbackend.h"
#include "clientstatementiteratorbackend.h"
#include "clientqueryresultiteratorbackend.h"

#include <soprano/backend.h>
#include <soprano/error.h>
#include <soprano/nodeiterator.h>
#include <soprano/statementiterator.h>
#include <soprano/queryresultiterator.h>


Soprano::Server::ClientModel::ClientModel( const Backend* backend, int modelId, ClientConnection* client )
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
    if ( m_client ) {
        Error::ErrorCode c = m_client->addStatement( m_modelId, statement );
        setError( m_client->lastError() );
        return c;
    }
    else {
        setError( "Not connected to server." );
        return Error::ERROR_UNKNOWN;
    }
}


Soprano::NodeIterator Soprano::Server::ClientModel::listContexts() const
{
    if ( m_client ) {
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
    else {
        setError( "Not connected to server." );
        return NodeIterator();
    }
}


Soprano::QueryResultIterator Soprano::Server::ClientModel::executeQuery( const QueryLegacy &query ) const
{
    if ( m_client ) {
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
    else {
        setError( "Not connected to server." );
        return QueryResultIterator();
    }
}


Soprano::StatementIterator Soprano::Server::ClientModel::listStatements( const Statement &partial ) const
{
    if ( m_client ) {
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
    else {
        setError( "Not connected to server." );
        return StatementIterator();
    }
}


Soprano::Error::ErrorCode Soprano::Server::ClientModel::removeStatements( const Statement &statement )
{
    if ( m_client ) {
        Error::ErrorCode c = m_client->removeStatements( m_modelId, statement );
        setError( m_client->lastError() );
        return c;
    }
    else {
        setError( "Not connected to server." );
        return Error::ERROR_UNKNOWN;
    }
}


int Soprano::Server::ClientModel::statementCount() const
{
    if ( m_client ) {
        int cnt = m_client->statementCount( m_modelId );
        setError( m_client->lastError() );
        return cnt;
    }
    else {
        setError( "Not connected to server." );
        return -1;
    }
}


bool Soprano::Server::ClientModel::containsStatements( const Statement &statement ) const
{
    if ( m_client ) {
        bool c = m_client->containsStatements( m_modelId, statement );
        setError( m_client->lastError() );
        return c;
    }
    else {
        setError( "Not connected to server." );
        return false;
    }
}


void Soprano::Server::ClientModel::closeIterator( int id ) const
{
    if ( m_client ) {
        clearError();
        if ( m_openIterators.contains( id ) ) {
            m_client->iteratorClose( id );
            m_openIterators.removeAll( id );
            setError( m_client->lastError() );
        }
    }
    else {
        setError( "Not connected to server." );
    }
}

#include "clientmodel.moc"
