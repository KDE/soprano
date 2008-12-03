/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2007-2008 Sebastian Trueg <trueg@kde.org>
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

//
// Almost all methods in this class are direct copies from ClientModel, would be great to share code.
//

#include "clienttransaction.h"
#include "clientconnection.h"
#include "clientnodeiteratorbackend.h"
#include "clientstatementiteratorbackend.h"
#include "clientqueryresultiteratorbackend.h"

#include "error.h"
#include "nodeiterator.h"
#include "statementiterator.h"
#include "queryresultiterator.h"



Soprano::Client::ClientTransaction::ClientTransaction( int transactionId, ClientModel* model )
    : Transaction( model ),
      ClientModelBase( model->connection() ),
      m_transactionId( transactionId ),
      m_client( model->connection() )
{
}


Soprano::Client::ClientTransaction::~ClientTransaction()
{
}


Soprano::Error::ErrorCode Soprano::Client::ClientTransaction::addStatement( const Statement &statement )
{
    if ( m_client ) {
        Error::ErrorCode c = m_client->addStatement( m_transactionId, statement );
        setError( m_client->lastError() );
        return c;
    }
    else {
        setError( "Not connected to server." );
        return Error::ErrorUnknown;
    }
}


Soprano::NodeIterator Soprano::Client::ClientTransaction::listContexts() const
{
    if ( m_client ) {
        int itId = m_client->listContexts( m_transactionId );
        setError( m_client->lastError() );
        if ( itId > 0 ) {
            ClientNodeIteratorBackend* it = new ClientNodeIteratorBackend( itId, this );
            addIterator( it );
            return it;
        }
        return NodeIterator();
    }
    else {
        setError( "Not connected to server." );
        return NodeIterator();
    }
}


Soprano::QueryResultIterator Soprano::Client::ClientTransaction::executeQuery( const QString& query, Query::QueryLanguage language, const QString& userQueryLanguage ) const
{
    if ( m_client ) {
        int itId = m_client->executeQuery( m_transactionId, query, language, userQueryLanguage );
        setError( m_client->lastError() );
        if ( itId > 0 ) {
            ClientQueryResultIteratorBackend* it = new ClientQueryResultIteratorBackend( itId, this );
            addIterator( it );
            return it;
        }
        return QueryResultIterator();
    }
    else {
        setError( "Not connected to server." );
        return QueryResultIterator();
    }
}


Soprano::StatementIterator Soprano::Client::ClientTransaction::listStatements( const Statement &partial ) const
{
    if ( m_client ) {
        int itId = m_client->listStatements( m_transactionId, partial );
        setError( m_client->lastError() );
        if ( itId > 0 ) {
            ClientStatementIteratorBackend* it = new ClientStatementIteratorBackend( itId, this );
            addIterator( it );
            return it;
        }
        return StatementIterator();
    }
    else {
        setError( "Not connected to server." );
        return StatementIterator();
    }
}


Soprano::Error::ErrorCode Soprano::Client::ClientTransaction::removeStatement( const Statement &statement )
{
    if ( m_client ) {
        Error::ErrorCode c = m_client->removeStatement( m_transactionId, statement );
        setError( m_client->lastError() );
        return c;
    }
    else {
        setError( "Not connected to server." );
        return Error::ErrorUnknown;
    }
}


Soprano::Error::ErrorCode Soprano::Client::ClientTransaction::removeAllStatements( const Statement &statement )
{
    if ( m_client ) {
        Error::ErrorCode c = m_client->removeAllStatements( m_transactionId, statement );
        setError( m_client->lastError() );
        return c;
    }
    else {
        setError( "Not connected to server." );
        return Error::ErrorUnknown;
    }
}


int Soprano::Client::ClientTransaction::statementCount() const
{
    if ( m_client ) {
        int cnt = m_client->statementCount( m_transactionId );
        setError( m_client->lastError() );
        return cnt;
    }
    else {
        setError( "Not connected to server." );
        return -1;
    }
}


bool Soprano::Client::ClientTransaction::containsStatement( const Statement &statement ) const
{
    if ( m_client ) {
        bool c = m_client->containsStatement( m_transactionId, statement );
        setError( m_client->lastError() );
        return c;
    }
    else {
        setError( "Not connected to server." );
        return false;
    }
}


bool Soprano::Client::ClientTransaction::containsAnyStatement( const Statement &statement ) const
{
    if ( m_client ) {
        bool c = m_client->containsAnyStatement( m_transactionId, statement );
        setError( m_client->lastError() );
        return c;
    }
    else {
        setError( "Not connected to server." );
        return false;
    }
}


Soprano::Node Soprano::Client::ClientTransaction::createBlankNode()
{
    if ( m_client ) {
        Node n = m_client->createBlankNode( m_transactionId );
        setError( m_client->lastError() );
        return n;
    }
    else {
        setError( "Not connected to server." );
        return Node();
    }
}


bool Soprano::Client::ClientTransaction::isEmpty() const
{
    if ( m_client ) {
        bool r = m_client->isEmpty( m_transactionId );
        setError( m_client->lastError() );
        return r;
    }
    else {
        setError( "Not connected to server." );
        return false;
    }
}


Soprano::Error::ErrorCode Soprano::Client::ClientTransaction::doRollback()
{
    if( m_client ) {
        Error::ErrorCode r = m_client->rollback( m_transactionId );
        setError( m_client->lastError() );
        return r;
    }
    else {
        setError( "Not connected to server." );
        return Error::ErrorUnknown;
    }
}


Soprano::Error::ErrorCode Soprano::Client::ClientTransaction::doCommit()
{
    if( m_client ) {
        Error::ErrorCode r = m_client->commit( m_transactionId );
        setError( m_client->lastError() );
        return r;
    }
    else {
        setError( "Not connected to server." );
        return Error::ErrorUnknown;
    }
}

#include "clienttransaction.moc"
