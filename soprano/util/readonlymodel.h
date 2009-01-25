/* 
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2008 Sebastian Trueg <trueg@kde.org>
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

#ifndef _SOPRANO_READ_ONLY_MODEL_H_
#define _SOPRANO_READ_ONLY_MODEL_H_

#include "model.h"
#include "soprano_export.h"

namespace Soprano {
    namespace Util {
        /**
         * \class ReadOnlyModel readonlymodel.h Soprano/Util/ReadOnlyModel
         *
         * \brief Simple filter model preventing any write operations.
         *
         * Stacking a ReadOnlyModel on top of another model will prevent
         * any write operation to be performed. All calls to non-const methods
         * are terminated by Error::ErrorPermissionDenied.
         *
         * \author Sebastian Trueg <trueg@kde.org>
         *
         * \since 2.1
         */
        class SOPRANO_EXPORT ReadOnlyModel : public Model
        {
            Q_OBJECT

        public:
            /**
             * Create a new read-only-model
             * \param parentModel The parent Model.
             */
            ReadOnlyModel( Model* parentModel = 0 );

            /**
             * Destructor
             */
            ~ReadOnlyModel();
    
            /**
             * Set the parent Model.
             * \param model The Model that this filter will forward any commands to.
             */
            void setParentModel( Model* model );

            /**
             * Default implementation simply pipes the call through to the parent model.
             */
            StatementIterator listStatements( const Statement &partial ) const;

            /**
             * Simply pipes the call through to the parent model.
             */
            NodeIterator listContexts() const;

            /**
             * Simply pipes the call through to the parent model.
             */
            QueryResultIterator executeQuery( const QString& query, Query::QueryLanguage language, const QString& userQueryLanguage = QString() ) const;

            /**
             * Simply pipes the call through to the parent model.
             */
            bool containsStatement( const Statement &statement ) const;

            /**
             * Simply pipes the call through to the parent model.
             */
            bool containsAnyStatement( const Statement &statement ) const;

            /**
             * Simply pipes the call through to the parent model.
             */
            bool isEmpty() const;

            /**
             * Simply pipes the call through to the parent model.
             */
            int statementCount() const;

            /**
             * Will do nothing but set an Error::ErrorPermissionDenied error.
             */
            Error::ErrorCode addStatement( const Statement& statement );

            /**
             * Will do nothing but set an Error::ErrorPermissionDenied error.
             */
            Error::ErrorCode removeStatement( const Statement& statement );

            /**
             * Will do nothing but set an Error::ErrorPermissionDenied error.
             */
            Error::ErrorCode removeAllStatements( const Statement& statement );

            /**
             * Will do nothing but set an Error::ErrorPermissionDenied error.
             */
            Node createBlankNode();

            using Model::addStatement;
            using Model::removeStatement;
            using Model::removeAllStatements;
            using Model::listStatements;
            using Model::containsStatement;
            using Model::containsAnyStatement;

        private:
            class Private;
            Private* const d;
        };
    }
}

#endif
