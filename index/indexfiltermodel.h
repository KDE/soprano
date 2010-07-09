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

#ifndef _SOPRANO_INDEX_MODEL_FILTER_H_
#define _SOPRANO_INDEX_MODEL_FILTER_H_

#include "filtermodel.h"
#include "soprano_export.h"

namespace Soprano {

    namespace Query {
        class Query;
    }

    /**
     * The %Soprano %Index provides a full text search index based on CLucene. It allows
     * very fast full text queries against the literal object nodes in a %Soprano Model.
     *
     * The perfect starting point is IndexFilterModel. Most users will not need more.
     *
     * In a future version of %Soprano the index will be integrated into the query API,
     * allowing for fast full text queries in combination with standard RDF queries.
     * At the moment these have to be done separately (see IndexFilterModel::executeQuery()).
     */
    namespace Index {

        class CLuceneIndex;
        class QueryResult;
        class IndexFilterModelPrivate;

        /**
         * \class IndexFilterModel indexfiltermodel.h Soprano/Index/IndexFilterModel
         *
         * \brief The IndexFilterModel provides a full text index around any Soprano Model.
         *
         * All statements with a literal object will be indexed. The literals can then be searched 
         * with CLucene queries. More details regarding queries can be found in the documentation fo
         * executeQuery().
         *
         * \author Sebastian Trueg <trueg@kde.org>
         */
        class SOPRANO_INDEX_EXPORT IndexFilterModel : public Soprano::FilterModel
        {
        public:
            /**
             * Create a new index model.
             *
             * \param indexDir The directory where the index should be stored. If the
             * directory already contains an index, it will be used. Otherwise a new
             * one will be created.
             * \param model The parent model to forward the calls to. If 0 the Model has to
             * be set later on with FilterModel::setParentModel.
             */
            IndexFilterModel( const QString& indexDir, Soprano::Model* model = 0 );

            /**
             * Create a new index model.
             *
             * \param index The index to be used. The filter model will NOT take ownership of
             * the index. The caller has to take care of deleting the index.
             * \param model The parent model to forward the calls to. If 0 the Model has to
             * be set later on with FilterModel::setParentModel.
             */
            IndexFilterModel( CLuceneIndex* index, Soprano::Model* model = 0 );
        
            /**
             * Destructor.
             */
            ~IndexFilterModel();

            /**
             * Retrieve the index used by this index model.
             */
            CLuceneIndex* index() const;

            /**
             * Adds a new statement.
             *
             * This will index the statement and then forward the call to the parent model.
             * Normally only statements with literal objects are indexed. An exception
             * are those statements with predicates that are set via setForceIndexPredicates.
             */
            Soprano::Error::ErrorCode addStatement( const Soprano::Statement &statement );

            /**
             * Removes a statement.
             *
             * This will remove the statement from the index and then forward the call to the parent model.
             */
            Soprano::Error::ErrorCode removeStatement( const Soprano::Statement &statement );

            /**
             * Removes statements.
             *
             * This will remove the statements from the index and then forward the call to the parent model.
             */
            Soprano::Error::ErrorCode removeAllStatements( const Soprano::Statement &statement );

            /**
             * The IndexFilterModel is currently based on CLucene. While the index itself is available
             * via index() and allows querying via CLucene queries it is not available over the 
             * Soprano::Client interface.
             * Thus, CLucene queries are supported through this method and will return QueryHit objects
             * wrapped in a QueryResultIterator.
             *
             * Future versions of %Soprano will support querying the index through the Soprano::Query API
             * (still unfinished and unstable).
             *
             * \param query The query string. This can be a CLucene query in which case the query will
             *              be passed to CLuceneIndex.
             * \param language The query language. Set to Soprano::Query::QueryLanguageUser for
             *                 CLucene queries.
             * \param userQueryLanguage If \p language equals Query::QueryLanguageUser
             *                          userQueryLanguage defines the language to use. Use <b>"lucene"</b>
             *                          to perform CLucene queries.
             *
             * \return An iterator over all results matching the query, 
             * on error an invalid iterator is returned. In case of a CLucene query the iterator will
             * wrap a set of QueryHit objects through the bindings <b>"resource"</b> and <b>"score"</b>.
             *
             * \sa CLuceneIndex::search()
             */
            QueryResultIterator executeQuery( const QString& query, Query::QueryLanguage language, const QString& userQueryLanguage = QString() ) const;

            /*
             * Extract full text matching parts of a %query and replace them with
             * results from an index %query.
             *
             * \param query The query to rewrite.
             *
             * \return A rewritten query stripped of all full test and regexp matching parts
             * and replaced by results from an index query.
             */
//        Query::Query evaluateAndRewriteQuery( const Query::Query& query ) const;

            /**
             * Set the number or addStatement operations that are to be cached in the index.
             * The default value is 1 which means that no caching occurs. Be aware that query
             * operations will always close cached transactions.
             *
             * \param size The number of operations that should be handled in one transaction.
             * Set to 1 to disable.
             *
             * \sa CLuceneIndex::startTransaction, CLuceneIndex::closeTransaction
             */
            void setTransactionCacheSize( int size );
        
