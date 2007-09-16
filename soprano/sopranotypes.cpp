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

#include "sopranotypes.h"

#include <QtCore/QString>


QString Soprano::serializationMimeType( RdfSerialization serialization )
{
    switch( serialization ) {
    case SERIALIZATION_RDF_XML:
        return QString::fromLatin1( "application/rdf+xml" );
    case SERIALIZATION_N3:
        return QString::fromLatin1( "text/rdf+n3" );
    case SERIALIZATION_N_TRIPLES:
        return QString::fromLatin1( "text/plain" ); // FIXME: is this correct?
    case SERIALIZATION_TURTLE:
        return QString::fromLatin1( "application/x-turtle" );
    case SERIALIZATION_TRIG:
        return QString::fromLatin1( "application/x-trig" );
    default:
        return QString();
    }
}


Soprano::RdfSerialization Soprano::mimeTypeToSerialization( const QString& mimetype )
{
    if ( mimetype == "application/rdf+xml" ||
         mimetype == "text/rdf" ) {
        return SERIALIZATION_RDF_XML;
    }
    else if ( mimetype == "application/rdf+n3" ||
              mimetype == "text/rdf+n3" ||
              mimetype == "text/n3" ) {
        return SERIALIZATION_N3;
    }
    // FIXME: what about text/plain?
    else if ( mimetype == "application/n-triples" ) {
        return SERIALIZATION_N_TRIPLES;
    }
    else if ( mimetype == "application/x-turtle" ||
              mimetype == "application/turtle") {
        return SERIALIZATION_TURTLE;
    }
    else if ( mimetype == "application/x-trig" ||
              mimetype == "application/trig") {
        return SERIALIZATION_TRIG;
    }
    return SERIALIZATION_UNKNOWN;
}


QString Soprano::Query::queryLanguageToString( QueryLanguage lang, const QString& userQueryLanguage )
{
    switch( lang ) {
    case QUERY_LANGUAGE_NONE:
        return QLatin1String( "none" );
    case QUERY_LANGUAGE_SPARQL:
        return QLatin1String( "SPARQL" );
    case QUERY_LANGUAGE_RDQL:
        return QLatin1String( "RDQL" );
    case QUERY_LANGUAGE_SERQL:
        return QLatin1String( "SERQL" );
    default:
        return userQueryLanguage;
    }
}
