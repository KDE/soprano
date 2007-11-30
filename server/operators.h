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

#ifndef _SOPRANO_SERVER_OPERATORS_H_
#define _SOPRANO_SERVER_OPERATORS_H_

#include <QtCore/QDataStream>

#include <soprano/error.h>

namespace Soprano {
    class Node;
    class LiteralValue;
    class Statement;
    class BackendSetting;
    class BindingSet;
    namespace Error {
        class Locator;
    }
}

QDataStream& operator<<( QDataStream& stream, const Soprano::Error::Locator& loc );
QDataStream& operator<<( QDataStream& stream, const Soprano::Error::Error& error );
QDataStream& operator<<( QDataStream& stream, const Soprano::Error::ErrorCode& error );
QDataStream& operator<<( QDataStream& stream, const Soprano::BackendSetting& setting );
QDataStream& operator<<( QDataStream& stream, const Soprano::LiteralValue& value );
QDataStream& operator<<( QDataStream& stream, const Soprano::Node& node );
QDataStream& operator<<( QDataStream& stream, const Soprano::Statement& statement );
QDataStream& operator<<( QDataStream& stream, const Soprano::BindingSet& set );

QDataStream& operator>>( QDataStream& stream, Soprano::Error::Locator& loc );
QDataStream& operator>>( QDataStream& stream, Soprano::Error::Error& error );
QDataStream& operator>>( QDataStream& stream, Soprano::Error::ErrorCode& error );
QDataStream& operator>>( QDataStream& stream, Soprano::BackendSetting& setting );
QDataStream& operator>>( QDataStream& stream, Soprano::LiteralValue& value );
QDataStream& operator>>( QDataStream& stream, Soprano::Node& node );
QDataStream& operator>>( QDataStream& stream, Soprano::Statement& statement );
QDataStream& operator>>( QDataStream& stream, Soprano::BindingSet& set );

#endif
