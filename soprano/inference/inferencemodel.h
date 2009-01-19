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
#include "soprano_export.h"

class QUrl;

namespace Soprano {

    class Statement;

    namespace Inference {

        class Rule;

        /**
         * \class InferenceModel inferencemodel.h Soprano/Inference/InferenceModel
         *
         * \brief The Soprano Inference Model provides a simple forward chaining inference engine
         * which uses the underlying parent model itself to store status information.
         *
         * The InferenceModel does perfect inference which means that removing of statements is supported
         * and results in a perfect update of the infered statements. There is only one exception:
         * If a model contains two statements in different named graphs that both have the same subject,
         * predicate, and object and trigger one rule then if one of these statements is removed the
         * infered statements are removed, too, although the second statement would still make the infered
         * one valid. This situation gets resolved once the same rule is triggered again by some other
         * added statement or performInference gets called.
         *
         * The inference is performed based on rules which are stored in Rule instances.
         * Rules can be created manually or parsed using a RuleParser.
         *
         * <b>The inference engine works roughly as follows:</b>
         *
         * Whenever a new statement is added it is compared to each rule to check if it could trigger this rule.
         * Then if it could trigger a rule this rule is applied to the whole model.
         *
         * If a rule produces a new infered statement the following data is created:
         * \li named graph A containing the infered statements
         * \li the statements that triggered the rule are stored in named graph sil:InferenceMetadata as
         * source statements of A (sil:sourceStatement). The inference model supports two ways of storing
         * source statements: plain and compressed (see setCompressedSourceStatements).
         *
         * Thus, when removing a statement it can easily be checked if this statement had been used to
         * infer another one by querying all named graphs that have this statement as a source statement.
         *
         * \author Sebastian Trueg <trueg@kde.org>
         */
        class SOPRANO_EXPORT InferenceModel : public FilterModel
        {
            Q_OBJECT

        public:
            InferenceModel( Model* parent );
            ~InferenceModel();

            /**
             * Add a new statement to the model. Inferencing will be done directly.
             * Inferenced statements are stored in additional named graphs.
             */
            Error::ErrorCode addStatement( const Statement& );

            /**
             * Remove one statement from the model.
             */
            Error::ErrorCode removeStatement( const Statement& );

            /**
             * Remove statements from the model.
             */
            Error::ErrorCode removeAllStatements( const Statement& );

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

            using FilterModel::addStatement;
            using FilterModel::removeStatement;
            using FilterModel::removeAllStatements;
        
        public Q_SLOTS:
            /**
             * Normally inferencing is done once new statements are added to the model or
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

            /**
             * If compressed statements are enabled source statements are stored compressed
             * in one literal value. Otherwise source statements are stored using rdf:subject,
             * rdf:predicate, rdf:object, and sil:context. Non-compressed statements are much
             * cleaner from an ontology design point of view while compressed statements take
             * much less space.
             *
             * By default comressed source statements are enabled.
             *
             * This method exists mainly for historical reasons and there normally is no need
             * to call it. Compressed statements should work well for most users.
             *
             * \param b If true compressed source statements are enabled (the default).
             */
            void setCompressedSourceStatements( bool b );

            /**
             * If the storage backend supports joined SPARQL queries via UNION
             * it makes sense to enable this.
             *
             * \param b If true InferenceModel will use optimized queries for
             * the inference during addStatement. This will speed up the 
             * process quite a lot as matching rules are only applied to the new statement.
             * This flag has no influcence on performInference() though.
             *
             * The default is to disable the optimized queries since the default
             * soprano redland backend does not support UNION.
             */
            void setOptimizedQueriesEnabled( bool b );

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
             * Get a list of all inference graphs (i.e. graphs that contain infered statements) that have statement 
             * as a source.
             */
            QList<Node> inferedGraphsForStatement( const Statement& statement ) const;

            /**
             * Create the statements to store an uncompressed source statement and add them to the parent model.
             * \return The URI of the uncompressed source statement resource.
             */
            QUrl storeUncompressedSourceStatement( const Statement& sourceStatement );

            class Private;
            Private* const d;
        };
    }
}

#endif
