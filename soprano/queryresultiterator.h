/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2006 Daniele Galdi <daniele.galdi@gmail.com>
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

#ifndef SOPRANO_RESULT_SET_H
#define SOPRANO_RESULT_SET_H

#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QSharedDataPointer>

#include <soprano/soprano_export.h>

namespace Soprano {

    class Model;
    class Node;
    class QueryResultIteratorBackend;
    class Statement;

    /**
     * An iterator for query results.
     *
     * Be aware that iterators in Soprano are shared objects which means
     * that copies of one iterator object work on the same data.
     *
     * Caution: backends such as redland tend to invalidate the iterators if
     * the underlaying model is changed.
     */
    class SOPRANO_EXPORT QueryResultIterator
    {
    public:
	QueryResultIterator();
	QueryResultIterator( const QueryResultIterator& );

	/**
	 * Create a new QueryResultIterator which uses qr as backend.
	 * QueryResultIterator will take ownership of the QueryResultIteratorBackend.
	 */
	QueryResultIterator( QueryResultIteratorBackend *qr );

	virtual ~QueryResultIterator();

	QueryResultIterator& operator=( const QueryResultIterator& );

	/**
	 * Advance to the next entry (statement or bindingset)
	 * \return true if the end of the QueryResultIterator was not reached yet.
	 * false if no more entries are found or if the QueryResultIterator was
	 * already invalidated by a call to model.
	 */
	bool next();

	/**
	 * Retrieve the current Statement after a call to next.
	 */
	Statement currentStatement() const;

	Node binding( const QString &name ) const;

	Node binding( int offset ) const;

	int bindingCount() const;

	QStringList bindingNames() const;

	bool isGraph() const;

	bool isBinding() const;

	bool isBool() const;

	bool boolValue() const;

	/**
	 *\return true if the Iterator is valid
	 */
	bool isValid() const;

	/**
	 *\return true if the Iterator is empty
	 */
	bool isEmpty() const;

    private:
	class Private;
	QSharedDataPointer<Private> d;
    };

}

#endif // SOPRANO_RESULT_SET_H
