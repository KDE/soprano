/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2006 Daniele Galdi <daniele.galdi@gmail.com>
 * Copyright (C) 2007-2008 Sebastian Trueg <trueg@kde.org>
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

#ifndef SOPRANO_QUERY_RESULT_H
#define SOPRANO_QUERY_RESULT_H

#include "iteratorbackend.h"
#include "bindingset.h"

#include <QtCore/QStringList>


namespace Soprano {

    class Node;
    class Statement;
    
    /**
     * \class QueryResultIteratorBackend queryresultiteratorbackend.h Soprano/QueryResultIteratorBackend
     *
     * \brief QueryResultIteratorBackend is the actual working horse behind QueryResultIterator.
     *
     * Each Backend implementation has to have its own version of QueryResultIteratorBackend.
     *
     * \author Daniele Galdi <daniele.galdi@gmail.com><br>Sebastian Trueg <trueg@kde.org>
     */
    class SOPRANO_EXPORT QueryResultIteratorBackend : public IteratorBackend<BindingSet>
    {
    public:
        virtual ~QueryResultIteratorBackend();

        /**
         * For internal reasons each backend implementation needs to always
         * return \p true for boolean results.
         *
         * \sa QueryResultIterator::next
         */
        virtual bool next() = 0;

        /**
         * The default implementation constructs the
         * BindingSet using binding() and bindingNames()
         *
         * \sa QueryResultIterator::current
         */
        virtual BindingSet current() const;

        /**
         * \sa QueryResultIterator::currentStatement
         */
        virtual Statement currentStatement() const = 0;

        /**
         * \sa QueryResultIterator::binding(const QString&)
         */
        virtual Node binding( const QString& name ) const = 0;

        /**
         * \sa QueryResultIterator::binding(int)
         */
        virtual Node binding( int offset ) const = 0;

        /**
         * \sa QueryResultIterator::bindingCount
         */
        virtual int bindingCount() const = 0;

        /**
         * \sa QueryResultIterator::bindingNames
         */
        virtual QStringList bindingNames() const = 0;

        /**
         * \sa QueryResultIterator::isGraph
         */
        virtual bool isGraph() const = 0;

        /**
         * \sa QueryResultIterator::isBinding
         */
        virtual bool isBinding() const = 0;

        /**
         * \sa QueryResultIterator::isBool
         */
        virtual bool isBool() const = 0;

        /**
         * \sa QueryResultIterator::boolValue
         */
        virtual bool boolValue() const = 0;

    protected:
        QueryResultIteratorBackend();
    };
}

#endif // SOPRANO_QUERY_RESULT_H

