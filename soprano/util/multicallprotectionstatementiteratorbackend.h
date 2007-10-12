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

#ifndef _SOPRANO_NONBLOCKING_STATEMENT_ITERATOR_BACKEND_H_
#define _SOPRANO_NONBLOCKING_STATEMENT_ITERATOR_BACKEND_H_

#include "iteratorbackend.h"
#include "statementiterator.h"
#include "multicallprotectioniteratorbase.h"
#include "statement.h"

namespace Soprano {
    namespace Util {
	class MultiCallProtectionModel;

	class MultiCallProtectionStatementIteratorBackend : public IteratorBackend<Statement>, public MultiCallProtectionIteratorBase
	{
	public:
	    MultiCallProtectionStatementIteratorBackend( const StatementIterator& it, MultiCallProtectionModel* model );
	    ~MultiCallProtectionStatementIteratorBackend();

	    bool next();
	    Soprano::Statement current() const;
	    void close();

	private:
	    StatementIterator m_iterator;
	};
    }
}

#endif
