/* This file is part of Soprano Project.
 *
 * Copyright (C) 2006 Daniele Galdi <daniele.galdi@gmail.com>
 * Copyright (C) 2007-2009 Sebastian Trueg <trueg@kde.org>
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

#ifndef SOPRANO_STATEMENT_H
#define SOPRANO_STATEMENT_H

#include "node.h"
#include "soprano_export.h"

namespace Soprano
{
    /**
     * \class Statement statement.h Soprano/Statement
     *
     * \brief A Statement instance represents one RDF quadruple.
     *
     * In %Soprano statements are quadruples, i.e. in addition to the subject, predicate, and
     * object nodes, they have a fourth node, the context. The context represents the named graph
     * in which the statement is stored. If the context is an empty node the statement is stored in
     * the default graph.
     *
     * A Statement is valid if subject, predicate, and object are valid. Invalid statements can, however,
     * be used in many methods such as Model::listStatements as wildwards.
     *
     * \sa Node
     *
     * \author Daniele Galdi <daniele.galdi@gmail.com><br>Sebastian Trueg <trueg@kde.org>
     */
    class SOPRANO_EXPORT Statement
    {
    public:
        //@{
        /**
         * Default Constructor, build an empty (invalid) Statement.
         */
        Statement();

        /**
         * Build a Statement with the given subject, predicate and object.
         *
         * \param subject The subject (cannot be of type Node::LiteralNode)
         *
         * \param predicate The predicate (has to be of type Node::ResourceNode or Node::EmptyNode)
         *
         * \param object The object can be of either Node type.
         *
         * \param context The context node (has to be of type Node::ResourceNode or Node::EmptyNode)
         */
        Statement( const Node &subject, const Node &predicate, const Node &object, const Node &context = Node() );

        Statement( const Statement &other );

        virtual ~Statement();

        Statement& operator=( const Statement& other );
        //@}

        //@{
        bool operator==( const Statement& other ) const;
        bool operator!=( const Statement& other ) const;

        /**
         * Match this statement against template statement \a other. The only difference
         * to operator== is that empty nodes are matched as wildcards,
         * i.e. they match any other node.
         *
         * Be aware that the following is NOT always true since only \a other
         * is treated a a wildcard:
         *
         * \code
         * // NOT always true:
         * a.matches(b) == b.matches(a)
         * \endcode
         *
         * \return \p true if this statement matches other, \p false if not.
         *
         * \sa Node::matches()
         */
        bool matches( const Statement& other ) const;
        //@}

        //@{
        /**
         * A Statement is valid if the subject is a resource or blank
         * node, the predicate is a resource node,
         * and the object is a valid node.
         *
         * \return @p true if the Statement is valid, @p false otherwise
         */
        bool isValid() const;
        //@}

        //@{
        /**
         * \return The subject.
         */
        Node subject() const;

        /**
         * \return The predicate.
         */
        Node predicate() const;

        /**
         * \return The object.
         */
        Node object() const;

        /**
         * \return The Context node.
         */
        Node context() const;
        //@}

        //@{
        /**
         * Change the Statement subject.
         *
         * \param subject The new subject.
         */
        void setSubject( const Node &subject );

        /**
         * Change the Statement predicate.
         *
         * \param predicate The new predicate.
         */
        void setPredicate( const Node &predicate );
        /**
         * Change the Statement object.
         *
         * \param object The new object.
         */
        void setObject( const Node &object );

        /**
         * Change the Statement context.
         *
         * \param context The new Context.
         */
        void setContext( const Node &context );
        //@}

    private:
        class Private;
        QSharedDataPointer<Private> d;
    };
}

/**
 * \relates Soprano::Statement
 */
SOPRANO_EXPORT QDebug operator<<( QDebug s, const Soprano::Statement& );

/**
 * \relates Soprano::Statement
 */
SOPRANO_EXPORT QTextStream& operator<<( QTextStream& s, const Soprano::Statement& );

#endif // SOPRANO_STATEMENT_H

