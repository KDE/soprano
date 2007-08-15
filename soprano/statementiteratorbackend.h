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

#ifndef SOPRANO_STATEMENT_ITERATOR_BACKEND_H
#define SOPRANO_STATEMENT_ITERATOR_BACKEND_H

#include <soprano/soprano_export.h>

namespace Soprano {

    class Statement;

    /**
     * \brief The actual work in a StatementIterator instance is done by a
     * StatementIteratorBackend.
     *
     * Each Backend implementation has to have its own version of StatementIteratorBackend.
     *
     * \author Daniele Galdi <daniele.galdi@gmail.com><br>Sebastian Trueg <trueg@kde.org>
     */
    class SOPRANO_EXPORT StatementIteratorBackend
    {
    public:
	virtual ~StatementIteratorBackend();

	/**
	 * Advance to the next Statement in the iterator.
	 *\return true if there is another Statement and false if the end has been reached.
	 */
	virtual bool next() = 0;

	/**
	 *\return the current Statement
	 */
	virtual Statement current() const = 0;

    protected:
	StatementIteratorBackend();
    };

}

#endif
