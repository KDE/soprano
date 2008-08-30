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

#include "dbusexportiterator.h"
#include "dbusstatementiteratoradaptor.h"
#include "dbusnodeiteratoradaptor.h"
#include "dbusqueryresultiteratoradaptor.h"
#include "dbusutil.h"

#include "statementiterator.h"
#include "nodeiterator.h"
#include "queryresultiterator.h"

#include <QtDBus/QDBusConnection>


class Soprano::Server::DBusExportIterator::Private
{
public:
    Private( DBusExportIterator* parent )
        : deleteOnClose( false ),
          q( parent ) {
    }

    void slotServiceOwnerChanged( const QString& name, const QString&, const QString& );

    StatementIterator statementIterator;
    NodeIterator nodeIterator;
    QueryResultIterator queryResultIterator;

    bool deleteOnClose;

    QString dbusObjectPath;
    QString dbusClient;

private:
    DBusExportIterator* q;
};


void Soprano::Server::DBusExportIterator::Private::slotServiceOwnerChanged( const QString& name, const QString&, const QString& )
{
    if ( name == dbusClient ) {
        if ( statementIterator.isValid() ) {
            statementIterator.close();
        }
        else if ( nodeIterator.isValid() ) {
            nodeIterator.close();
        }
        else if ( queryResultIterator.isValid() ) {
            queryResultIterator.close();
        }
        if ( deleteOnClose ) {
            q->deleteLater();
        }
    }
}


Soprano::Server::DBusExportIterator::DBusExportIterator( StatementIterator it, QObject* parent )
    : QObject( parent ),
      d( new Private(this) )
{
    d->statementIterator = it;
    ( void )new DBusStatementIteratorAdaptor( this );
}


Soprano::Server::DBusExportIterator::DBusExportIterator( NodeIterator it, QObject* parent )
    : QObject( parent ),
      d( new Private(this) )
{
    d->nodeIterator = it;
    ( void )new DBusNodeIteratorAdaptor( this );
}


Soprano::Server::DBusExportIterator::DBusExportIterator( QueryResultIterator it, QObject* parent )
    : QObject( parent ),
      d( new Private(this) )
{
    d->queryResultIterator = it;
    ( void )new DBusQueryResultIteratorAdaptor( this );
}


Soprano::Server::DBusExportIterator::~DBusExportIterator()
{
    delete d;
}


Soprano::StatementIterator Soprano::Server::DBusExportIterator::statementIterator() const
{
    return d->statementIterator;
}


Soprano::NodeIterator Soprano::Server::DBusExportIterator::nodeIterator() const
{
    return d->nodeIterator;
}


Soprano::QueryResultIterator Soprano::Server::DBusExportIterator::queryResultIterator() const
{
    return d->queryResultIterator;
}


QString Soprano::Server::DBusExportIterator::dbusObjectPath() const
{
    return d->dbusObjectPath;
}


bool Soprano::Server::DBusExportIterator::deleteOnClose() const
{
    return d->deleteOnClose;
}


void Soprano::Server::DBusExportIterator::setDeleteOnClose( bool deleteOnClose )
{
    d->deleteOnClose = deleteOnClose;
}


bool Soprano::Server::DBusExportIterator::registerIterator( const QString& dbusObjectPath, const QString& dbusClient )
{
    if ( !d->dbusObjectPath.isEmpty() ) {
        unregisterIterator();
    }

    if( QDBusConnection::sessionBus().registerObject( dbusObjectPath, this ) ) {
        connect( QDBusConnection::sessionBus().interface(), SIGNAL(serviceOwnerChanged(const QString&, const QString&, const QString&)),
                 this, SLOT(slotServiceOwnerChanged(const QString&, const QString&, const QString&)) );

        d->dbusObjectPath = dbusObjectPath;
        d->dbusClient = dbusClient;
        return true;
    }
    else {
        setError( DBus::convertError( QDBusConnection::sessionBus().lastError() ) );
        d->dbusObjectPath = QString();
        d->dbusClient = QString();
        return false;
    }
}


void Soprano::Server::DBusExportIterator::unregisterIterator()
{
    disconnect( QDBusConnection::sessionBus().interface(), SIGNAL(serviceOwnerChanged(const QString&, const QString&, const QString&)),
                this, SLOT(slotServiceOwnerChanged(const QString&, const QString&, const QString&)) );
    d->dbusObjectPath = QString();
    d->dbusClient = QString();
    QDBusConnection::sessionBus().unregisterObject( d->dbusObjectPath );
}

#include "dbusexportiterator.moc"
