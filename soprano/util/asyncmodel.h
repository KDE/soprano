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

#ifndef _SOPRANO_ASYNC_MODEL_H_
#define _SOPRANO_ASYNC_MODEL_H_

#include "filtermodel.h"
#include "soprano_export.h"


namespace Soprano {

    class Statement;
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
             * as been emitted.
             */
            QVariant value() const { return m_result; }

            /** \cond internal_asyncresult_members */
            /**
             * Internal method. Do not call.
             */
            void setResult( const QVariant& result, const Error::Error& error ) {
                m_result = result;
                setError( error );
                emit resultReady( this );
                deleteLater();
            }
            /** \endcond */

        private:
            AsyncResult()
                : QObject( 0 ) {}

            QVariant m_result;

            friend class AsyncModel;
        };

        class AsyncModelPrivate;

        /**
         * \class AsyncModel asyncmodel.h Soprano/Server/AsyncModel
         *
         * \brief Filter model that allows to perform operations
         * asyncroneously.
         *
         * The main purpose is to protect a Model against deadlocks
         * in a single threaded situation.
         *
         * Usage:
         * \code
         * AsyncResult* result = model->listStatementsAsync( s );
         * connect( result, SIGNAL(resultReady(AsyncResult*)),
         *          this, SLOT(slotResultReady(AsyncResult*)) );
         * \endcode
         *
         * \author Sebastian Trueg <trueg@kde.org>
         *
         * \since 2.1
         */
        class SOPRANO_EXPORT AsyncModel : public FilterModel
        {
            Q_OBJECT

        public:
            /**
             * Create a new Model.
             *
             * \param parent The parent model to forward the operations to.
             */
            AsyncModel( Model* parent = 0 );

            /**
             * Destructor.
             */
            ~AsyncModel();

            /**
             * Asyncroneously add the Statement to the Model.
             *
             * \param statement The Statement to add.
             *
             * \sa addStatement
             *
             * \return an AsyncResult with result type Error::ErrorCode
             * object which will signal when the result is ready.
             */
            AsyncResult* addStatementAsync( const Statement& statement );

            /**
             * Asyncroneously remove one statement. For removing statements
             * with wildward matching see removeAllStatementsAsync().
             *
             * \param statement The statement that should be removed.
             * This has to be a valid statement.
             *
             * \sa removeStatement
             *
             * \return an AsyncResult with result type Error::ErrorCode
             * object which will signal when the result is ready.
             */
            AsyncResult* removeStatementAsync( const Statement& statement );

            /**
             * Asyncroneously remove all statements that match the partial statement.
             * For removing one specific statement see removeStatement().
             *
             * \param statement A possible partially defined statement that serves as
             * a filter for all statements that should be removed.
             *
             * \sa removeAllStatements
             *
             * \return an AsyncResult with result type Error::ErrorCode
             * object which will signal when the result is ready.
             */
            AsyncResult* removeAllStatementsAsync( const Statement& statement );

            /**
             * Asyncroneously check if the Model does contain any Statement.
             *
             * \sa isEmpty
             *
             * \return an AsyncResult with result type bool
             * object which will signal when the result is ready.
             */
            AsyncResult* isEmptyAsync() const;

            /**
             * Asyncroneously determine the number of statements stored in this Model.
             *
             * \sa statementCount
             *
             * \return an AsyncResult with result type int
             * object which will signal when the result is ready.
             */
            AsyncResult* statementCountAsync() const;

            /**
             * Asyncroneously return an iterator over Model Statements that "partial"
             * match the input Statement.
             *
             * \param statement The partial Statement to match.
             *
             * \sa listStatements
             *
             * \return an AsyncResult with result type StatementIterator
             * object which will signal when the result is ready.
             */
            AsyncResult* listStatementsAsync( const Statement& statement ) const;

            /**
             * Asyncroneously list all contexts in the model, i.e. all named graphs.
             *
             * \sa listContexts
             *
             * \return an AsyncResult with result type NodeIterator
             * object which will signal when the result is ready.
             */
            AsyncResult* listContextsAsync() const;

            /**
             * Asyncroneously execute the given query over the Model.
             *
             * This is a const read-only method. As such Model implementations should not
             * support SPARQL extensions such as INSERT or UPDATE through this method.
             * A future version of %Soprano will provide an additional API for queries
             * that change the Model.
             *
             * \param query The query to evaluate.
             * \param language The %query language used to encode \p query.
             * \param userQueryLanguage If \p language equals Query::QueryLanguageUser
             * userQueryLanguage defines the language to use.
             * 
             * \sa executeQuery
             *
             * \return an AsyncResult with result type QueryResultIterator
             * object which will signal when the result is ready.
             */
            AsyncResult* executeQueryAsync( const QString& query, 
                                            Query::QueryLanguage language, 
                                            const QString& userQueryLanguage = QString() ) const;

            /**
             * AsyncResult check if the model contains a statements.
             *
             * \param statement The statement in question. This has to be a valid statement,
             * i.e. subject, predicate, and object need to be defined. If the context node
             * is empty the default graph is searched.
             *
             * \sa containsStatement
             *
             * \return an AsyncResult with result type bool
             * object which will signal when the result is ready.
             */
            AsyncResult* containsStatementAsync( const Statement& statement ) const;

            /**
             * Asyncroneously check if the model contains certain statements.
             *
             * \param statement A partially defined statement that serves as
             * a pattern.
             *
             * \sa containsAnyStatement
             *
             * \return an AsyncResult with result type bool
             * object which will signal when the result is ready.
             */
            AsyncResult* containsAnyStatementAsync( const Statement& statement ) const;

            /**
             * Asyncroneously create a new blank node with a unique identifier.
             *
             * \sa createBlankNode
             *
             * \return an AsyncResult with result type Node
             * object which will signal when the result is ready.
             */
            AsyncResult* createBlankNodeAsync();

        private:
            AsyncModelPrivate* const d;

            Q_PRIVATE_SLOT( d, void _s_executeNextCommand() )
        };
    }
}

#endif
