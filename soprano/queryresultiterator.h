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
    class StatementIterator;
    class BindingSet;

    /**
     * \brief An iterator for query results.
     *
     * %Query results in %Soprano are wrapped in a QueryResultIterator.
     *
     * Iteartors in %Soprano are very easy to use through the method
     * next() and a set of retrieval methods such as currentStatement() or currentBindings().
     * The retrieval methods can be called subsequetially to retrieve the current bindings or 
     * statement until next() has been called again.
     *
     * \code
     * QueryResultIterator it = model->query( someGraphQuery );
     * while( it.next() ) {
     *    doSomething( it.currentStatement() );
     * }
     *
     * QueryResultIterator it2 = model->query( someTupleQuery );
     * while( it.next() ) {
     *    doSomethingElse( it.currentBindings() );
     *    doSomethingCompletelyDifferent( it.binding( "x" ) );
     *    doSomethingEntirelyDifferent( it.binding( 0 ) );
     * }
     * \endcode
     *
     * Backends such as redland tend to invalidate the iterators if
     * the underlaying model is changed. Thus, it is always a good idea to cache
     * the results if they are to be used to modify the model:
     *
     * \code
     * QueryResultIterator it = model->query( someTupleQuery );
     * QList<BindingSet> allBindings = it.allBindings();
     * Q_FOREACH( BindingSet s, allBindings ) {
     *    modifyTheModel( model, s );
     * }
     * \endcode
     *
     * \warning Backends such as redland tend to invalidate the iterators if
     * the underlaying model is changed.
     *
     * \author Daniele Galdi <daniele.galdi@gmail.com><br>Sebastian Trueg <trueg@kde.org>
     */
    class SOPRANO_EXPORT QueryResultIterator
    {
    public:
	/**
	 * Creates and empty, invlid iterator.
	 */
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

	/**
	 * Convinience method that puts all current bindings into one map.
	 */
	BindingSet currentBindings() const;

	Node binding( const QString &name ) const;

	Node binding( int offset ) const;

	int bindingCount() const;

	QStringList bindingNames() const;

	bool isGraph() const;

	bool isBinding() const;

	bool isBool() const;

	bool boolValue() const;

	/**
	 * \return \p true if the Iterator is valid, \p false otherwise, i.e.
	 * it has no backend .
	 */
	bool isValid() const;

	/**
	 * Convinience method that collects all binding sets that are left
	 * in the iterator.
	 */
	QList<BindingSet> allBindings();

	/**
	 * Conviniece method that creates an iterator over the statements in this query result.
	 * This method does only make sense for graph queries.
	 *
	 * \warning The new iterator is just a wrapper around this one. Thus, changing it will also
	 * change this one.
	 * 
	 * \return A wrapper iterator over the statements in a graph query.
	 */
	StatementIterator iterateStatements();

    private:
	class Private;
	QSharedDataPointer<Private> d;
    };

}

#endif // SOPRANO_RESULT_SET_H
