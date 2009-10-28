/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2007-2008 Sebastian Trueg <trueg@kde.org>
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
     * \class BackendSetting backend.h Soprano/BackendSetting
     *
     * \brief Wraps one setting for Model creation.
     *
     * A BackendSetting consists of an option type and an option value. The option type can either be a predefined
     * value from Soprano::BackendOption or a user defined string value that depends on the Backend implementation. In the
     * latter case option has to be set to Soprano::BackendOptionUser.
     *
     * \author Sebastian Trueg <trueg@kde.org>
     *
     * \sa Backend::createModel()<br/>BackendSettings
     */
    class SOPRANO_EXPORT BackendSetting
    {
    public:
        /**
         * Create an empty setting.
         * \sa Soprano::BackendOptionNone
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
         * The option that this setting sets. If Soprano::BackendOptionUser
         * the option is identified by userOptionName().
         */
        BackendOption option() const;

        /**
         * The name of the user option if option() is Soprano::BackendOptionUser.
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
     * List of BackendSetting.
     *
     * \sa isOptionInSettings, settingInSettings, valueInSettings
     */
    typedef QList<BackendSetting> BackendSettings;

    /**
     * Check if a certain option is defined in a list of BackendSettings.
     *
     * \param settings The list to check.
     * \param option The option to check.
     * \param userOptionName If \p option is Soprano::BackendOptionUser, this
     * defines the user option to be checked.
     *
     * \return \p true if the option is defined.
     *
     * \relates BackendSetting
     *
     * \since 2.2
     */
    SOPRANO_EXPORT bool isOptionInSettings( const BackendSettings& settings, BackendOption option, const QString& userOptionName = QString() );

    /**
     * Get a setting from the set, allowing to modify it.
     *
     * \param settings The list to check.
     * \param option The option to extract.
     * \param userOptionName If \p option is Soprano::BackendOptionUser, this
     * defines the user option to be extracted.
     *
     * \return The extracted setting. An empty setting is added to \p settings in case the option is not found in \p settings.
     *
     * \relates BackendSetting
     *
     * \since 2.2
     */
    SOPRANO_EXPORT BackendSetting& settingInSettings( BackendSettings& settings, BackendOption option, const QString& userOptionName = QString() );

    /**
     * \overload
     *
     * \param settings The list to check.
     * \param userOptionName The user option to be extracted.
     *
     * \return The extracted setting. An empty setting is added to \p settings in case the option is not found in \p settings.
     *
     * \relates BackendSetting
     *
     * \since 2.4
     */
    SOPRANO_EXPORT BackendSetting& settingInSettings( BackendSettings& settings, const QString& userOptionName );

    /**
     * Get a setting from the set.
     *
     * \param settings The list to check.
     * \param option The option to extract.
     * \param userOptionName If \p option is Soprano::BackendOptionUser, this
     * defines the user option to be extracted.
     *
     * \return The extracted setting. If the setting is not found an empty BackendSetting is returned.
     *
     * \relates BackendSetting
     *
     * \since 2.4
     */
    SOPRANO_EXPORT BackendSetting settingInSettings( const BackendSettings& settings, BackendOption option, const QString& userOptionName = QString() );

    /**
     * \overload
     *
     * \param settings The list to check.
     * \param userOptionName The user option to be extracted.
     *
     * \return The extracted setting. If the setting is not found an empty BackendSetting is returned.
     *
     * \relates BackendSetting
     *
     * \since 2.4
     */
    SOPRANO_EXPORT BackendSetting settingInSettings( const BackendSettings& settings, const QString& userOptionName = QString() );

    /**
     * Retrieve the value of an option.
     *
     * \param settings The list to check.
     * \param option The option to retrieve.
     * \param userOptionName If \p option is Soprano::BackendOptionUser, this
     * defines the user option to be retrieved.
     *
     * \return The value of the specified option or an invalid variant if the set
     * does not contain the option.
     *
     * \relates BackendSetting
     *
     * \since 2.2
     */
    SOPRANO_EXPORT QVariant valueInSettings( const BackendSettings& settings, BackendOption option, const QString& userOptionName = QString() );

    /**
     * \overload
     *
     * \param settings The list to check.
     * \param userOptionName The user option to retrieve.
     * \param defaultValue The default to use if \p option is not part of \p settings
     *
     * \return The value of the specified option or \p default if \p settings
     * does not contain the option.
     *
     * \relates BackendSetting
     *
     * \since 2.4
     */
    SOPRANO_EXPORT QVariant valueInSettings( const BackendSettings& settings, const QString& userOptionName, const QVariant& defaultValue = QVariant() );

    /**
     * Retrieve the value of an option with a fallback default.
     *
     * \param settings The list to check.
     * \param option The option to retrieve.
     * \param defaultValue The default to use if \p option is not part of \p settings
     *
     * \return The value of the specified option or \p default if \p settings
     * does not contain the option.
     *
     * \relates BackendSetting
     *
     * \since 2.4
     */
    SOPRANO_EXPORT QVariant valueInSettingsWithDefault( const BackendSettings& settings, BackendOption option, const QVariant& defaultValue );


    /**
     * \class Backend backend.h Soprano/Backend
     *
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
         * Error::ErrorInvalidArgument.
         *
         * \sa BackendSetting
         */
        virtual StorageModel* createModel( const BackendSettings& settings = BackendSettings() ) const = 0;

        /**
         * Phyically delete all data for a specific model. For most backends this means deleting some files
         * on the hard disk. With others it may mean to delete tables from an SQL database.
         *
         * \param settings The settings that were used to create the model which should be deleted.
         *                 For most backends the Soprano::BackendOptionStorageDir setting is probably most important.
         *                 If the settings do not provide enough information to uniquely identify the model
         *                 to delete, the method should be terminated with an Error::ErrorInvalidArgument
         *                 error.
         *
         * \return \p true if the data was successfully removed, \p false otherwise. ErrorCache::lastError()
         *         may provide more detailed error information in the latter case.
         *
         * \since 2.1
         */
        virtual bool deleteModelData( const BackendSettings& settings ) const = 0;

        /**
         * Each backend can support a set of features. Backends without any features do not make much sense.
         * If the features include Soprano::BackendFeatureUser additional user features not defined in
         * Backend::BackendFeature can be supported via supportedUserFeatures().
         *
         * \return A combination of Soprano::BackendFeature values.
         */
        virtual BackendFeatures supportedFeatures() const = 0;

        /**
         * A Backend can support additional features that are not defined in Backend::BackendFeature.
         * These user defined features have string identifiers. If a backend supports additional features
         * it has to include Soprano::BackendFeatureUser in supportedFeatures().
         *
         * The default implementation returns an empty list.
         *
         * \return the list of supported user features.
         */
        virtual QStringList supportedUserFeatures() const;

        /**
         * Check if a backend supports certain features. If feature includes Soprano::BackendFeatureUser
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

Q_DECLARE_INTERFACE(Soprano::Backend, "org.soprano.plugins.Backend/2.1")

#endif
