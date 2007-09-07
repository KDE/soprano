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
     * \class FilterModel filtermodel.h soprano/FilterModel
     *
     * \brief A FilterModel is a virtual model that wraps another Model.
     * 
     * FilterModels can be stacked to provide filtering on each of
     * the Model's methods. A FilterModel has a parent model which
     * is the data base for this filter. The parent can be an actual
     * StorageModel or another FilterModel.
     *
     * <b>Error handling:</b>
     *
     * The FilterModel "inherits" its parent Model's errors, i.e. FilterModel::lastError()
     * always equals FilterModel::parent()->lastError() (If the FilterModel implementations
     * are called in subclasses).
     *
     * \author Sebastian Trueg <trueg@kde.org>
     */
    class SOPRANO_EXPORT FilterModel : public Model
    {
	Q_OBJECT

    public:
	/**
	 * Destructor
	 */
	virtual ~FilterModel();

	//@{
	/**
	 * Set the parent Model.
	 * \param model The Model that this filter will forward any commands to.
	 */
	virtual void setParentModel( Model* model );

	/**
	 * Get the parent model.
	 * \return The parent Model or 0 if none has been set.
	 */
	virtual Model* parentModel() const;
	//@}

	//@{
	/**
	 * Default implementation simply pipes the call through to the parent model.
	 */
	virtual Error::ErrorCode addStatement( const Statement &statement );
	//@}

	//@{
	/**
	 * Default implementation simply pipes the call through to the parent model.
	 */
	virtual Error::ErrorCode removeStatement( const Statement &statement );

	/**
	 * Default implementation simply pipes the call through to the parent model.
	 */
	virtual Error::ErrorCode removeAllStatements( const Statement &statement );
	//@}

	//@{
	/**
	 * Default implementation simply pipes the call through to the parent model.
	 */
	virtual StatementIterator listStatements( const Statement &partial ) const;

	/**
	 * Default implementation simply pipes the call through to the parent model.
	 */
	virtual NodeIterator listContexts() const;

	/**
	 * Default implementation simply pipes the call through to the parent model.
	 */
	virtual QueryResultIterator executeQuery( const QueryLegacy &query ) const;
	//@}

	//@{
	/**
	 * Default implementation simply pipes the call through to the parent model.
	 */
	virtual bool containsStatement( const Statement &statement ) const;

	/**
	 * Default implementation simply pipes the call through to the parent model.
	 */
	virtual bool containsAnyStatement( const Statement &statement ) const;
	//@}

	//@{
	/**
	 * Default implementation simply pipes the call through to the parent model.
	 */
	virtual bool isEmpty() const;

	/**
	 * Default implementation simply pipes the call through to the parent model.
	 */
	virtual int statementCount() const;
	//@}

	//@{
	/**
	 * Default implementation simply pipes the call through to the parent model.
	 */
	virtual Error::ErrorCode write( QTextStream &os ) const;
	//@}

	//@{
	/**
	 * Default implementation simply pipes the call through to the parent model.
	 */
	virtual Node createBlankNode();
	//@}

    protected:
	/**
	 * Create an empty filter model.
	 */
	FilterModel();

	/**
	 * Create a new FilterModel which works on parent.
	 * Calls setParentModel.
	 * \param parent The parent Model.
	 */
	FilterModel( Model* parent );

	/**
	 * Handle a statementsAdded() signal from the parent Model.
	 *
	 * The default implementation simply re-emits the signal.
	 */
	virtual void parentStatementsAdded();

	/**
	 * Handle a statementsRemoved() signal from the parent Model.
	 *
	 * The default implementation simply re-emits the signal.
	 */
	virtual void parentStatementsRemoved();

    private Q_SLOTS:
	/**
	 * This slot gets connected to Model::statementsAdded of the parent
	 * model.
	 */
	void slotStatementsAdded();

	/**
	 * This slot gets connected to Model::statementsRemoved of the parent
	 * model.
	 */
	void slotStatementsRemoved();

    private:
	class Private;
	Private* const d;
    };
}

#endif
