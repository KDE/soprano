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

#ifndef _SORPANO_INDEX_QUERY_HIT_H_
#define _SORPANO_INDEX_QUERY_HIT_H_

#include "soprano_export.h"

#include <QtCore/QSharedDataPointer>

namespace Soprano {

    class Node;

    namespace Index {
        /**
         * \class QueryHit indexqueryhit.h Soprano/Index/QueryHit
         *
         * \brief QueryHit represents a hit from an IndexFilterModel query. 
         * It basically is an RDF resource combined with a hit score.
         */
        class SOPRANO_INDEX_EXPORT QueryHit
        {
        public:
            /**
             * Create an empty hit.
             */
            QueryHit();

            /**
             * Default copy constructor.
             */
            QueryHit( const QueryHit& );
        
            /**
             * Create a new hit.
             * \param resource The resource of the hit.
             * \param score The hit score.
             */
            QueryHit( const Node& resource, double score );

            /**
             * Destructor.
             */
            ~QueryHit();

            QueryHit& operator=( const QueryHit& );

            double score() const;
            Node resource() const;

        private:
            class Private;
            QSharedDataPointer<Private> d;
        };
    }
}

#endif
