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

#ifndef _SOPRANO_SIMPLE_TRANSACTION_H_
#define _SOPRANO_SIMPLE_TRANSACTION_H_

#include "transaction.h"

namespace Soprano {
    namespace Util {

        class SimpleTransactionPrivate;
        class SimpleTransactionModelPrivate;

        /**
         * \brief Simple Transaction implementation as returned by 
         * SimpleTransactionModel::startTransaction.
         *
         * The SimpleTransaction caches added and removed statements in
         * two sets.
         *
         * Except for listContexts and executeQuery all methods return
         * proper values depending on the cache.
         *
         * \author Sebastian Trueg <trueg@kde.org>
         */
        class SimpleTransaction : public Soprano::Transaction
        {
        public:
            SimpleTransaction( SimpleTransactionModelPrivate* p );
            ~SimpleTransaction();

            /**
             * A SimpleTransaction is invalid only if the original
             * model is deleted.
             */
            bool isValid() const;

            Error::ErrorCode addStatement( const Statement& statement );
            Error::ErrorCode removeStatement( const Statement& statement );
            Error::ErrorCode removeAllStatements( const Statement& statement );
            StatementIterator listStatements( const Statement& partial ) const;
            bool containsStatement( const Statement& statement ) const;
            bool containsAnyStatement( const Statement& statement ) const;
            NodeIterator listContexts() const;
            QueryResultIterator executeQuery( const QString& query, Query::QueryLanguage language, const QString& userQueryLanguage = QString() ) const;
            bool isEmpty() const;
            int statementCount() const;
            Node createBlankNode();

            /**
             * Called from the SimpleTransactionModel's destructor
             * to avoid crashes.
             */
            void invalidate();
            
        protected:
            Error::ErrorCode doRollback();
            Error::ErrorCode doCommit();

        private:
            SimpleTransactionPrivate* const d;
        };
    }
}

#endif
