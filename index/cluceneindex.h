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

#ifndef _SOPRANO_CLUCENE_INDEX_H_
#define _SOPRANO_CLUCENE_INDEX_H_

#include <QtCore/QString>

#include "statement.h"
#include "soprano_export.h"
#include "error.h"


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
	 * \class CLuceneIndex cluceneindex.h soprano/Index/CLuceneIndex
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
	 * In the %Soprano index each resouce gets its own clucene document. Only statements with literal
	 * objects are indexed. The clucene is identified through the statements' subject and then a new
	 * field is created that uses the statement's predicate as field name and the statement's object
	 * as value. The index ignores all context information.
	 *
	 * \warning <b>The API is subject to change. Most likely CLucene classes will be wrapped and hidden from the public API.</b>
	 *
	 * \author Sebastian Trueg <trueg@kde.org>
	 */
	class SOPRANO_EXPORT CLuceneIndex : public Error::ErrorCache
	{
	public:
	    /**
	     * \param analyzer The analyzer to be used. If 0 a standard analyzer will be created.
	     */
	    CLuceneIndex( lucene::analysis::Analyzer* analyzer = 0 );
	    ~CLuceneIndex();

	    /**
	     * Open the index.
	     * \param folder The folder where the index is stored.
	     * \param force If true any CLucene locks on the folder are removed. This is useful
	     * if a previous session crashed and left an unused lock lying around.
	     */
	    bool open( const QString& folder, bool force = false );

	    void close();

	    bool isOpen() const;

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

	    /**
	     * Get a document for a specific resource. This is only possible after starting a transaction.
	     *
	     * \param resource The resource for which a document is requested.
	     *
	     * \return The document representing the resource or 0 if no transaction has been started or
	     * a clucene error occured.
	     */
	    lucene::document::Document* documentForResource( const Node& resource );
	    
	    /**
	     * Add a field to a document.
	     *
	     * \param resource The resource to identify the document.
	     * \param field The clucene field to add. The index will take ownership of field.
	     *
	     * \return An error code or 0 on success
	     */
	    Error::ErrorCode addFieldToResourceDocument( const Node& resource, lucene::document::Field* field );

	    /**
	     * Evaluates the given query and returns the results as a Hits instance.
	     */
	    lucene::search::Hits* search( const QString& query );

	    /**
	     * Evaluates the given query and returns the results as a Hits instance.
	     */
	    lucene::search::Hits* search( lucene::search::Query* query );

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

	    /**
	     * Returns the Resource corresponding with the specified Document number.
	     * Note that all of Lucene's restrictions of using document numbers apply.
	     */
	    Soprano::Node getResource( int documentNumber );

	    /**
	     * Returns the Resource corresponding with the specified Document.
	     */
	    Soprano::Node getResource( lucene::document::Document* document );

	    /**
	     * Dump the index contents to the stream
	     */
	    void dump( QTextStream& ) const;

	private:
	    class Private;
	    Private* const d;
	};
    }
}

uint qHash( const Soprano::Node& node );

#endif