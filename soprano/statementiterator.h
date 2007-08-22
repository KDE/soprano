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

#ifndef SOPRANO_STATEMENT_ITERATOR_H
#define SOPRANO_STATEMENT_ITERATOR_H

#include <QtCore/QSharedDataPointer>
#include <QtCore/QList>

#include <soprano/soprano_export.h>

namespace Soprano {

    class Node;
    class NodeIterator;
    class Statement;
    class StatementIteratorBackend;

    /**
     * \brief An iterator that provides a stream of Statements.
     *
     * Iteartors in %Soprano are very easy to use through two methods
     * next() and current(). Instead of the latter operator*() can also be used.
     * Both can be called subsequetially to retrieve the current Statement
     * until next() has been called again.
     *
     * \code
     * while( it.next() ) {
     *    doSomething( *it );
     *    doSomethingElse( it.current() );
     * }
     * \endcode
     *
     * Backends such as redland tend to invalidate the iterators if
     * the underlaying model is changed. Thus, it is always a good idea to cache
     * the results if they are to be used to modify the model:
     *
     * \code
     * StatementIterator it = model->listStatements();
     * QList<Statement> allStatements = it.allStatements();
     * Q_FOREACH( Statement s, allStatements ) {
     *    modifyTheModel( model, s );
     * }
     * \endcode
     *
     * \warning Be aware that iterators in Soprano are shared objects which means
     * that copies of one iterator object work on the same data.
     * 
     * \author Daniele Galdi <daniele.galdi@gmail.com><br>Sebastian Trueg <trueg@kde.org>
     */
    class SOPRANO_EXPORT StatementIterator
    {
    public:
	/**
	 * Creates and empty, invalid iterator.
	 */
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
	 * This is equivalent to current().
	 *
	 * \return The Statement the iterator currently points to or
	 * an invalid one if next has never been called.
	 */
	Statement operator*() const;

	/**
	 * \return \p true if the Iterator is valid, \p false otherwise, i.e.
	 * it has no backend .
	 */
	bool isValid() const;

	/**
	 * Convinience method which extracts all statements (this does not include the
	 * statements that have already been read from the iterator) from the iterator
	 * and returns them in a list.
	 *
	 * Be aware that after calling this method the iterator will be invalid.
	 *
	 * \return A list of all statements that rest in the iterator.
	 */
	QList<Statement> allStatements();

	/**
	 * Conviniece method that creates an iterator over the subject nodes of the statements
	 * in this iterator.
	 *
	 * \warning The new iterator is just a wrapper around this one. Thus, changing it will also
	 * change this one.
	 * 
	 * \return A wrapper iterator over the subject nodes.
	 */
	NodeIterator iterateSubjects();

	/**
	 * Conviniece method that creates an iterator over the predicate nodes of the statements
	 * in this iterator.
	 *
	 * \warning The new iterator is just a wrapper around this one. Thus, changing it will also
	 * change this one.
	 * 
	 * \return A wrapper iterator over the predicate nodes.
	 */
	NodeIterator iteratePredicates();

	/**
	 * Conviniece method that creates an iterator over the object nodes of the statements
	 * in this iterator.
	 *
	 * \warning The new iterator is just a wrapper around this one. Thus, changing it will also
	 * change this one.
	 * 
	 * \return A wrapper iterator over the object nodes.
	 */
	NodeIterator iterateObjects();

	/**
	 * Conviniece method that creates an iterator over the context nodes of the statements
	 * in this iterator.
	 *
	 * \warning The new iterator is just a wrapper around this one. Thus, changing it will also
	 * change this one.
	 * 
	 * \return A wrapper iterator over the context nodes.
	 */
	NodeIterator iterateContexts();

    protected:
	/**
	 * Set the backend to read the actual data from.
	 * A previous backend will be deleted if there are no other StatementIterator
	 * instances using it.
	 */
	void setBackend( StatementIteratorBackend* b );

    private:
	class Private;
	QSharedDataPointer<Private> d;
    };
}

#endif // SOPRANO_STATEMENT_ITERATOR_H

