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
#include "error.h"
#include "soprano_export.h"
#include "sopranotypes.h"

#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtCore/QSharedDataPointer>


namespace Soprano
{
    class StorageModel;

    /**
     * \brief Wraps one setting for Model creation.
     *
     * A BackendSetting consists of an option type and an option value. The option type can either be a predefined
     * value from Soprano::BackendOption or a user defined string value that depends on the Backend implementation. In the 
     * latter case option has to be set to Soprano::BACKEND_OPTION_USER.
     *
     * \sa Backend::createModel()
     *
     * \author Sebastian Trueg <trueg@kde.org>
     */
    class BackendSetting
    {
    public:
	/**
	 * Create an empty setting.
	 * \sa Soprano::BACKEND_OPTION_NONE
	 */
	BackendSetting();

	/**
	 * Create a boolean setting with a default true value.
	 */
	BackendSetting( BackendOption option );

	/**
	 * Create a standard setting with option \p s and value \p value_.
	 */
	BackendSetting( BackendOption s, const QVariant& value_ );

	/**
	 * Create a user setting with user option name \p userOption and value \p value_.
	 */
	BackendSetting( const QString& userOption, const QVariant& value_ );

	/**
	 * Copy constructor.
	 */
	BackendSetting( const BackendSetting& other );

	/**
	 * Destructor.
	 */
	~BackendSetting();

	/**
	 * Copy operator.
	 */
	BackendSetting& operator=( const BackendSetting& other );

	/**
	 * The option that this setting sets. If Soprano::BACKEND_OPTION_USER 
	 * the option is identified by userOptionName().
	 */
	BackendOption option() const;

	/**
	 * The name of the user option if option() is Soprano::BACKEND_OPTION_USER.
	 */
	QString userOptionName() const;

	/**
	 * The value of the setting.
	 */
	QVariant value() const;

	/**
	 * Set the value of the Setting.
	 */
	void setValue( const QVariant& value );

    private:
	class Private;
	QSharedDataPointer<Private> d;
    };

    /**
     * \brief Soprano::Backend defines the interface for a Soprano backend plugin.
     *
     * The Backend interface defines two important methods: createModel() and supportedFeatures().
     * It inherits from Error::ErrorCache for error handling and subclasses should use clearError()
     * and setError() to report the status.
     *
     * \sa \ref soprano_writing_plugins
     *
     * \author Sebastian Trueg <trueg@kde.org>
     */
    class SOPRANO_EXPORT Backend : public Plugin, public Error::ErrorCache
    {
    public:
	Backend( const QString& name );
	virtual ~Backend();

	/**
	 * Creates a new RDF model with options.
	 * The caller takes ownership and has to care about deletion.
	 *
	 * \param settings The settings that should be used to create the Model. Backend implementations
	 * should never ignore settings but rather return 0 if an option is not supported. Backends can,
	 * however, define their own default settings. Invalid settings should result in an Error with value
	 * Error::ERROR_INVALID_ARGUMENT.
	 */
	virtual StorageModel* createModel( const QList<BackendSetting>& settings = QList<BackendSetting>() ) const = 0;

	/**
	 * Each backend can support a set of features. Backends without any features do not make much sense.
	 * If the features include Soprano::BACKEND_FEATURE_USER additional user features not defined in 
	 * Backend::BackendFeature can be supported via supportedUserFeatures().
	 *
	 * \return A combination of Soprano::BackendFeature values.
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

Q_DECLARE_INTERFACE(Soprano::Backend, "org.soprano.plugins.Backend/2.0")

#endif
