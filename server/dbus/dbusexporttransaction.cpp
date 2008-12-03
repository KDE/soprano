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

#include "dbusexporttransaction.h"
#include "dbustransactionadaptor.h"
#include "dbusutil.h"
#include "transaction.h"

#include <QtDBus/QDBusConnection>


class Soprano::Server::DBusExportTransaction::Private
{
public:
    Private( DBusExportTransaction* parent )
        : q( parent ) {
    }

    void slotServiceOwnerChanged( const QString& name, const QString&, const QString& );

    Transaction* transaction;
    QString dbusClient;

private:
    DBusExportTransaction* q;
};


void Soprano::Server::DBusExportTransaction::Private::slotServiceOwnerChanged( const QString& name, const QString&, const QString& )
{
    if ( name == dbusClient ) {
        qDebug() << "client" << dbusClient << "went down. closing transaction:" << transaction;
        delete transaction;
        transaction = 0;
        q->deleteLater();
    }
}



Soprano::Server::DBusExportTransaction::DBusExportTransaction( Transaction* t )
    : DBusExportModel( t ),
      d( new Private( this ) )
{
    qDebug() << "DBusExportTransaction()" << this;
    d->transaction = t;
    ( void )new DBusTransactionAdaptor( this );
}


Soprano::Server::DBusExportTransaction::~DBusExportTransaction()
{
    qDebug() << "~DBusExportTransaction()" << this;
    delete d;
}


Soprano::Transaction* Soprano::Server::DBusExportTransaction::transaction() const
{
    return d->transaction;
}


bool Soprano::Server::DBusExportTransaction::registerTransaction( const QString& dbusObjectPath, const QString& dbusClient )
{
    if ( registerModel( dbusObjectPath ) ) {
        connect( QDBusConnection::sessionBus().interface(), SIGNAL(serviceOwnerChanged(const QString&, const QString&, const QString&)),
                 this, SLOT(slotServiceOwnerChanged(const QString&, const QString&, const QString&)) );
        d->dbusClient = dbusClient;
        return true;
    }
    else {
        setError( DBus::convertError( QDBusConnection::sessionBus().lastError() ) );
        d->dbusClient = QString();
        return false;
    }
}

#include "dbusexporttransaction.moc"
