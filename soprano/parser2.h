/* This file is part of Soprano
 *
 * Copyright (C) 2006 Daniele Galdi <daniele.galdi@gmail.com>
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

#ifndef SOPRANO_PARSER2_H
#define SOPRANO_PARSER2_H

#include "plugin.h"
#include "soprano_export.h"
#include "sopranotypes.h"
#include "error.h"

#include <QtCore/QObject>

class QIODevice;
class QUrl;


namespace Soprano
{
    class StatementIterator;

    /**
     * \class Parser2 parser2.h Soprano/Parser2
     *
     * \brief Soprano::Parser2 defines the interface for a %Soprano RDF parser plugin.
     *
     * In %Soprano 2.2 Parser has been deprecated in favor of Parser2. Parser is kept around
     * for binary compatibility.
     *
     * Each parser plugin may support multiple RDF serializations (supportedSerializations()).
     *
     * \section Usage
     *
     * Using a Parser2 is straightforward. One starts by getting a plugin that supports the requested
     * RDF data serialization:
     *
     * \code
     * Soprano::Parser2* p = Soprano::PluginManager::instance()->discoverParser2ForSerialization( Soprano::SerializationRdfXml );
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
     * \warning Be aware that parsing may continue in the returned StatementIterator when calling StatementIterator::next().
     * Thus, the QIODevice passed to parse() needs to stay valid until the iterator is closed.
     *
     * \sa \ref soprano_writing_plugins
     *
     * \author Sebastian Trueg <trueg@kde.org>
     *
     * \since 2.2
     */
    class SOPRANO_EXPORT Parser2 : public Plugin, public Error::ErrorCache
    {
    public:
        /**
         * Destructor
         */
        virtual ~Parser2();

        /**
         * The serialiazation types supported by this parser.
         * \return A combination of Soprano::RdfSerialization types. If
         * the list contains Soprano::SerializationUser the parser 
         * supports additional RDF serialiazations not
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
         *       requested serialization. It allows the extension of the %Soprano Parser2 interface with new
         *       RDF serializations that are not officially supported by %Soprano.
         *
         * \return \p true if the parser is able to parse RDF data encoded
         * in serialization s, \p false otherwise.
         */
        bool supportsSerialization( RdfSerialization s, const QString& userSerialization = QString() ) const;

        /**
         * Read a serialized RDF model from an io device,
         * using the supplied baseURI to resolve any relative URI references.
         *
         * \param device The device to read the serialized RDF data from. Be aware that parser implementations may
         *               finish the parsing inside the returned iterator. Thus, the device needs to stay valid until
         *               the iterator is closed.
         * \param baseUri The base URI to be used for relative references.
         * \param serialization The serialization used for the string data from the stream.
         * \param userSerialization If serialization is set to Soprano::SerializationUser this parameter specifies the
         *       serialization to use. It allows the extension of the %Soprano Parser2 interface with new
         *       RDF serializations that are not officially supported by %Soprano.
         *
         * \return An iterator that iterates over the result statements. An invalid iterator is returned on error. 
         * See Error::ErrorCache::lastError() for details in that case.
         */ 
        virtual StatementIterator parse( QIODevice* device, const QUrl& baseUri, RdfSerialization serialization, const QString& userSerialization = QString() ) const = 0;

        /**
         * Parse an RDF model which has been serialized in a file,
         * using the supplied baseURI to resolve any relative URI references.
         *
         * The default implementation calls parse() on a QFile.
         *
         * \param filename The name (path) of the file to parse
         * \param baseUri The base URI to be used for relative references.
         * \param serialization The serialization used in the file.
         * \param userSerialization If serialization is set to Soprano::SerializationUser this parameter specifies the
         *       serialization to use. It allows the extension of the %Soprano Parser2 interface with new
         *       RDF serializations that are not officially supported by %Soprano.
         *
         * \return An iterator that iterates over the result statements. An invalid iterator is returned on error. 
         * See Error::ErrorCache::lastError() for details in that case.
         */
        virtual StatementIterator parseFile( const QString& filename, const QUrl& baseUri, RdfSerialization serialization, const QString& userSerialization = QString() ) const;

        /**
         * Parse an RDF model which has been serialized into a string,
         * using the supplied baseURI to resolve any relative URI references.
         *
         * The default implementation calls parse() on a QBuffer.
         *
         * \param data The serialized RDF data.
         * \param baseUri The base URI to be used for relative references.
         * \param serialization The serialization used for the string data.
         * \param userSerialization If serialization is set to Soprano::SerializationUser this parameter specifies the
         *       serialization to use. It allows the extension of the %Soprano Parser2 interface with new
         *       RDF serializations that are not officially supported by %Soprano.
         *
         * \return An iterator that iterates over the result statements. An invalid iterator is returned on error. 
         * See Error::ErrorCache::lastError() for details in that case.
         */
        virtual StatementIterator parseString( const QByteArray& data, const QUrl& baseUri, RdfSerialization serialization, const QString& userSerialization = QString() ) const;

    protected:
        /**
         * Constructor.
         * \sa Plugin(const QString&)
         */
        Parser2( const QString& name );

    private:
        class Private;
        Private* const d;
    };
}

Q_DECLARE_INTERFACE(Soprano::Parser2, "org.soprano.plugins.Parser/2.0")

#endif // SOPRANO_PARSER2_H

