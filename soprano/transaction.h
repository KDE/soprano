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

#ifndef _SOPRANO_TRANSACTION_H_
#define _SOPRANO_TRANSACTION_H_

#include "model.h"
#include "soprano_export.h"

namespace Soprano {
    /**
     * \class Transaction transaction.h Soprano/Transaction
     *
     * \brief Represents on transaction as started via Model::startTransaction.
     *
     * A %Transaction encapsulates a set of commands performed on a Model.
     * The changes are not final in the model until commit() is called.
     * Also, all operations can be reverted by calling rollback() or
     * simply deleting the Transaction object without commiting.
     * This allows to simply group a set of commands which only make 
     * sense as a whole. If any of the commands fails or some other
     * operation does, the Transaction can simply be rolled back.
     *
     * As Transaction is a subclass of Model, it not only provides
     * all the convinience methods Model provides, but it is also
     * perfectly valid to stack an arbitrary set of FilterModel 
     * implementations on top of a Transaction.
     *
     * \sa Model::startTransaction
     *
     * \author Sebastian Trueg <trueg@kde.org>
     *
     * \since 3.0
     */
    class SOPRANO_EXPORT Transaction : public Model
    {
    public:
        /**
         * Destructor. If the transaction is not commited
         * on destruction, it will be rolled back.
         */
        virtual ~Transaction();

        /**
         * \return The Model that created this transaction object.
         * \sa Model::startTransaction
         */
        Model* model() const;

        /**
         * Check if the transaction object is valid. A transaction
         * is valid until it has been commited.
         *
         * \return \p true if the transaction is valid, \p false
         * otherwise.
         *
         * The default implementation handles commited and rolled
         * back transactions as invalid.
         */
        virtual bool isValid() const;

        /**
         * Rollback the transaction. This reverts all operations
         * done so far.
         *
         * FIXME: should a Transaction stay valid after rollback? If not, there
         * is no real need for this method - one could simply delete the object.
         *
         * \return Error::ErrorNone on success, an error code otherwise.
         */
        Error::ErrorCode rollback();

        /**
         * Commit the transaction, ie. apply the operations done after
         * the transaction has been started. Once commited a Transaction
         * object becomes useless and should be deleted.
         *
         * \return Error::ErrorNone on success, an error code otherwise.
         */
        Error::ErrorCode commit();

    protected:
        /**
         * \param model The model that created the Transaction.
         *
         * \sa model()
         */
        Transaction( Model* model );

        /**
         * This method needs to be implemented in subclasses to actually
         * perform the rollback. Will only be called on valid Transaction
         * objects.
         */
        virtual Error::ErrorCode doRollback() = 0;

        /**
         * This method needs to be implemented in subclasses to actually
         * perform the commit. Will only be called on valid Transaction
         * objects.
         */
        virtual Error::ErrorCode doCommit() = 0;

    private:
        class Private;
        Private* const d;
    };
}

#endif
