/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2009 Sebastian Trueg <trueg@kde.org>
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

#ifndef SOPRANO_GRAPH_H
#define SOPRANO_GRAPH_H

#include <QtCore/QList>
#include <QtCore/QSharedDataPointer>

#include "soprano_export.h"
#include "error.h"
#include "sopranotypes.h"
#include "node.h"
#include "statement.h"


namespace Soprano
{
    class StatementIterator;
    class NodeIterator;

    /**
     * \class Graph graph.h Soprano/Graph
     *
     * \brief A simple collection of statements.
     *
     * A Graph is a simple in-memory collection of Statements. It is supposed
     * to be used where one needs a quick way to exchange or store a small number
     * of statements. It is basically a fancy QSet of statements.
     *
     * In comparision to Model it does not support queries, it does not use a specific
     * backend plugin, and it is not optimized in any way.
     *
     * One graph does not represent one named graph, i.e. one context, it can contain
     * Statements with different context nodes.
     *
     * \author Sebastian Trueg <trueg@kde.org>
     *
     * \since 2.3
     */
    class SOPRANO_EXPORT Graph
    {
    public:
        /**
         * Create an empty graph.
         */
        Graph();

        /**
         * Copy constructor.
         */
        Graph( const Graph& );

        /**
         * Create a graph from a list of statements.
         *
         * \sa operator=(const QList<Statement>&)
         */
        Graph( const QList<Statement>& );

        /**
         * Destructor
         */
        ~Graph();

        //@{
        /**
         * Add the Statement to the Graph.
         *
         * \param statement The Statement to add.
         */
        void addStatement( const Statement& statement );

        /**
         * \overload
         */
        void addStatement( const Node& subject, const Node& predicate, const Node& object, const Node& context = Node() );

        /**
         * \overload
         */
        void addStatements( const QList<Statement>& statements );
        //@}

        //@{
        /**
         * Remove one statement. For removing statements with wildward matching see removeAllStatements().
         *
         * \param statement The statement that should be removed. This has to be a valid statement.
         *
         * \return Error::ErrorNone on success and an error code if statement was invalid or an error
         * occured.
         */
        void removeStatement( const Statement& statement );

        /**
         * \overload
         */
        void removeStatement( const Node& subject, const Node& predicate, const Node& object, const Node& context = Node() );

        /**
         * Remove all statements that match the partial statement. For removing
         * one specific statement see removeStatement().
         *
         * \param statement A possible partially defined statement that serves as
         * a filter for all statements that should be removed.
         */
        void removeAllStatements( const Statement& statement = Statement() );

        /**
         * \overload
         *
         * \param subject The subject node to match. Can be empty as a wildcard.
         * \param predicate The predicate node to match. Can be empty as a wildcard.
         * \param object The object node to match. Can be empty as a wildcard.
         * \param context The context node to match. Can be empty as a wildcard.
         */
        void removeAllStatements( const Node& subject, const Node& predicate, const Node& object, const Node& context = Node() );

        /**
         * Convenience method which removes all %statements in statements.
         */
        void removeStatements( const QList<Statement>& statements );

        /**
         * Convenience method that removes all statements in the context.
         */
        void removeContext( const Node& );
        //@}


        //@{
        /**
         * Return an iterator over Graph Statements that "partial"
         * match the input Statement.
         *
         * \param partial The partial Statement to match.
         *
         * \return An iterator for all the matched Statements, on error an invalid iterator is returned.
         */
        StatementIterator listStatements( const Statement& partial = Statement() ) const;

        /**
         * \overload
         *
         * \param subject The subject node to match. Can be empty as a wildcard.
         * \param predicate The predicate node to match. Can be empty as a wildcard.
         * \param object The object node to match. Can be empty as a wildcard.
         * \param context The context node to match. Can be empty as a wildcard.
         *
         * \return An iterator for all the matched Statements, on error an invalid iterator is returned.
         */
        StatementIterator listStatements( const Node& subject, const Node& predicate, const Node& object, const Node& context = Node() ) const;

