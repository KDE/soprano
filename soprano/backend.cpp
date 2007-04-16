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

class Soprano::Backend::Private
{
public:
  QString name;
};


Soprano::Backend::Backend( const QString& name )
{
  d = new Private;
  d->name = name;
}


Soprano::Backend::~Backend()
{
  delete d;
}


QString Soprano::Backend::backendName() const
{
  return d->name;
}


bool Soprano::Backend::hasFeature( const QString& feature ) const
{
  return features().contains( feature );
}


bool Soprano::Backend::hasFeatures( const QStringList& featureList ) const
{
  for( QStringList::const_iterator it = featureList.begin(); it != featureList.end(); ++it )
    if( !hasFeature( *it ) )
      return false;
  return true;
}
