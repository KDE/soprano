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

#include "desktopfile.h"

static const char* s_desktopEntryGroup = "Desktop Entry";

QString Soprano::DesktopFile::desktopEntryGroup()
{
    return QLatin1String( s_desktopEntryGroup );
}


class Soprano::DesktopFile::Private
{
public:
};

Soprano::DesktopFile::DesktopFile()
    : d( 0 )
{
}


Soprano::DesktopFile::DesktopFile( const QString& path )
    : d( 0 )
{
    open( path );
}


Soprano::DesktopFile::~DesktopFile()
{
}


bool Soprano::DesktopFile::open( const QString& path )
{
    if ( IniFile::open( path ) ) {
        return groups().count() == 1 && groups()[0] == desktopEntryGroup();
    }
    else {
        return false;
    }
}


QString Soprano::DesktopFile::name() const
{
    return readEntry( desktopEntryGroup(), QLatin1String( "Name" ) );
}


QString Soprano::DesktopFile::comment() const
{
    return readEntry( desktopEntryGroup(), QLatin1String( "Comment" ) );
}


QString Soprano::DesktopFile::type() const
{
    return readEntry( desktopEntryGroup(), QLatin1String( "Type" ) );
}


QString Soprano::DesktopFile::serviceTypes() const
{
    return readEntry( desktopEntryGroup(), QLatin1String( "ServiceTypes" ) );
}
