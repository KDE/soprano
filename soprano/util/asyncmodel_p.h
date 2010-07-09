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

#ifndef _SOPRANO_ASYNC_MODEL_PRIVATE_H_
#define _SOPRANO_ASYNC_MODEL_PRIVATE_H_

#include "asyncmodel.h"
#include "asynccommand.h"
#include "iteratorbackend.h"

#include <QtCore/QLinkedList>


namespace Soprano {
    namespace Util {

        class AsyncIteratorHandle;

        class AsyncModelPrivate
        {
        public:
            AsyncModelPrivate( AsyncModel* parent );
            ~AsyncModelPrivate();

            AsyncModel::AsyncModelMode mode;

            QLinkedList<Command*> commandQueue;

            // only used for single threaded mode
            QList<AsyncIteratorHandle*> openIterators;

            void addIterator( AsyncIteratorHandle* );
            void removeIterator( AsyncIteratorHandle* );
            void enqueueCommand( Command* );

            void _s_executeNextCommand();

        private:
            AsyncModel* m_model;
        };
    }
}

#endif
