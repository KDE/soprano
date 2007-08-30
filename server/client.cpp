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

#include "client.h"
#include "operators.h"
#include "commands.h"

#include <soprano/node.h>
#include <soprano/statement.h>
#include <soprano/model.h>
#include <soprano/querylegacy.h>
#include <soprano/bindingset.h>
#include <soprano/backend.h>

#include <QtNetwork/QTcpSocket>
#include <QtCore/QMutex>
#include <QtCore/QMutexLocker>


class Soprano::Server::Client::Private
{
public:
    Private()
        : socket( 0 ) {
    }

    QTcpSocket* socket;
    QMutex mutex;
};


Soprano::Server::Client::Client( QObject* parent )
    : QObject( parent ),
      d( new Private() )
{
    d->socket = new QTcpSocket( this );
    connect( d->socket, SIGNAL(error(QAbstractSocket::SocketError)),
             this, SLOT(slotError(QAbstractSocket::SocketError)) );
}


Soprano::Server::Client::~Client()
{
    close();
    delete d;
}


bool Soprano::Server::Client::open( const QHostAddress& address, quint16 port )
{
    d->socket->abort();
    d->socket->connectToHost( address, port );
    if ( !d->socket->waitForConnected() ) {
        setError( d->socket->errorString() );
        return false;
    }
    else {
        clearError();
        return true;
    }
}


bool Soprano::Server::Client::isOpen()
{
    return d->socket->state() == QAbstractSocket::ConnectedState;
}


void Soprano::Server::Client::close()
{
    d->socket->disconnectFromHost();
}


void Soprano::Server::Client::slotError( QAbstractSocket::SocketError error )
{
    qDebug() << "Error: " << error;
}


int Soprano::Server::Client::createModel( const QList<BackendSetting>& settings )
{
//    qDebug() << "(Client::createModel)";
    QMutexLocker( &d->mutex );

    QDataStream stream( d->socket );

    stream << COMMAND_CREATE_MODEL << settings;

    if ( !d->socket->waitForReadyRead() ) {
        setError( "Command timed out." );
        return 0;
    }

    quint32 id;
    Error::Error error;

    stream >> id >> error;

    setError( error );
    return id;
}


Soprano::BackendFeatures Soprano::Server::Client::supportedFeatures()
{
//    qDebug() << "(Client::supportedFeatures)";
    QMutexLocker( &d->mutex );

    QDataStream stream( d->socket );

    stream << COMMAND_SUPPORTED_FEATURES;

    if ( !d->socket->waitForReadyRead() ) {
        setError( "Command timed out." );
        return 0;
    }

    quint32 features;
    Error::Error error;

    stream >> features >> error;

    setError( error );
    return ( BackendFeatures )features;
}


Soprano::Error::ErrorCode Soprano::Server::Client::addStatement( int modelId, const Statement &statement )
{
//    qDebug() << "(Client::addStatement)";
    QMutexLocker( &d->mutex );

    QDataStream stream( d->socket );

    stream << COMMAND_MODEL_ADD_STATEMENT << ( quint32 )modelId << statement;

    if ( !d->socket->waitForReadyRead() ) {
        setError( "Command timed out." );
        return Error::ERROR_UNKNOWN;
    }

    Error::ErrorCode ec;
    Error::Error error;
    stream >> ec >> error;

    setError( error );
    return ec;
}


int Soprano::Server::Client::listContexts( int modelId )
{
//    qDebug() << "(Client::listContexts)";
    QMutexLocker( &d->mutex );

    QDataStream stream( d->socket );

    stream << COMMAND_MODEL_LIST_CONTEXTS << ( quint32 )modelId;

    if ( !d->socket->waitForReadyRead() ) {
        setError( "Command timed out." );
        return Error::ERROR_UNKNOWN;
    }

    quint32 itId;
    Error::Error error;
    stream >> itId >> error;

    setError( error );
    return itId;
}


