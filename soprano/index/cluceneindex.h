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
    }
}

namespace Soprano {
    namespace Index {

	/**
	 * The CLuceneIndex provides a wrapper around a CLucene index 
	 * which stores RDF statements.
	 */
	class SOPRANO_EXPORT CLuceneIndex
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
	     */
	    bool open( const QString& folder );

	    void close();

	    bool isOpen() const;

	    /**
	     * Indexes a statement.
	     * \return An error code or 0 on success
	     */
	    int addStatement( const Soprano::Statement& );

	    /**
	     * Removes a statement from the index.
	     * \return An error code or 0 on success
	     */
	    int removeStatement( const Soprano::Statement& );

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

#endif
