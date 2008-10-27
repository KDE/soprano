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

#ifndef _SOPRANO_INDEX_FILTER_MODEL_PRIVATE_H_
#define _SOPRANO_INDEX_FILTER_MODEL_PRIVATE_H_

#include <QtCore/QSet>
#include "qurlhash.h"
#include "statement.h"

namespace Soprano {
    namespace Index {
        class IndexFilterModelPrivate
        {
        public:
            IndexFilterModelPrivate();

            bool deleteIndex;
            CLuceneIndex* index;
            
            QSet<QUrl> indexOnlyPredicates;
            QSet<QUrl> forceIndexPredicates;
            
            int transactionCacheSize;
            int transactionCacheId;
            int transactionCacheCount;
            
            void startTransaction();
            void closeTransaction();

            bool storeStatement( const Statement& s ) const;
            bool indexStatement( const Statement& s ) const;
        };
    }
}

#endif
