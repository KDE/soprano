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

#include "bindingset.h"
#include "node.h"

#include <QtCore/QHash>
#include <QtCore/QStringList>
#include <QtCore/QSharedData>



class Soprano::BindingSet::Private : public QSharedData
{
public:
    QHash<QString, Node> bindingMap;
    QStringList names;
};


Soprano::BindingSet::BindingSet()
    : d( new Private() )
{
}


Soprano::BindingSet::BindingSet( const BindingSet& other )
{
    d = other.d;
}


Soprano::BindingSet::~BindingSet()
{
}


Soprano::BindingSet& Soprano::BindingSet::operator=( const BindingSet& other )
{
    d = other.d;
    return *this;
}


QStringList Soprano::BindingSet::bindingNames() const
{
    return d->names;
}


Soprano::Node Soprano::BindingSet::operator[]( int offset ) const
{
    return d->bindingMap[d->names[offset]];
}


Soprano::Node Soprano::BindingSet::operator[]( const QString name ) const
{
    return value( name );
}


Soprano::Node Soprano::BindingSet::value( int offset ) const
{
    return d->bindingMap[d->names[offset]];
}


Soprano::Node Soprano::BindingSet::value( const QString& name ) const
{
    QHash<QString, Node>::const_iterator it = d->bindingMap.find( name );
    if ( it != d->bindingMap.constEnd() ) {
        return *it;
    }
    else {
        return Node();
    }
}


bool Soprano::BindingSet::contains( const QString& name ) const
{
    return d->bindingMap.contains( name );
}


int Soprano::BindingSet::count() const
{
    return d->bindingMap.size();
}


void Soprano::BindingSet::insert( const QString& name, const Node& value )
{
    d->names.append( name );
    d->bindingMap.insert( name, value );
}


QDebug operator<<( QDebug s, const Soprano::BindingSet& b )
{
    QStringList n = b.bindingNames();
    for ( int i = 0; i < n.count(); ++i ) {
        s.nospace() << n[i] << " -> " << b[n[i]];
        if ( i+1 < n.count() ) {
            s.nospace() << "; ";
        }
    }
    return s;
}
