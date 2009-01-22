/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2007-2008 Sebastian Trueg <trueg@kde.org>
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

#ifndef _SOPRANO_SIMPLE_NODE_ITERATOR_H_
#define _SOPRANO_SIMPLE_NODE_ITERATOR_H_

#include "nodeiterator.h"
#include "soprano_export.h"

#include <QtCore/QList>

namespace Soprano {
    namespace Util {
        /**
         * \class SimpleNodeIterator simplenodeiterator.h Soprano/Util/SimpleNodeIterator
         *
         * \brief The most simple version of a NodeIterator simply iterates over
         * a list of Nodes.
         *
         * The SimpleNodeIterator has been designed as a trivial extension
         * to NodeIterator and can be used as a drop-in-replacement as shown
         * below:
         *
         * \code
         * QList<Node> list;
         * SimpleNodeIterator it( list );
         *
         * // it and it2 iterate over the exact same data
         * NodeIterator it2 = it;
         * \endcode
         *
         * \author Sebastian Trueg <trueg@kde.org>
         */
        class SOPRANO_EXPORT SimpleNodeIterator : public NodeIterator
        {
        public:
            SimpleNodeIterator();
            SimpleNodeIterator( const QList<Node>& );
            SimpleNodeIterator( const SimpleNodeIterator& );
            virtual ~SimpleNodeIterator();

            SimpleNodeIterator& operator=( const QList<Node>& );
            SimpleNodeIterator& operator=( const SimpleNodeIterator& );

        private:
            class Private;
            Private* const d;
        };
    }
}

#endif
