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

#include "plugin.h"
#include "soprano_export.h"
#include "sopranotypes.h"

#include <QtCore/QStringList>

namespace Soprano
{
    class Model;

    /**
     * Wraps one setting for Model creation.
     *
     * \sa Backend::createModel()
     */
    class BackendSetting
    {
    public:
	/**
	 * Create an empty setting.
	 */
	BackendSetting();

	/**
	 * Create a standard setting with option \p s and value \p value_.
	 */
	BackendSetting( BackendOption s, const QString& value_ );

	/**
	 * Create a user setting with user option name \p userOption and value \p value_.
	 */
	BackendSetting( const QString& userOption, const QString& value_ );

	BackendOption option;    /**< The option that this setting sets. If Soprano::BACKEND_OPTION_USER the option is identified by userSettingName. */
	QString userOptionName;  /**< The name of the user setting if setting is set to Soprano::BACKEND_OPTION_USER */
	QString value;           /**< The value of the setting. For boolean options such as BACKEND_OPTION_ENABLE_INFERENCE the value can be ignored. */
    };

    /**
     * \brief Soprano::Backend defines the interface for a Soprano backend plugin.
     *
     * To create a new backend simply create a class that implements this interface
     * and is derived from QObject. Then use the Q_INTERFACES macro to define that it
     * is in fact a Backend plugin and export the plugin via the Q_EXPORT_PLUGIN2 macro.
     *
     * \code
     * class MyBackend : public QObject, public Soprano::Backend
     * {
     *   Q_OBJECT
     *   Q_INTERFACES(Soprano::Backend)
     *
     *  public:
     *   Model* createModel() const;
     *   Model* createModel( const QString& name, const QStringList& options = QStringList() ) const;
     * };
     * \endcode
     *
     * In the implementation file export the plugin so it can be picked up by the
     * plugin loading framework:
     *
     * \code
     * Q_EXPORT_PLUGIN2(soprano_mybackend, MyBackend)
     * \endcode
     *
     * \author Sebastian Trueg <trueg@kde.org>
     */
    class SOPRANO_EXPORT Backend : public Plugin
    {
    public:
	Backend( const QString& name );
	virtual ~Backend();

	/**
	 * Creates a new RDF model with options.
	 * The caller takes ownership and has to care about deletion.
	 *
	 * \param settings The settings that should be used to create the Model. Backend implementations
	 *  should never ignore settings but rather return 0 if an option is not supported. Backends can,
	 * however, define their own default settings.
	 */
	virtual Model* createModel( const QList<BackendSetting>& settings = QList<BackendSetting>() ) const = 0;

	/**
	 * Each backend can support a set of features. Backends without any features do not make much sense.
	 * If the features include Soprano::BACKEND_FEATURE_USER additional user features not defined in 
	 * Backend::BackendFeature can be supported via supportedUserFeatures().
	 *
	 * \return A combination of BackendFeature values.
	 */
	virtual BackendFeatures supportedFeatures() const = 0;

	/**
	 * A Backend can support additional features that are not defined in Backend::BackendFeature.
	 * These user defined features have string identifiers. If a backend supports additional features
	 * it has to include Soprano::BACKEND_FEATURE_USER in supportedFeatures().
	 *
	 * The default implementation returns an empty list.
	 *
	 * \return the list of supported user features.
	 */
	virtual QStringList supportedUserFeatures() const;

	/**
	 * Check if a backend supports certain features. If feature includes Soprano::BACKEND_FEATURE_USER
	 * the list if userFeatures is also compared.
	 *
	 * \return \p true if the backend does support the requested features, \p false otherwise.
	 */
	bool supportsFeatures( BackendFeatures feature, const QStringList& userFeatures = QStringList() ) const;

    private:
	class Private;
	Private* const d;
    };
}

Q_DECLARE_INTERFACE(Soprano::Backend, "org.soprano.plugins.Backend/1.0")

#endif
