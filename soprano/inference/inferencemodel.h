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

#ifndef _SOPRANO_INFERENCE_MODEL_H_
#define _SOPRANO_INFERENCE_MODEL_H_

#include "filtermodel.h"

namespace Soprano {

    class Statement;

    namespace Inference {

	class Rule;

	/**
	 * \brief The Soprano Inference Model provides a simple forward chaining inference engine
	 * which uses the underlying parent model itself to store status information.
	 *
	 * The InferenceModel does perfect inference which means that removing of statements is supported
	 * and results in a perfect update of the infered statements.
	 *
	 * This is done by storing a rather large amount of status data in a special named graph
	 * (see Vocabulary::SIL::INFERENCE_METADATA). Thus, by using this inference model one gains
	 * inference of excellent quality but with a big "waste" of space.
	 *
	 * The inference is performed based on rules which are stored in Rule instances.
	 * Rules can be created manually or parsed using a RuleParser.
	 *
	 * <b>The inference engine works roughly as follows:</b>
	 *
	 * Whenever a new statement is added and it applies to one of the rules the following is generated:
	 * \li named graph A is created containing the infered statements
	 * \li the statements that triggered the rule are stored in named graph sil:InferenceMetadata as
	 * source statements of A (sil:sourceStatement).
	 *
	 * Thus, when removing a statement it can easily be checked if this statement had been used to
	 * infer another one by querying all named graphs that have this statement as a source statement.
	 */
	class InferenceModel : public FilterModel
	{
	    Q_OBJECT

	public:
	    InferenceModel( Model* parent );
	    ~InferenceModel();

	    /**
	     * Add a new statement to the model. Inferencing will be done directly.
	     * Inferenced statements are stored in additional named graphs.
	     */
	    ErrorCode addStatement( const Statement& );

	    /**
	     * Remove a statement from the model.
	     */
	    ErrorCode removeStatements( const Statement& );

	    /**
	     * Add an inference rule to the set of rules.
	     * This method will not trigger any inference action. If inference
	     * is necessary call performInference() after adding the new rules.
	     */
	    void addRule( const Rule& );

	    /**
	     * Set the inference rules to be used.
	     * This method will not trigger any inference action. If inference
	     * is necessary call performInference() after adding the new rules.
	     */
	    void setRules( const QList<Rule>& rules );
	    
	public Q_SLOTS:
	    /**
	     * Normally inferencing is done one new statements are added to the model or
	     * statements are removed. This method performs inferencing on the whole model.
	     * It is useful for initializing a model that already contains statements or
	     * update the model if it has been modified bypassing this filter model.
	     *
	     * Tha latter can easily be done by connecting the Model::statementsAdded and 
	     * Model::statementsRemoved signals to this slot.
	     */
	    void performInference();

	    /**
	     * Removes all statements infered by this model.
	     * This can be useful if the parent model has been changed without informing
	     * the inference model and statements have been removed.
	     */
	    void clearInference();

	private:
	    /**
	     * Create all infered statements that result from adding statement. Calls inferRule.
	     *
	     * \param statement The new statement, only matching rules will be recalculated.
	     * \param recurse If true inferred statements will recursively triggeer inferStatement.
	     *
	     * \return the number of new statements infered.
	     */
	    int inferStatement( const Statement& statement, bool recurse = false );

	    /**
	     * Create all infered statements that result from applying rule to the model.
	     *
	     * \param rule The rule to apply.
	     * \param recurse If true inferred statements will recursively triggeer inferStatement.
	     *
	     * \return the number of new statements infered.
	     */
	    int inferRule( const Rule& rule, bool recurse );

	    /**
	     * We can only handle removing one statement at a time.
	     */
	    void removeStatement( const Statement& );

	    /**
	     * Get a list of all inference graphs (i.e. graphs that contain infered statements) that have statement 
	     * as a source.
	     */
	    QList<Node> inferedGraphsForStatement( const Statement& statement ) const;

	    class Private;
	    Private* const d;
	};
    }
}

uint qHash( const Soprano::Node& node );

#endif
