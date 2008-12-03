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
#include "raptorparserwrapper.h"
#include "raptorparserstatementiteratorbackend.h"
#include "raptor-config.h"

#include "statement.h"
#include "statementiterator.h"

#include <QtCore/QUrl>
#include <QtCore/QtPlugin>
#include <QtCore/QDebug>


Soprano::Raptor::Parser::Parser()
    : QObject(),
      Soprano::Parser2( "raptor" )
{
}


Soprano::Raptor::Parser::~Parser()
{
}


Soprano::RdfSerializations Soprano::Raptor::Parser::supportedSerializations() const
{
    return(  SerializationRdfXml
             |SerializationNTriples
             |SerializationTurtle
#ifdef RAPTOR_HAVE_TRIG
             |SerializationTrig
#endif
        );
}


Soprano::StatementIterator Soprano::Raptor::Parser::parse( QIODevice* dev,
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
    return new ParserStatementIteratorBackend( dev, false, parser );
}

Q_EXPORT_PLUGIN2(soprano_raptorparser, Soprano::Raptor::Parser)

#include "raptorparser.moc"
