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
    class Query;
    class ResultSet;
    class Statement;
    class StatementIterator;

    /**
     * A Model is the central RDF storage class in Soprano. It is a queryable
     * collection of statements.
     */
    class SOPRANO_EXPORT Model : public QObject
    {
	Q_OBJECT

    public:
	virtual ~Model();

	/**
	 * Add the Statement to the Model.
	 *
	 * \param statement The Statement to add.
	 */
	virtual ErrorCode addStatement( const Statement &statement ) = 0;

	/**
	 * Convinience method which adds all statements in model to this Model.
	 */
	ErrorCode addModel( const Model &model );

	/**
	 * Convinience method which adds all statements in iter to this Model.
	 */
	ErrorCode addStatements( const StatementIterator &iter );

	/**
	 * Convinience method which adds all %statements in statements to this Model.
	 */
	ErrorCode addStatements( const QList<Statement> &statements );

	/**
	 * \return true if the Model doesn't contains any Statement.
	 */
	virtual bool isEmpty() const = 0;

	/**
	 * \return A List of Context Nodes.
	 */
	virtual QList<Node> listContexts() const = 0;

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

	/**
	 * Execute the given query over the Model.
	 *
	 * \return All the Statements that match the query.
	 */
	virtual ResultSet executeQuery( const Query &query ) const = 0;

	/**
	 * Return an iterator over Model Statements that "partial"
	 * match the input Statement.
	 *
	 * \param partial The partial Statement to match.
	 *
	 * \return An iterator for all the matched Statements.
	 */
	virtual StatementIterator listStatements( const Statement &partial ) const = 0;

	/**
	 * Convinience method which lists all statements in this Model.
	 */
	StatementIterator listStatements() const;

	/**
	 * Convinience method which lists all statements in context.
	 */
	StatementIterator listStatementsInContext( const Node &context ) const;

	/**
	 * Remove all statements that match the partial statement.
	 *
	 * \param statement A possible partially defined statement that serves as
	 * a filter for all statements that should be removed.
	 */
	virtual ErrorCode removeStatements( const Statement &statement ) = 0;

	/**
	 * Convinience method which removes all %statements in statements.
	 */
	ErrorCode removeStatements( const QList<Statement> &statements );

        /**
	 * Convinience method that removes all statements in the context.
	 */
	ErrorCode removeContext( const Node& );

	/**
	 * Convinience method that clear the Model of all statements
	 */
	ErrorCode removeAllStatements();

	/**
	 * The number of statements stored in this Model.
	 * \return The size of the Model. 
	 *         -1 if not supported.
	 */
	virtual int statementCount() const = 0;

	/**
	 * Write all statements in this Model to os.
	 *
	 * Default implementation is based on Model::listStatements
	 */
	virtual ErrorCode write( QTextStream &os ) const;

	/**
	 * Print all statements in this Model to stdout.
	 *
	 * Default implementation is based on Model::write
	 */
	virtual ErrorCode print() const;

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
	class Private;
	Private* const d;
    };
}

#endif // SOPRANO_MODEL_H

