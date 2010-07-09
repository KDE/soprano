/*
 * This file is part of Soprano Project
 *
 * Copyright (C) 2009 Sebastian Trueg <trueg@kde.org>
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

#ifndef _SOPRANO_VIRTUOSO_CONFIGURATOR_H_
#define _SOPRANO_VIRTUOSO_CONFIGURATOR_H_

#include "backend.h"

#include <QtCore/QMap>

namespace Soprano {
    namespace ODBC {
        class Connection;
    }

    namespace Virtuoso {
        class DatabaseConfigurator
        {
        public:
            DatabaseConfigurator( ODBC::Connection* connection );
            ~DatabaseConfigurator();

            /**
             * Supported configuration settings:
             * -----------------------
             * indexes       - comma separated list of indexes that have been created (example: "spog,posg,opsg,gspo")
             * fulltextindex - config state, one of
             *                 "none" (no fulltext indexing),
             *                 "sync" (string literals will be indexed syncroneous - might slow things down)
             *                 "N" (update all N minutes)
             * -----------------------
             */
            bool configureServer( const BackendSettings& settings );

        private:
            /**
             * \return The stored value
             */
            bool updateIndexes( const QString& indexes );

            /**
             * \return The stored value
             */
            bool updateFulltextIndexState( const QString& state );

            /**
             * \param enable If true the rule will be added if not found, otherwise it will be removed.
             */
            bool updateFulltextIndexRules( bool enable );

            /**
             * \return A list of indexes (in the form OSPG) which are currently configured on the server
             */
            QStringList configuredIndexes();

            ODBC::Connection* m_connection;
        };
    }
}

#endif
