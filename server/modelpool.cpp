/*
 * This file is part of Soprano Project.
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

#include "modelpool.h"
#include "servercore.h"
#include "randomgenerator.h"

#include "model.h"

#include <QtCore/QHash>
#include <QtCore/QMutex>
#include <QtCore/QMutexLocker>


class Soprano::Server::ModelPool::Private
{
public:
    ServerCore* core;

    QHash<quint32, Model*> modelIdMap;
    QHash<QString, quint32> modelNameMap;

    QMutex mutex;

    quint32 generateUniqueId() {
        quint32 id = 0;
        do {
            id = RandomGenerator::instance()->randomInt();
        } while ( modelIdMap.contains( id ) );
        return id;
    }
};


Soprano::Server::ModelPool::ModelPool( ServerCore* core )
    : d( new Private() )
{
    d->core = core;
}


Soprano::Server::ModelPool::~ModelPool()
{
    delete d;
}


Soprano::Model* Soprano::Server::ModelPool::modelById( quint32 id ) const
{
    QMutexLocker locker( &d->mutex );
    QHash<quint32, Model*>::iterator it = d->modelIdMap.find( id );
    if ( it != d->modelIdMap.end() ) {
        return *it;
    }
    return 0;
}


Soprano::Model* Soprano::Server::ModelPool::modelByName( const QString& name )
{
    return modelById( idForModelName( name ) );
}


quint32 Soprano::Server::ModelPool::idForModelName( const QString& name )
{
    QMutexLocker locker( &d->mutex );
    quint32 id = 0;

    QHash<QString, quint32>::const_iterator it = d->modelNameMap.constFind( name );
    if ( it != d->modelNameMap.constEnd() ) {
        id = *it;
    }
    else {
        Model* model = d->core->model( name );
        if ( model ) {
            id = d->generateUniqueId();
            d->modelIdMap.insert( id, model );
            d->modelNameMap.insert( name, id );
        }
    }

    return id;
}


void Soprano::Server::ModelPool::removeModel( const QString& name )
{
    QMutexLocker locker( &d->mutex );
    d->modelIdMap.remove( d->modelNameMap[name] );
    d->modelNameMap.remove( name );
}
