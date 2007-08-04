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

#ifndef SOPRANO_BACKEND_REDLAND_STATEMENT_ITERATOR_H
#define SOPRANO_BACKEND_REDLAND_STATEMENT_ITERATOR_H

#include <redland.h>

#include "statementiteratorprivate.h"
#include "node.h"

#include <QtCore/QSharedDataPointer>

namespace Soprano 
{
    class Statement;

    namespace Redland {
	class RedlandModel;

	class RedlandStatementIterator: public Soprano::StatementIteratorPrivate
	    {
	    public:
		/**
		 * param forceContext Since redland does not set a context if we list statements from one specific context
		 * we have to force it this way.
		 */
		explicit RedlandStatementIterator( const RedlandModel* model, librdf_stream *s, const Node& forceContext = Node() );

		~RedlandStatementIterator();

		bool hasNext() const;

		Soprano::Statement next() const;

		void close() const;

	    private:
		mutable const RedlandModel* m_model;
		mutable librdf_stream* m_stream;
		Node m_forceContext;
	    };

    }
}

#endif // SOPRANO_BACKEND_REDLAND_STATEMENT_ITERATOR_H

