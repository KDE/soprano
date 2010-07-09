/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2009 Sebastian Trueg <trueg@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef _SOPRANO_UTIL_ASYNC_QUERY_H_
#define _SOPRANO_UTIL_ASYNC_QUERY_H_

#include <QtCore/QObject>
#include "error.h"
#include "sopranotypes.h"
#include "soprano_export.h"

namespace Soprano {

    class Statement;
    class BindingSet;
    class Node;
    class Model;

    namespace Util {
        /**
         * \class AsyncQuery asyncquery.h Soprano/Util/AsyncQuery
         *
         * \brief A wrapper around Soprano::Model which executes a query
         * in a separate thread and allows to iterate the results
         * asyncroneously.
         *
         * In contrast to AsyncModel everything is asyncroneous, not only
         * the execution of the query itself, but also the iteration.
         *
         * For executing a query asyncroneously simply use the static executeQuery()
         * method which will return a pointer to the newly created query object.
         *
         * AsyncQuery objects will always delete themselves once the end of
         * the iterator is reached and the finished signal has been emitted.
         * This also means that boolean results need to be read in a slot
         * connected to the finished() signal.
         *
         * Typical usage would be to connect to the nextReady signal and call
         * next() in it to trigger async iteration to the next element.
         *
         * \sa Soprano::Util::AsyncModel
         *
         * \author Sebastian Trueg <trueg@kde.org>
         *
         * \since 2.4
         */
        class SOPRANO_EXPORT AsyncQuery : public QObject, public Error::ErrorCache
        {
            Q_OBJECT

        public:
            /**
             * Delete the query. This will cancel an unfinished query.
             * Be aware that normally there is no need to delete the query
             * object as it will auto-delete itself once finished.
             *
             * \sa close()
             */
            ~AsyncQuery();

            //@{
            /**
             * Retrieve the current Statement after nextReady has been emitted.
             * This method does only make sense for graph queries.
             */
            Statement currentStatement() const;

            /**
             * Convenience method that puts all current bindings into one map.
             * This method does only make sense for tuple queries.
             */
            BindingSet currentBindings() const;

            /**
             * This method does only make sense for boolean queries.
             *
             * \return The result of a boolean query (SPARQL ASK).
             *
             * \sa isBool()
             */
            bool boolValue() const;
            //@}

            //@{
            /**
             * Get the current binding for a variable.
             *
             * \param name The name of the requested variable.
             *
             * This method does only make sense for tuple queries.
             *
             * \return The binding for the requested variable or and invalid
             * node if the bindings do not contain the variable.
             */
            Node binding( const QString &name ) const;

            /**
             * Get the current binding for a variable by index.
             *
             * \param offset The index of the requested variable.
             *
             * This method does only make sense for tuple queries.
             *
             * \return The binding for the requested variable or and invalid
             * node if offset is out of bounds, i.e. bigger or equal to bindingCount().
             */
            Node binding( int offset ) const;

            /**
             * The number of bindings in this query result.
             *
             * This method does only make sense for tuple queries.
             *
             * \return The number of bindings.
             */
            int bindingCount() const;

            /**
             * This method does only make sense for tuple queries.
             *
             * \return The names of the bound variables in this query result.
             */
            QStringList bindingNames() const;
            //@}

            //@{
            /**
             * Check if this is a graph result.
             *
             * \return \p true if this result refers to a graph query, i.e. currentStatement()
             * returns valid values.
             */
            bool isGraph() const;

            /**
             * Check if this is a tuple result.
             *
             * \return \p true if this result refers to a tuple query, i.e. currentBindings(),
             * binding(), bindingCount(), and bindingNames() return valid values.
             */
            bool isBinding() const;

            /**
             * Check if this is a boolean result.
             *
             * There is no need to call next() for boolean results. However, for internal reasons
             * backends need to always return \p true for boolean queries.
             *
             * \return \p true if this result refers to a boolean query (SPARQL ASK), i.e.
             * boolValue() returns a valid value.
             */
            bool isBool() const;
            //@}

        public Q_SLOTS:
            /**
             * Trigger iteration to the next element. Be aware that this has not to be called
             * for the first element which is emitted automatically.
             * Once the next result has been retrieved the nextReady signal is emitted.
             *
             * \return \p true if successful, \p false if the iteration reached the end.
             */
            bool next();

            /**
             * Closes the query. This will cancel the query if it is not finished yet.
             * Afterwards the query will delete itself. It has the same effect as
             * deleting the query object manually.
             *
             * finished() will always be emitted in case the query was not finished yet.
             */
            void close();

        Q_SIGNALS:
            /**
             * Emitted once the next value is ready when iterating the result via
             * next(). Will be emitted automatically for the first element.
             *
             * \param query The query itself for convinience.
             */
            void nextReady( Soprano::Util::AsyncQuery* query );

            /**
             * Emitted once the last element has been read and the internal
             * iterator is finished after the last call to next() or if the result
             * of a boolean query is available.
             *
             * Once this signals has been emitted the query will delete itself.
             * In a slot connected to this signal ErrorCache::lastError() can be
             * used to retrieve information about the success of the query.
             *
             * \param query The query itself for convinience.
             */
            void finished( Soprano::Util::AsyncQuery* query );

        public:
            /**
             * Create a new query object.
             *
             * \param model The model to execute the query on.
             * \param query The query to evaluate.
             * \param language The %query language used to encode \p query.
             * \param userQueryLanguage If \p language equals Query::QueryLanguageUser
             * userQueryLanguage defines the language to use.
             *
             * \sa Model::executeQuery
             *
             * \return A new AsyncQuery instance which is ready to be used or 0 if \p model
             * is 0. The query will delete itself once it is finished. It can also be deleted
             * at any point in time to cancel the query.
             */
            static AsyncQuery* executeQuery( Soprano::Model* model,
                                             const QString& query,
                                             Query::QueryLanguage language,
                                             const QString& userQueryLanguage = QString() );

        private:
            AsyncQuery();

            class Private;
            Private* const d;

            Q_PRIVATE_SLOT( d, void _s_finished() )
            Q_PRIVATE_SLOT( d, void _s_emitNextReady() )
        };
    }
}

#endif
