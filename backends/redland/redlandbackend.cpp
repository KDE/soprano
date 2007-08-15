/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2007 Sebastian Trueg <trueg@kde.org>
 *
 * Based on RedlandModelFactory.cpp
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

#include "redlandbackend.h"

#include <redland.h>

#include "redlandworld.h"
#include "redlandmodel.h"

#include <QtCore/QtPlugin>
#include <QtCore/QDebug>


Q_EXPORT_PLUGIN2(soprano_redlandbackend, Soprano::Redland::BackendPlugin)


Soprano::Redland::BackendPlugin::BackendPlugin()
  : QObject(),
    Backend( "redland" )
{
}


Soprano::Model* Soprano::Redland::BackendPlugin::createModel() const
{
    return World::self()->createModel();
}


Soprano::Model* Soprano::Redland::BackendPlugin::createModel( const QString& name, const QStringList& options ) const
{
    return World::self()->createModel( name, options );
}


QStringList Soprano::Redland::BackendPlugin::features() const
{
    return QString( "memory,contexts" ).split( ',' );
}

#include "redlandbackend.moc"
