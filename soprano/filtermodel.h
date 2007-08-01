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

#ifndef _SOPRANO_FILTER_MODEL_H_
#define _SOPRANO_FILTER_MODEL_H_

#include "model.h"
#include "soprano_export.h"


namespace Soprano {
    /**
     * FilterModels can be stacked to provide filtering on each of
     * the Model's methods. A FilterModel has a parent model which
     * is the data base for this filter. The parent can be an actual
     * Model or another FilterModel.
     */
    class SOPRANO_EXPORT FilterModel : public Model
    {
	Q_OBJECT

    public:
	/**
	 * Create a new FilterModel which works on parent.
	 * \param parent The parent Model.
	 */
	FilterModel( Model* parent );
	virtual ~FilterModel();

	Model* parent() const;

	/**
	 * Default implementation simple pipes the call through to the parent model.
	 * \sa Model::add( const Statement &statement )
	 */
	virtual ErrorCode add( const Statement &statement );

	/**
	 * Default implementation simple pipes the call through to the parent model.
	 * \sa Model::isEmpty()
	 */
	virtual bool isEmpty() const;

	/**
	 * Default implementation simple pipes the call through to the parent model.
	 * \sa Model::contexts()
	 */
	virtual QList<Node> contexts() const;

	/**
	 * Default implementation simple pipes the call through to the parent model.
	 * \sa Model::contains( const Statement &statement )
	 */
	virtual bool contains( const Statement &statement ) const;

	/**
	 * Default implementation simple pipes the call through to the parent model.
	 * \sa Model::contains( const Node &context )
	 */
	virtual bool contains( const Node &context ) const;

	/**
	 * Default implementation simple pipes the call through to the parent model.
	 * \sa Model::executeQuery
	 */
	virtual ResultSet executeQuery( const Query &query ) const;

	/**
	 * Default implementation simple pipes the call through to the parent model.
	 * \sa Model::listStatements()
	 */
	virtual StatementIterator listStatements() const;

	/**
	 * Default implementation simple pipes the call through to the parent model.
	 * \sa Model::listStatements( const Node &context )
	 */
	virtual StatementIterator listStatements( const Node &context ) const;

	/**
	 * Default implementation simple pipes the call through to the parent model.
	 * \sa Model::listStatements( const Statement &partial )
	 */
	virtual StatementIterator listStatements( const Statement &partial ) const;

	/**
	 * Default implementation simple pipes the call through to the parent model.
	 * \sa Model::remove( const Statement &statement )
	 */
	virtual ErrorCode remove( const Statement &statement );

	/**
	 * Default implementation simple pipes the call through to the parent model.
	 * \sa Model::remove( const QList<Statement> &statements )
	 */
	virtual ErrorCode remove( const QList<Statement> &statements );

	/**
	 * Default implementation simple pipes the call through to the parent model.
	 * \sa Model::remove( const Node &context )
	 */
	virtual ErrorCode remove( const Node &context );

	/**
	 * Default implementation simple pipes the call through to the parent model.
	 * \sa Model::removeAll( const Statement &statement )
	 */
	virtual ErrorCode removeAll( const Statement &statement );

	/**
	 * Default implementation simple pipes the call through to the parent model.
	 * \sa Model::removeAll()
	 */
	ErrorCode removeAll();

	/**
	 * Default implementation simple pipes the call through to the parent model.
	 * \sa Model::size()
	 */
	virtual int size() const;

	/**
	 * Default implementation simple pipes the call through to the parent model.
	 * \sa Model::write
	 */
	virtual ErrorCode write( QTextStream &os ) const;

	/**
	 * Default implementation simple pipes the call through to the parent model.
	 * \sa Model::print
	 */
	virtual ErrorCode print() const;

    protected:
	/**
	 * Simply pipes the Model signals from the parent through to this
	 * instance.
	 * Call this in the constructor if you have no need to modify the 
	 * results of the signals.
	 */
	void setupDefaultConnections();

    private:
	class Private;
	Private* const d;
    };
}

#endif