            /**
             * The number of addStatement operations to cache in one transaction.
             *
             * \sa setTransactionCacheSize
             */
            int transactionCacheSize() const;

            /**
             * Rebuild the complete index. This means that the index will be cleared and all 
             * literal statements will be re-indexed.
             *
             * This method is purely intended for maintenance.
             *
             * \since 2.1
             */
            void rebuildIndex();

            /**
             * Optimize the index for search. This makes sense after adding or
             * removing a large number of statements.
             *
             * \sa CLuceneIndex::optimize
             *
             * \since 2.2
             */
            void optimizeIndex();

            /**
             * Add a predicate which should only be indexed. This might be useful
             * for very large literals whose value is of no interest but which
             * should be searchable.
             *
             * \param predicate The predicate that should only be indexed
             * but not stored in the underlying Model.
             *
             * \sa indexOnlyPredicates, setIndexOnlyPredicates
             *
             * \since 2.1
             */
            void addIndexOnlyPredicate( const QUrl& predicate );

            /**
             * Set the predicates that should only be indexed. This might be useful
             * for very large literals whose value is of no interest but which
             * should be searchable.
             *
             * \param predicates A list of predicates that should only be indexed
             * but not stored in the underlying Model.
             *
             * \sa indexOnlyPredicates, addIndexOnlyPredicate
             *
             * \since 2.1
             */
            void setIndexOnlyPredicates( const QList<QUrl>& predicates );

            /**
             * The IndexFilterModel supports to not forward certain predicates to
             * the parent Model but only index their value. This might be useful
             * for very large literals whose value is of no interest but which
             * should be searchable.
             *
             * \return A list of predicates that will only be indexed but not
             * stored in the underlying Model.
             *
             * \sa addIndexOnlyPredicate, setIndexOnlyPredicates
             *
             * \since 2.1
             */
            QList<QUrl> indexOnlyPredicates() const;

            /**
             * Add a predicate which should be indexed even if the object is a resource.
             * See setForceIndexPredicates for a detailed explanation.
             *
             * \param predicate The predicate that should be indexed
             * in any case.
             *
             * \sa forceIndexPredicates, setForceIndexPredicates
             *
             * \since 2.2
             */
            void addForceIndexPredicate( const QUrl& predicate );

            /**
             * Normally only statements with a literal object are indexed when added
             * thorugh addStatement. In some cases however, it is useful to also index 
             * resource objects.
             *
             * Statement with a resource object (a URI) and a predicate that matches
             * one of the force index predicates, are converted to strings using
             * QUrl::toEncoded and added to the index non-tokenized. Thus, the resources
             * will be searchable directly via a 'field:uri' query but not via the
             * default search  field.
             *
             * A typical and very useful predicate is Vocabulary::RDF::type(). 
             * This allows to restrict the type of resources in lucene queries:
             *
             * \code
             * model->executeQuery( QString( "foobar AND %1:%2" )
             *                      .arg( encodeUriForLuceneQuery( RDF::type() ) )
             *                      .arg( encodeUriForLuceneQuery( myType ) ),
             *                      Query::QueryLanguageUser,
             *                      "lucene" );
             * \endcode
             *
             * \param predicates The predicates that should be indexed
             * in any case.
             *
             * \sa forceIndexPredicates, addForceIndexPredicate
             *
             * \since 2.2
             */
            void setForceIndexPredicates( const QList<QUrl>& predicates );

            /**
             * See setForceIndexPredicates for a detailed explanation.
             *
             * \return A list of predicates that will be indexed even if the object
             * is not a literal.
             *
             * \sa addForceIndexPredicate, setForceIndexPredicates
             *
             * \since 2.2
             */
            QList<QUrl> forceIndexPredicates() const;

            /**
             * Encodes a string to be used in a lucene query. String values
             * may contain characters that are reserved in lucene queries.
             * These are property escaped by this method.
             *
             * This method converts an arbitrary string into a string that can be used
             * in a lucene query.
             *
             * \param value The string to be encoded.
             *
             * \return An encoded and escaped string representation of the 
             * provided string.
             *
             * \sa encodeUriForLuceneQuery
             *
             * \since 2.2
             */
            static QString encodeStringForLuceneQuery( const QString& value );

            /**
             * Encodes a URI to be used in a lucene query. URIs often contain
             * characters that are reserved in lucene queries and, thus, need
             * to be escaped. In addition, the URIs are encoded by the index
             * model for storage in clucene.
             *
             * This method converts a URI into a string that can be used
             * in a lucene query.
             *
             * \param uri The URI to be encoded.
             *
             * \return An encoded and escaped string representation of the URI.
             *
             * \sa encodeStringForLuceneQuery
             *
             * \since 2.2
             */
            static QString encodeUriForLuceneQuery( const QUrl& uri );

            using FilterModel::addStatement;
            using FilterModel::removeStatement;
            using FilterModel::removeAllStatements;

        private:
            IndexFilterModelPrivate* const d;
        };
    }
}

#endif
