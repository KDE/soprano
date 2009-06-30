/* This file is part of Soprano
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

#include "serializer.h"
#include "statementiterator.h"

#include <QtCore/QTextStream>
#include <QtCore/QStringList>
#include <QtCore/QDebug>


class Soprano::Serializer::Private
{
public:
    QHash<QString, QUrl> prefixes;
};


Soprano::Serializer::Serializer( const QString& name )
    : Plugin( name ),
      d( new Private() )
{
}


Soprano::Serializer::~Serializer()
{
    delete d;
}


bool Soprano::Serializer::supportsSerialization( RdfSerialization s, const QString& userSerialization ) const
{
    if ( s == SerializationUser || s == SerializationUnknown ) {
        return supportedUserSerializations().contains( userSerialization );
    }
    else {
        return supportedSerializations() & s;
    }
}


QStringList Soprano::Serializer::supportedUserSerializations() const
{
    return QStringList();
}


void Soprano::Serializer::addPrefix( const QString& qname, const QUrl& uri ) const
{
    d->prefixes.insert( qname, uri );
}


void Soprano::Serializer::clearPrefixes() const
{
    d->prefixes.clear();
}


QHash<QString, QUrl> Soprano::Serializer::prefixes() const
{
    return d->prefixes;
}
