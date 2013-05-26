/*
 * This file is part of Soprano Project
 *
 * Copyright (C) 2009 Sebastian Trueg <trueg@kde.org>
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

#ifndef _SOPRANO_ODBC_QUERY_RESULT_ITERATOR_BACKEND_P_H_
#define _SOPRANO_ODBC_QUERY_RESULT_ITERATOR_BACKEND_P_H_

#include "odbcqueryresult.h"

#include "statement.h"
#include "statementiterator.h"
#include "nodeiterator.h"
#include "queryresultiterator.h"
#include "literalvalue.h"
#include "node.h"

#include <QtCore/QVector>
#include <QtCore/QBitArray>
#include <QtCore/QHash>
#include <QtCore/QMutex>


namespace Soprano {
    class VirtuosoModelPrivate;

    namespace Virtuoso {
        class QueryResultIteratorBackendPrivate
        {
        public:
            ODBC::QueryResult* m_queryResult;

            QStringList bindingNames;
            QHash<QString, int> bindingIndexHash;

            QVector<Node> bindingCache;
            QBitArray bindingCachedFlags;

            enum ResultType {
                UnknownResult = 0,
                GraphResult,
                AskResult,
                BindingResult,
                MethodCallResult
            };
            ResultType m_resultType;

            bool askResult;

            StatementIterator graphIterator;

            bool m_methodCallResultIterated;

            VirtuosoModelPrivate* m_model;

            QMutex m_closeMutex;
        };
    }
}

#endif
