/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2007-2009 Sebastian Trueg <trueg@kde.org>
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

#ifndef _SOPRANO_CLUCENE_INDEX_H_
#define _SOPRANO_CLUCENE_INDEX_H_

#include <QtCore/QString>

#include "statement.h"
#include "soprano_export.h"
#include "error.h"
#include "iterator.h"
#include "indexqueryhit.h"


namespace lucene {
    namespace search {
        class Hits;
        class Query;
    }
    namespace analysis {
        class Analyzer;
    }
    namespace document {
        class Document;
        class Field;
    }
}

namespace Soprano {

    namespace Index {

        /**
         * \class CLuceneIndex cluceneindex.h Soprano/Index/CLuceneIndex
         *
         * \brief The CLuceneIndex provides a wrapper around a CLucene index
         * which stores RDF statements.
         *
         * It is used by the IndexFilterModel to actually handle the index. It has been
         * made visible in the public API to provide the possibility for advanced queries
         * and data modifications.
         *
         * CLuceneIndex is thread-safe.
         *
         * <b>Data organization</b>
         *
         * In the %Soprano index each resource gets its own clucene document. Only statements with literal
         * objects are indexed. The clucene is identified through the statements' subject and then a new
         * field is created that uses the statement's predicate as field name and the statement's object
         * as value. The index ignores all context information.
         *
         * \warning <b>The API is subject to change. Most likely CLucene classes will be wrapped and hidden from the public API.</b>
         *
         * \author Sebastian Trueg <trueg@kde.org>
         */
        class SOPRANO_INDEX_EXPORT CLuceneIndex : public Error::ErrorCache
        {
        public:
            //@{
            /**
             * \param analyzer The analyzer to be used. If 0 a standard analyzer will be created.
             */
            CLuceneIndex( lucene::analysis::Analyzer* analyzer = 0 );

            /**
             * Destructor.
             *
             * Calls close().
             */
            ~CLuceneIndex();
            //@}

            //@{
            /**
             * Open the index.
             * \param folder The folder where the index is stored.
             * \param force If true any CLucene locks on the folder are removed. This is useful
             * if a previous session crashed and left an unused lock lying around.
             */
            bool open( const QString& folder, bool force = false );

            /**
             * Close the index. Write back any changes, close any open transactions. (Is called in ~CLuceneIndex())
             */
            void close();

            /**
             * \return \p true If the index has been opened successfully.
             * \sa open()
             */
            bool isOpen() const;
            //@}

            //@{
            /**
             * Optionally set the analyzer to be used for query parsing. By default the index
             * analyzer specified in the constructor is used.
             *
             * CLuceneIndex will not take ownership of the analyzer.
             *
             * \sa search(const QString&)
             *
             * \since 2.3
             */
            void setQueryAnalyzer( lucene::analysis::Analyzer* analyzer );

            /**
             * Get the query analyzer set in setQueryAnalyzer. This method is only
             * provided for convenience.
             *
             * \return The analyzer set in setQueryAnalyzer or 0 if none was set.
             *
             * \since 2.3
             */
            lucene::analysis::Analyzer* queryAnalyzer() const;
            //@}

            //@{
            /**
             * Get the number of indexed resources.
             *
             * \return The number of indexed resoruces or -1 on error (In the case of error
             * lastError() provides more information.
             */
            int resourceCount() const;
            //@}

            //@{
            /**
             * Start a new transaction. After calling this method multiple fields and statements may be added to the
             * index and nothing is written back to disk. A transaction has to be closed. Otherwise the data will not be written to the index.
             * (All transactions are closed on deletion.)
             *
             * Methods such as addStatement will start and close a transaction internally if none has been started
             * before.
             *
             * \return A transaction id that has to be used to close the transaction. This is a safety mechanism to ensure
             * that no other user closes one's transaction. If another transaction has already been started 0 is returned.
             */
            int startTransaction();

            /**
             * Close a transaction and write the changes back to the index.
             *
             * \param id The transaction ID as returned by startTransaction()
             *
             * \return \p true if the transaction was closed,  false if no transaction was started,
             * a wrong transaction id has been supplied, or a clucene error occured.
             */
            bool closeTransaction( int id );
            //@}

            //@{
            /**
             * Indexes a statement.
             * \return An error code or 0 on success
             */
            Error::ErrorCode addStatement( const Soprano::Statement& );

            /**
             * Removes a statement from the index.
             * \return An error code or 0 on success
             */
            Error::ErrorCode removeStatement( const Soprano::Statement& );
            //@}

            //@{
            /**
             * Get a document for a specific resource. This is only possible after starting a transaction.
             *
             * \param resource The resource for which a document is requested.
             *
             * \return The document representing the resource or 0 if no transaction has been started or
             * a clucene error occured.
             *
             * \warning This is an advanced method. Calling this method is thread-safe but using the
             * returned document is not.
             */
            lucene::document::Document* documentForResource( const Node& resource );
            //@}

            //@{
            /**
             * Evaluates the given query.
             * Each hit is a resource and a score. Resource properties may be read from the model.
             *
             * \param query The query in the CLucene query language.
             *
             * \return The results as an iterator over QueryHit objects or an invalid iterator
             * on error.
             */
            Iterator<QueryHit> search( const QString& query );

            /**
             * Evaluates the given query.
             * Each hit is a resource and a score. Resource properties may be read from the model.
             *
             * \param query The query to evaluate. The iterator takes ownership of the query.
             * Do not delete.it! (If anyone using this understands the weird CLucene memory
             * management enough to fix this issue, please...)
             *
             * \return The results as an iterator over QueryHit objects or an invalid iterator
             * on error.
             *
             * \warning The result iterator uses the query object.
             */
            Iterator<QueryHit> search( lucene::search::Query* query );
            //@}

#if 0
            /**
             * Gets the score for a particular Resource and query. Returns a value < 0
             * when the Resource does not match the query.
             */
            double getScore( const Soprano::Node& resource, const QString& query );

            /**
             * Gets the score for a particular Resource and query. Returns a value < 0
             * when the Resource does not match the query.
             */
            double getScore( const Soprano::Node& resource, lucene::search::Query* query );
#endif

            /**
             * Returns the Resource corresponding with the specified Document.
             */
            static Soprano::Node getResource( lucene::document::Document* document );

            /**
             * This is a temporary method which supports the advanced query interface based on
             * CLucene classes.
             *
             * The %Soprano index creates one special field for each indexed resource which contains
             * all data to allow simple queries without property names or URIs.
             *
             * \return The name of the default CLucene search field to be queried when no properties
             * are known.
             */
            static QString defaultSearchField();

            //@{
            /**
             * Dump the index contents to the stream
             */
            void dump( QTextStream& ) const;

            /**
             * Clear the index, i.e. delete all entries.
             *
             * \since 2.1
             */
            void clear();

            /**
             * Optimize the index for search. This makes sense after adding or
             * removing a large number of statements.
             *
             * \since 2.2
             */
            void optimize();
            //@}

        private:
            class Private;
            Private* const d;
        };
    }
}

#endif
