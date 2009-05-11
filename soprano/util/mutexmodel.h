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

#ifndef _SOPRANO_MUTEX_MODEL_H_
#define _SOPRANO_MUTEX_MODEL_H_

#include "filtermodel.h"
#include "soprano_export.h"

namespace Soprano {
    namespace Util {
        class MutexIteratorBase;

        /**
         * \class MutexModel mutexmodel.h Soprano/Util/MutexModel
         *
         * \brief Protect a Model for multiple calls in multi-threaded
         * applications.
         *
         * In most cases one does not need to use this FilterModel since Models
         * created by Soprano::createModel and Soprano::Backend::createModel
         * are thread-safe. It may, however, come in handy for custom models.
         *
         * \author Sebastian Trueg <trueg@kde.org>
         */
        class SOPRANO_EXPORT MutexModel : public FilterModel
        {
            Q_OBJECT

        public:
            enum ProtectionMode {
                /**
                 * In PlainMultiThreading mode only one action can be performed
                 * at the same time. Calls block until the previous call is done.
                 *
                 * \warning In this mode nesting iterators will result in a deadlock.
                 */
                PlainMultiThreading,

                /**
                 * In ReadWriteMultiThreading mode multiple read operations
                 * can be performed at the same time but only one write 
                 * operation.
                 *
                 * In general write operations take precedence over read 
                 * operations when locking. However, the same thread can perform
                 * nested read locks an arbitrary number of times.
                 */
                ReadWriteMultiThreading,
        
                /**
                 * \deprecated This was a buggy mode which was impossible to fix.
                 * Use Soprano::Util::AsyncModel instead.
                 */
                ReadWriteSingleThreading
            };

            /**
             * Create a new MutexModel.
             *
             * \param mode The mode to use.
             * \param parent The parent Model to forward
             *        the actual calls to.
             */
            MutexModel( ProtectionMode mode, Model* parent = 0 );

            /**
             * Destructor.
             */
            ~MutexModel();

            Error::ErrorCode addStatement( const Statement &statement );
            Error::ErrorCode removeStatement( const Statement &statement );
            Error::ErrorCode removeAllStatements( const Statement &statement );
            StatementIterator listStatements( const Statement &partial ) const;
            NodeIterator listContexts() const;
            QueryResultIterator executeQuery( const QString& query, 
                                              Query::QueryLanguage language,
                                              const QString& userQueryLanguage = QString() ) const;
            bool containsStatement( const Statement &statement ) const;
            bool containsAnyStatement( const Statement &statement ) const;
            bool isEmpty() const;
            int statementCount() const;

            using FilterModel::addStatement;
            using FilterModel::removeStatement;
            using FilterModel::removeAllStatements;
            using FilterModel::listStatements;
            using FilterModel::containsStatement;
            using FilterModel::containsAnyStatement;

        private:
            void removeIterator( MutexIteratorBase* it ) const;

            class Private;
            Private* const d;

            friend class MutexIteratorBase;
        };
    }
}

#endif
