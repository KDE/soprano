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

#include "backend.h"


Soprano::BackendSetting::BackendSetting()
    : option( BACKEND_OPTION_NONE )
{
}


Soprano::BackendSetting::BackendSetting( BackendOption s, const QString& value_ )
    : option( s ),
      value( value_ )
{
}


Soprano::BackendSetting::BackendSetting( const QString& userOption, const QString& value_ )
    : option( BACKEND_OPTION_USER ),
      userOptionName( userOption ),
      value( value_ )
{
}


class Soprano::Backend::Private
{
public:
};


Soprano::Backend::Backend( const QString& name )
    : Plugin( name ),
      d( new Private() )
{
}


Soprano::Backend::~Backend()
{
    delete d;
}


QStringList Soprano::Backend::supportedUserFeatures() const
{
    return QStringList();
}


bool Soprano::Backend::supportsFeatures( BackendFeatures features, const QStringList& userFeatureList ) const
{
    if ( !( supportedFeatures() & features ) ) {
        return false;
    }

    if ( features & BACKEND_FEATURE_USER ) {
        for( QStringList::const_iterator it = userFeatureList.begin(); it != userFeatureList.end(); ++it ) {
            if( !supportedUserFeatures().contains( *it ) ) {
                return false;
            }
        }
    }

    return true;
}
