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

#ifndef _SOPRANO_TRANSACTION_FACTORY_H_
#define _SOPRANO_TRANSACTION_FACTORY_H_

#include "error.h"
#include "soprano_export.h"

namespace Soprano {

    class Transaction;

    /**
     * \class TransactionFactory transactionfactory.h Soprano/TransactionFactory
     *
     * \brief Base class for transaction factories.
     *
     * When Transactions were introduced in %Soprano 2.2 Model::startTransaction
     * could not become a virtual method since binary compatibility with 2.1
     * was necessary. Thus, the TransactionFactory class came in to act as a
     * wrapper for the Transaction creation.
     *
     * Backend implementations and FilterModel subclasses wanting to support
     * transactions need to provide a TransactionFactory and set it via 
     * Model::setTransactionFactory.
     *
     * \sa Model::setTransactionFactory, Model::startTransaction
     *
     * \author Sebastian Trueg <trueg@kde.org>
     *
     * \since 2.2
     */
    class SOPRANO_EXPORT TransactionFactory : public Error::ErrorCache
    {
    public:
        /**
         * Desctructor
         */
        virtual ~TransactionFactory();

        /**
         * Create a new Transaction object. This method
         * needs to be reimplemented instead of Model::startTransaction.
         */
        virtual Transaction* startTransaction() = 0;

    protected:
        /**
         * Constructor
         */
        TransactionFactory();

    private:
        class Private;
        Private* const d;
    };
}

#endif
