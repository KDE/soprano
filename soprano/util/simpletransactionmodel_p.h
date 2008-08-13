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

#ifndef _SOPRANO_SIMPLE_TRANSACTION_MODEL_P_H_
#define _SOPRANO_SIMPLE_TRANSACTION_MODEL_P_H_

#include <QtCore/QMutex>
#include <QtCore/QSet>

#include "simpletransaction.h"

namespace Soprano {

    uint qHash( const Soprano::Statement& s );

    namespace Util {

        class SimpleTransactionModel;
        class SimpleTransactionStatementIteratorBackend;

        class SimpleTransactionModelPrivate
        {
        public:
            SimpleTransactionModel* model;
            QList<SimpleTransaction*> openTransactions;
            QMutex mutex;
        };

        class SimpleTransactionPrivate
        {
        public:
            SimpleTransactionModelPrivate* modelPrivate;

            QSet<Statement> addedStatements;
            QSet<Statement> removedStatements;

            QList<SimpleTransactionStatementIteratorBackend*> openIterators;

            /**
             * Checks if the cache contains the statement, including
             * wildcards. Thus, the result would be the same if one
             * would commit on an empty parent model.
             */
            bool containsAnyStatement( const Statement& s );
        };
    }
}

#endif
