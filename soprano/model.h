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

#include <soprano/soprano_export.h>
#include <soprano/error.h>

class QTextStream;

namespace Soprano
{
    class Node;
    class Query;
    class ResultSet;
    class Statement;
    class StatementIterator;

// FIXME: what about a QList<Statement> allStatements method?
    class SOPRANO_EXPORT Model : public QObject
    {
	Q_OBJECT

    public:
	Model();

	virtual ~Model();

	/**
	 * Add all the Statements present in the given Model to the
	 * current Model.
	 *
	 * \param model The Model to add.
	 */
	// FIXME: why is this not a virtual method? There is for example librdf_add_submodel
	ErrorCode add( const Model &model );

	/**
	 * Add Statements to the Model.
	 *
	 * \param iter The StatementIterator to add.
	 */
	ErrorCode add( const StatementIterator &iter );
  
	/**
	 * Add Statements to the Model with a Context.
	 *
	 * If Context is empty, this is equivalent to add( const StatementIterator &iter ).
	 *
	 * \param iter The StatementIterator to add.
	 * \param context The Context node.
	 * \deprecated
	 */
	ErrorCode add( const StatementIterator &iter, const Node &context );

	/**
	 * Add all Statements to the Model.
	 *
	 * \param statements A list of Statements to add.
	 */
	// FIXME: why not virtual?
	ErrorCode add( const QList<Statement> &statements );

	/**
	 * Add all Statements to the Model.
	 *
	 * If Context is empty, this is equivalent to add( const QList<Statement> &statements ).
	 *
	 * \param statements A list of Statements to add.
	 * \param context The Context node.
	 * \depreacted
	 */
	ErrorCode add( const QList<Statement> &statements, const Node &context );

	/**
	 * Add the Statement to the Model.
	 *
	 * \param statement The Statement to add.
	 */
	virtual ErrorCode add( const Statement &statement ) = 0;

	/**
	 * Add a statement to a model with a context. 
	 * It must be a complete statement - all of subject, predicate, object
	 * parts must be present. 
	 *
	 * \param statement The Statement to add.
	 * \param context The Context node which overwrites the statement's context.
	 *
	 * \deprecated Use add( const Statement& )
	 */
	virtual ErrorCode add( const Statement &statement, const Node &context );

	/**
	 * \return true if the Model doesn't contains any Statement.
	 */
	virtual bool isEmpty() const;

	/**
	 * \return A List of Context Nodes.
	 */
	virtual QList<Node> contexts() const = 0;

	/**
	 * The default implementation is simply based on listStatements( const Statement& ).
	 *
	 * \return true if the Model contains the given Statement.
	 */
	virtual bool contains( const Statement &statement ) const;

	/**
	 * The default implementation is simply based on listStatements( const Node& ).
	 *
	 * \return true if the Model contains the given Context node.
	 */
	virtual bool contains( const Node &context ) const;

	/**
	 * Execute the given query over the Model.
	 *
	 * \return All the Statements that match the query.
	 */
	virtual ResultSet executeQuery( const Query &query ) const = 0;

	/**
	 * \return An iterator for all Model Statements.
	 */
	virtual StatementIterator listStatements() const;

	/**
	 * List all statements in a model context.
	 * The default implementation just calls listStatements( const Statement& ).
	 *
	 * \param context The Context Node.
	 *
	 * \return An iterator for all Context Model Statements.
	 */
	virtual StatementIterator listStatements( const Node &context ) const;

	/**
	 * Return an iterator over Model Statements that "partial"
	 * match the input Statement, in the given Context.
	 *
	 * \param partial The partial Statement to match.
	 * \param context The Context node which overwrites the statement's context.
	 *
	 * \return An iterator for all the matched Statements.
	 *
	 * \deprecated Use listStatements( const Statement& )
	 */
	virtual StatementIterator listStatements( const Statement &partial, const Node &context ) const;

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
	 * Return an iterator over Model statements that "partial"
	 * match the input parameter.
	 *
	 * \param subject The Subject node (can be empty)
	 * \param predicate The Predicate node (can be empty)
	 * \param object The Object node (can be empty)
	 *
	 * \return An iterator for all the matched Statements.
	 *
	 * \deprecated Use listStatements( const Statement& )
	 */
	StatementIterator listStatements( const Node &subject, const Node &predicate, const Node &object ) const;

	/**
	 * Remove a Statement from the Model in a Context. 
	 * It must be a complete statement - all of subject, predicate, object
	 * parts must be present.
	 *
	 * \param statement The Statement to remove.
	 * \param context The Context node which overwrites the statement's context.
	 *
	 * \deprecated Use remove( const Statement& )
	 */
	virtual ErrorCode remove(const Statement &statement, const Node &context );

	/**
	 * Remove a Statement from the Model. 
	 * It must be a complete statement - all of subject, predicate, object
	 * parts must be present. 
	 *
	 * \param statement The Statement to remove.
	 */
	virtual ErrorCode remove( const Statement &statement ) = 0;

	/**
	 * Remove a list of Statements from the Model. 
	 * 
	 * \param statements The Statements to remove.
	 */
	virtual ErrorCode remove( const QList<Statement> &statements );

	/**
	 * Remove Statements from a Model with the given Context.
	 *
	 * \param context The Context.
	 */
	virtual ErrorCode remove( const Node &context ) = 0;

	/**
	 * \deprecated Use removeAll( const Statement& )
	 */
	virtual ErrorCode removeAll( const Statement &statement, const Node &context );

	virtual ErrorCode removeAll( const Statement &statement );

	/**
	 * Remove all the statements matching (s, p, o) from this model.
	 *
	 * \param subject The Subject node (can be empty)
	 * \param predicate The Predicate node (can be empty)
	 * \param object The Object node (can be empty)
	 * \deprecated use removeAll( const Statement &statement )
	 */
	virtual ErrorCode removeAll( const Node &subject, const Node &predicate, const Node &object );

	/**
	 * Remove all the statements from this model.
	 */
	ErrorCode removeAll();

	/**
	 * \return The size of the Model (number of Stamenent). 
	 *         -1 if not supported.
	 */
	virtual int size() const = 0;

	/**
	 * Write a model to a textstream. The default implementation uses listStatements
	 * to get all statements in the model which might be very inefficient.
	 *
	 * \param The stream.
	 *
	 * \return ERROR_UNKNOW if an Error occurred.
	 */
	virtual ErrorCode write( QTextStream &os ) const;

	/**
	 * Print the Model to the stdout. The default implementation uses listStatements
	 * to get all statements in the model which might be very inefficient.
	 *
	 * \return ERROR_UNKNOW if an Error occurred.
	 */
	virtual ErrorCode print() const;

    signals:
	void statementsAdded();
	void statementsRemoved();

    private:
	class Private;
	Private* const d;
    };
}

#endif // SOPRANO_MODEL_H

