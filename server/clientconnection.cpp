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

#include "clientconnection.h"
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


class Soprano::Server::ClientConnection::Private
{
public:
    Private()
        : socket( 0 ) {
    }

    QTcpSocket* socket;
    QMutex mutex;
};


Soprano::Server::ClientConnection::ClientConnection( QObject* parent )
    : QObject( parent ),
      d( new Private() )
{
    d->socket = new QTcpSocket( this );
    connect( d->socket, SIGNAL(error(QAbstractSocket::SocketError)),
             this, SLOT(slotError(QAbstractSocket::SocketError)) );
}


Soprano::Server::ClientConnection::~ClientConnection()
{
    close();
    delete d;
}


bool Soprano::Server::ClientConnection::open( const QHostAddress& address, quint16 port )
{
    d->socket->abort();
    d->socket->connectToHost( address, port );
    if ( !d->socket->waitForConnected() ) {
        setError( d->socket->errorString() );
        return false;
    }
    else {
        if ( !checkProtocolVersion() ) {
            close();
            return false;
        }
        else {
            return true;
        }
    }
}


bool Soprano::Server::ClientConnection::isOpen()
{
    return d->socket->state() == QAbstractSocket::ConnectedState;
}


void Soprano::Server::ClientConnection::close()
{
    d->socket->disconnectFromHost();
}


void Soprano::Server::ClientConnection::slotError( QAbstractSocket::SocketError error )
{
    qDebug() << "Error: " << error;
}


