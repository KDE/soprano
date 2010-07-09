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

#include "plugin.h"
#include "soprano_export.h"
#include "sopranotypes.h"
#include "error.h"

#include <QtCore/QObject>

class QTextStream;
class QUrl;


namespace Soprano
{
    class StatementIterator;

    /**
     * \class Parser parser.h Soprano/Parser
     *
     * \brief Soprano::Parser defines the interface for a Soprano RDF parser plugin.
     *
     * Each parser plugin may support multiple RDF serializations (supportedSerializations()).
     *
     *
     * \section Usage
     *
     * Using a Parser is straightforward. One starts by getting a plugin that supports the requested
     * RDF data serialization:
     *
     * \code
     * Soprano::Parser* p = Soprano::PluginManager::instance()->discoverParserForSerialization( Soprano::SerializationRdfXml );
     * \endcode
     *
     * Then parsing RDF data is done in a single method call resulting in a StatementIterator over
     * the resulting graph (since parsers may support multiple serializations one always needs to
     * provide the serialization type unless a parser plugin support autodetection).
     *
     * \code
     * Soprano::StatementIterator it = p->parseFile( "myrdffile.rdf", Soprano::SerializationRdfXml );
     * \endcode
     *
     * \sa \ref soprano_writing_plugins
     *
     * \author Daniele Galdi <daniele.galdi@gmail.com><br>Sebastian Trueg <trueg@kde.org>
     */
    class SOPRANO_EXPORT Parser : public Plugin, public Error::ErrorCache
    {
    public:
        virtual ~Parser();

        /**
         * The serialiazation types supported by this parser.
         * \return A combination of Soprano::RdfSerialization types. If
         * the list contains Soprano::SerializationUser the parser 
         * supports additional RDF serializations not
         * officially supported by %Soprano.
         */
        virtual RdfSerializations supportedSerializations() const = 0;

        /**
         * A parser can support additional RDF serializations that are not defined in Soprano::RdfSerialization.
         * In that case supportedSerializations() has to include Soprano::SerializationUser.
         *
         * The default implementation returns an empty list.
         *
         * \return A list of supported user RDF serializations.
         */
        virtual QStringList supportedUserSerializations() const;

        /**
         * Check if a plugin supports a specific serialization.
         *
         * \param s The requested serialization.
         * \param userSerialization If serialization is set to Soprano::SerializationUser this parameter specifies the
         *       requested serialization. It allows the extension of the %Soprano Parser interface with new
         *       RDF serializations that are not officially supported by %Soprano.
         *
         * \return \p true if the parser is able to parse RDF data encoded
         * in serialization s, \p false otherwise.
         */
        bool supportsSerialization( RdfSerialization s, const QString& userSerialization = QString() ) const;

        /**
         * Parse an RDF model which has been serialized in a file,
         * using the supplied baseURI to resolve any relative URI references.
         *
         * The default implementation simply calls parseStream() on an opened
         * QFile instance.
         *
         * \param filename The name (path) of the file to parse
         * \param baseUri The base URI to be used for relative references.
         * \param serialization The serialization used in the file.
         * \param userSerialization If serialization is set to Soprano::SerializationUser this parameter specifies the
         *       serialization to use. It allows the extension of the %Soprano Parser interface with new
         *       RDF serializations that are not officially supported by %Soprano.
         *
         * \return An iterator that iterates over the result statements.
         */
        virtual StatementIterator parseFile( const QString& filename, const QUrl& baseUri, RdfSerialization serialization, const QString& userSerialization = QString() ) const;

        /**
         * Parse an RDF model which has been serialized into a string,
         * using the supplied baseURI to resolve any relative URI references.
         *
         * The default implementation simply calls parseStream().
         *
         * \param data The serialized RDF string.
         * \param baseUri The base URI to be used for relative references.
         * \param serialization The serialization used for the string data.
         * \param userSerialization If serialization is set to Soprano::SerializationUser this parameter specifies the
         *       serialization to use. It allows the extension of the %Soprano Parser interface with new
         *       RDF serializations that are not officially supported by %Soprano.
         *
         * \return An iterator that iterates over the result statements.
         */
        virtual StatementIterator parseString( const QString& data, const QUrl& baseUri, RdfSerialization serialization, const QString& userSerialization = QString() ) const;

        /**
         * Read a serialized RDF model from a test stream,
         * using the supplied baseURI to resolve any relative URI references.
         *
         * \param stream The text stream to read the serialized RDF data from.
         * \param baseUri The base URI to be used for relative references.
         * \param serialization The serialization used for the string data from the stream.
         * \param userSerialization If serialization is set to Soprano::SerializationUser this parameter specifies the
         *       serialization to use. It allows the extension of the %Soprano Parser interface with new
         *       RDF serializations that are not officially supported by %Soprano.
         *
         * \return An iterator that iterates over the result statements.
         */ 
        virtual StatementIterator parseStream( QTextStream& stream, const QUrl& baseUri, RdfSerialization serialization, const QString& userSerialization = QString() ) const = 0;

    protected:
        Parser( const QString& name );

    private:
        class Private;
        Private* const d;
    };
}

Q_DECLARE_INTERFACE(Soprano::Parser, "org.soprano.plugins.Parser/1.0")

#endif // SOPRANO_PARSER_H

