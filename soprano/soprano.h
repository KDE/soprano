/* 
 * This file is part of Soprano Project
 *
 * Copyright (C) 2006 Daniele Galdi <daniele.galdi@gmail.com>
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

#ifndef SOPRANO_H
#define SOPRANO_H

#include <soprano/query.h>
#include <soprano/queryresultiterator.h>
#include <soprano/node.h>
#include <soprano/nodeiterator.h>
#include <soprano/literalvalue.h>
#include <soprano/statement.h>
#include <soprano/statementiterator.h>
#include <soprano/simplestatementiterator.h>
#include <soprano/queryresultstatementiterator.h>
#include <soprano/model.h>
#include <soprano/storagemodel.h>
#include <soprano/filtermodel.h>
#include <soprano/parser.h>
#include <soprano/backend.h>
#include <soprano/pluginmanager.h>
#include <soprano/error.h>

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
     * \param features The features that are requested.
     * \param userFeatures If features contain Soprano::BACKEND_FEATURE_USER this paramter states the additionally requested user features.
     *
     * \return a backend that supports the features defined in \a features.
     *
     * \sa PluginManager::discoverBackendByFeatures()
     */
    SOPRANO_EXPORT const Backend* discoverBackendByFeatures( BackendFeatures features, const QStringList& userFeatures = QStringList() );

    /**
     * By default and if available backend "redland" is used.
     */
    SOPRANO_EXPORT void setUsedBackend( const Backend* );

    SOPRANO_EXPORT const Backend* usedBackend();

    /**
     * Creates a new RDF storage using the backend set via setUsedBackend.
     * The caller takes ownership and has to care about deletion.
     *
     * \param settings The settings that should be used to create the Model. Backend implementations
     *  should never ignore settings but rather return 0 if an option is not supported. Backends can,
     * however, define their own default settings.
     *
     * \sa Model, Backend::createModel
     */
    SOPRANO_EXPORT Model* createModel( const QList<BackendSetting>& settings = QList<BackendSetting>() );
}

#endif // SOPRANO_H
