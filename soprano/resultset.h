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

// FIXME: The ResultSet itself should not behave like an iterator if possible
namespace Soprano {

    class Model;
    class Node;
    class QueryResult;
    class Statement;

    class SOPRANO_EXPORT ResultSet
    {
    public:
	ResultSet();
	ResultSet( const ResultSet& );
	ResultSet( QueryResult *qr );

	virtual ~ResultSet();

	ResultSet& operator=( const ResultSet& );

	/**
	 * Advance to the next entry (statement or bindingset)
	 * \return true if the end of the ResultSet was not reached yet.
	 * false if no more entries are found or if the ResultSet was
	 * already invalidated by a call to model.
	 */
	// FIXME: make this match the StatementIterator model or the other way around
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
	 * Retrieve a Model containing all Statements from a graph query.
	 *
	 * After calling this method the ResultSet is invalidated, i.e. can not
	 * be reused.
	 *
	 * \return A new Model containing all result Statements from the graph query
	 * or 0 in case this ResultSet does not represent a graph query or next has
	 * already been called to iterate over the results.
	 * Caution: the caller has to take care of deleting the 
	 * returned model!
	 */
	Model *model() const;

    private:
	class Private;
	QSharedDataPointer<Private> d;
    };

}

#endif // SOPRANO_RESULT_SET_H
