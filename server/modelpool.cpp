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
#include "transaction.h"

#include <QtCore/QHash>


class Soprano::Server::ModelPool::Private
{
public:
    ServerCore* core;

    QHash<quint32, Model*> modelIdMap;
    QHash<quint32, Transaction*> transactionIdMap;
    QHash<QString, quint32> modelNameMap;

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
    qDeleteAll( d->transactionIdMap );
    delete d;
}


Soprano::Model* Soprano::Server::ModelPool::modelById( quint32 id ) const
{
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
    d->modelIdMap.remove( d->modelNameMap[name] );
    d->modelNameMap.remove( name );
}


quint32 Soprano::Server::ModelPool::insertTransaction( Transaction* t )
{
    quint32 id = d->generateUniqueId();
    d->modelIdMap.insert( id, t );
    d->transactionIdMap.insert( id, t );
    return id;
}


Soprano::Transaction* Soprano::Server::ModelPool::transactionById( quint32 id ) const
{
    QHash<quint32, Transaction*>::iterator it = d->transactionIdMap.find( id );
    if ( it != d->transactionIdMap.end() ) {
        return *it;
    }
    return 0;
}


bool Soprano::Server::ModelPool::deleteTransaction( quint32 id )
{
    QHash<quint32, Transaction*>::iterator it = d->transactionIdMap.find( id );
    if ( it != d->transactionIdMap.end() ) {
        delete it.value();
        d->transactionIdMap.erase( it );
        d->modelIdMap.remove( id );
        return true;
    }
    return false;
}
