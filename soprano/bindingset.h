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


class QStringList;

namespace Soprano {

    class Node;

    /**
     * \class BindingSet bindingset.h Soprano/BindingSet
     *
     * \brief Represents one set of bindings in the result of a select query.
     *
     * BindingSet is mostly a convenience class for caching of query
     * results.
     *
     * \sa QueryResultIterator
     *
     * \author Sebastian Trueg <trueg@kde.org>
     */
    class SOPRANO_EXPORT BindingSet
    {
    public:
        /**
         * Create an emtpy set.
         */
        BindingSet();

        /**
         * Copy constructor.
         */
        BindingSet( const BindingSet& other );

        /**
         * Destructor.
         */
        ~BindingSet();

        BindingSet& operator=( const BindingSet& );

        /**
         * \return The names of the bound variables in this set.
         */
        QStringList bindingNames() const;

        /**
         * Get the binding for a variable by index.
         *
         * This is equivalent to value(int) const.
         */
        Node operator[]( int offset ) const;

        /**
         * Get the binding for a variable.
         *
         * This is equivalent to value(const QString&) const.
         */
        Node operator[]( const QString name ) const;

        /**
         * Get the binding for a variable by index.
         *
         * \param offset The index of the requested variable.
         *
         * \return The binding for the requested variable or and invalid
         * node if offset is out of bounds, i.e. bigger or equal to count().
         *
         * \sa QueryResultIterator::binding(int) const.
         */    
        Node value( int offset ) const;

        /**
         * Get the binding for a variable.
         *
         * \param name The name of the requested variable.
         *
         * \return The binding for the requested variable or and invalid
         * node if the bindings do not contain the variable.
         *
         * \sa QueryResultIterator::binding(const QString&) const.
         */
        Node value( const QString& name ) const;

        /**
         * Check if a certain variable has a binding in this set.
         *
         * \param name The variable name.
         *
         * \return \p true if this set contains a binding for the 
         * variable name, \p false otherwise.
         */
        bool contains( const QString& name ) const;

        /**
         * The number of bindings in this set.
         *
         * \return The number of bindings.
         */
        int count() const;

        /**
         * Insert a new binding into the set.
         */
        void insert( const QString& name, const Node& value );

        /**
         * Replaces a value in the binding set.
         *
         * \since 2.3
         */
        void replace( int offset, const Node& value );

        /**
         * Replaces a value in the binding set.
         *
         * \since 2.3
         */
        void replace( const QString& name, const Node& value );

    private:
        class Private;
        QSharedDataPointer<Private> d;
    };
}

class QDebug;
class QTextStream;

SOPRANO_EXPORT QDebug operator<<( QDebug s, const Soprano::BindingSet& );
SOPRANO_EXPORT QTextStream& operator<<( QTextStream& s, const Soprano::BindingSet& );

#endif
