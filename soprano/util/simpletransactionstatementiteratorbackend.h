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

#ifndef _SOPRANO_SIMPLE_TRANSACTION_STATEMENT_ITERATOR_BACKEND_H_
#define _SOPRANO_SIMPLE_TRANSACTION_STATEMENT_ITERATOR_BACKEND_H_

#include "iteratorbackend.h"
#include "statementiterator.h"

#include <QtCore/QSet>


namespace Soprano {
    namespace Util {

        class SimpleTransactionPrivate;

        class SimpleTransactionStatementIteratorBackend : public IteratorBackend<Statement>
        {
        public:
            SimpleTransactionStatementIteratorBackend( const StatementIterator& originalIt, SimpleTransactionPrivate* transactionPrivate );
            ~SimpleTransactionStatementIteratorBackend();

            bool next();
            Statement current() const;
            void close();

        private:
            SimpleTransactionPrivate* m_transactionPrivate;

            // the original iterator
            StatementIterator m_iterator;

            // the iterator over the added statements
            QSet<Statement>::const_iterator m_addedCacheIterator;

            // state, i.e. did we already fully iterate the original iterator
            bool m_iteratorDone;
        };
    }
}

#endif