int Soprano::Server::Client::executeQuery( int modelId, const QueryLegacy &query )
{
//    qDebug() << "(Client::executeQuery)";
    QMutexLocker( &d->mutex );

    QDataStream stream( d->socket );

    stream << COMMAND_MODEL_QUERY << ( quint32 )modelId << query.query() << ( quint16 )query.type();

    if ( !d->socket->waitForReadyRead() ) {
        setError( "Command timed out." );
        return Error::ERROR_UNKNOWN;
    }

    quint32 itId;
    Error::Error error;
    stream >> itId >> error;

    setError( error );
    return itId;
}


int Soprano::Server::Client::listStatements( int modelId, const Statement &partial )
{
//    qDebug() << "(Client::listStatements)";
    QMutexLocker( &d->mutex );

    QDataStream stream( d->socket );

    stream << COMMAND_MODEL_LIST_STATEMENTS << ( quint32 )modelId << partial;

    if ( !d->socket->waitForReadyRead() ) {
        setError( "Command timed out." );
        return Error::ERROR_UNKNOWN;
    }

    quint32 itId;
    Error::Error error;
    stream >> itId >> error;

    setError( error );
    return itId;
}


Soprano::Error::ErrorCode Soprano::Server::Client::removeStatements( int modelId, const Statement &statement )
{
//    qDebug() << "(Client::removeStatements)";
    QMutexLocker( &d->mutex );

    QDataStream stream( d->socket );

    stream << COMMAND_MODEL_REMOVE_STATEMENTS << ( quint32 )modelId << statement;

    if ( !d->socket->waitForReadyRead() ) {
        setError( "Command timed out." );
        return Error::ERROR_UNKNOWN;
    }

    Error::ErrorCode ec;
    Error::Error error;
    stream >> ec >> error;

    setError( error );
    return ec;
}


int Soprano::Server::Client::statementCount( int modelId )
{
//    qDebug() << "(Client::statementCount)";
    QMutexLocker( &d->mutex );

    QDataStream stream( d->socket );

    stream << COMMAND_MODEL_STATEMENT_COUNT << ( quint32 )modelId;

    if ( !d->socket->waitForReadyRead() ) {
        setError( "Command timed out." );
        return Error::ERROR_UNKNOWN;
    }

    qint32 count;
    Error::Error error;
    stream >> count >> error;

    setError( error );
    return count;
}


bool Soprano::Server::Client::containsStatements( int modelId, const Statement &statement )
{
//    qDebug() << "(Client::containsStatements)";
    QMutexLocker( &d->mutex );

    QDataStream stream( d->socket );

    stream << COMMAND_MODEL_CONTAINS_STATEMENTS << ( quint32 )modelId << statement;

    if ( !d->socket->waitForReadyRead() ) {
        setError( "Command timed out." );
        return false;
    }

    bool r;
    Error::Error error;
    stream >> r >> error;

    setError( error );
    return r;
}


bool Soprano::Server::Client::isEmpty( int modelId )
{
//    qDebug() << "(Client::isEmpty)";
    QMutexLocker( &d->mutex );

    QDataStream stream( d->socket );

    stream << COMMAND_MODEL_IS_EMPTY << ( quint32 )modelId;

    if ( !d->socket->waitForReadyRead() ) {
        setError( "Command timed out." );
        return false;
    }

    bool r;
    Error::Error error;
    stream >> r >> error;

    setError( error );
    return r;
}


bool Soprano::Server::Client::iteratorNext( int id )
{
//    qDebug() << "(Client::iteratorNext)";
    QMutexLocker( &d->mutex );

    QDataStream stream( d->socket );

    stream << COMMAND_ITERATOR_NEXT << ( quint32 )id;

    if ( !d->socket->waitForReadyRead() ) {
        setError( "Command timed out." );
        return false;
    }

    bool r;
    Error::Error error;
    stream >> r >> error;

    setError( error );
    return r;
}


