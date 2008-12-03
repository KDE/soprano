/*
 * This file is part of Soprano Project
 *
 * Copyright (C) 2008 Sebastian Trueg <trueg@kde.org>
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

#include "virtuosobackend.h"

Soprano::Virtuoso::BackendPlugin::BackendPlugin()
    : Backend()
{
}


Soprano::Virtuoso::BackendPlugin::~BackendPlugin()
{
}


Soprano::StorageModel* Soprano::Virtuoso::BackendPlugin::createModel( const BackendSettings& settings ) const
{
    // TODO: get the static instance of the virtuoso server and create or open a database
}


bool Soprano::Virtuoso::BackendPlugin::deleteModelData( const BackendSettings& settings ) const
{

}


Soprano::BackendFeatures Soprano::Virtuoso::BackendPlugin::supportedFeatures() const
{
    return( BackendFeatureInference|
            BackendFeatureAddStatement|
            BackendFeatureRemoveStatements|
            BackendFeatureListStatements|
            BackendFeatureQuery|
            BackendFeatureContext );
}


bool Soprano::Virtuoso::BackendPlugin::isAvailable() const
{
    // TODO: check if virtuoso is installed
}

#include "virtuosobackend.moc"
