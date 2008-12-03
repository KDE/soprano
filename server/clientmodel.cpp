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

#include "clientmodel.h"
#include "clienttransaction.h"
#include "clientconnection.h"
#include "clientnodeiteratorbackend.h"
#include "clientstatementiteratorbackend.h"
#include "clientqueryresultiteratorbackend.h"

#include "backend.h"
#include "error.h"
#include "nodeiterator.h"
#include "statementiterator.h"
#include "queryresultiterator.h"
#include "transactionfactory.h"

namespace Soprano {
    namespace Client {
        class ClientModelTransactionFactory : public TransactionFactory
        {
        public:
            ClientModelTransactionFactory( ClientModel* model, int modelId )
                : m_model( model ),
                  m_modelId( modelId ) {
            }

            ~ClientModelTransactionFactory() {
                for ( int i = 0; i < m_openTransactions.count(); ++i ) {
                    m_model->connection()->closeTransaction( m_openTransactions[i] );
                }
            }

            /**
             * called by ClientTransaction on delete
             */
            void closeTransaction( int id ) {
                if ( m_openTransactions.contains( id ) ) {
                    m_model->connection()->closeTransaction( id );
                    m_openTransactions.removeAll( id );
                    setError( m_model->connection()->lastError() );
                }
            }

            Transaction* startTransaction() {
                int id = m_model->connection()->startTransaction( m_modelId );
                setError( m_model->connection()->lastError() );
                if ( id > 0 ) {
                    ClientTransaction* t = new ClientTransaction( id, m_model );
                    m_openTransactions.append( id );
                    return t;
                }
                return 0;
            }

        private:
            ClientModel* m_model;
            int m_modelId;
            QList<quint32> m_openTransactions;
        };
    }
}


Soprano::Client::ClientModel::ClientModel( const Backend* backend, int modelId, ClientConnection* client )
    : StorageModel( backend ),
      ClientModelBase( client ),
      m_modelId( modelId ),
      m_client( client )
{
    setTransactionFactory( new ClientModelTransactionFactory( this, modelId ) );
}


Soprano::Client::ClientModel::~ClientModel()
{
}


Soprano::Error::ErrorCode Soprano::Client::ClientModel::addStatement( const Statement &statement )
{
    if ( m_client ) {
        Error::ErrorCode c = m_client->addStatement( m_modelId, statement );
        setError( m_client->lastError() );
        return c;
    }
    else {
        setError( "Not connected to server." );
        return Error::ErrorUnknown;
    }
}


Soprano::NodeIterator Soprano::Client::ClientModel::listContexts() const
{
    if ( m_client ) {
        int itId = m_client->listContexts( m_modelId );
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


Soprano::QueryResultIterator Soprano::Client::ClientModel::executeQuery( const QString& query, Query::QueryLanguage language, const QString& userQueryLanguage ) const
{
    if ( m_client ) {
        int itId = m_client->executeQuery( m_modelId, query, language, userQueryLanguage );
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


Soprano::StatementIterator Soprano::Client::ClientModel::listStatements( const Statement &partial ) const
{
    if ( m_client ) {
        int itId = m_client->listStatements( m_modelId, partial );
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


Soprano::Error::ErrorCode Soprano::Client::ClientModel::removeStatement( const Statement &statement )
{
    if ( m_client ) {
        Error::ErrorCode c = m_client->removeStatement( m_modelId, statement );
        setError( m_client->lastError() );
        return c;
    }
    else {
        setError( "Not connected to server." );
        return Error::ErrorUnknown;
    }
}


Soprano::Error::ErrorCode Soprano::Client::ClientModel::removeAllStatements( const Statement &statement )
{
    if ( m_client ) {
        Error::ErrorCode c = m_client->removeAllStatements( m_modelId, statement );
        setError( m_client->lastError() );
        return c;
    }
    else {
        setError( "Not connected to server." );
        return Error::ErrorUnknown;
    }
}


int Soprano::Client::ClientModel::statementCount() const
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


bool Soprano::Client::ClientModel::containsStatement( const Statement &statement ) const
{
    if ( m_client ) {
        bool c = m_client->containsStatement( m_modelId, statement );
        setError( m_client->lastError() );
        return c;
    }
    else {
        setError( "Not connected to server." );
        return false;
    }
}


bool Soprano::Client::ClientModel::containsAnyStatement( const Statement &statement ) const
{
    if ( m_client ) {
        bool c = m_client->containsAnyStatement( m_modelId, statement );
        setError( m_client->lastError() );
        return c;
    }
    else {
        setError( "Not connected to server." );
        return false;
    }
}


Soprano::Node Soprano::Client::ClientModel::createBlankNode()
{
    if ( m_client ) {
        Node n = m_client->createBlankNode( m_modelId );
        setError( m_client->lastError() );
        return n;
    }
    else {
        setError( "Not connected to server." );
        return Node();
    }
}


bool Soprano::Client::ClientModel::isEmpty() const
{
    if ( m_client ) {
        bool r = m_client->isEmpty( m_modelId );
        setError( m_client->lastError() );
        return r;
    }
    else {
        setError( "Not connected to server." );
        return false;
    }
}

#include "clientmodel.moc"
