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

#include "readonlymodel.h"

namespace {
    Soprano::Error::Error createPermissionDeniedError() {
        return Soprano::Error::Error( "Soprano::Util::ReadOnlyModel - no write support", Soprano::Error::ErrorPermissionDenied );
    }
}


Soprano::Util::ReadOnlyModel::ReadOnlyModel( Model* parentModel )
    : FilterModel( parentModel )
{
}


Soprano::Error::ErrorCode Soprano::Util::ReadOnlyModel::addStatement( const Statement& )
{
    setError( createPermissionDeniedError() );
    return Error::ErrorPermissionDenied;
}


Soprano::Error::ErrorCode Soprano::Util::ReadOnlyModel::removeStatement( const Statement& )
{
    setError( createPermissionDeniedError() );
    return Error::ErrorPermissionDenied;
}


Soprano::Error::ErrorCode Soprano::Util::ReadOnlyModel::removeAllStatements( const Statement& )
{
    setError( createPermissionDeniedError() );
    return Error::ErrorPermissionDenied;
}


Soprano::Node Soprano::Util::ReadOnlyModel::createBlankNode()
{
    setError( createPermissionDeniedError() );
    return Node();
}

#include "readonlymodel.moc"
