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

#ifndef _SERVER_BACKEND_H_
#define _SERVER_BACKEND_H_

#include <soprano/backend.h>
#include "../soprano/soprano_export.h"

#include <QObject>

namespace Soprano
{
  namespace Server
  {
      /**
       * \brief Backend to access RDF storage models through the %Soprano daemon.
       *
       * The %Soprano server backend can be accessed normally through PluginManager::discoverBackendByName()
       * with name "sopranoserver" or by simply creating one manually. The latter solution can have advantages
       * as it uses a new connection to the server.
       *
       * A ServerBackend keeps one open connection to the %Soprano server until the last Model is deleted.
       * This has the advantage that calling methods in the Models is very fast since there is no need for
       * establishing a new connection. Since the local Model instances on the client side are just very slim
       * wrappers creating them through createModel() is very fast.
       *
       * Additonal BackendOptions supported:
       * \li name - The name for the Model to be created. The daemon handles a set of models which are identified by a unique name.
       */
      class SOPRANO_EXPORT ServerBackend : public QObject, public Soprano::Backend
      {
	  Q_OBJECT
	  Q_INTERFACES(Soprano::Backend)

      public:
	  static const quint16 DEFAULT_PORT;

	  /**
	   * Create a new ServerBackend instance.
	   *
	   * \param port The port the %Soprano server is running on.
	   */
	  ServerBackend( quint16 port = DEFAULT_PORT );
	  ~ServerBackend();

	  StorageModel* createModel( const QString& name ) const;
	  StorageModel* createModel( const QList<BackendSetting>& settings = QList<BackendSetting>() ) const;

	  BackendFeatures supportedFeatures() const;

      private Q_SLOTS:
	  void modelDeleted();

      private:
	  class Private;
	  Private* const d;
      };
  }
}

#endif