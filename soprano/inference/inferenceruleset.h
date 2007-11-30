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

#ifndef _INFERENCE_RULESET_H_
#define _INFERENCE_RULESET_H_

#include "soprano_export.h"

#include <QtCore/QSharedDataPointer>
#include <QtCore/QStringList>


namespace Soprano {
    namespace Inference {

        /**
         * %Soprano defines a number of rule sets.
         */
        enum StandardRuleSet {
            RDFS,  /**< RDFS closure rules. */
            NRL    /**< NRL (Nepomuk Representation Language closure rules. */
        };

        class Rule;

        /**
         * \class RuleSet inferenceruleset.h Soprano/Inference/RuleSet
         *
         * \brief A set of inference rules.
         *
         * RuleSet represents a set of inference rules. Each Rule
         * has a unique name.
         *
         * \author Sebastian Trueg <trueg@kde.org>
         */
        class SOPRANO_EXPORT RuleSet
        {
        public:
            /**
             * Create an empty RuleSet.
             */
            RuleSet();
        
            /**
             * Copy constructor.
             */
            RuleSet( const RuleSet& );

            /**
             * Destructor.
             */
            ~RuleSet();

            RuleSet& operator=( const RuleSet& );

            void clear();

            /**
             * Add a Rule to the set.
             *
             * \param name The name the new rule is supposed to be
             * stored under.
             * \param rule The new rule to store in the set.
             */
            void insert( const QString& name, const Rule& rule );

            /**
             * Number of rules in the set.
             * \return The number of rules.
             */
            int count() const;

            Rule at( int index ) const;
            Rule operator[]( int index ) const;

            Rule rule( const QString& name ) const;
            Rule operator[]( const QString& name ) const;

            QStringList ruleNames() const;

            QList<Rule> allRules() const;

            /**
             * Read a standard %Soprano rulset.
             *
             * \sa RuleParser
             */
            static RuleSet standardRuleSet( StandardRuleSet );

        private:
            class Private;
            QSharedDataPointer<Private> d;
        };
    }
}

#endif
