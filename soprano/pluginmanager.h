/*
 * This file is part of Soprano Project.
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

#ifndef _SOPRANO_PLUGIN_MANAGER_H_
#define _SOPRANO_PLUGIN_MANAGER_H_

#include "sopranotypes.h"
#include "sopranomacros.h"
#include "soprano_export.h"

#include <QtCore/QObject>
#include <QtCore/QStringList>


namespace Soprano
{
    class Backend;
    class Parser;
    class Parser2;
    class Serializer;
    class Serializer2;

    /**
     * \class PluginManager pluginmanager.h Soprano/PluginManager
     *
     * \brief The PluginManager loads and maintains all %Soprano plugins.
     *
     * Normally there is no need to use this class as all important methods have
     * global counterparts in the Soprano namespace.
     *
     * \sa \ref soprano_writing_plugins
     *
     * \author Sebastian Trueg <trueg@kde.org>
     */
    class SOPRANO_EXPORT PluginManager : public QObject
    {
        Q_OBJECT

    public:
        ~PluginManager();

        //@{
        /**
         * Check if a certain plugin is available without actually loading the plugin.
         *
         * \return \p true if the backend plugin specified by \a name could be
         * found, \p false otherwise.
         *
         * \warning Since %Soprano does not actually try to load the plugin there may
         * be rare cases in which this method returns \p true while actually loading
         * the plugin later on fails.
         *
         * \since 2.2
         */
        bool backendAvailable( const QString& name );

        /**
         * Find a backend plugin by its name.
         *
         * \return the backend specified by \a name or null if could not
         * be found.
         */
        const Backend* discoverBackendByName( const QString& name );

        /**
         * Find a backend plugin by its features.
         *
         * \param features The features that are requested, a combination of Soprano::BackendFeature flags.
         * \param userFeatures If features contain Backend::BackendFeatureUser this paramter states the additionally requested user features.
         *
         * \return a Backend that supports the features defined in \a features.
         */
        const Backend* discoverBackendByFeatures( BackendFeatures features, const QStringList& userFeatures = QStringList() );

        QList<const Backend*> allBackends();
        //@}

        //@{
        /**
         * Check if a certain plugin is available without actually loading the plugin.
         *
         * \return \p true if the parser plugin specified by \a name could be
         * found, \p false otherwise.
         *
         * \warning Since %Soprano does not actually try to load the plugin there may
         * be rare cases in which this method returns \p true while actually loading
         * the plugin later on fails.
         *
         * \since 2.2
         */
        bool parserAvailable( const QString& name );

        /**
         * Find a parser plugin by its name.
         *
         * \return the parser specified by \a name or null if could not
         * be found.
         *
         * \since 2.2
         */
        const Parser2* discoverParser2ByName( const QString& name );

        /**
         * Find a Parser instance that is able to parse RDF data serialized as
         * serialization.
         *
         * \param serialization The requested serialization.
         * \param userSerialization If serialization is set to Soprano::SerializationUser this parameter specifies the
         *       serialization to use. It allows the extension of the %Soprano Parser interface with new
         *       RDF serializations that are not officially supported by %Soprano.
         *
         * \return A Parser plugin that can parse RDF data encoded in the requested 
         * serialization or 0 if no such plugin could be found.
         *
         * \since 2.2
         */
        const Parser2* discoverParser2ForSerialization( RdfSerialization serialization, const QString& userSerialization = QString() );

        /**
         * Get a list of all parser plugins available
         *
         * \since 2.2
         */
        QList<const Parser2*> allParser2s();

        /**
         * \deprecated use discoverParser2ByName
         */
        SOPRANO_DEPRECATED const Parser* discoverParserByName( const QString& name );

        /**
         * \deprecated use discoverParser2ForSerialization
         */
        SOPRANO_DEPRECATED const Parser* discoverParserForSerialization( RdfSerialization serialization, const QString& userSerialization = QString() );

        /**
         * \deprecated use allParser2s
         */
        SOPRANO_DEPRECATED QList<const Parser*> allParsers();
        //@}

        //@{
        /**
         * Check if a certain plugin is available without actually loading the plugin.
         *
         * \return \p true if the serializer plugin specified by \a name could be
         * found, \p false otherwise.
         *
         * \warning Since %Soprano does not actually try to load the plugin there may
         * be rare cases in which this method returns \p true while actually loading
         * the plugin later on fails.
         *
         * \since 2.2
         */
        bool serializerAvailable( const QString& name );

        /**
         * Find a serializer plugin by its name.
         *
         * \return the serializer specified by \a name or null if could not
         * be found.
         *
         * \since 2.2
         */
        const Serializer2* discoverSerializer2ByName( const QString& name );

        /**
         * Find a Serializer2 instance that is able to encode RDF data using
         * serialization.
         *
         * \param serialization The requested serialization.
         * \param userSerialization If serialization is set to Soprano::SerializationUser this parameter specifies the
         *       serialization to use. It allows the extension of the %Soprano Serializer2 interface with new
         *       RDF serializations that are not officially supported by %Soprano.
         *
         * \return A Serializer2 plugin that can serialize RDF data encoded in the requested 
         * serialization or 0 if no such plugin could be found.
         *
         * \since 2.2
         */
        const Serializer2* discoverSerializer2ForSerialization( RdfSerialization serialization, const QString& userSerialization = QString() );

        /**
         * Get a list of all serializer plugins available
         *
         * \since 2.2
         */
        QList<const Serializer2*> allSerializer2s();

        /**
         * \deprecated use discoverSerializer2ByName
         */
        SOPRANO_DEPRECATED const Serializer* discoverSerializerByName( const QString& name );

        /**
         * \deprecated use discoverSerializer2ForSerialization
         */
        SOPRANO_DEPRECATED const Serializer* discoverSerializerForSerialization( RdfSerialization serialization, const QString& userSerialization = QString() );

        /**
         * \deprecated use allSerializer2s
         */
        SOPRANO_DEPRECATED QList<const Serializer*> allSerializers();
        //@}

        /** \cond query_api_disabled */

        //@{
        /**
         * Find a query parser plugin by its name.
         *
         * \return the query parser specified by \a name or null if could not
         * be found.
         */
//    const Query::Parser* discoverQueryParserByName( const QString& name );

        /**
         * Find a Query::Parser instance that is able to parse the specified query language.
         *
         * \param lang The language the plugin is supposed to support.
         * \param userQueryLanguage If lang is set to Query::QueryLanguageUser this parameter specifies the
         *       query language to use. It allows the extension of the %Soprano Query interface with new
         *       query languages that are not officially supported by %Soprano.
         *
         * \return A Query::Parser plugin that can parse query language lang or 0 if no such plugin could be found.
         */
//    const Query::Parser* discoverQueryParserForQueryLanguage( Query::QueryLanguage lang, const QString& userQueryLanguage = QString() );

//    QList<const Query::Parser*> allQueryParsers();
        //@}

        //@{
        /**
         * Find a query serializer plugin by its name.
         *
         * \return the query serializer specified by \a name or null if could not
         * be found.
         */
//    const Query::Serializer* discoverQuerySerializerByName( const QString& name );

        /**
         * Find a Query::Serializer instance that is able to serialize the specified query language.
         *
         * \param lang The language the plugin is supposed to support.
         * \param userQueryLanguage If lang is set to Query::QueryLanguageUser this parameter specifies the
         *       query language to use. It allows the extension of the %Soprano Query interface with new
         *       query languages that are not officially supported by %Soprano.
         *
         * \return A Query::Serializer plugin that can parse query language lang or 0 if no such plugin could be found.
         */
//    const Query::Serializer* discoverQuerySerializerForQueryLanguage( Query::QueryLanguage lang, const QString& userQueryLanguage = QString() );

//    QList<const Query::Serializer*> allQuerySerializers();
        //@}

        /** \endcond query_api_disabled */

        /**
         * Get the singleton instance of the PluginManager
         */
        static PluginManager* instance();

    private:
        PluginManager( QObject* parent = 0 );
        void loadAllPlugins();
        void loadPlugin( const QString& path );
        void loadPlugins( const QString& path );

        class Private;
        Private* const d;
    };
}

#endif
