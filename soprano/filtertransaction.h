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

#ifndef _SOPRANO_FILTER_TRANSACTION_H_
#define _SOPRANO_FILTER_TRANSACTION_H_

#include "transaction.h"
#include "soprano_export.h"


namespace Soprano {
    /**
     * \class FilterTransaction filtertransaction.h Soprano/FilterTransaction
     *
     * \brief A FilterTransaction is a virtual transaction that wraps another Transaction.
     * 
     * The FilterTransaction is intended to act as a basis for Transaction implementations
     * for FilterModels. The idea is that the FilterModel creates Transactions that 
     * reimplement the necessary methods from FilterTransaction and simply let the rest
     * be forwarded to the Transaction created by their parent model.
     *
     * \author Sebastian Trueg <trueg@kde.org>
     *
     * \since 2.2
     */
    class SOPRANO_EXPORT FilterTransaction : public Transaction
    {
        Q_OBJECT

    public:
        /**
         * Destructor
         */
        virtual ~FilterTransaction();

        //@{
        /**
         * Get the parent transaction.
         * \return The parent Transaction.
         */
        Transaction* parentTransaction() const;
        //@}

        //@{
        /**
         * Default implementation simply pipes the call through to the parent transaction.
         */
        virtual Error::ErrorCode addStatement( const Statement &statement );
        //@}

        //@{
        /**
         * Default implementation simply pipes the call through to the parent transaction.
         */
        virtual Error::ErrorCode removeStatement( const Statement &statement );

        /**
         * Default implementation simply pipes the call through to the parent transaction.
         */
        virtual Error::ErrorCode removeAllStatements( const Statement &statement );
        //@}

        //@{
        /**
         * Default implementation simply pipes the call through to the parent transaction.
         */
        virtual StatementIterator listStatements( const Statement &partial ) const;

        /**
         * Default implementation simply pipes the call through to the parent transaction.
         */
        virtual NodeIterator listContexts() const;

        /**
         * Default implementation simply pipes the call through to the parent transaction.
         */
        virtual QueryResultIterator executeQuery( const QString& query, Query::QueryLanguage language, const QString& userQueryLanguage = QString() ) const;
        //@}

        //@{
        /**
         * Default implementation simply pipes the call through to the parent transaction.
         */
        virtual bool containsStatement( const Statement &statement ) const;

        /**
         * Default implementation simply pipes the call through to the parent transaction.
         */
        virtual bool containsAnyStatement( const Statement &statement ) const;
        //@}

        //@{
        /**
         * Default implementation simply pipes the call through to the parent transaction.
         */
        virtual bool isEmpty() const;

        /**
         * Default implementation simply pipes the call through to the parent transaction.
         */
        virtual int statementCount() const;
        //@}

        //@{
        /**
         * Default implementation simply pipes the call through to the parent transaction.
         */
        virtual Error::ErrorCode write( QTextStream &os ) const;
        //@}

        //@{
        /**
         * Default implementation simply pipes the call through to the parent transaction.
         */
        virtual Node createBlankNode();
        //@}

        using Transaction::addStatement;
        using Transaction::removeStatement;
        using Transaction::removeAllStatements;
        using Transaction::listStatements;
        using Transaction::containsStatement;
        using Transaction::containsAnyStatement;

    protected:
        /**
         * Create a filter transaction.
         *
         * \param parent The parent Transaction to forward all calls to.
         * FilterTransaction will take ownership of parent.
         * \model The model that created the FilterTransactionTransaction.
         */
        FilterTransaction( Transaction* parent, Model* model );

        /**
         * Default implementation simply pipes the call through to the parent transaction.
         */
        virtual Error::ErrorCode doRollback();

        /**
         * Default implementation simply pipes the call through to the parent transaction.
         */
        virtual Error::ErrorCode doCommit();

        /**
         * Handle a statementsAdded() signal from the parent Transaction.
         *
         * The default implementation simply re-emits the signal.
         */
        virtual void parentStatementsAdded();

        /**
         * Handle a statementsRemoved() signal from the parent Transaction.
         *
         * The default implementation simply re-emits the signal.
         */
        virtual void parentStatementsRemoved();

        /**
         * Handle a statementsAdded() signal from the parent Transaction.
         *
         * The default implementation simply re-emits the signal.
         */
        virtual void parentStatementAdded( const Statement& );

        /**
         * Handle a statementsRemoved() signal from the parent Transaction.
         *
         * The default implementation simply re-emits the signal.
         */
        virtual void parentStatementRemoved( const Statement& );

    private Q_SLOTS:
        /**
         * This slot gets connected to Transaction::statementsAdded of the parent
         * transaction.
         */
        void slotStatementsAdded();

        /**
         * This slot gets connected to Transaction::statementsRemoved of the parent
         * transaction.
         */
        void slotStatementsRemoved();

        /**
         * This slot gets connected to Transaction::statementAdded of the parent
         * transaction.
         */
        void slotStatementAdded( const Soprano::Statement& );

        /**
         * This slot gets connected to Transaction::statementRemoved of the parent
         * transaction.
         */
        void slotStatementRemoved( const Soprano::Statement& );

    private:
        class Private;
        Private* const d;
    };
}

#endif
