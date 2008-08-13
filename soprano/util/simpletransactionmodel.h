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

#ifndef _SOPRANO_TRANSACTION_MODEL_H_
#define _SOPRANO_TRANSACTION_MODEL_H_

#include "filtermodel.h"
#include "soprano_export.h"

namespace Soprano {
    namespace Util {

        class SimpleTransactionModelPrivate;

        /**
         * \class SimpleTransactionModel simpletransactionmodel.h Soprano/Util/SimpleTransactionModel
         *
         * \brief Simple transaction filter model which handles transactions as a
         * sets of statements (added and removed).
         *
         * Transactions are handled as a set
         * of statements. Listing of statements is supported. Queries are always
         * performed on the parent model and, thus, do ignore the statements
         * in the transaction instance.
         *
         * An arbitrary number of Transactions can be open at the same time. Also
         * operations on the main model are still possible. These are simply
         * forwarded to the parent model.
         *
         * This %Model may be used to provide very simple transaction support
         * for backends that do not support it natively (see Soprano::BackendFeatureTransactions).
         *
         * \author Sebastian Trueg <trueg@kde.org>
         *
         * \since 3.0
         */
        class SOPRANO_EXPORT SimpleTransactionModel : public FilterModel
        {
            Q_OBJECT

        public:
            /**
             * Create a new TransactionModel.
             *
             * \param parent The parent Model to forward
             *        the actual calls to.
             */
            SimpleTransactionModel( Model* parent = 0 );

            /**
             * Destructor.
             */
            virtual ~SimpleTransactionModel();

            /**
             * Creates a new Transaction instance. Statements added and
             * removed from this Transaction are cached in simple sets.
             *
             * The Transaction instance supports all methods properly
             * except for Model::listContexts and Model::executeQuery. These are simply
             * forwarded to the parent model and, thus, do not include
             * the operations in the Transaction that have no yet been
             * commited.
             *
             * \return A new Transaction object. The caller needs to take
             * care of deletion.
             */
            Transaction* startTransaction();

        private:
            SimpleTransactionModelPrivate* const d;
        };
    }
}

#endif
