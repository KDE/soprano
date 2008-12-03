/*
 * This file is part of Soprano Project.
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

#ifndef _SOPRANO_SERVER_CLIENT_TRANSACTION_H_
#define _SOPRANO_SERVER_CLIENT_TRANSACTION_H_

#include "transaction.h"
#include "clientmodel.h"

#include <QtCore/QList>
#include <QtCore/QPointer>

#include "clientconnection.h"


namespace Soprano {
    namespace Client {
        class ClientTransaction : public Transaction, public ClientModelBase
        {
            Q_OBJECT

        public:
            ClientTransaction( int transactionId, ClientModel* );
            ~ClientTransaction();

            Error::ErrorCode addStatement( const Statement &statement );
            NodeIterator listContexts() const;
            QueryResultIterator executeQuery( const QString& query, Query::QueryLanguage language, const QString& userQueryLanguage ) const;
            StatementIterator listStatements( const Statement &partial ) const;
            Error::ErrorCode removeStatement( const Statement &statement );
            Error::ErrorCode removeAllStatements( const Statement &statement );
            int statementCount() const;
            bool containsStatement( const Statement &statement ) const;
            bool containsAnyStatement( const Statement &statement ) const;
            Node createBlankNode();
            bool isEmpty() const;

        protected:
            Error::ErrorCode doRollback();
            Error::ErrorCode doCommit();

        private:
            int m_transactionId;
            QPointer<ClientConnection> m_client;
        };
    }
}

#endif
