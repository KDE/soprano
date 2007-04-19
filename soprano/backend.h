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

#ifndef _SOPRANO_BACKEND_H_
#define _SOPRANO_BACKEND_H_

#include <soprano/soprano_export.h>

#include <QtCore/QStringList>

namespace Soprano
{
  class Model;
  class Parser;

  /**
   * Soprano::Backend defines the interface for a Soprano backend plugin.
   *
   * To create a new backend simply create a class that implements this interface
   * and is derived from QObject. Then use the Q_INTERFACE macro to define that it
   * is in fact a Backend plugin and export the plugin via the Q_EXPORT_PLUGIN2 macro.
   *
   * \code
   * class MyBackend : public QObject, public Soprano::Backend
   * {
   *   Q_OBJECT
   *   Q_INTERFACES(Soprano::Backend)
   *
   *  public:
   *   Storage* createStorage( const QString& name, const QStringList& options = QStringList() ) const;
   *   Parser* createParser( const QStringList& options = QStringList() ) const;
   * };
   * \endcode
   *
   * In the implementation file export the plugin so it can be picked up by the
   * plugin loading framework:
   *
   * \code
   * Q_EXPORT_PLUGIN2(soprano_mybackend, MyBackend)
   * \endcode
   */
  class SOPRANO_EXPORT Backend
    {
    public:
      Backend( const QString& name );
      virtual ~Backend();

      QString backendName() const;

      /**
       * Creates a simple memory model
       * The caller takes ownership and has to care about deletion.
       */
      virtual Model* createModel() const = 0;

      /**
       * Creates a new RDF model with options.
       * The caller takes ownership and has to care about deletion.
       *
       * \param name the name of the model, can be used for storage.
       *
       * \param options Specify optional options for the created model. Options are key/value
       *        pairs in the form of "key=value".
       *        Possible options may include (options always depend on the implementation)
       *        \li storagePath Where to store the data on the local harddrive
       *        \li storageType The database backend used, i.e. berkdb or sqlite or memory
       */
      virtual Model* createModel( const QString& name, const QStringList& options = QStringList() ) const = 0;

      /**
       * Create a new RDF parser.
       * The caller takes ownership and has to care about deletion.
       *
       * \param options optional options. Unused for now.
       */
      virtual Parser* createParser( const QStringList& options = QStringList() ) const = 0;

      /**
       * Features include:
       * \li memory The backend supports a memory storage.
       * \li contexts The backend supports RDF contexts.
       * \li parser The backend supports Parser creation.
       *
       * \return the list of supported features.
       */
      virtual QStringList features() const = 0;

      // FIXME: make the features an enumeration
      bool hasFeature( const QString& feature ) const;
      bool hasFeatures( const QStringList& feature ) const;

    private:
      class Private;
      Private* const d;
    };
}

Q_DECLARE_INTERFACE(Soprano::Backend, "org.soprano.plugins.Backend/1.0")

#endif
