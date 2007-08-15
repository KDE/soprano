/*
 * This file is part of Soprano Project
 *
 * Copyright (C) 2006 Duncan Mac-Vicar <duncan@kde.org>
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

#include "raptorparser.h"
#include "../../backends/redland/redlandworld.h"
#include "../../backends/redland/redlandstatementiterator.h"

#include <soprano/statementiterator.h>

#include <redland.h>

#include <QtCore/QUrl>
#include <QtCore/QtPlugin>


Q_EXPORT_PLUGIN2(soprano_raptorparser, Soprano::Raptor::Parser)


Soprano::Raptor::Parser::Parser()
    : QObject(),
      Soprano::Parser( "raptor" )
{
}


Soprano::Raptor::Parser::~Parser()
{
}


Soprano::RdfSerializations Soprano::Raptor::Parser::supportedSerializations() const
{
    return RDF_XML|N_TRIPLES|TURTLE;
}


Soprano::StatementIterator Soprano::Raptor::Parser::parseFile( const QString& filename,
                                                               const QUrl& baseUri,
                                                               RdfSerialization serialization ) const
{
    QUrl uri( QUrl::fromLocalFile( filename ) );
    if ( uri.scheme().isEmpty() ) {
        // we need to help the stupid librdf file url handling
        uri.setScheme("file");
    }

    librdf_uri* redlandUri = librdf_new_uri( Redland::World::self()->worldPtr(),
                                             (unsigned char *) uri.toString().toLatin1().data() );
    if ( !redlandUri ) {
        return StatementIterator();
    }

    librdf_parser *parser = librdf_new_parser( Redland::World::self()->worldPtr(),
                                               0, // use all factories
                                               serializationMimeType( serialization ).toLatin1().data(),
                                               0 ); // what is the URI of the syntax used for?
    if ( !parser ) {
        librdf_free_uri( redlandUri );
        return StatementIterator();
    }

    librdf_uri* redlandBaseUri = 0;
    if ( !baseUri.toString().isEmpty() ) {
        redlandBaseUri = librdf_new_uri( Redland::World::self()->worldPtr(),
                                         (unsigned char *) baseUri.toString().toLatin1().data() );
    }

    librdf_stream* stream = librdf_parser_parse_as_stream( parser, redlandUri, redlandBaseUri );

    librdf_free_uri( redlandUri );
    librdf_free_parser( parser );

    if ( stream ) {
        return StatementIterator();
    }
    else {
        return new Redland::RedlandStatementIterator( 0, stream, Node() );
    }
}


Soprano::StatementIterator Soprano::Raptor::Parser::parseString( const QString& data,
                                                                 const QUrl& baseUri,
                                                                 RdfSerialization serialization ) const
{
    librdf_parser* parser = librdf_new_parser( Redland::World::self()->worldPtr(),
                                               0, // use all factories
                                               serializationMimeType( serialization ).toLatin1().data(),
                                               0 ); // what is the URI of the syntax used for?
    if ( !parser ) {
        return StatementIterator();
    }

    librdf_uri* redlandBaseUri = 0;
    if ( !baseUri.toString().isEmpty() ) {
        redlandBaseUri = librdf_new_uri( Redland::World::self()->worldPtr(),
                                         (unsigned char *) baseUri.toString().toLatin1().data() );
    }

    // FIXME: do we need to convert the string data into something else than UTF8 with some serialization
    librdf_stream* stream = librdf_parser_parse_string_as_stream( parser,
                                                                  ( const unsigned char* )data.toUtf8().data(),
                                                                  redlandBaseUri );
    librdf_free_parser( parser );

    if ( stream ) {
        return StatementIterator();
    }
    else {
        return new Redland::RedlandStatementIterator( 0, stream, Node() );
    }
}


Soprano::StatementIterator
Soprano::Raptor::Parser::parseStream( QTextStream* stream,
                                      const QUrl& baseUri,
                                      RdfSerialization serialization ) const
{
    return parseString( stream->readAll(), baseUri, serialization );
}

#include "raptorparser.moc"
