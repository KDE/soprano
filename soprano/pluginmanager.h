/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2007-2009 Sebastian Trueg <trueg@kde.org>
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
#include "soprano_export.h"

#include <QtCore/QObject>
#include <QtCore/QStringList>


namespace Soprano
{
    class Backend;
    class Parser;
    class Serializer;

    namespace Query {
        class Parser;
        class Serializer;
    }

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
         * Set the plugin search path. The PluginManager searches a set of folders for installed
         * plugins. Here a plugin consists of a desktop file describing it and the actual plugin
         * library file. For loading custom plugins manually see loadCustomPlugin.
         *
         * \param path The folders that PluginManager should search for plugin description files.
         * \param useDefaults If true PluginManager will also search the default plugin paths
         *
         * \sa \ref soprano_writing_plugins
         *
         * \since 2.3
         */
        void setPluginSearchPath( const QStringList& path, bool useDefaults = true );

        /**
         * Load a custom plugin from a library at the specified path.
         *
         * \param path The path to the library file that contains the plugin. Be aware that
         * plugins loaded via this method do not need a desktop file as described
         *
         * PluginManager uses the name the plugin provides (Plugin::pluginName)
         * to manage the plugin.
         *
         * \return \p true if the plugin could be loaded successfully, \p false
         * if no suported plugin could be found at path.
         *
         * \since 2.3
         */
        bool loadCustomPlugin( const QString& path );

        /**
         * Load a custom Backend plugin from a library at the specified path.
         *
         * \param path The path to the library file that contains the plugin.
         *
         * PluginManager uses the name the plugin provides (Plugin::pluginName)
         * to manage the plugin.
         *
         * \return The loaded Backend plugin or 0 in case no suported plugin could
         * be found at path or the found plugin is not a backend.
         *
         * \since 2.3
         */
        const Backend* loadCustomBackend( const QString& path );

        /**
         * Load a custom Parser plugin from a library at the specified path.
         *
         * \param path The path to the library file that contains the plugin.
         *
         * PluginManager uses the name the plugin provides (Plugin::pluginName)
         * to manage the plugin.
         *
         * \return The loaded Parser plugin or 0 in case no suported plugin could
         * be found at path or the found plugin is not a backend.
         *
         * \since 2.3
         */
        const Parser* loadCustomParser( const QString& path );

        /**
         * Load a custom Serializer plugin from a library at the specified path.
         *
         * \param path The path to the library file that contains the plugin.
         *
         * PluginManager uses the name the plugin provides (Plugin::pluginName)
         * to manage the plugin.
         *
         * \return The loaded Serializer plugin or 0 in case no suported plugin could
         * be found at path or the found plugin is not a backend.
         *
         * \since 2.3
         */
        const Serializer* loadCustomSerializer( const QString& path );
        //@}

        //@{
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
         * Find a parser plugin by its name.
         *
         * \return the parser specified by \a name or null if could not
         * be found.
         */
        const Parser* discoverParserByName( const QString& name );

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
         */
        const Parser* discoverParserForSerialization( RdfSerialization serialization, const QString& userSerialization = QString() );

        QList<const Parser*> allParsers();
        //@}

        //@{
        /**
         * Find a serializer plugin by its name.
         *
         * \return the serializer specified by \a name or null if could not
         * be found.
         */
        const Serializer* discoverSerializerByName( const QString& name );

        /**
         * Find a Serializer instance that is able to encode RDF data using
         * serialization.
         *
         * \param serialization The requested serialization.
         * \param userSerialization If serialization is set to Soprano::SerializationUser this parameter specifies the
         *       serialization to use. It allows the extension of the %Soprano Serializer interface with new
         *       RDF serializations that are not officially supported by %Soprano.
         *
         * \return A Serializer plugin that can serialize RDF data encoded in the requested
         * serialization or 0 if no such plugin could be found.
         */
        const Serializer* discoverSerializerForSerialization( RdfSerialization serialization, const QString& userSerialization = QString() );

        QList<const Serializer*> allSerializers();
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

        friend class PluginManagerFactory;
    };
}

#endif
