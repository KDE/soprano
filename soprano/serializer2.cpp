/* This file is part of Soprano
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

#include "serializer2.h"
#include "statementiterator.h"

#include <QtCore/QBuffer>
#include <QtCore/QFile>
#include <QtCore/QStringList>
#include <QtCore/QDebug>


class Soprano::Serializer2::Private
{
};


Soprano::Serializer2::Serializer2( const QString& name )
    : Plugin( name ),
      d( new Private() )
{
}


Soprano::Serializer2::~Serializer2()
{
    delete d;
}


bool Soprano::Serializer2::supportsSerialization( RdfSerialization s, const QString& userSerialization ) const
{
    if ( s == SerializationUser || s == SerializationUnknown ) {
        return supportedUserSerializations().contains( userSerialization );
    }
    else {
        return supportedSerializations() & s;
    }
}


QStringList Soprano::Serializer2::supportedUserSerializations() const
{
    return QStringList();
}


bool Soprano::Serializer2::serializeToFile( StatementIterator it, const QString& filename, RdfSerialization serialization, const QString& userSerialization ) const
{
    QFile f( filename );
    if( f.open( QIODevice::WriteOnly ) ) {
        clearError();
        return serialize( it, &f, serialization, userSerialization );
    }
    else {
        setError( QString("Unable to open file %1").arg( filename ), Error::ErrorInvalidArgument );
        return false;
    }
}


bool Soprano::Serializer2::serializeToString( StatementIterator it, QByteArray& string, RdfSerialization serialization, const QString& userSerialization ) const
{
    QBuffer buf( &string );
    return serialize( it, &buf, serialization, userSerialization );
}
