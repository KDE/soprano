/* This file is part of Soprano
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

#ifndef SOPRANO_SERIALIZER_H
#define SOPRANO_SERIALIZER_H

#include "plugin.h"
#include "soprano_export.h"
#include "sopranotypes.h"
#include "error.h"

#include <QtCore/QObject>
#include <QtCore/QHash>
#include <QtCore/QUrl>
#include <QtCore/QString>

class QTextStream;


namespace Soprano
{
    class StatementIterator;

    /**
     * \class Serializer serializer.h Soprano/Serializer
     *
     * \brief Soprano::Serializer defines the interface for a Soprano RDF serializer plugin.
     *
     * Each serializer plugin may support multiple RDF serializations (supportedSerializations()).
     *
     * \section Usage
     *
     * Using a Serializer is straightforward. One starts by getting a plugin that supports the requested
     * RDF data serialization:
     *
     * \code
     * Soprano::Serializer* s = Soprano::PluginManager::instance()->discoverSerializerForSerialization( Soprano::SerializationRdfXml );
     * \endcode
     *
     * Then serializing RDF data is done in a single method call which writes the serialized data to a QTextStream:
     *
     * \code
     * QTextStream stream( stdout );
     * s->serialize( model->listStatements(), stream, Soprano::SerializationRdfXml );
     * \endcode
     *
     * \sa \ref soprano_writing_plugins
     *
     * \author Sebastian Trueg <trueg@kde.org>
     */
    class SOPRANO_EXPORT Serializer : public Plugin, public Error::ErrorCache
    {
    public:
        virtual ~Serializer();

        /**
         * The serialiazation types supported by this serializer.
         * \return A combination of Soprano::RdfSerialization types. If
         * the list contains Soprano::SerializationUser the serializer
         * supports additional RDF serialiazations not
         * officially supported by %Soprano.
         */
        virtual RdfSerializations supportedSerializations() const = 0;

        /**
         * A serializer can support additional RDF serializations that are not defined in Soprano::RdfSerialization.
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
         *       requested serialization. It allows the extension of the %Soprano Serializer interface with new
         *       RDF serializations that are not officially supported by %Soprano.
         *
         * \return \p true if the serializer is able to parse RDF data encoded
         * in serialization s, \p false otherwise.
         */
        bool supportsSerialization( RdfSerialization s, const QString& userSerialization = QString() ) const;

        /**
         * Serialize a list of statements.
         *
         * \param it An iterator containing the statements to be serialized.
         * \param stream The stream the serialized data should be written to.
         * \param serialization The encoding to be used.
         * \param userSerialization If serialization is set to Soprano::SerializationUser this parameter specifies the
         *       serialization to use. It allows the extension of the %Soprano Serializer interface with new
         *       RDF serializations that are not officially supported by %Soprano.
         *
         * \return \p true if the %serialization was successful,  false otherwise.
         */
        virtual bool serialize( StatementIterator it, QTextStream& stream, RdfSerialization serialization, const QString& userSerialization = QString() ) const = 0;

        /**
         * Add a prefix to be used by the serializer.
         *
         * Be aware that serializer instances are reused. Thus, it is highly recommended to clear prefixes after using the Serializer.
         * A future version of %Soprano will have a revised %Serializer API which handles this issue more effective.
         *
         * Method is const for historical reasons.
         *
         * \since 2.3
         */
        void addPrefix( const QString& qname, const QUrl& uri ) const;

        /**
         * Clear all prefixes set via addPrefix.
         *
         * Method is const for historical reasons.
         *
         * \since 2.3
         */
        void clearPrefixes() const;

        /**
         * Retrieve all prefixes set via addPrefix.
         *
         * \return a QHash containing of the prefixe qnames and their URIs.
         *
         * \since 2.3
         */
        QHash<QString, QUrl> prefixes() const;

    protected:
        Serializer( const QString& name );

    private:
        class Private;
        Private* const d;
    };
}

Q_DECLARE_INTERFACE(Soprano::Serializer, "org.soprano.plugins.Serializer/1.0")

#endif

