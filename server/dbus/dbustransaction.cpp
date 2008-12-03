/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2007-2008 Sebastian Trueg <trueg@kde.org>
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

#include "dbustransaction.h"
#include "dbusmodel.h"
#include "dbusmodelinterface.h"
#include "dbustransactioninterface.h"
#include "dbusutil.h"
#include "dbusclientnodeiteratorbackend.h"
#include "dbusclientstatementiteratorbackend.h"
#include "dbusclientqueryresultiteratorbackend.h"

#include "backend.h"
#include "error.h"
#include "nodeiterator.h"
#include "statementiterator.h"
#include "queryresultiterator.h"
#include "transactionfactory.h"

#include <QDebug>

//
// Most methods in this class are direct copies from DBusModel, it would be great to find a way to share the code
//

class Soprano::Client::DBusTransaction::Private
{
public:
    DBusModelInterface* modelInterface;
    DBusTransactionInterface* transactionInterface;
    QDBus::CallMode callMode;
};


Soprano::Client::DBusTransaction::DBusTransaction( DBusModel* model, const QString& serviceName, const QString& dbusObject )
    : Transaction( model ),
      d( new Private() )
{
    d->modelInterface = new DBusModelInterface( serviceName, dbusObject, QDBusConnection::sessionBus(), this );
    d->transactionInterface = new DBusTransactionInterface( serviceName, dbusObject, QDBusConnection::sessionBus(), this );
    d->callMode = QDBus::Block;
    connect( d->modelInterface, SIGNAL( statementsAdded() ),
             this, SIGNAL( statementsAdded() ) );
    connect( d->modelInterface, SIGNAL( statementsRemoved() ),
             this, SIGNAL( statementsRemoved() ) );
    connect( d->modelInterface, SIGNAL( statementAdded(const Soprano::Statement&) ),
             this, SIGNAL( statementAdded(const Soprano::Statement&) ) );
    connect( d->modelInterface, SIGNAL( statementRemoved(const Soprano::Statement&) ),
             this, SIGNAL( statementRemoved(const Soprano::Statement&) ) );
}


Soprano::Client::DBusTransaction::~DBusTransaction()
{
    delete d;
}


void Soprano::Client::DBusTransaction::setAsyncCalls( bool b )
{
    d->callMode = b ? QDBus::BlockWithGui : QDBus::Block;
}


bool Soprano::Client::DBusTransaction::asyncCalls() const
{
    return d->callMode == QDBus::BlockWithGui;
}


Soprano::Error::ErrorCode Soprano::Client::DBusTransaction::addStatement( const Statement &statement )
{
    QDBusReply<int> reply = d->modelInterface->addStatement( statement, d->callMode );
    setError( DBus::convertError( reply.error() ) );
    if ( lastError() ) {
        return Error::convertErrorCode( lastError().code() );
    }
    else {
        return ( Error::ErrorCode )reply.value();
    }
}


Soprano::NodeIterator Soprano::Client::DBusTransaction::listContexts() const
{
    QDBusReply<QString> reply = d->modelInterface->listContexts( d->callMode );
    setError( DBus::convertError( reply.error() ) );
    if ( reply.isValid() ) {
        return new DBusClientNodeIteratorBackend( d->modelInterface->service(), reply.value() );
    }
    else {
        return 0;
    }
}


Soprano::QueryResultIterator Soprano::Client::DBusTransaction::executeQuery( const QString& query, Query::QueryLanguage language, const QString& userQueryLanguage ) const
{
    QDBusReply<QString> reply = d->modelInterface->executeQuery( query, Query::queryLanguageToString( language, userQueryLanguage ), d->callMode );
    setError( DBus::convertError( reply.error() ) );
    if ( reply.isValid() ) {
        return new DBusClientQueryResultIteratorBackend( d->modelInterface->service(), reply.value() );
    }
    else {
        return 0;
    }
}


Soprano::StatementIterator Soprano::Client::DBusTransaction::listStatements( const Statement &partial ) const
{
    QDBusReply<QString> reply = d->modelInterface->listStatements( partial, d->callMode );
    setError( DBus::convertError( reply.error() ) );
    if ( reply.isValid() ) {
        return new DBusClientStatementIteratorBackend( d->modelInterface->service(), reply.value() );
    }
    else {
        return 0;
    }
}


Soprano::Error::ErrorCode Soprano::Client::DBusTransaction::removeStatement( const Statement &statement )
{
    QDBusReply<int> reply = d->modelInterface->removeStatement( statement, d->callMode );
    setError( DBus::convertError( reply.error() ) );
    if ( lastError() ) {
        return Error::convertErrorCode( lastError().code() );
    }
    else {
        return ( Error::ErrorCode )reply.value();
    }
}


Soprano::Error::ErrorCode Soprano::Client::DBusTransaction::removeAllStatements( const Statement &statement )
{
    QDBusReply<int> reply = d->modelInterface->removeAllStatements( statement, d->callMode );
    setError( DBus::convertError( reply.error() ) );
    if ( lastError() ) {
        return Error::convertErrorCode( lastError().code() );
    }
    else {
        return ( Error::ErrorCode )reply.value();
    }
}


int Soprano::Client::DBusTransaction::statementCount() const
{
    QDBusReply<int> reply = d->modelInterface->statementCount( d->callMode );
    setError( DBus::convertError( reply.error() ) );
    if ( lastError() ) {
        return 0;
    }
    else {
        return reply.value();
    }
}


bool Soprano::Client::DBusTransaction::isEmpty() const
{
    QDBusReply<bool> reply = d->modelInterface->isEmpty( d->callMode );
    setError( DBus::convertError( reply.error() ) );
    if ( lastError() ) {
        return 0;
    }
    else {
        return reply.value();
    }
}


bool Soprano::Client::DBusTransaction::containsStatement( const Statement &statement ) const
{
    QDBusReply<bool> reply = d->modelInterface->containsStatement( statement, d->callMode );
    setError( DBus::convertError( reply.error() ) );
    if ( lastError() ) {
        return false;
    }
    else {
        return reply.value();
    }
}


bool Soprano::Client::DBusTransaction::containsAnyStatement( const Statement &statement ) const
{
    QDBusReply<bool> reply = d->modelInterface->containsAnyStatement( statement, d->callMode );
    setError( DBus::convertError( reply.error() ) );
    if ( lastError() ) {
        return false;
    }
    else {
        return reply.value();
    }
}


Soprano::Node Soprano::Client::DBusTransaction::createBlankNode()
{
    QDBusReply<Node> reply = d->modelInterface->createBlankNode( d->callMode );
    setError( DBus::convertError( reply.error() ) );
    return reply.value();
}


Soprano::Error::ErrorCode Soprano::Client::DBusTransaction::doRollback()
{
    QDBusReply<int> reply = d->transactionInterface->rollback( d->callMode );
    setError( DBus::convertError( reply.error() ) );
    if ( lastError() ) {
        return Error::convertErrorCode( lastError().code() );
    }
    else {
        return ( Error::ErrorCode )reply.value();
    }
}


Soprano::Error::ErrorCode Soprano::Client::DBusTransaction::doCommit()
{
    QDBusReply<int> reply = d->transactionInterface->commit( d->callMode );
    setError( DBus::convertError( reply.error() ) );
    if ( lastError() ) {
        return Error::convertErrorCode( lastError().code() );
    }
    else {
        return ( Error::ErrorCode )reply.value();
    }
}

#include "dbustransaction.moc"
