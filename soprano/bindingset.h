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

#ifndef _SOPRANO_QUERY_BINDINGS_H_
#define _SOPRANO_QUERY_BINDINGS_H_

#include "soprano_export.h"

#include <QtCore/QSharedDataPointer>
#include <QtCore/QDebug>

namespace Soprano {

    class Node;

    /**
     * \brief Represents one set of bindings in the result of a select query.
     *
     * BindingSet is mostly a convinience class for caching of query
     * results.
     *
     * \sa QueryResultIterator
     *
     * \author Sebastian Trueg <trueg@kde.org>
     */
    class SOPRANO_EXPORT BindingSet
    {
    public:
	BindingSet();
	BindingSet( const BindingSet& other );
	~BindingSet();

	BindingSet& operator=( const BindingSet& );

	QStringList bindingNames() const;

	Node operator[]( int offset ) const;
	Node operator[]( const QString name ) const;

	Node value( int offset ) const;

	/**
	 * \return The binding for name or an invalid node if the BindingSet does
	 * not contain a binding called name.
	 */
	Node value( const QString& name ) const;

	bool contains( const QString& name ) const;

	int count() const;

	void insert( const QString& name, const Node& value );

    private:
	class Private;
	QSharedDataPointer<Private> d;
    };
}

SOPRANO_EXPORT QDebug operator<<( QDebug s, const Soprano::BindingSet& );

#endif
