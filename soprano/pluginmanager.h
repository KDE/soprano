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
#include "soprano_export.h"

#include <QtCore/QObject>
#include <QtCore/QList>


namespace Soprano
{
  class Backend;

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
       * \return A Parser plugin that can parse RDF data encoded in the requested 
       * serialization or 0 if no such plugin could be found.
       */
      const Parser* discoverParserForSerialization( RdfSerialization serialization );

      QList<const Backend*> allBackends();

      QList<const Parser*> allParsers();

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
