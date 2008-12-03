/*
 * This file is part of Soprano Project
 *
 * Copyright (C) 2006 Duncan Mac-Vicar <duncan@kde.org>
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

#include "raptorparserlegacy.h"
#include "raptorparserwrapper.h"
#include "raptorparserstatementiteratorbackend.h"
#include "raptor-config.h"

#include "statement.h"
#include "statementiterator.h"

#include <QtCore/QUrl>
#include <QtCore/QtPlugin>
#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QTextStream>


Soprano::Raptor::ParserLegacy::ParserLegacy()
    : QObject(),
      Soprano::Parser( "raptor" )
{
}


Soprano::Raptor::ParserLegacy::~ParserLegacy()
{
}


Soprano::RdfSerializations Soprano::Raptor::ParserLegacy::supportedSerializations() const
{
    return(  SerializationRdfXml
             |SerializationNTriples
             |SerializationTurtle
#ifdef RAPTOR_HAVE_TRIG
             |SerializationTrig
#endif
        );
}


Soprano::StatementIterator Soprano::Raptor::ParserLegacy::parseFile( const QString& filename,
                                                                     const QUrl& baseUri,
                                                                     RdfSerialization serialization,
                                                                     const QString& userSerialization ) const
{
    clearError();

    ParserWrapper* parser = ParserWrapper::createParser( baseUri, serialization, userSerialization );
    if ( !parser ) {
        setError( QString( "Failed to create parser for serialization %1" ).arg( serializationMimeType( serialization, userSerialization ) ) );
        return StatementIterator();
    }

    QFile* f = new QFile( filename );
    if ( f->open( QIODevice::ReadOnly ) ) {
        // give control and ownership of all the raptor stuff to the iterator
        return new ParserStatementIteratorBackend( f, true, parser );
    }
    else {
        delete f;
        setError( QString( "Could not open file %1 for reading." ).arg( filename ) );
        return StatementIterator();
    }

}


Soprano::StatementIterator Soprano::Raptor::ParserLegacy::parseString( const QString& data,
                                                                       const QUrl& baseUri,
                                                                       RdfSerialization serialization,
                                                                       const QString& userSerialization ) const
{
    clearError();

    ParserWrapper* parser = ParserWrapper::createParser( baseUri, serialization, userSerialization );
    if ( !parser ) {
        setError( QString( "Failed to create parser for serialization %1" ).arg( serializationMimeType( serialization, userSerialization ) ) );
        return StatementIterator();
    }

    // give control and ownership of all the raptor stuff to the iterator
    return new ParserStatementIteratorBackend( data, parser );
}


Soprano::StatementIterator
Soprano::Raptor::ParserLegacy::parseStream( QTextStream& stream,
                                            const QUrl& baseUri,
                                            RdfSerialization serialization,
                                            const QString& userSerialization ) const
{
    clearError();

    ParserWrapper* parser = ParserWrapper::createParser( baseUri, serialization, userSerialization );
    if ( !parser ) {
        setError( QString( "Failed to create parser for serialization %1" ).arg( serializationMimeType( serialization, userSerialization ) ) );
        return StatementIterator();
    }

    // give control and ownership of all the raptor stuff to the iterator
    return new ParserStatementIteratorBackend( &stream, false, parser );
}


Q_EXPORT_PLUGIN2(soprano_raptorparserlegacy, Soprano::Raptor::ParserLegacy)

#include "raptorparserlegacy.moc"
