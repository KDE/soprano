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
	virtual ~FilterModel();

	void setParentModel( Model* );

	Model* parentModel() const;

	/**
	 * Default implementation simple pipes the call through to the parent model.
	 * \sa Model::add( const Statement &statement )
	 */
	virtual ErrorCode addStatement( const Statement &statement );

	/**
	 * Default implementation simple pipes the call through to the parent model.
	 * \sa Model::isEmpty()
	 */
	virtual bool isEmpty() const;

	/**
	 * Default implementation simple pipes the call through to the parent model.
	 * \sa Model::listContexts()
	 */
	virtual QList<Node> listContexts() const;

	/**
	 * Default implementation simple pipes the call through to the parent model.
	 * \sa Model::containsStatements( const Statement &statement )
	 */
	virtual bool containsStatements( const Statement &statement ) const;

	/**
	 * Default implementation simple pipes the call through to the parent model.
	 * \sa Model::executeQuery
	 */
	virtual QueryResultIterator executeQuery( const Query &query ) const;

	/**
	 * Default implementation simple pipes the call through to the parent model.
	 * \sa Model::listStatements( const Statement &partial )
	 */
	virtual StatementIterator listStatements( const Statement &partial ) const;

	/**
	 * Default implementation simple pipes the call through to the parent model.
	 * \sa Model::removeStatements( const Statement &statement )
	 */
	virtual ErrorCode removeStatements( const Statement &statement );

	/**
	 * Default implementation simple pipes the call through to the parent model.
	 * \sa Model::statementCount()
	 */
	virtual int statementCount() const;

    protected:
	/**
	 * Create an empty filter model.
	 */
	FilterModel();

	/**
	 * Create a new FilterModel which works on parent.
	 * \param parent The parent Model.
	 */
	FilterModel( Model* parent );

    protected Q_SLOTS:
	/**
	 * This slot gets connected to Model::statementsAdded of the parent
	 * model.
	 * The default implementation simply re-emits the signal.
	 */
	virtual void slotStatementsAdded();

	/**
	 * This slot gets connected to Model::statementsRemoved of the parent
	 * model.
	 * The default implementation simply re-emits the signal.
	 */
	virtual void slotStatementsRemoved();

    private:
	class Private;
	Private* const d;
    };
}

#endif
