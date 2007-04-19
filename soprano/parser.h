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

#ifndef SOPRANO_PARSER_H
#define SOPRANO_PARSER_H

#include <soprano/soprano_export.h>

class QTextStream;
class QString;
class QUrl;


namespace Soprano
{
    class Model;

    /**
     * Different types of RDF serialization.
     */
    enum RdfSerialization {
	AUTO_DETECTION = 0, /**< The parser is supposed to auto detect the serialization type. Might not be reliable. */
	RDF_XML,            /**< Standard RDF/XML serialization */
	N_TRIPLES,          /**< N-Triples as defined by W3: http://www.w3.org/TR/rdf-testcases/#ntriples */
	TURTLE,             /**< Turtle - Terse RDF Triple Language: http://www.dajobe.org/2004/01/turtle/ */
	TRIG                /**< TriG - Turtle + Named Graphs: http://sites.wiwiss.fu-berlin.de/suhl/bizer/TriG/ */
    };

    /**
     * \return The mimetype of serialization or an empty string is serialization is AUTO_DETECTION
     */
    QString serializationMimeType( RdfSerialization serialization );


    class SOPRANO_EXPORT Parser
	{
	public:
	    virtual ~Parser();

	    /**
	     * \deprecated
	     * Calls parseFile with RDF_XML serialization
	     */
	    Model* parse( const QUrl& url ) const;

	    /**
	     * Parse an RDF model which has been serialized in a file,
	     * using the supplied baseURI to resolve any relative URI references.
	     *
	     * The default implementation simply calls parseStream on an opened
	     * QFile instance.
	     *
	     * \param filename The name (path) of the file to parse
	     * \param baseUri The base URI to be used for relative references.
	     * \param serialization The serialization used in the file.
	     */
	    virtual Model* parseFile( const QString& filename, const QUrl& baseUri, RdfSerialization serialization = AUTO_DETECTION ) const;

	    /**
	     * Parse an RDF model which has been serialized into a string,
	     * using the supplied baseURI to resolve any relative URI references.
	     *
	     * The default implementation simply calls parseStream.
	     *
	     * \param data The serialized RDF string.
	     * \param baseUri The base URI to be used for relative references.
	     * \param serialization The serialization used for the string data.
	     */
	    virtual Model* parseString( const QString& data, const QUrl& baseUri, RdfSerialization serialization = AUTO_DETECTION ) const;

	    /**
	     * Read a serialized RDF model from a test stream,
	     * using the supplied baseURI to resolve any relative URI references.
	     *
	     * \param stream The text stream to read the serialized RDF data from.
	     * \param baseUri The base URI to be used for relative references.
	     * \param serialization The serialization used for the string data from the stream.
	     */ 
	    virtual Model* parseStream( QTextStream* stream, const QUrl& baseUri, RdfSerialization serialization = AUTO_DETECTION ) const = 0;

	protected:
	    Parser();
	};

}

#endif // SOPRANO_PARSER_H

