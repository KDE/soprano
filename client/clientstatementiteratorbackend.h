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

#ifndef _SOPRANO_SERVER_CLIENT_STATEMENT_ITERATOR_H_
#define _SOPRANO_SERVER_CLIENT_STATEMENT_ITERATOR_H_

#include "iteratorbackend.h"

#include <QtCore/QPointer>

namespace Soprano 
{
    class Statement;

    namespace Client {

        class ClientModel;

        class ClientStatementIteratorBackend: public Soprano::IteratorBackend<Statement>
        {
        public:
            ClientStatementIteratorBackend( int itId, ClientModel* model );
            ~ClientStatementIteratorBackend();

            bool next();
            Soprano::Statement current() const;
            void close();

        private:
            int m_iteratorId;
            QPointer<ClientModel> m_model;
        };
    }
}

#endif

