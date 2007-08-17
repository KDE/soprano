/* This file is part of Soprano
 *
 * Copyright (C) 2006 Daniele Galdi <daniele.galdi@gmail.com>
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

#include "parser.h"
#include "statementiterator.h"

#include <QtCore/QTextStream>
#include <QtCore/QFile>
#include <QtCore/QUrl>
#include <QtCore/QStringList>
#include <QtCore/QDebug>


class Soprano::Parser::Private
{
};


Soprano::Parser::Parser( const QString& name )
    : Plugin( name ),
      d( new Private() )
{
}


Soprano::Parser::~Parser()
{
    delete d;
}


Soprano::StatementIterator Soprano::Parser::parseFile( const QString& filename, const QUrl& baseUri, RdfSerialization serialization, const QString& userSerialization ) const
{
    QFile f( filename );
    if ( f.open( QIODevice::ReadOnly | QIODevice::Text ) ) {
        QTextStream s( &f );
        return parseStream( &s, baseUri, serialization, userSerialization );
    }
    else {
        qDebug() << "(Soprano::Parser) unable to open file " << filename;
        return 0;
    }
}


Soprano::StatementIterator Soprano::Parser::parseString( const QString& data, const QUrl& baseUri, RdfSerialization serialization, const QString& userSerialization ) const
{
    QString buffer( data );
    QTextStream s( &buffer, QIODevice::ReadOnly );
    return parseStream( &s, baseUri, serialization, userSerialization );
}


bool Soprano::Parser::supportsSerialization( RdfSerialization s, const QString& userSerialization ) const
{
    if ( s == SERIALIZATION_USER ) {
        return supportedUserSerializations().contains( userSerialization );
    }
    else {
        return supportedSerializations() & s;
    }
}


QStringList Soprano::Parser::supportedUserSerializations() const
{
    return QStringList();
}
