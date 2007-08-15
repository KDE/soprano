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

#include "soprano.h"
#include "pluginmanager.h"
#include "backend.h"

static const Soprano::Backend* s_defaultBackend = 0;

static void initSoprano()
{
    if( !s_defaultBackend )
        Soprano::setUsedBackend( Soprano::discoverBackendByName( "redland" ) );
}


const Soprano::Backend* Soprano::discoverBackendByName( const QString& name )
{
    return Soprano::PluginManager::instance()->discoverBackendByName( name );
}


const Soprano::Backend* Soprano::discoverBackendByFeatures( const QStringList& features )
{
    return Soprano::PluginManager::instance()->discoverBackendByFeatures( features );
}


void Soprano::setUsedBackend( const Backend* b )
{
    s_defaultBackend = b;
}


const Soprano::Backend* Soprano::usedBackend()
{
    initSoprano();
    return s_defaultBackend;
}


Soprano::Model* Soprano::createModel()
{
    initSoprano();

    if( s_defaultBackend )
        return s_defaultBackend->createModel();
    else
        return 0;
}


Soprano::Model* Soprano::createModel( const QString& name, const QStringList& options )
{
    initSoprano();

    if( s_defaultBackend )
        return s_defaultBackend->createModel( name, options );
    else
        return 0;
}
