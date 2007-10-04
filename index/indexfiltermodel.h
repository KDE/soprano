/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2007 Sebastian Trueg <trueg@kde.org>
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

    namespace Index {

	class CLuceneIndex;
	class QueryResult;

	/**
	 * \class IndexFilterModel indexfiltermodel.h soprano/Index/IndexFilterModel
	 *
	 * \brief The IndexFilterModel provides a full text index around any Soprano Model.
	 *
	 * All statements with a literal object will be indexed.
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
	     * \param language The query language. Set to Soprano::Query::QUERY_LANGUAGE_USER for
	     *                 CLucene queries.
	     * \param userQueryLanguage If \p language equals Query::QUERY_LANGUAGE_USER
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

	    /**
	     * Extract full text matching parts of a %query and replace them with
	     * results from an index %query.
	     *
	     * \param query The query to rewrite.
	     *
	     * \return A rewritten query stripped of all full test and regexp matching parts
	     * and replaced by results from an index query.
	     */
//	    Query::Query evaluateAndRewriteQuery( const Query::Query& query ) const;

	private:
	    class Private;
	    Private* const d;
	};
    }
}

#endif
