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

#ifndef SOPRANO_NODE_ITERATOR_BACKEND_H
#define SOPRANO_NODE_ITERATOR_BACKEND_H

#include <soprano/soprano_export.h>

namespace Soprano {

    class Node;

    /**
     * \brief The actual work in a NodeIterator instance is done by a
     * NodeIteratorBackend.
     *
     * Each Backend implementation has to have its own version of NodeIteratorBackend.
     *
     * \author Sebastian Trueg <trueg@kde.org>
     */
    class SOPRANO_EXPORT NodeIteratorBackend
    {
    public:
	virtual ~NodeIteratorBackend();

	/**
	 * Advance to the next Node in the iterator.
	 *\return true if there is another Node and false if the end has been reached.
	 */
	virtual bool next() = 0;

	/**
	 *\return the current Node
	 */
	virtual Node current() const = 0;

    protected:
	NodeIteratorBackend();
    };

}

#endif
