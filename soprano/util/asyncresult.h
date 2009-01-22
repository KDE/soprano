/* 
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2008-2009 Sebastian Trueg <trueg@kde.org>
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

#ifndef _SOPRANO_ASYNC_RESULT_H_
#define _SOPRANO_ASYNC_RESULT_H_

#include <QtCore/QObject>
#include <QtCore/QVariant>

#include "error.h"
#include "soprano_export.h"


namespace Soprano {

    class StatementIterator;
    class Node;
    class NodeIterator;
    class QueryResultIterator;

    namespace Util {

        class AsyncModel;

        /**
         * \class AsyncResult asyncmodel.h Soprano/Util/AsyncResult
         *
         * \brief A delayed result as returned by AsyncModel.
         *
         * \author Sebastian Trueg <trueg@kde.org>
         *
         * \since 2.1
         */
        class SOPRANO_EXPORT AsyncResult : public QObject, public Error::ErrorCache
        {
            Q_OBJECT

        Q_SIGNALS:
            /**
             * Emitted once the async operation is completed
             * and the result can be read.
             *
             * The result will delete itself.
             */
            void resultReady( Soprano::Util::AsyncResult* );

        public:
            /**
             * Constructor method to ensure binary compatibility.
             *
             * \since 2.2
             */
            static AsyncResult* createResult();

            ~AsyncResult();

            /**
             * The result of the async operation. Its type is dependent
             * on the operation (for example Error::ErrorCode for 
             * AsyncModel::addStatementAsync or StatementIterator for 
             * AsyncModel::listStatementsAsync). Types may need to be
             * registered with Q_DECLARE_METATYPE.
             *
             * Use Error::ErrorCache::lastError() to check
             * for error details.
             *
             * This value is not ready before resultReady()
             * has been emitted. <b>Do only use this in a slot connected to
             * resultReady.</b>
             *
             * \sa errorCode, statementIterator, nodeIterator, queryResultIterator, node
             */
            QVariant value() const { return m_result; }

            /**
             * Convinience method which converts value() into
             * a Error::ErrorCode as returned for the following AsyncModel methods:
             * \li AsyncModel::addStatementAsync
             * \li AsyncModel::removeStatementAsync
             * \li AsyncModel::removeAllStatementsAsync
             *
             * \sa value()
             *
             * \since 2.2
             */
            Error::ErrorCode errorCode() const;

            /**
             * Convinience method which converts value() into
             * a StatementIterator as returned AsyncModel::listStatementsAsync.
             *
             * \sa value()
             *
             * \since 2.2
             */
            StatementIterator statementIterator() const;

            /**
             * Convinience method which converts value() into
             * a StatementIterator as returned AsyncModel::listContextsAsync.
             *
             * \sa value()
             *
             * \since 2.2
             */
            NodeIterator nodeIterator() const;

            /**
             * Convinience method which converts value() into
             * a StatementIterator as returned AsyncModel::executeQueryAsync.
             *
             * \sa value()
             *
             * \since 2.2
             */
            QueryResultIterator queryResultIterator() const;

            /**
             * Convinience method which converts value() into
             * a StatementIterator as returned AsyncModel::createBlankNodeAsync.
             *
             * \sa value()
             *
             * \since 2.2
             */
            Node node() const;

            /**
             * Sets the final result and emits the resultReady signal. This method
             * should never be called by clients.
             */
            void setResult( const QVariant& result, const Error::Error& error );

        private Q_SLOTS:
            void slotResultReady();

        private:
            AsyncResult();

            QVariant m_result;

            friend class AsyncModel;
        };
    }
}

#endif
