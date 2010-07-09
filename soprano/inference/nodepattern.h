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

#ifndef _SOPRANO_NODE_PATTERN_H_
#define _SOPRANO_NODE_PATTERN_H_

#include <QtCore/QSharedDataPointer>
#include <QtCore/QString>

#include "soprano_export.h"
#include "node.h"

namespace Soprano {

    class BindingSet;

    namespace Inference {
        /**
         * \class NodePattern nodepattern.h Soprano/Inference/NodePattern
         *
         * \brief Smallest building block of an inference Rule.
         *
         * There are two types of NodePattern:
         * \li Simple nodes that wrap around an instance or Node
         *     and only match against that particular node.
         * \li Variable nodes that match against every Node and
         *     have a name.
         *
         * \author Sebastian Trueg <trueg@kde.org>
         */
        class SOPRANO_EXPORT NodePattern
        {
        public:
            NodePattern();
            NodePattern( const Node& );
            NodePattern( const QString& varname );
            NodePattern( const NodePattern& );
            ~NodePattern();

            NodePattern& operator=( const NodePattern& );

            bool isVariable() const;

            /**
             * \returns the associated resource of an invalid Node
             * if this is a variable.
             */
            Node resource() const;

            /**
             * \returns The name of the variable or an empty string
             * if this is not a variable.
             */
            QString variableName() const;

            /**
             * Match a Node against this pattern.
             * Variables will match any Node while
             * specific %node patterns will only match
             * that one particular Node.
             * \return true if node mathes this pattern.
             */
            bool match( const Node& node ) const;

            QString createSparqlNodePattern( const BindingSet& bindings ) const;

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

SOPRANO_EXPORT QDebug operator<<( QDebug s, const Soprano::Inference::NodePattern& );

#endif
