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

#ifndef SOPRANO_ITERATOR_BACKEND_H
#define SOPRANO_ITERATOR_BACKEND_H

#include "soprano_export.h"
#include "error.h"

namespace Soprano {

    /**
     * \class IteratorBackend iteratorbackend.h Soprano/IteratorBackend
     *
     * \brief The actual work in a Iterator instance is done by an
     * IteratorBackend.
     *
     * \author Daniele Galdi <daniele.galdi@gmail.com><br>Sebastian Trueg <trueg@kde.org>
     */
    template<class T> class IteratorBackend : public Error::ErrorCache
    {
    public:
        virtual ~IteratorBackend() {}

        /**
         * Advance to the next element in the iterator.
         *
         * Implementations of this method should reset the error by eith calling
         * clearError() or setError().
         *
         *\return true if there is another  and false if the end has been reached.
         *
         * \sa Iterator::next()
         */
        virtual bool next() = 0;

        /**
         * Get the element the iterator currently points to.
         *
         * Implementations of this method should reset the error by eith calling
         * clearError() or setError().
         *
         * \return the current element.
         *
         * \sa Iterator::current()
         */
        virtual T current() const = 0;

        /**
         * Close the iterator and release any locks on the underlying Model.
         *
         * Implementations of this method should reset the error by eith calling
         * clearError() or setError().
         *
         * \sa Iterator::close()
         */
        virtual void close() = 0;

    protected:
        IteratorBackend() {}
    };
}

#endif
