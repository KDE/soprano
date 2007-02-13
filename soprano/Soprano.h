/* 
 * This file is part of Soprano Project
 *
 * Copyright (C) 2006 Daniele Galdi <daniele.galdi@gmail.com>
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

#ifndef SOPRANO_H
#define SOPRANO_H

#include <soprano/Query.h>
#include <soprano/ResultSet.h>
#include <soprano/Node.h>
#include <soprano/Statement.h>
#include <soprano/StatementIterator.h>
#include <soprano/Model.h>
#include <soprano/Parser.h>
#include <soprano/backend.h>
#include <soprano/pluginmanager.h>

namespace Soprano {
  /**
   * Find a backend plugin by its name.
   *
   * \return the backend specified by \a name or null if could not
   * be found.
   */
  SOPRANO_EXPORT const Backend* discoverBackendByName( const QString& name );
  
  /**
   * Find a backend plugin by its features.
   *
   * \return a backend that supports the features defined in \a features.
   */
  SOPRANO_EXPORT const Backend* discoverBackendByFeatures( const QStringList& features );

  /**
   * By default and if available backend "redland" is used.
   */
  SOPRANO_EXPORT void setUsedBackend( const Backend* );

  SOPRANO_EXPORT const Backend* usedBackend();

  /**
   * Creates a simple memory model
   * The caller takes ownership and has to care about deletion.
   */
  SOPRANO_EXPORT Model* createModel();

  /**
   * Creates a new RDF storage using the backend set via setUsedBackend.
   * The caller takes ownership and has to care about deletion.
   *
   * \param name The name of the storage
   *
   * \param options Specify optional options for the created model. Options are key/value
   *        pairs in the form of "key=value".
   *        Possible options may include (options always depend on the implementation)
   *        \li storagePath Where to store the data on the local harddrive
   *        \li storageType The database backend used, i.e. berkdb or sqlite or memory
   *
   * \sa Model, Backend::createModel
   */
  SOPRANO_EXPORT Model* createModel( const QString& name, const QStringList& options = QStringList() );

  /**
   * Create a new RDF parser using the backend set via setUsedBackend.
   * The caller takes ownership and has to care about deletion.
   *
   * \param options optional options. Unused for now.
   */
  SOPRANO_EXPORT Parser* createParser( const QStringList& options = QStringList() );
}

#endif // SOPRANO_H
