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

#include "clientnodeiteratorbackend.h"
#include "clientconnection.h"
#include "clientmodelbase.h"

#include "node.h"


Soprano::Client::ClientNodeIteratorBackend::ClientNodeIteratorBackend( int itId, const ClientModelBase* client )
    : ClientIteratorBase( itId, client)
{
}


Soprano::Client::ClientNodeIteratorBackend::~ClientNodeIteratorBackend()
{
    close();
}


bool Soprano::Client::ClientNodeIteratorBackend::next()
{
    if ( modelBase() ) {
        bool r = modelBase()->connection()->iteratorNext( iteratorId() );
        setError( modelBase()->connection()->lastError() );
        return r;
    }
    else {
        setError( "Connection to server closed." );
        return false;
    }
}


Soprano::Node Soprano::Client::ClientNodeIteratorBackend::current() const
{
    if ( modelBase() ) {
        Node s = modelBase()->connection()->nodeIteratorCurrent( iteratorId() );
        setError( modelBase()->connection()->lastError() );
        return s;
    }
    else {
        setError( "Connection to server closed." );
        return Node();
    }
}


void Soprano::Client::ClientNodeIteratorBackend::close()
{
    closeIterator();
}
