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

#ifndef _SOPRANO_MODEL_POOL_H_
#define _SOPRANO_MODEL_POOL_H_

#include <qglobal.h>

namespace Soprano {

    class Model;
    class Transaction;

    namespace Server {

        class ServerCore;

        /**
         * The ModelPool serves as a bridge between the ServerCore
         * and the ServerConnection. Its sole purpose is to provide
         * a unique mapping between Models and model ids for all
         * ServerConnection instances.
         *
         * This is necessary since one client can open several connections
         * in a multi-threaded environment and model ids, thus, need to be
         * unique.
         *
         * \author Sebastian Trueg <trueg@kde.org>
         */
        class ModelPool
        {
        public:
            ModelPool( ServerCore* );
            ~ModelPool();

            Model* modelById( quint32 modelId ) const;

            /**
             * Will create non-existing models.
             */
            Model* modelByName( const QString& name );

            /**
             * Will create non-existing models.
             */
            quint32 idForModelName( const QString& name );

            /**
             * Will only remove the model from the cache, not
             * delete it or remove it from the core.
             */
            void removeModel( const QString& name );

            /**
             * inserts a transaction into the pool.
             * Transactions are special models without
             * a name. They can be accessed throuh 
             * modelById and transactionById
             */
            quint32 insertTransaction( Transaction* t );

            Transaction* transactionById( quint32 id ) const;

            /**
             * \return false if no Transaction for id could be found.
             */
            bool deleteTransaction( quint32 id );

        private:
            class Private;
            Private* const d;
        };
    }
}

#endif