Soprano::Node Soprano::Server::Client::nodeIteratorCurrent( int id )
{
//    qDebug() << "(Client::nodeIteratorCurrent)";
    QMutexLocker( &d->mutex );

    QDataStream stream( d->socket );

    stream << COMMAND_ITERATOR_CURRENT << ( quint32 )id;

    if ( !d->socket->waitForReadyRead() ) {
        setError( "Command timed out." );
        return Node();
    }

    // special case: see ServerConnection::Private::iteratorCurrent
    Node node;
    Error::Error error;
    stream >> error;
    if ( !error ) {
        stream >> node;
    }
    setError( error );
    return node;
}


Soprano::Statement Soprano::Server::Client::statementIteratorCurrent( int id )
{
//    qDebug() << "(Client::statementIteratorCurrent)";
    QMutexLocker( &d->mutex );

    QDataStream stream( d->socket );

    stream << COMMAND_ITERATOR_CURRENT << ( quint32 )id;

    if ( !d->socket->waitForReadyRead() ) {
        setError( "Command timed out." );
        return Statement();
    }

    // special case: see ServerConnection::Private::iteratorCurrent
    Statement statement;
    Error::Error error;
    stream >> error;
    if ( !error ) {
        stream >> statement;
    }

    setError( error );
    return statement;
}


Soprano::BindingSet Soprano::Server::Client::queryIteratorCurrent( int id )
{
//    qDebug() << "(Client::queryIteratorCurrent)";
    QMutexLocker( &d->mutex );

    QDataStream stream( d->socket );

    stream << COMMAND_ITERATOR_CURRENT << ( quint32 )id;

    if ( !d->socket->waitForReadyRead() ) {
        setError( "Command timed out." );
        return BindingSet();
    }

    // special case: see ServerConnection::Private::iteratorCurrent
    BindingSet set;
    Error::Error error;
    stream >> error;
    if ( !error ) {
        stream >> set;
    }

    setError( error );
    return set;
}


Soprano::Statement Soprano::Server::Client::queryIteratorCurrentStatement( int id )
{
//    qDebug() << "(Client::queryIteratorCurrentStatement)";
    QMutexLocker( &d->mutex );

    QDataStream stream( d->socket );

    stream << COMMAND_ITERATOR_QUERY_CURRENT_STATEMENT << ( quint32 )id;

    if ( !d->socket->waitForReadyRead() ) {
        setError( "Command timed out." );
        return Statement();
    }

    Statement statement;
    Error::Error error;
    stream >> statement >> error;

    setError( error );
    return statement;
}


int Soprano::Server::Client::queryIteratorType( int id )
{
//    qDebug() << "(Client::queryIteratorType)";
    QMutexLocker( &d->mutex );

    QDataStream stream( d->socket );

    stream << COMMAND_ITERATOR_QUERY_TYPE << ( quint32 )id;

    if ( !d->socket->waitForReadyRead() ) {
        setError( "Command timed out." );
        return 0;
    }

    quint8 type;
    Error::Error error;
    stream >> type >> error;

    setError( error );
    return type;
}


bool Soprano::Server::Client::queryIteratorBoolValue( int id )
{
//    qDebug() << "(Client::queryIteratorBoolValue)";
    QMutexLocker( &d->mutex );

    QDataStream stream( d->socket );

    stream << COMMAND_ITERATOR_QUERY_BOOL_VALUE << ( quint32 )id;

    if ( !d->socket->waitForReadyRead() ) {
        setError( "Command timed out." );
        return false;
    }

    bool v;
    Error::Error error;
    stream >> v >> error;

    setError( error );
    return v;
}


void Soprano::Server::Client::iteratorClose( int id )
{
//    qDebug() << "(Client::iteratorClose)";
    QMutexLocker( &d->mutex );

    QDataStream stream( d->socket );

    stream << COMMAND_ITERATOR_CLOSE << ( quint32 )id;

    if ( !d->socket->waitForReadyRead() ) {
        setError( "Command timed out." );
        return;
    }

    Error::Error error;
    stream >> error;

    setError( error );
}

#include "client.moc"
