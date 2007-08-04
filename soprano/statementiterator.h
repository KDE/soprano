/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2006 Daniele Galdi <daniele.galdi@gmail.com>
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

#ifndef SOPRANO_STATEMENT_ITERATOR_H
#define SOPRANO_STATEMENT_ITERATOR_H

#include <QtCore/QSharedDataPointer>
#include <soprano/soprano_export.h>

namespace Soprano {

    class Statement;
    class StatementIteratorBackend;

    /**
     * An iterator that provides a stream of Statements.
     *
     * Be aware that iterators in Soprano are shared objects which means
     * that copies of one iterator object work on the same data.
     */
    class SOPRANO_EXPORT StatementIterator
    {
    public:
	StatementIterator();

	/**
	 * Create a new StatementIterator instance that uses sti as backend.
	 * StatementIterator will take ownership of the backend.
	 */
	StatementIterator( StatementIteratorBackend *sti );

	StatementIterator( const StatementIterator &sti );

	virtual ~StatementIterator();

	StatementIterator& operator=( const StatementIterator& );

	/**
	 * Advances to the next statement in the iterator.
	 *\return true if another Statement can be read from the iterator,
	 * false if the end has been reached.
	 */
	bool next();

	/**
	 *\return the current Statement, this is not valid until after
	 * the first call to next.
	 */
	Statement current() const;

	/**
	 * Retrieve the current Statement in the iterator.
	 *
	 * This is equivalent to current.
	 *
	 * \return The Statement the iterator currently points to or
	 * an invlid one if next has never been called.
	 */
	Statement operator*() const;

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

#endif // SOPRANO_STATEMENT_ITERATOR_H

