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

#ifndef _SOPRANO_CLUCENE_DOCUMENT_WRAPPER_H_
#define _SOPRANO_CLUCENE_DOCUMENT_WRAPPER_H_

#include <QtCore/QString>


namespace lucene {
    namespace document {
	class Document;
    }
    namespace index {
	class IndexWriter;
    }
}

namespace Soprano {
    namespace Index {
	/**
	 * Transparently handles adding of properties to a CLucene
	 * Document.
	 */
	class CLuceneDocumentWrapper
	{
	public:
	    CLuceneDocumentWrapper( lucene::document::Document* );
	    ~CLuceneDocumentWrapper();

	    void addProperty( const QString& field, const QString& text );
	
	    /**
	     * Checks whether a field occurs with a specified value in a Document.
	     */
	    bool hasProperty( const QString& field, const QString& text ) const;

	    /**
	     * Determines the number of properties stored in a Document.
	     */
	    int numberOfPropertyFields() const;

	    /**
	     * Stores and indexes an ID in a Document.
	     */
	    void addID( const QString& id );

	    void save( lucene::index::IndexWriter* );

	private:
	    class Private;
	    Private* const d;
	};
    }
}

#endif
