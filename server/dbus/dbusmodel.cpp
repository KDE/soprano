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

#include "dbusmodel.h"
#include "dbusmodelinterface.h"
#include "dbusutil.h"
#include "dbusclientnodeiteratorbackend.h"
#include "dbusclientstatementiteratorbackend.h"
#include "dbusclientqueryresultiteratorbackend.h"

#include "backend.h"
#include "error.h"
#include "nodeiterator.h"
#include "statementiterator.h"
#include "queryresultiterator.h"

#include <QDebug>

class Soprano::Client::DBusModel::Private
{
public:
    DBusModelInterface* interface;
    QDBus::CallMode callMode;
};


Soprano::Client::DBusModel::DBusModel( const QString& serviceName, const QString& dbusObject, const Backend* backend )
    : StorageModel( backend ),
      d( new Private() )
{
    d->interface = new DBusModelInterface( serviceName, dbusObject, QDBusConnection::sessionBus(), this );
    d->callMode = QDBus::Block;
    connect( d->interface, SIGNAL( statementsAdded() ),
             this, SIGNAL( statementsAdded() ) );
    connect( d->interface, SIGNAL( statementsRemoved() ),
             this, SIGNAL( statementsRemoved() ) );
    connect( d->interface, SIGNAL( statementAdded(const Soprano::Statement&) ),
             this, SIGNAL( statementAdded(const Soprano::Statement&) ) );
    connect( d->interface, SIGNAL( statementRemoved(const Soprano::Statement&) ),
             this, SIGNAL( statementRemoved(const Soprano::Statement&) ) );
}


Soprano::Client::DBusModel::~DBusModel()
{
   delete d;
}


void Soprano::Client::DBusModel::setAsyncCalls( bool b )
{
    d->callMode = b ? QDBus::BlockWithGui : QDBus::Block;
}


bool Soprano::Client::DBusModel::asyncCalls() const
{
    return d->callMode == QDBus::BlockWithGui;
}


Soprano::Error::ErrorCode Soprano::Client::DBusModel::addStatement( const Statement &statement )
{
    QDBusReply<int> reply = d->interface->addStatement( statement, d->callMode );
    setError( DBus::convertError( reply.error() ) );
    if ( lastError() ) {
        return Error::convertErrorCode( lastError().code() );
    }
    else {
        return ( Error::ErrorCode )reply.value();
    }
}


Soprano::NodeIterator Soprano::Client::DBusModel::listContexts() const
{
    QDBusReply<QString> reply = d->interface->listContexts( d->callMode );
    setError( DBus::convertError( reply.error() ) );
    if ( reply.isValid() ) {
        return new DBusClientNodeIteratorBackend( d->interface->service(), reply.value() );
    }
    else {
        return 0;
    }
}


Soprano::QueryResultIterator Soprano::Client::DBusModel::executeQuery( const QString& query, Query::QueryLanguage language, const QString& userQueryLanguage ) const
{
    QDBusReply<QString> reply = d->interface->executeQuery( query, Query::queryLanguageToString( language, userQueryLanguage ), d->callMode );
    setError( DBus::convertError( reply.error() ) );
    if ( reply.isValid() ) {
        return new DBusClientQueryResultIteratorBackend( d->interface->service(), reply.value() );
    }
    else {
        return 0;
    }
}


Soprano::StatementIterator Soprano::Client::DBusModel::listStatements( const Statement &partial ) const
{
    QDBusReply<QString> reply = d->interface->listStatements( partial, d->callMode );
    setError( DBus::convertError( reply.error() ) );
    if ( reply.isValid() ) {
        return new DBusClientStatementIteratorBackend( d->interface->service(), reply.value() );
    }
    else {
        return 0;
    }
}


Soprano::Error::ErrorCode Soprano::Client::DBusModel::removeStatement( const Statement &statement )
{
    QDBusReply<int> reply = d->interface->removeStatement( statement, d->callMode );
    setError( DBus::convertError( reply.error() ) );
    if ( lastError() ) {
        return Error::convertErrorCode( lastError().code() );
    }
    else {
        return ( Error::ErrorCode )reply.value();
    }
}


Soprano::Error::ErrorCode Soprano::Client::DBusModel::removeAllStatements( const Statement &statement )
{
    QDBusReply<int> reply = d->interface->removeAllStatements( statement, d->callMode );
    setError( DBus::convertError( reply.error() ) );
    if ( lastError() ) {
        return Error::convertErrorCode( lastError().code() );
    }
    else {
        return ( Error::ErrorCode )reply.value();
    }
}


int Soprano::Client::DBusModel::statementCount() const
{
    QDBusReply<int> reply = d->interface->statementCount( d->callMode );
    setError( DBus::convertError( reply.error() ) );
    if ( lastError() ) {
        return 0;
    }
    else {
        return reply.value();
    }
}


bool Soprano::Client::DBusModel::isEmpty() const
{
    QDBusReply<bool> reply = d->interface->isEmpty( d->callMode );
    setError( DBus::convertError( reply.error() ) );
    if ( lastError() ) {
        return 0;
    }
    else {
        return reply.value();
    }
}


bool Soprano::Client::DBusModel::containsStatement( const Statement &statement ) const
{
    QDBusReply<bool> reply = d->interface->containsStatement( statement, d->callMode );
    setError( DBus::convertError( reply.error() ) );
    if ( lastError() ) {
        return false;
    }
    else {
        return reply.value();
    }
}


bool Soprano::Client::DBusModel::containsAnyStatement( const Statement &statement ) const
{
    QDBusReply<bool> reply = d->interface->containsAnyStatement( statement, d->callMode );
    setError( DBus::convertError( reply.error() ) );
    if ( lastError() ) {
        return false;
    }
    else {
        return reply.value();
    }
}


Soprano::Node Soprano::Client::DBusModel::createBlankNode()
{
    QDBusReply<Node> reply = d->interface->createBlankNode( d->callMode );
    setError( DBus::convertError( reply.error() ) );
    return reply.value();
}

#include "dbusmodel.moc"
