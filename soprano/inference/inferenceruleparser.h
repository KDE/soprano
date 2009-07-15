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

#ifndef _INFERENCE_RULE_PARSER_H_
#define _INFERENCE_RULE_PARSER_H_

#include <QtCore/QString>
#include <QtCore/QHash>
#include <QtCore/QUrl>

#include "soprano_export.h"

namespace Soprano {
    namespace Inference {

        class RuleSet;
        class Rule;

        /**
         * \class RuleParser inferenceruleparser.h Soprano/Inference/RuleParser
         *
         * \brief The RuleParser can be used to parse %Soprano rule files.
         *
         * \sa RuleSet::standardRuleSet()
         *
         * \author Sebastian Trueg <trueg@kde.org>
         */
        class SOPRANO_EXPORT RuleParser
        {
        public:
            RuleParser();
            ~RuleParser();

            /**
             * Parse rules from a file. The parsed rules
             * can be accessed via rules().
             *
             * \return \p true on success, \p false otherwise.
             */
            bool parseFile( const QString& path );

            /**
             * Parse a single rule from a string.
             *
             * \param line A single line defining the rule
             *
             * \return The parsed rule in case \p line is a valid
             * rule line, an invalid rule otherwise. The parsed rule
             * can also be accessed via rules().
             *
             * \since 2.3
             */
            Rule parseRule( const QString& line );

            /**
             * Retrieve the rules parsed in parseFile and parseRule
             */
            RuleSet rules() const;

            /**
             * Add a prefix to use during rule parsing.
             *
             * \param qname The abbreviated name of the prefix
             * \param uri The namespace to use for the prefix
             *
             * \since 2.3
             */
            void addPrefix( const QString& qname, const QUrl& uri );

            /**
             * Retrieve all prefixes set via addPrefix.
             *
             * \return a QHash containing of the prefixe qnames and their URIs.
             *
             * \since 2.3
             */
            QHash<QString, QUrl> prefixes() const;

            /**
             * Clear all parsed rules and prefixes.
             *
             * \since 2.3
             */
            void clear();

        private:
            class Private;
            Private* const d;
        };
    }
}

#endif