int Soprano::Server::ClientConnection::createModel( const QString& name, const QList<BackendSetting>& settings )
{
//    qDebug() << "(ClientConnection::createModel)";
    QMutexLocker( &d->mutex );

    QDataStream stream( d->socket );

    stream << COMMAND_CREATE_MODEL << name << settings;

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


Soprano::BackendFeatures Soprano::Server::ClientConnection::supportedFeatures()
{
//    qDebug() << "(ClientConnection::supportedFeatures)";
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


Soprano::Error::ErrorCode Soprano::Server::ClientConnection::addStatement( int modelId, const Statement &statement )
{
//    qDebug() << "(ClientConnection::addStatement)";
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


int Soprano::Server::ClientConnection::listContexts( int modelId )
{
//    qDebug() << "(ClientConnection::listContexts)";
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


int Soprano::Server::ClientConnection::executeQuery( int modelId, const QueryLegacy &query )
{
//    qDebug() << "(ClientConnection::executeQuery)";
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


int Soprano::Server::ClientConnection::listStatements( int modelId, const Statement &partial )
{
//    qDebug() << "(ClientConnection::listStatements)";
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


Soprano::Error::ErrorCode Soprano::Server::ClientConnection::removeAllStatements( int modelId, const Statement &statement )
{
//    qDebug() << "(ClientConnection::removeAllStatements)";
    QMutexLocker( &d->mutex );

    QDataStream stream( d->socket );

    stream << COMMAND_MODEL_REMOVE_ALL_STATEMENTS << ( quint32 )modelId << statement;

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


Soprano::Error::ErrorCode Soprano::Server::ClientConnection::removeStatement( int modelId, const Statement &statement )
{
//    qDebug() << "(ClientConnection::removeStatement)";
    QMutexLocker( &d->mutex );

    QDataStream stream( d->socket );

    stream << COMMAND_MODEL_REMOVE_STATEMENT << ( quint32 )modelId << statement;

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


int Soprano::Server::ClientConnection::statementCount( int modelId )
{
//    qDebug() << "(ClientConnection::statementCount)";
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


bool Soprano::Server::ClientConnection::containsStatement( int modelId, const Statement &statement )
{
//    qDebug() << "(ClientConnection::containsStatement)";
    QMutexLocker( &d->mutex );

    QDataStream stream( d->socket );

    stream << COMMAND_MODEL_CONTAINS_STATEMENT << ( quint32 )modelId << statement;

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


bool Soprano::Server::ClientConnection::containsAnyStatement( int modelId, const Statement &statement )
{
//    qDebug() << "(ClientConnection::containsAnyStatement)";
    QMutexLocker( &d->mutex );

    QDataStream stream( d->socket );

    stream << COMMAND_MODEL_CONTAINS_ANY_STATEMENT << ( quint32 )modelId << statement;

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


bool Soprano::Server::ClientConnection::isEmpty( int modelId )
{
//    qDebug() << "(ClientConnection::isEmpty)";
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


Soprano::Node Soprano::Server::ClientConnection::createBlankNode( int modelId )
{
//    qDebug() << "(ClientConnection::isEmpty)";
    QMutexLocker( &d->mutex );

    QDataStream stream( d->socket );

    stream << COMMAND_MODEL_CREATE_BLANK_NODE << ( quint32 )modelId;

    if ( !d->socket->waitForReadyRead() ) {
        setError( "Command timed out." );
        return Node();
    }

    Node n;
    Error::Error error;
    stream >> n >> error;

    setError( error );
    return n;
}


bool Soprano::Server::ClientConnection::iteratorNext( int id )
{
//    qDebug() << "(ClientConnection::iteratorNext)";
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


Soprano::Node Soprano::Server::ClientConnection::nodeIteratorCurrent( int id )
{
//    qDebug() << "(ClientConnection::nodeIteratorCurrent)";
    QMutexLocker( &d->mutex );

    QDataStream stream( d->socket );

    stream << COMMAND_ITERATOR_CURRENT_NODE << ( quint32 )id;

    if ( !d->socket->waitForReadyRead() ) {
        setError( "Command timed out." );
        return Node();
    }

    Node node;
    Error::Error error;
    stream >> node >> error;

    setError( error );
    return node;
}


Soprano::Statement Soprano::Server::ClientConnection::statementIteratorCurrent( int id )
{
//    qDebug() << "(ClientConnection::statementIteratorCurrent)";
    QMutexLocker( &d->mutex );

    QDataStream stream( d->socket );

    stream << COMMAND_ITERATOR_CURRENT_STATEMENT << ( quint32 )id;

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


Soprano::BindingSet Soprano::Server::ClientConnection::queryIteratorCurrent( int id )
{
//    qDebug() << "(ClientConnection::queryIteratorCurrent)";
    QMutexLocker( &d->mutex );

    QDataStream stream( d->socket );

    stream << COMMAND_ITERATOR_CURRENT_BINDINGSET << ( quint32 )id;

    if ( !d->socket->waitForReadyRead() ) {
        setError( "Command timed out." );
        return BindingSet();
    }

    BindingSet set;
    Error::Error error;
    stream >> set >> error;

    setError( error );
    return set;
}


Soprano::Statement Soprano::Server::ClientConnection::queryIteratorCurrentStatement( int id )
{
//    qDebug() << "(ClientConnection::queryIteratorCurrentStatement)";
    QMutexLocker( &d->mutex );

    QDataStream stream( d->socket );

    stream << COMMAND_ITERATOR_CURRENT_STATEMENT << ( quint32 )id;

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


int Soprano::Server::ClientConnection::queryIteratorType( int id )
{
//    qDebug() << "(ClientConnection::queryIteratorType)";
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


bool Soprano::Server::ClientConnection::queryIteratorBoolValue( int id )
{
//    qDebug() << "(ClientConnection::queryIteratorBoolValue)";
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


void Soprano::Server::ClientConnection::iteratorClose( int id )
{
//    qDebug() << "(ClientConnection::iteratorClose)";
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


bool Soprano::Server::ClientConnection::checkProtocolVersion()
{
    QMutexLocker( &d->mutex );

    QDataStream stream( d->socket );
    stream << COMMAND_SUPPORTS_PROTOCOL_VERSION << ( quint32 )PROTOCOL_VERSION;

    // wait for a reply, but not forever, in case we are connected to something unknown
    if ( !d->socket->waitForReadyRead() ) {
        setError( "Command timed out." );
        return false;
    }

    bool reply;
    stream >> reply;

    if ( reply ) {
        clearError();
    }
    else {
        setError( QString( "Server does not support protocol version %1" ).arg( PROTOCOL_VERSION ) );
    }
    return reply;
}

#include "clientconnection.moc"
