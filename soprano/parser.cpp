/* This file is part of Soprano
 *
 * Copyright (C) 2006 Daniele Galdi <daniele.galdi@gmail.com>
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

#include <QtCore/QTextStream>
#include <QtCore/QFile>
#include <QtCore/QUrl>
#include <QtCore/QDebug>


using namespace Soprano;

Parser::Parser()
{
}


Parser::~Parser()
{
}


Model* Parser::parse( const QUrl& url ) const
{
    return parseFile( url.toLocalFile(), url, RDF_XML );
}


Model* Parser::parseFile( const QString& filename, const QUrl& baseUri, RdfSerialization serialization ) const
{
    QFile f( filename );
    if ( f.open( QIODevice::ReadOnly | QIODevice::Text ) ) {
        QTextStream s( &f );
        return parseStream( &s, baseUri, serialization );
    }
    else {
        qDebug() << "(Soprano::Parser) unable to open file " << filename;
        return 0;
    }
}


Model* Parser::parseString( const QString& data, const QUrl& baseUri, RdfSerialization serialization ) const
{
    QString buffer( data );
    QTextStream s( &buffer, QIODevice::ReadOnly );
    return parseStream( &s, baseUri, serialization );
}


QString Soprano::serializationMimeType( RdfSerialization serialization )
{
    switch( serialization ) {
    case RDF_XML:
        return QString::fromLatin1( "application/rdf+xml" );
    case N_TRIPLES:
        return QString::fromLatin1( "text/plain" );
    case TURTLE:
        return QString::fromLatin1( "application/x-turtle" );
    case TRIG:
        return QString::fromLatin1( "application/x-trig" );
    default:
        return QString();
    }
}
