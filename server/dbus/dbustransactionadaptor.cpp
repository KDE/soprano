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

#include "dbustransactionadaptor.h"
#include "dbusexporttransaction.h"
#include "dbusutil.h"
#include "transaction.h"


class Soprano::Server::DBusTransactionAdaptor::Private
{
public:
    DBusExportTransaction* transaction;
};


Soprano::Server::DBusTransactionAdaptor::DBusTransactionAdaptor( DBusExportTransaction* parent )
    : DBusModelAdaptor( parent ),
      d( new Private() )
{
    qDebug() << "DBusTransactionAdaptor()" << this;
    d->transaction = parent;
}


Soprano::Server::DBusTransactionAdaptor::~DBusTransactionAdaptor()
{
    qDebug() << "~DBusTransactionAdaptor()" << this;
    delete d;
}


int Soprano::Server::DBusTransactionAdaptor::commit( const QDBusMessage& m )
{
    int errorCode = ( int )d->transaction->transaction()->commit();
    if ( d->transaction->transaction()->lastError() ) {
        DBus::sendErrorReply( m, d->transaction->transaction()->lastError() );
    }
    d->transaction->deleteLater();
    return errorCode;
}


int Soprano::Server::DBusTransactionAdaptor::rollback( const QDBusMessage& m )
{
    int errorCode = ( int )d->transaction->transaction()->rollback();
    if ( d->transaction->transaction()->lastError() ) {
        DBus::sendErrorReply( m, d->transaction->transaction()->lastError() );
    }
    d->transaction->deleteLater();
    return errorCode;
}

#include "dbustransactionadaptor.moc"
