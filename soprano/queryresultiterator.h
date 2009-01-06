/*
 * This file is part of Soprano Project.
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

#ifndef SOPRANO_RESULT_SET_H
#define SOPRANO_RESULT_SET_H

#include "iterator.h"
#include "bindingset.h"
#include "statement.h"
#include "soprano_export.h"

#include <QtCore/QString>
#include <QtCore/QStringList>


namespace Soprano {

    class Node;
    class NodeIterator;
    class QueryResultIteratorBackend;
    class StatementIterator;
    class BindingSet;

    /**
     * \class QueryResultIterator queryresultiterator.h Soprano/QueryResultIterator
     *
     * \brief An iterator for query results.
     *
     * %Query results in %Soprano are wrapped in a QueryResultIterator.
     *
     * %Query iterators are returned by Model::executeQuery(). In contrast to
     * NodeIterator or StatementIterator %QueryResultIterator has a set of different
     * access methods for the current dataset which can be one of three things:
     *
     * \li A Statement: Graph query results are represented as a stream of statements.
     *     See also currentStatement() and iterateStatements().
     * \li A BindingSet: Tuple query results are represented by a set of variable bindings
     *     according to the variables used in the query. The bindings can be accessed as a set
     *     through the normal Iterator method current() or separately through #binding(int) const or
     *     #binding(const QString&) const.
     * \li A boolean value: This is a special case in which the query was a boolean query
     *     (a SPARQL ASK query). In this case there is nothing to iterate but only a single
     *     boolean value which can be accessed through boolValue().
     *
     * <b>Example:</b>
     *
     * \code
     * QueryResultIterator it = model->executeQuery( someGraphQuery );
     * while( it.next() ) {
     *    doSomething( it.currentStatement() );
     * }
     *
     * QueryResultIterator it2 = model->executeQuery( someTupleQuery );
     * while( it.next() ) {
     *    doSomethingElse( it.currentBindings() );
     *    doSomethingCompletelyDifferent( it.binding( "x" ) );
     *    doSomethingEntirelyDifferent( it.binding( 0 ) );
     * }
     * \endcode
     *
     * Many backends do lock the underlying Model during iteration. Thus, 
     * it is always a good idea to cache the results if they are to be used
     * to modify the model to prevent a deadlock:
     *
     * \code
     * Soprano::QueryResultIterator it = model->executeQuery( someTupleQuery );
     * QList<BindingSet> allBindings = it.allBindings();
     * Q_FOREACH( Soprano::BindingSet bs, allBindings ) {
     *    modifyTheModel( model, bs );
     * }
     * \endcode
     *
     * Iterators have to be closed.
     * This can either be achieved by deleting the iterator, finishing it (next() does return \p false),
     * or calling close(). Before that other operations on the Model may block.
     *
     * Iterators are not thread-safe. Two threads using the same iterator may result
     * in undefined behaviour and even crashes. An iterator needs to be closed by the
     * same thread that opened it (except if the iterator contains special code to handle such
     * a situation.)
     *
     * \warning Be aware that iterators in Soprano are shared objects which means
     * that copies of one iterator object work on the same data.
     *
     * For further details on %Soprano iterators see Iterator.
     *
     * \author Daniele Galdi <daniele.galdi@gmail.com><br>Sebastian Trueg <trueg@kde.org>
     */
    class SOPRANO_EXPORT QueryResultIterator : public Iterator<BindingSet>
    {
    public:
        //@{
        /**
         * Creates and empty, invalid iterator.
         */
        QueryResultIterator();

        /**
         * Copy constructor. Copies of iterators share their data.
         */
        QueryResultIterator( const QueryResultIterator& );

        /**
         * Create a new QueryResultIterator which uses qr as backend.
         * QueryResultIterator will take ownership of the QueryResultIteratorBackend.
         */
        QueryResultIterator( QueryResultIteratorBackend *qr );

        /**
         * Destructor.
         */
        virtual ~QueryResultIterator();

        /**
         * Copies of iterators share their data.
         */
        QueryResultIterator& operator=( const QueryResultIterator& );
        //@}

        //@{
        /**
         * Retrieve the current Statement after a call to next.
         * This method does only make sense for graph queries.
         */
        Statement currentStatement() const;

        /**
         * Convenience method that puts all current bindings into one map.
         * This method does only make sense for tuple queries.
         */
        BindingSet currentBindings() const;

        /**
         * This method does only make sense for boolean queries.
         *
         * \return The result of a boolean query (SPARQL ASK).
         *
         * \sa isBool()
         */
        bool boolValue() const;
        //@}

        //@{
        /**
         * Get the current binding for a variable by index.
         *
         * \param offset The index of the requested variable.
         *
         * This is equivalent to binding(int) const.
         *
         * \return The binding for the requested variable or and invalid
         * node if offset is out of bounds, i.e. bigger or equal to bindingCount().
         *
         * \since 2.2
         */
        Node operator[]( int offset ) const;

        /**
         * Get the current binding for a variable.
         *
         * \param name The name of the requested variable.
         *
         * This is equivalent to binding(const QString&) const.
         *
         * \return The binding for the requested variable or and invalid
         * node if the bindings do not contain the variable.
         *
         * \since 2.2
         */
        Node operator[]( const QString name ) const;

        /**
         * Get the current binding for a variable.
         *
         * \param name The name of the requested variable.
         *
         * This method does only make sense for tuple queries.
         *
         * \return The binding for the requested variable or and invalid
         * node if the bindings do not contain the variable.
         */
        Node binding( const QString &name ) const;

        /**
         * Get the current binding for a variable by index.
         *
         * \param offset The index of the requested variable.
         *
         * This method does only make sense for tuple queries.
         *
         * \return The binding for the requested variable or and invalid
         * node if offset is out of bounds, i.e. bigger or equal to bindingCount().
         */    
        Node binding( int offset ) const;

        /**
         * The number of bindings in this query result.
         *
         * This method does only make sense for tuple queries.
         *
         * \return The number of bindings.
         */
        int bindingCount() const;

        /**
         * This method does only make sense for tuple queries.
         *
         * \return The names of the bound variables in this query result.
         */
        QStringList bindingNames() const;
        //@}

        //@{
        /**
         * Check if this is a graph result.
         *
         * \return \p true if this result refers to a graph query, i.e. currentStatement()
         * and iterateStatements() return valid values.
         */
        bool isGraph() const;

        /**
         * Check if this is a tuple result.
         *
         * \return \p true if this result refers to a tuple query, i.e. currentBindings(),
         * binding(), bindingCount(), bindingNames(), and allBindings() return valid values.
         */
        bool isBinding() const;

        /**
         * Check if this is a boolean result.
         *
         * There is no need to call next() for boolean results. However, for internal reasons
         * backends need to always return \p true for boolean queries.
         *
         * \return \p true if this result refers to a boolean query (SPARQL ASK), i.e.
         * boolValue() returns a valid value.
         */
        bool isBool() const;
        //@}

        //@{
        /**
         * Convenience method that collects all binding sets that are left
         * in the iterator.
         */
        QList<BindingSet> allBindings();

        /**
         * Convenience method that creates an iterator over the statements in this query result.
         * This method does only make sense for graph queries.
         *
         * \warning The new iterator is just a wrapper around this one. Thus, changing it will also
         * change this one.
         * 
         * \return A wrapper iterator over the statements in a graph query.
         */
        StatementIterator iterateStatements() const;

        /**
         * Convenience method that creates an iterator over one column of bindings in this query result.
         * This method does only make sense for tuple queries.
         *
         * \param variableName The name of the requested variable.
         *
         * \warning The new iterator is just a wrapper around this one. Thus, changing it will also
         * change this one.
         * 
         * \return A wrapper iterator over one column in a tuple query or an invalid iterator if the
         * result does not contain bindings for variableName.
         */
        NodeIterator iterateBindings( const QString& variableName ) const;

        /**
         * Convenience method that creates an iterator over one column of bindings in this query result.
         * This method does only make sense for tuple queries.
         *
         * \param offset The index of the requested variable.
         *
         * \warning The new iterator is just a wrapper around this one. Thus, changing it will also
         * change this one.
         * 
         * \return A wrapper iterator over one column in a tuple query or an invalid iterator
         * if offset is out of bounds, i.e. bigger or equal to bindingCount().
         */
        NodeIterator iterateBindings( int offset ) const;

        /**
         * Convenience method that creates an iterator over statements constructed from the values of the 
         * provided bindings.
         *
         * The typical usage would be with a query as follows:
         *
         * \code
         * Soprano::StatementIterator it =
         *     model->executeQuery( "select * where { graph ?c { ?s ?p ?o . } }" )
         *     .iterateStatementsFromBindings( "s", "p", "o", "c" );
         * \endcode
         *
         * \param subjectBindingName The name of the binding that will be used to set the subject of the 
         * constructed statements.
         * \param predicateBindingName The name of the binding that will be used to set the predicate of the 
         * constructed statements.
         * \param objectBindingName The name of the binding that will be used to set the object of the 
         * constructed statements.
         * \param contextBindingName The name of the binding that will be used to set the context of the 
         * constructed statements.
         * \param templateStatement If any of the provided binding names is empty the corresponding nodes
         * in the resulting statements will be filled by templateStatement.
         *
         * \warning The new iterator is just a wrapper around this one. Thus, changing it will also
         * change this one.
         *
         * \return A wrapper iterator over statements constructed from the specified bindings.
         *
         * \since 2.2
         */
        StatementIterator iterateStatementsFromBindings( const QString& subjectBindingName, 
                                                         const QString& predicateBindingName, 
                                                         const QString& objectBindingName, 
                                                         const QString& contextBindingName = QString(),
                                                         const Statement& templateStatement = Statement() ) const;
        //@}
    };
}

#endif // SOPRANO_RESULT_SET_H