        /**
         * Convenience method which lists all statements in context.
         *
         * \return An iterator for all the matched Statements, on error an invalid iterator is returned.
         */
        StatementIterator listStatementsInContext( const Node& context ) const;

        /**
         * List all contexts in the graph, i.e. all named graphs.
         *
         * \return An iterator over context Nodes, on error an invalid iterator is returned.
         */
        NodeIterator listContexts() const;
        //@}


        //@{
        /**
         * Check if the graph contains certain statements.
         *
         * \param statement A partially defined statement that serves as
         * a pattern.
         *
         * \return true if the Graph contains a Statement matching the given statement
         * pattern.
         */
        bool containsAnyStatement( const Statement& statement ) const;

        /**
         * \overload
         *
         * \param subject The subject node to match. Can be empty as a wildcard.
         * \param predicate The predicate node to match. Can be empty as a wildcard.
         * \param object The object node to match. Can be empty as a wildcard.
         * \param context The context node to match. Can be empty as a wildcard.
         */
        bool containsAnyStatement( const Node& subject, const Node& predicate, const Node& object, const Node& context = Node() ) const;

        /**
         * Check if the graph contains a statements.
         *
         * \param statement The statement in question. This has to be a valid statement,
         * i.e. subject, predicate, and object need to be defined. If the context node
         * is empty the default graph is searched.
         *
         * \return \p true if the Graph contains the Statement, \p false otherwise or
         * is statement is invalid.
         */
        bool containsStatement( const Statement& statement ) const;

        /**
         * \overload
         */
        bool containsStatement( const Node& subject, const Node& predicate, const Node& object, const Node& context = Node() ) const;

        /**
         * Convenience method which is based on containsAnyStatement
         */
        bool containsContext( const Node& context ) const;

        /**
         * \return true if the Graph doesn't contains any Statement.
         */
        bool isEmpty() const;

        /**
         * The number of statements stored in this Graph.
         * \return The size of the Graph.
         */
        int statementCount() const;
        //@}

        //@{
        /**
         * Convert the Graph into a list
         */
        QList<Statement> toList() const;
        //@}

        /**
         * \name Operators
         */
        //@{
        /**
         * Assings \a g to this graph and returns a refernce to this graph.
         */
        Graph& operator=( const Graph& g );

        /**
         * Assings \a statements to this graph and returns a refernce to this graph.
         */
        Graph& operator=( const QList<Statement>& statements );

        /**
         * Returns a graph containing all statements from this graph and from \a g.
         */
        Graph operator+( const Graph& g ) const;

        /**
         * Returns a graph containing all statements from this graph and from \a s.
         */
        Graph operator+( const Statement& s ) const;

        /**
         * Inserts the statements in graph \a g into this graph and returns
         * a reference to this graph.
         */
        Graph& operator+=( const Graph& g );

        /**
         * Inserts the statement \a s into this graph and returns
         * a reference to this graph.
         */
        Graph& operator+=( const Statement& s );

        /**
         * Substracts all statements in \a g from this graph and returns
         * a reference to this graph.
         */
        Graph operator-( const Graph& ) const;

        /**
         * Substracts \a s from this graph and returns
         * a reference to this graph.
         */
        Graph operator-( const Statement& s ) const;

        /**
         * Removes the statements in graph \a g from this graph and returns
         * a reference to this graph.
         */
        Graph& operator-=( const Graph& g );

        /**
         * Removes the statement \a s from this graph and returns
         * a reference to this graph.
         */
        Graph& operator-=( const Statement& s );

        /**
         * \sa operator+=
         */
        Graph& operator<<( const Graph& );

        /**
         * \sa operator+=
         */
        Graph& operator<<( const Statement& );

        /**
         * Comparision operator.
         *
         * \return \p true if this graph and \a g contain the
         * same statements, \p false otherwise.
         */
        bool operator==( const Graph& g ) const;

        /**
         * Comparision operator.
         *
         * \return \p true if this graph contains a statement
         * \a g does not contain or the other way around, \p false otherwise.
         */
        bool operator!=( const Graph& g ) const;
        //@}

    private:
        class Private;
        QSharedDataPointer<Private> d;
    };
}

#endif
