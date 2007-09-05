/*
 * This file is part of Soprano Project
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

#ifndef SOPRANO_QUERY_H
#define SOPRANO_QUERY_H

#include "soprano_export.h"
#include "sopranotypes.h"

#include <QtCore/QSharedDataPointer>

class QString;

namespace Soprano
{
    /**
     * \class QueryLegacy querylegacy.h soprano/QueryLegacy
     *
     * \brief Deprecated query class.
     *
     * \deprecated Will be replaced by a real query API soon.
     */
    class SOPRANO_EXPORT QueryLegacy
    {
    public:
	enum QueryType {
	    SPARQL = Query::QUERY_LANGUAGE_SPARQL, /**< \sa Query::QUERY_LANGUAGE_SPARQL */
	    RDQL = Query::QUERY_LANGUAGE_RDQL      /**< \sa Query::QUERY_LANGUAGE_RDQL */
	};

	QueryLegacy( const QString &query, QueryType type );
	QueryLegacy( const QueryLegacy &other );
	~QueryLegacy();

	QueryLegacy& operator=( const QueryLegacy& other );

	QueryType type() const;

	QString query() const;

	static QString queryTypeToString( QueryType queryType);
	static int queryTypeFromString( const QString& );

    private:
	class Private;
	QSharedDataPointer<Private> d;
    };
}

#endif
