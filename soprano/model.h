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

#ifndef SOPRANO_MODEL_H
#define SOPRANO_MODEL_H

#include <QtCore/QObject>
#include <QtCore/QList>

#include "soprano_export.h"
#include "error.h"


class QTextStream;

namespace Soprano
{
    class Node;
    class QueryLegacy;
    class QueryResultIterator;
    class Statement;
    class StatementIterator;
    class NodeIterator;

    /**
     * \brief A Model is the central class in %Soprano. It is a queryable
     * collection of RDF quadruples, i.e statements.
     *
     * Model itself is just an interface for numerous implementations. There are basically
     * two types of Models in %Soprano:
     *
     * \li StorageModel is the base class for Model implementations that actually store RDF quadruples.
     *     StorageModels are created transparently by %Soprano backend plugins: Backend::createModel()
     * \li FilterModel is the base class for all filter models. FilterModels can be stacked on top of
     *     a StorageModel to perform arbitrary tasks like inference or actual content filtering. An
     *     important FilterModel is Inference::InferenceModel.
     *
     * The simplest way to create a memory Model is to use the default Backend:
     *
     * \code
     * Model* memModel = Soprano::createModel();
     * \endcode
     *
     * <b>%Error handling:</b>
     *
     * Model is based on %Soprano's own error handling system which tries to emulate exceptions to a certain extend.
     * Most methods in Model have a means of reporting if an operation was successful or not. For additional error
     * information Model inherits ErrorCache which provides the method lastError().
     *
     * Thus, advanced error handling would look as follows:
     *
     * \code
     * Soprano::Model* model = Soprano::createModel();
     * Soprano::Statement invalidStatement;
     * if( model->addStatement( invalidStatement ) != Error::ERROR_NONE ) {
     *    showErrorMessage( model->lastError().message() );
     * }
     * \endcode
     *
     * \sa \ref soprano_error_handling
     *
     * Model is thread-safe when used with a thread-safe backend (all "official" %Soprano backends are thread-safe).
     * However, it is recommended to create Model instances in the main thread.
     *
     * \author Daniele Galdi <daniele.galdi@gmail.com><br>Sebastian Trueg <trueg@kde.org>
     */
    class SOPRANO_EXPORT Model : public QObject, public Error::ErrorCache
    {
	Q_OBJECT

    public:
	virtual ~Model();

	/**
	 * Add the Statement to the Model.
	 *
	 * \param statement The Statement to add.
	 */
	virtual Error::ErrorCode addStatement( const Statement &statement ) = 0;

	/**
	 * Convinience method which adds all statements in model to this Model.
	 */
	Error::ErrorCode addModel( const Model &model );

	/**
	 * Convinience method which adds all statements in iter to this Model.
	 */
	Error::ErrorCode addStatements( StatementIterator iter );

	/**
	 * Convinience method which adds all %statements in statements to this Model.
	 */
	Error::ErrorCode addStatements( const QList<Statement> &statements );

	/**
	 * \return true if the Model doesn't contains any Statement.
	 */
	virtual bool isEmpty() const = 0;

	/**
	 * List all contexts in the model, i.e. all named graphs.
	 *
	 * \return An iterator over context Nodes, on error an invalid iterator is returned.
	 */
	virtual NodeIterator listContexts() const = 0;

	/**
	 * The default implementation is simply based on listStatements( const Statement& ).
	 *
	 * \return true if the Model contains the given Statement.
	 */
	virtual bool containsStatements( const Statement &statement ) const = 0;

	/**
	 * Convinience method which is based on containsStatements
	 */
	bool containsContext( const Node &context ) const;

	// possible interfaces for queries:
	// 1. the most simple one: a string query. let the backend do the parsing, make our Query stuff optional
	//
	// Advantages:
	//  * Simple, no change in backends necessary
	//
	// Disadvantages:
	//  * Our Query API is only optional and using it means a lot of converting
	//  * No possibility of reporting syntax errors.
	//
	//QueryResultIterator query( const QString& query, QueryLanguage language, const QString& userQueryLanguage = QString() ) const;

	// 2. Use our query stuff
	// 2.1 only Query, no string representation without serializer
	// 2.2 Query does contain the original string it was parsed from (seems not very clean API-wise)
	//
	// Advantages:
	//  * Cleaner Model API
	//  * Possibility of query optimization and modification by FilterModels
	//
	// Disadvantages:
	//  * More complicated for Model users: query objects have to be created, in many cases using a QueryParser?
	//  * The backend needs to support the Query API or use a serializer to convert the query back to string form
	//
	//QueryResultIterator query( const Query::Query& query ) const;

	// 3. Support both, provide default implementation of the first one in StorageModel by using QueryParsers

	/**
	 * Execute the given query over the Model.
	 *
	 * \return All the Statements that match the query, on error an invalid iterator is returned.
	 */
	virtual QueryResultIterator executeQuery( const QueryLegacy &query ) const = 0;

	/**
	 * Return an iterator over Model Statements that "partial"
	 * match the input Statement.
	 *
	 * \param partial The partial Statement to match.
	 *
	 * \return An iterator for all the matched Statements, on error an invalid iterator is returned.
	 */
	virtual StatementIterator listStatements( const Statement &partial ) const = 0;

	/**
	 * Convinience method which lists all statements in this Model.
	 *
	 * \return An iterator for all the matched Statements, on error an invalid iterator is returned.
	 */
	StatementIterator listStatements() const;

	/**
	 * Convinience method which lists all statements in context.
	 *
	 * \return An iterator for all the matched Statements, on error an invalid iterator is returned.
	 */
	StatementIterator listStatementsInContext( const Node &context ) const;

	/**
	 * Remove all statements that match the partial statement.
	 *
	 * \param statement A possible partially defined statement that serves as
	 * a filter for all statements that should be removed.
	 */
	virtual Error::ErrorCode removeStatements( const Statement &statement ) = 0;

	/**
	 * Convinience method which removes all %statements in statements.
	 */
	Error::ErrorCode removeStatements( const QList<Statement> &statements );

        /**
	 * Convinience method that removes all statements in the context.
	 */
	Error::ErrorCode removeContext( const Node& );

	/**
	 * Convinience method that clear the Model of all statements
	 */
	Error::ErrorCode removeAllStatements();

	/**
	 * The number of statements stored in this Model.
	 * \return The size of the Model, or -1 on error.
	 */
	virtual int statementCount() const = 0;

	/**
	 * Write all statements in this Model to os.
	 *
	 * Default implementation is based on Model::listStatements
	 */
	virtual Error::ErrorCode write( QTextStream &os ) const;

	/**
	 * Print all statements in this Model to stdout.
	 *
	 * Default implementation is based on Model::write
	 */
	virtual Error::ErrorCode print() const;
	
    signals:
	/**
	 * Emitted when new statements have been added to the model.
	 *
	 * Implementations of this interface have to emit this signal.
	 */
	void statementsAdded();

	/**
	 * Emitted when statements have been removed from the model.
	 *
	 * Implementations of this interface have to emit this signal.
	 */
	void statementsRemoved();

    protected:
	Model();

    private:
	/**
	 * Model instances are not meant to be copied.
	 */
	Model( const Model& );
	Model& operator=( const Model& );

	class Private;
	Private* const d;
    };
}

#endif // SOPRANO_MODEL_H

