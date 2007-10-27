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

#include "sopranopluginfile.h"


class Soprano::SopranoPluginFile::Private
{
public:
};

Soprano::SopranoPluginFile::SopranoPluginFile()
    : d( 0 )
{
}


Soprano::SopranoPluginFile::SopranoPluginFile( const QString& path )
    : d( 0 )
{
    open( path );
}


Soprano::SopranoPluginFile::~SopranoPluginFile()
{
   //For the future
   //delete d;
}


bool Soprano::SopranoPluginFile::open( const QString& path )
{
    if ( DesktopFile::open( path ) ) {
        return !library().isEmpty() && !sopranoVersion().isEmpty();
    }
    else {
        return false;
    }
}


QString Soprano::SopranoPluginFile::library() const
{
    return readEntry( desktopEntryGroup(), QLatin1String( "X-Soprano-Library" ) );
}


QString Soprano::SopranoPluginFile::pluginAuthor() const
{
    return readEntry( desktopEntryGroup(), QLatin1String( "X-Soprano-Plugin-Author" ) );
}


QString Soprano::SopranoPluginFile::pluginEmail() const
{
    return readEntry( desktopEntryGroup(), QLatin1String( "X-Soprano-Plugin-EMail" ) );
}


QString Soprano::SopranoPluginFile::pluginWebsite() const
{
    return readEntry( desktopEntryGroup(), QLatin1String( "X-Soprano-Plugin-Website" ) );
}


QString Soprano::SopranoPluginFile::pluginLicense() const
{
    return readEntry( desktopEntryGroup(), QLatin1String( "X-Soprano-Plugin-License" ) );
}


QString Soprano::SopranoPluginFile::pluginVersion() const
{
    return readEntry( desktopEntryGroup(), QLatin1String( "X-Soprano-Plugin-Version" ) );
}


QString Soprano::SopranoPluginFile::sopranoVersion() const
{
    return readEntry( desktopEntryGroup(), QLatin1String( "X-Soprano-Version" ) );
}
