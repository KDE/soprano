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

#ifndef _SOPRANO_STATEMENT_PATTERN_H_
#define _SOPRANO_STATEMENT_PATTERN_H_

#include <QtCore/QSharedDataPointer>

#include "soprano_export.h"
#include "node.h"


namespace Soprano {

    class Statement;
    class BindingSet;

    namespace Inference {

        class NodePattern;

        /**
         * \class StatementPattern statementpattern.h Soprano/Inference/StatementPattern
         *
         * \brief Building block of an inference Rule.
         *
         * A StatementPattern consists of three NodePattern that can either
         * be variables or already bound ndoes.
         *
         * StatementPattern is used to define preconditions and effects in Rule.
         *
         * \author Sebastian Trueg <trueg@kde.org>
         */
        class SOPRANO_EXPORT StatementPattern
        {
        public:
            /**
             * Creates an invalid pattern
             */
            StatementPattern();
            StatementPattern( const NodePattern&, const NodePattern&, const NodePattern& );
            StatementPattern( const StatementPattern& );
            ~StatementPattern();

            StatementPattern operator=( const StatementPattern& );

            NodePattern subjectPattern() const;
            NodePattern predicatePattern() const;
            NodePattern objectPattern() const;

            bool match( const Statement& ) const;

            QString createSparqlGraphPattern( const BindingSet& bindings ) const;

            /**
             * \return \p true if the pattern is valid.
             *
             * \since 2.3
             */
            bool isValid() const;

        private:
            class Private;
            QSharedDataPointer<Private> d;
        };
    }
}

SOPRANO_EXPORT QDebug operator<<( QDebug s, const Soprano::Inference::StatementPattern& );

#endif
