/*
 * This file is part of Soprano Project
 *
 * Copyright (C) 2006 Daniele Galdi <daniele.galdi@gmail.com>
 * Copyright (C) 2007-2009 Sebastian Trueg <trueg@kde.org>
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

#ifndef SOPRANO_BACKEND_REDLAND_MODEL_H
#define SOPRANO_BACKEND_REDLAND_MODEL_H

#define LIBRDF_MODEL_FEATURE_CONTEXTS "http://feature.librdf.org/model-contexts"

#include <QtCore/QtGlobal>
#include <QtCore/QTextStream>

#include <redland.h>

#include "storagemodel.h"
#include "statement.h"
#include "node.h"

namespace Soprano {

    class QueryResultIterator;
    class StatementIterator;

    namespace Redland {

        class World;
        class RedlandStatementIterator;
        class RedlandQueryResult;
        class NodeIteratorBackend;

        class RedlandModel: public Soprano::StorageModel
        {
        public:
            RedlandModel( const Backend*, librdf_model *model, librdf_storage *storage, World* world );
            ~RedlandModel();

            World* world() const;

            librdf_model *redlandModel() const;

            Error::ErrorCode addStatement( const Statement &statement );

            virtual NodeIterator listContexts() const;

            bool containsStatement( const Statement &statement ) const;
            bool containsAnyStatement( const Statement &statement ) const;

            Soprano::QueryResultIterator executeQuery( const QString &query, Query::QueryLanguage language, const QString& userQueryLanguage = QString() ) const;

            Soprano::StatementIterator listStatements( const Statement &partial ) const;

            Error::ErrorCode removeStatement( const Statement &statement );

            Error::ErrorCode removeAllStatements( const Statement &statement );

            int statementCount() const;

            Node createBlankNode();

        private:
            class Private;
            Private *d;

            void removeIterator( RedlandStatementIterator* it ) const;
            void removeIterator( NodeIteratorBackend* it ) const;
            void removeQueryResult( RedlandQueryResult* r ) const;

            Error::ErrorCode removeOneStatement( const Statement &statement );

            friend class RedlandStatementIterator;
            friend class RedlandQueryResult;
            friend class NodeIteratorBackend;
        };

    }
}

#endif // SOPRANO_BACKEND_REDLAND_MODEL_H

