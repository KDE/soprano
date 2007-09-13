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

#include "dbusclientmodel.h"
#include "modelinterface.h"
#include "util.h"
#include "dbusclientnodeiteratorbackend.h"
#include "dbusclientstatementiteratorbackend.h"
#include "dbusclientqueryresultiteratorbackend.h"

#include <soprano/backend.h>
#include <soprano/error.h>
#include <soprano/querylegacy.h>
#include <soprano/nodeiterator.h>
#include <soprano/statementiterator.h>
#include <soprano/queryresultiterator.h>

#include <QDebug>

Soprano::Server::DBusClientModel::DBusClientModel( const QString& serviceName, const QString& dbusObject, const Backend* backend )
    : StorageModel( backend )
{
    m_interface = new DBusModelInterface( serviceName, dbusObject, QDBusConnection::sessionBus(), this );
}


Soprano::Server::DBusClientModel::~DBusClientModel()
{
}


Soprano::Error::ErrorCode Soprano::Server::DBusClientModel::addStatement( const Statement &statement )
{
    QDBusReply<int> reply = m_interface->addStatement( statement );
    setError( DBus::convertError( reply.error() ) );
    if ( lastError() ) {
        return Error::convertErrorCode( lastError().code() );
    }
    else {
        return ( Error::ErrorCode )reply.value();
    }
}


Soprano::NodeIterator Soprano::Server::DBusClientModel::listContexts() const
{
    QDBusReply<QString> reply = m_interface->listContexts();
    setError( DBus::convertError( reply.error() ) );
    if ( reply.isValid() ) {
        return new DBusClientNodeIteratorBackend( m_interface->service(), reply.value() );
    }
    else {
        return 0;
    }
}


Soprano::QueryResultIterator Soprano::Server::DBusClientModel::executeQuery( const QueryLegacy &query ) const
{
    QDBusReply<QString> reply = m_interface->executeQuery( query.query(), ( int )query.type() );
    setError( DBus::convertError( reply.error() ) );
    if ( reply.isValid() ) {
        return new DBusClientQueryResultIteratorBackend( m_interface->service(), reply.value() );
    }
    else {
        return 0;
    }
}


Soprano::StatementIterator Soprano::Server::DBusClientModel::listStatements( const Statement &partial ) const
{
    QDBusReply<QString> reply = m_interface->listStatements( partial );
    setError( DBus::convertError( reply.error() ) );
    if ( reply.isValid() ) {
        return new DBusClientStatementIteratorBackend( m_interface->service(), reply.value() );
    }
    else {
        return 0;
    }
}


Soprano::Error::ErrorCode Soprano::Server::DBusClientModel::removeStatement( const Statement &statement )
{
    QDBusReply<int> reply = m_interface->removeStatement( statement );
    setError( DBus::convertError( reply.error() ) );
    if ( lastError() ) {
        return Error::convertErrorCode( lastError().code() );
    }
    else {
        return ( Error::ErrorCode )reply.value();
    }
}


Soprano::Error::ErrorCode Soprano::Server::DBusClientModel::removeAllStatements( const Statement &statement )
{
    QDBusReply<int> reply = m_interface->removeAllStatements( statement );
    setError( DBus::convertError( reply.error() ) );
    if ( lastError() ) {
        return Error::convertErrorCode( lastError().code() );
    }
    else {
        return ( Error::ErrorCode )reply.value();
    }
}


int Soprano::Server::DBusClientModel::statementCount() const
{
    QDBusReply<int> reply = m_interface->statementCount();
    setError( DBus::convertError( reply.error() ) );
    if ( lastError() ) {
        return 0;
    }
    else {
        return reply.value();
    }
}


bool Soprano::Server::DBusClientModel::containsStatement( const Statement &statement ) const
{
    QDBusReply<bool> reply = m_interface->containsStatement( statement );
    setError( DBus::convertError( reply.error() ) );
    if ( lastError() ) {
        return false;
    }
    else {
        return reply.value();
    }
}


bool Soprano::Server::DBusClientModel::containsAnyStatement( const Statement &statement ) const
{
    QDBusReply<bool> reply = m_interface->containsAnyStatement( statement );
    setError( DBus::convertError( reply.error() ) );
    if ( lastError() ) {
        return false;
    }
    else {
        return reply.value();
    }
}


Soprano::Node Soprano::Server::DBusClientModel::createBlankNode()
{
    QDBusReply<Node> reply = m_interface->createBlankNode();
    setError( DBus::convertError( reply.error() ) );
    return reply.value();
}

#include "dbusclientmodel.moc"
