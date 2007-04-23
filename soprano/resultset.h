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

    class SOPRANO_EXPORT ResultSet
    {
    public:
	ResultSet();
	ResultSet( const ResultSet& );
	ResultSet( QueryResult *qr );

	virtual ~ResultSet();

	ResultSet& operator=( const ResultSet& );

	bool next() const;

	Node binding( const QString &name ) const;

	Node binding( int offset ) const;

	int bindingCount() const;

	QStringList bindingNames() const;

	bool isGraph() const;

	bool isBinding() const;

	bool isBool() const;

	bool boolValue() const;

	Model *model() const;

    private:
	class Private;
	QSharedDataPointer<Private> d;
    };

}

#endif // SOPRANO_RESULT_SET_H
