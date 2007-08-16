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

#include "parser.h"
#include "query/query.h"
#include "soprano_export.h"

#include <QtCore/QObject>
#include <QtCore/QList>


namespace Soprano
{
  class Backend;

  namespace Query {
      class Parser;
  }

  /**
   * \brief The PluginManager loads and maintains all %Soprano plugins.
   *
   * Normally there is no need to use this class as all important methods have
   * global counterparts in the Soprano namespace.
   *
   * \author Sebastian Trueg <trueg@kde.org>
   */
  class SOPRANO_EXPORT PluginManager : public QObject
    {
      Q_OBJECT

    public:
      ~PluginManager();

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
       * \return a backend that supports the features defined in \a features.
       */
      const Backend* discoverBackendByFeatures( const QStringList& features );

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
       * \param userSerialization If serialization is set to Soprano::SERIALIZATION_USER this parameter specifies the
       *       serialization to use. It allows the extension of the %Soprano Parser interface with new
       *       RDF serializations that are not officially supported by %Soprano.
       *
       * \return A Parser plugin that can parse RDF data encoded in the requested 
       * serialization or 0 if no such plugin could be found.
       */
      const Parser* discoverParserForSerialization( RdfSerialization serialization, const QString& userSerialization = QString() );

      /**
       * Find a query parser plugin by its name.
       *
       * \return the query parser specified by \a name or null if could not
       * be found.
       */
      const Query::Parser* discoverQueryParserByName( const QString& name );

      /**
       * Find a Query::Parser instance that is able to parse the specified query language.
       *
       * \param lang The language the plugin is supposed to support.
       * \param userQueryLanguage If lang is set to Query::QUERY_LANGUAGE_USER this parameter specifies the
       *       query language to use. It allows the extension of the %Soprano Query interface with new
       *       query languages that are not officially supported by %Soprano.
       *
       * \return A Query::Parser plugin that can parse query language lang or 0 if no such plugin could be found.
       */
      const Query::Parser* discoverQueryParserForQueryLanguage( Query::QueryLanguage lang, const QString& userQueryLanguage = QString() );

      QList<const Backend*> allBackends();

      QList<const Parser*> allParsers();

      QList<const Query::Parser*> allQueryParsers();

      static PluginManager* instance();

    private:
      PluginManager( QObject* parent = 0 );

      void loadAllPlugins();
      void loadPlugins( const QString& path );

      const QStringList libraryPath();

      class Private;
      Private* const d;
    };
}

#endif
