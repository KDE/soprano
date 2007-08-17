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

#include "queryserializer.h"
#include "query.h"

#include <QtCore/QStringList>


class Soprano::Query::Serializer::Private
{
};


Soprano::Query::Serializer::Serializer( const QString& name )
    : Plugin( name ),
      d( new Private() )
{
}


Soprano::Query::Serializer::~Serializer()
{
    delete d;
}


bool Soprano::Query::Serializer::supportsQueryLanguage( QueryLanguage lang, const QString& userQueryLanguage ) const
{
    if ( lang == QUERY_LANGUAGE_USER ) {
        return supportedUserQueryLanguages().contains( userQueryLanguage );
    }
    else {
        return supportedQueryLanguages() & lang;
    }
}


QStringList Soprano::Query::Serializer::supportedUserQueryLanguages() const
{
    return QStringList();
}
