/* 
 * This file is part of Soprano Project
 *
 * Copyright (C) 2006 Daniele Galdi <daniele.galdi@gmail.com>
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

#ifndef SOPRANO_BACKEND_REDLAND_WORLD_H
#define SOPRANO_BACKEND_REDLAND_WORLD_H

#include <redland.h>

#include "error.h"

#include <QtCore/QStringList>


namespace Soprano {

    class Node;
    class Statement;

    namespace Redland {

        class RedlandModel;

        class World : public Soprano::Error::ErrorCache
        {
        public:
            World();
            ~World();

            librdf_world* worldPtr() const;

            // make error methods public
            void setError( const Error::Error& e ) const { ErrorCache::setError( e ); }
            void clearError() const { ErrorCache::clearError(); }

            inline Error::Error lastError() const { return ErrorCache::lastError(); }

            inline Error::Error lastError( const Error::Error& fallBackError ) const {
                if( ErrorCache::lastError() ) {
                    return ErrorCache::lastError();
                }
                else {
                    return fallBackError;
                }
            }

            librdf_node *createNode( const Node &node );
            librdf_statement *createStatement( const Statement &statement );

            Soprano::Node createNode( librdf_node *node );
            Soprano::Statement createStatement( librdf_statement *st );

            /**
             * Calls librdf_free_node if \a node is not null.
             * The only reason for this method are the stupid asserts that clutter stderr.
             */
            void freeNode( librdf_node* node );

            /**
             * Calls librdf_free_statement if \a statement is not null.
             * The only reason for this method are the stupid asserts that clutter stderr.
             */
            void freeStatement( librdf_statement* statement );

        private:
            librdf_world * m_world;
        };
    }
}

#endif // SOPRANO_BACKEND_REDLAND_WORLD_H
