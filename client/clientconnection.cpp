/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2007-2012 Sebastian Trueg <trueg@kde.org>
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
#include "clientconnection_p.h"
#include "commands.h"
#include "socketstream.h"
#include "socket.h"

#include "node.h"
#include "statement.h"
#include "model.h"
#include "bindingset.h"
#include "backend.h"

#include <QtCore/QThread>
#include <QtCore/QMutex>
#include <QtCore/QMutexLocker>
#include <QtCore/QTime>
#include <QtCore/QHash>


using namespace Soprano::Server;

namespace {
    const int s_defaultTimeout = 600000;
}


Soprano::Client::ClientConnection::ClientConnection( QObject* parent )
    : QObject( parent ),
      d( new ClientConnectionPrivate() )
{
    d->socket = 0;
}


Soprano::Client::ClientConnection::~ClientConnection()
{
    delete d->socket;
    delete d;
}


Soprano::Socket *Soprano::Client::ClientConnection::getSocket()
{
    return d->socket;
}


int Soprano::Client::ClientConnection::createModel( const QString& name, const QList<BackendSetting>& settings )
{
    Q_UNUSED( settings );
    //qDebug() << this << QTime::currentTime().toString( "hh:mm:ss.zzz" ) << QThread::currentThreadId() << "(ClientConnection::createModel)";

    Socket* socket = getSocket();
    if ( !socket )
        return 0;
    SocketStream stream( socket );

    if (!stream.writeUnsignedInt16(COMMAND_CREATE_MODEL) ||
        !stream.writeString(name)) {
        setError( "Write error", Soprano::Error::ErrorTimeout );
        socket->close();
        return 0;
    }

    if ( !socket->waitForReadyRead(s_defaultTimeout) ) {
        setError( "Command timed out.", Soprano::Error::ErrorTimeout );
        // We cannot recover from a timeout, thus we force a reconnect
        socket->close();
        return 0;
    }

    quint32 id;
    Error::Error error;

    stream.readUnsignedInt32( id );
    stream.readError( error );

    setError( error );
    //qDebug() << this << QTime::currentTime().toString( "hh:mm:ss.zzz" ) << QThread::currentThreadId() << "(ClientConnection::createModel) end";
    return id;
}


void Soprano::Client::ClientConnection::removeModel( const QString& name )
{
    //qDebug() << this << QTime::currentTime().toString( "hh:mm:ss.zzz" ) << QThread::currentThreadId() << "(ClientConnection::removeModel)";

    Socket* socket = getSocket();
    if ( !socket )
        return;
    SocketStream stream( socket );

    if (!stream.writeUnsignedInt16(COMMAND_REMOVE_MODEL) ||
        !stream.writeString(name)) {
        setError( "Write error", Soprano::Error::ErrorTimeout );
        socket->close();
        return;
    }

    if ( !socket->waitForReadyRead(s_defaultTimeout) ) {
        setError( "Command timed out.", Soprano::Error::ErrorTimeout );
        // We cannot recover from a timeout, thus we force a reconnect
        socket->close();
        return;
    }

    Error::Error error;

    stream.readError( error );

    setError( error );
    //qDebug() << this << QTime::currentTime().toString( "hh:mm:ss.zzz" ) << QThread::currentThreadId() << "(ClientConnection::removeModel) end";
}


Soprano::BackendFeatures Soprano::Client::ClientConnection::supportedFeatures()
{
    //qDebug() << this << QTime::currentTime().toString( "hh:mm:ss.zzz" ) << QThread::currentThreadId() << "(ClientConnection::supportedFeatures)";

    Socket* socket = getSocket();
    if ( !socket )
        return BackendFeatureNone;
    SocketStream stream( socket );

    if (!stream.writeUnsignedInt16(COMMAND_SUPPORTED_FEATURES)) {
        setError( "Write error", Soprano::Error::ErrorTimeout );
        socket->close();
        return 0;
    }

    if ( !socket->waitForReadyRead(s_defaultTimeout) ) {
        setError( "Command timed out.", Soprano::Error::ErrorTimeout );
        // We cannot recover from a timeout, thus we force a reconnect
        socket->close();
        return 0;
    }

    quint32 features;
    Error::Error error;

    stream.readUnsignedInt32( features );
    stream.readError( error );

    setError( error );
    //qDebug() << this << QTime::currentTime().toString( "hh:mm:ss.zzz" ) << QThread::currentThreadId() << "(ClientConnection::supportedFeatures) end";
    return ( BackendFeatures )features;
}


Soprano::Error::ErrorCode Soprano::Client::ClientConnection::addStatement( int modelId, const Statement &statement )
{
    //qDebug() << this << QTime::currentTime().toString( "hh:mm:ss.zzz" ) << QThread::currentThreadId() << "(ClientConnection::addStatement)";

    Socket* socket = getSocket();
    if ( !socket )
        return Error::convertErrorCode( lastError().code() );
    SocketStream stream( socket );

    if (!stream.writeUnsignedInt16( COMMAND_MODEL_ADD_STATEMENT ) ||
        !stream.writeUnsignedInt32( ( quint32 )modelId ) ||
        !stream.writeStatement(statement)) {
        setError( "Write error", Soprano::Error::ErrorTimeout );
        socket->close();
        return Error::ErrorUnknown;
    }

    if ( !socket->waitForReadyRead(s_defaultTimeout) ) {
        setError( "Command timed out.", Soprano::Error::ErrorTimeout );
        // We cannot recover from a timeout, thus we force a reconnect
        socket->close();
        return Error::ErrorUnknown;
    }

    Error::ErrorCode ec;
    Error::Error error;
    stream.readErrorCode( ec );
    stream.readError( error );

    setError( error );
    //qDebug() << this << QTime::currentTime().toString( "hh:mm:ss.zzz" ) << QThread::currentThreadId() << "(ClientConnection::addStatement) end";
    return ec;
}


int Soprano::Client::ClientConnection::listContexts( int modelId )
{
    //qDebug() << this << QTime::currentTime().toString( "hh:mm:ss.zzz" ) << QThread::currentThreadId() << "(ClientConnection::listContexts)";

    Socket* socket = getSocket();
    if ( !socket )
        return 0;
    SocketStream stream( socket );

    if (!stream.writeUnsignedInt16( COMMAND_MODEL_LIST_CONTEXTS ) ||
        !stream.writeUnsignedInt32( ( quint32 )modelId ) ) {
        setError( "Write error", Soprano::Error::ErrorTimeout );
        socket->close();
        return 0;
    }

    if ( !socket->waitForReadyRead(s_defaultTimeout) ) {
        setError( "Command timed out.", Soprano::Error::ErrorTimeout );
        // We cannot recover from a timeout, thus we force a reconnect
        socket->close();
        return 0;
    }

    quint32 itId;
    Error::Error error;
    stream.readUnsignedInt32( itId );
    stream.readError( error );

    setError( error );
    //qDebug() << this << QTime::currentTime().toString( "hh:mm:ss.zzz" ) << QThread::currentThreadId() << "(ClientConnection::listContexts) end";
    return itId;
}


int Soprano::Client::ClientConnection::executeQuery( int modelId, const QString &query, Query::QueryLanguage type, const QString& userQueryLanguage )
{
    //qDebug() << this << QTime::currentTime().toString( "hh:mm:ss.zzz" ) << QThread::currentThreadId() << "(ClientConnection::executeQuery)";

    Socket* socket = getSocket();
    if ( !socket )
        return 0;
    SocketStream stream( socket );

    if (!stream.writeUnsignedInt16( COMMAND_MODEL_QUERY ) ||
        !stream.writeUnsignedInt32( ( quint32 )modelId ) ||
        !stream.writeString( query ) ||
        !stream.writeUnsignedInt16( ( quint16 )type ) ||
        !stream.writeString( userQueryLanguage ) ) {
        setError( "Write error", Soprano::Error::ErrorTimeout );
        socket->close();
        return 0;
    }

    if ( !socket->waitForReadyRead(s_defaultTimeout) ) {
        setError( "Command timed out.", Soprano::Error::ErrorTimeout );
        // We cannot recover from a timeout, thus we force a reconnect
        socket->close();
        return 0;
    }

    quint32 itId;
    Error::Error error;
    stream.readUnsignedInt32( itId );
    stream.readError( error );

    setError( error );
    //qDebug() << this << QTime::currentTime().toString( "hh:mm:ss.zzz" ) << QThread::currentThreadId() << "(ClientConnection::executeQuery) end";
    return itId;
}


int Soprano::Client::ClientConnection::listStatements( int modelId, const Statement &partial )
{
    //qDebug() << this << QTime::currentTime().toString( "hh:mm:ss.zzz" ) << QThread::currentThreadId() << "(ClientConnection::listStatements)";

    Socket* socket = getSocket();
    if ( !socket )
        return 0;
    SocketStream stream( socket );

    if (!stream.writeUnsignedInt16( COMMAND_MODEL_LIST_STATEMENTS ) ||
        !stream.writeUnsignedInt32( ( quint32 )modelId ) ||
        !stream.writeStatement( partial ) ) {
        setError( "Write error", Soprano::Error::ErrorTimeout );
        socket->close();
        return 0;
    }

    if ( !socket->waitForReadyRead(s_defaultTimeout) ) {
        setError( "Command timed out.", Soprano::Error::ErrorTimeout );
        // We cannot recover from a timeout, thus we force a reconnect
        socket->close();
        return 0;
    }

    quint32 itId;
    Error::Error error;
    stream.readUnsignedInt32( itId );
    stream.readError( error );

    setError( error );
    //qDebug() << this << QTime::currentTime().toString( "hh:mm:ss.zzz" ) << QThread::currentThreadId() << "(ClientConnection::listStatements) end";
    return itId;
}


Soprano::Error::ErrorCode Soprano::Client::ClientConnection::removeAllStatements( int modelId, const Statement &statement )
{
    //qDebug() << this << QTime::currentTime().toString( "hh:mm:ss.zzz" ) << QThread::currentThreadId() << "(ClientConnection::removeAllStatements)";

    Socket* socket = getSocket();
    if ( !socket )
        return Error::convertErrorCode( lastError().code() );
    SocketStream stream( socket );

    if (!stream.writeUnsignedInt16( COMMAND_MODEL_REMOVE_ALL_STATEMENTS ) ||
        !stream.writeUnsignedInt32( ( quint32 )modelId ) ||
        !stream.writeStatement( statement ) ) {
        setError( "Write error", Soprano::Error::ErrorTimeout );
        socket->close();
        return Error::ErrorUnknown;
    }

    if ( !socket->waitForReadyRead(s_defaultTimeout) ) {
        setError( "Command timed out.", Soprano::Error::ErrorTimeout );
        // We cannot recover from a timeout, thus we force a reconnect
        socket->close();
        return Error::ErrorUnknown;
    }

    Error::ErrorCode ec;
    Error::Error error;
    stream.readErrorCode( ec );
    stream.readError( error );

    setError( error );
    //qDebug() << this << QTime::currentTime().toString( "hh:mm:ss.zzz" ) << QThread::currentThreadId() << "(ClientConnection::removeAllStatements) end";
    return ec;
}


Soprano::Error::ErrorCode Soprano::Client::ClientConnection::removeStatement( int modelId, const Statement &statement )
{
    //qDebug() << this << QTime::currentTime().toString( "hh:mm:ss.zzz" ) << QThread::currentThreadId() << "(ClientConnection::removeStatement)";

    Socket* socket = getSocket();
    if ( !socket )
        return Error::convertErrorCode( lastError().code() );
    SocketStream stream( socket );

    if (!stream.writeUnsignedInt16( COMMAND_MODEL_REMOVE_STATEMENT ) ||
        !stream.writeUnsignedInt32( ( quint32 )modelId ) ||
        !stream.writeStatement( statement ) ) {
        setError( "Write error", Soprano::Error::ErrorTimeout );
        socket->close();
        return Error::ErrorUnknown;
    }

    if ( !socket->waitForReadyRead(s_defaultTimeout) ) {
        setError( "Command timed out.", Soprano::Error::ErrorTimeout );
        // We cannot recover from a timeout, thus we force a reconnect
        socket->close();
        return Error::ErrorUnknown;
    }

    Error::ErrorCode ec;
    Error::Error error;
    stream.readErrorCode( ec );
    stream.readError( error );

    setError( error );
    //qDebug() << this << QTime::currentTime().toString( "hh:mm:ss.zzz" ) << QThread::currentThreadId() << "(ClientConnection::removeStatement) end";
    return ec;
}


int Soprano::Client::ClientConnection::statementCount( int modelId )
{
    //qDebug() << this << QTime::currentTime().toString( "hh:mm:ss.zzz" ) << QThread::currentThreadId() << "(ClientConnection::statementCount)";

    Socket* socket = getSocket();
    if ( !socket )
        return -1;
    SocketStream stream( socket );

    if (!stream.writeUnsignedInt16( COMMAND_MODEL_STATEMENT_COUNT ) ||
        !stream.writeUnsignedInt32( ( quint32 )modelId ) ) {
        setError( "Write error", Soprano::Error::ErrorTimeout );
        socket->close();
        return -1;
    }

    if ( !socket->waitForReadyRead(s_defaultTimeout) ) {
        setError( "Command timed out.", Soprano::Error::ErrorTimeout );
        // We cannot recover from a timeout, thus we force a reconnect
        socket->close();
        return -1;
    }

    qint32 count;
    Error::Error error;
    stream.readInt32( count );
    stream.readError( error );

    setError( error );
    //qDebug() << this << QTime::currentTime().toString( "hh:mm:ss.zzz" ) << QThread::currentThreadId() << "(ClientConnection::statementCount) end";
    return count;
}


bool Soprano::Client::ClientConnection::containsStatement( int modelId, const Statement &statement )
{
    //qDebug() << this << QTime::currentTime().toString( "hh:mm:ss.zzz" ) << QThread::currentThreadId() << "(ClientConnection::containsStatement)";

    Socket* socket = getSocket();
    if ( !socket )
        return false;
    SocketStream stream( socket );

    if (!stream.writeUnsignedInt16( COMMAND_MODEL_CONTAINS_STATEMENT ) ||
        !stream.writeUnsignedInt32( ( quint32 )modelId ) ||
        !stream.writeStatement( statement ) ) {
        setError( "Write error", Soprano::Error::ErrorTimeout );
        socket->close();
        return false;
    }

    if ( !socket->waitForReadyRead(s_defaultTimeout) ) {
        setError( "Command timed out.", Soprano::Error::ErrorTimeout );
        // We cannot recover from a timeout, thus we force a reconnect
        socket->close();
        return false;
    }

    bool r;
    Error::Error error;
    stream.readBool( r );
    stream.readError( error );

    setError( error );
    //qDebug() << this << QTime::currentTime().toString( "hh:mm:ss.zzz" ) << QThread::currentThreadId() << "(ClientConnection::containsStatement) end";
    return r;
}


bool Soprano::Client::ClientConnection::containsAnyStatement( int modelId, const Statement &statement )
{
    //qDebug() << this << QTime::currentTime().toString( "hh:mm:ss.zzz" ) << QThread::currentThreadId() << "(ClientConnection::containsAnyStatement)";

    Socket* socket = getSocket();
    if ( !socket )
        return false;
    SocketStream stream( socket );

    if (!stream.writeUnsignedInt16( COMMAND_MODEL_CONTAINS_ANY_STATEMENT ) ||
        !stream.writeUnsignedInt32( ( quint32 )modelId ) ||
        !stream.writeStatement( statement ) ) {
        setError( "Write error", Soprano::Error::ErrorTimeout );
        socket->close();
        return false;
    }

    if ( !socket->waitForReadyRead(s_defaultTimeout) ) {
        setError( "Command timed out.", Soprano::Error::ErrorTimeout );
        // We cannot recover from a timeout, thus we force a reconnect
        socket->close();
        return false;
    }

    bool r;
    Error::Error error;
    stream.readBool( r );
    stream.readError( error );

    setError( error );
    //qDebug() << this << QTime::currentTime().toString( "hh:mm:ss.zzz" ) << QThread::currentThreadId() << "(ClientConnection::containsAnyStatement) end";
    return r;
}


bool Soprano::Client::ClientConnection::isEmpty( int modelId )
{
    //qDebug() << this << QTime::currentTime().toString( "hh:mm:ss.zzz" ) << QThread::currentThreadId() << "(ClientConnection::isEmpty)";

    Socket* socket = getSocket();
    if ( !socket )
        return false;
    SocketStream stream( socket );

    if (!stream.writeUnsignedInt16( COMMAND_MODEL_IS_EMPTY ) ||
        !stream.writeUnsignedInt32( ( quint32 )modelId )) {
        setError( "Write error", Soprano::Error::ErrorTimeout );
        socket->close();
        return false;
    }

    if ( !socket->waitForReadyRead(s_defaultTimeout) ) {
        setError( "Command timed out.", Soprano::Error::ErrorTimeout );
        // We cannot recover from a timeout, thus we force a reconnect
        socket->close();
        return false;
    }

    bool r;
    Error::Error error;
    stream.readBool( r );
    stream.readError( error );

    setError( error );
    //qDebug() << this << QTime::currentTime().toString( "hh:mm:ss.zzz" ) << QThread::currentThreadId() << "(ClientConnection::isEmpty) end";
    return r;
}


Soprano::Node Soprano::Client::ClientConnection::createBlankNode( int modelId )
{
    //qDebug() << this << QTime::currentTime().toString( "hh:mm:ss.zzz" ) << QThread::currentThreadId() << "(ClientConnection::createBlankNode)";

    Socket* socket = getSocket();
    if ( !socket )
        return Node();
    SocketStream stream( socket );

    if (!stream.writeUnsignedInt16( COMMAND_MODEL_CREATE_BLANK_NODE ) ||
        !stream.writeUnsignedInt32( ( quint32 )modelId ) ) {
        setError( "Write error", Soprano::Error::ErrorTimeout );
        socket->close();
        return Node();
    }

    if ( !socket->waitForReadyRead(s_defaultTimeout) ) {
        setError( "Command timed out.", Soprano::Error::ErrorTimeout );
        // We cannot recover from a timeout, thus we force a reconnect
        socket->close();
        return Node();
    }

    Node n;
    Error::Error error;
    stream.readNode( n );
    stream.readError( error );

    setError( error );
    //qDebug() << this << QTime::currentTime().toString( "hh:mm:ss.zzz" ) << QThread::currentThreadId() << "(ClientConnection::createBlankNode) end";
    return n;
}


bool Soprano::Client::ClientConnection::iteratorNext( int id )
{
    //qDebug() << this << QTime::currentTime().toString( "hh:mm:ss.zzz" ) << QThread::currentThreadId() << "(ClientConnection::iteratorNext)";

    Socket* socket = getSocket();
    if ( !socket )
        return false;
    SocketStream stream( socket );

    if (!stream.writeUnsignedInt16( COMMAND_ITERATOR_NEXT ) ||
        !stream.writeUnsignedInt32( ( quint32 )id ) ) {
        setError( "Write error", Soprano::Error::ErrorTimeout );
        socket->close();
        return false;
    }

    if ( !socket->waitForReadyRead(s_defaultTimeout) ) {
        setError( "Command timed out.", Soprano::Error::ErrorTimeout );
        // We cannot recover from a timeout, thus we force a reconnect
        socket->close();
        return false;
    }

    bool r;
    Error::Error error;
    stream.readBool( r );
    stream.readError( error );

    setError( error );
    //qDebug() << this << QTime::currentTime().toString( "hh:mm:ss.zzz" ) << QThread::currentThreadId() << "(ClientConnection::iteratorNext) end";
    return r;
}


Soprano::Node Soprano::Client::ClientConnection::nodeIteratorCurrent( int id )
{
    //qDebug() << this << QTime::currentTime().toString( "hh:mm:ss.zzz" ) << QThread::currentThreadId() << "(ClientConnection::nodeIteratorCurrent)";

    Socket* socket = getSocket();
    if ( !socket )
        return Node();
    SocketStream stream( socket );

    if (!stream.writeUnsignedInt16( COMMAND_ITERATOR_CURRENT_NODE ) ||
        !stream.writeUnsignedInt32( ( quint32 )id ) ) {
        setError( "Write error", Soprano::Error::ErrorTimeout );
        socket->close();
        return Node();
    }

    if ( !socket->waitForReadyRead(s_defaultTimeout) ) {
        setError( "Command timed out.", Soprano::Error::ErrorTimeout );
        // We cannot recover from a timeout, thus we force a reconnect
        socket->close();
        return Node();
    }

    Node node;
    Error::Error error;
    stream.readNode( node );
    stream.readError( error );

    setError( error );
    //qDebug() << this << QTime::currentTime().toString( "hh:mm:ss.zzz" ) << QThread::currentThreadId() << "(ClientConnection::nodeIteratorCurrent) end";
    return node;
}


Soprano::Statement Soprano::Client::ClientConnection::statementIteratorCurrent( int id )
{
    //qDebug() << this << QTime::currentTime().toString( "hh:mm:ss.zzz" ) << QThread::currentThreadId() << "(ClientConnection::statementIteratorCurrent)";

    Socket* socket = getSocket();
    if ( !socket )
        return Statement();
    SocketStream stream( socket );

    if (!stream.writeUnsignedInt16( COMMAND_ITERATOR_CURRENT_STATEMENT ) ||
        !stream.writeUnsignedInt32( ( quint32 )id ) ) {
        setError( "Write error", Soprano::Error::ErrorTimeout );
        socket->close();
        return Statement();
    }

    if ( !socket->waitForReadyRead(s_defaultTimeout) ) {
        setError( "Command timed out.", Soprano::Error::ErrorTimeout );
        // We cannot recover from a timeout, thus we force a reconnect
        socket->close();
        return Statement();
    }

    Statement statement;
    Error::Error error;
    stream.readStatement( statement );
    stream.readError( error );

    setError( error );
    //qDebug() << this << QTime::currentTime().toString( "hh:mm:ss.zzz" ) << QThread::currentThreadId() << "(ClientConnection::statementIteratorCurrent) end";
    return statement;
}


Soprano::BindingSet Soprano::Client::ClientConnection::queryIteratorCurrent( int id )
{
    //qDebug() << this << QTime::currentTime().toString( "hh:mm:ss.zzz" ) << QThread::currentThreadId() << "(ClientConnection::queryIteratorCurrent)";

    Socket* socket = getSocket();
    if ( !socket )
        return BindingSet();
    SocketStream stream( socket );

    if (!stream.writeUnsignedInt16( COMMAND_ITERATOR_CURRENT_BINDINGSET ) ||
        !stream.writeUnsignedInt32( ( quint32 )id ) ) {
        setError( "Write error", Soprano::Error::ErrorTimeout );
        socket->close();
        return BindingSet();
    }

    if ( !socket->waitForReadyRead(s_defaultTimeout) ) {
        setError( "Command timed out.", Soprano::Error::ErrorTimeout );
        // We cannot recover from a timeout, thus we force a reconnect
        socket->close();
        return BindingSet();
    }

    BindingSet set;
    Error::Error error;
    stream.readBindingSet( set );
    stream.readError( error );

    setError( error );
    //qDebug() << this << QTime::currentTime().toString( "hh:mm:ss.zzz" ) << QThread::currentThreadId() << "(ClientConnection::queryIteratorCurrent) end";
    return set;
}


Soprano::Statement Soprano::Client::ClientConnection::queryIteratorCurrentStatement( int id )
{
    //qDebug() << this << QTime::currentTime().toString( "hh:mm:ss.zzz" ) << QThread::currentThreadId() << "(ClientConnection::queryIteratorCurrentStatement)";

    Socket* socket = getSocket();
    if ( !socket )
        return Statement();
    SocketStream stream( socket );

    if (!stream.writeUnsignedInt16( COMMAND_ITERATOR_CURRENT_STATEMENT ) ||
        !stream.writeUnsignedInt32( ( quint32 )id ) ) {
        setError( "Write error", Soprano::Error::ErrorTimeout );
        socket->close();
        return Statement();
    }

    if ( !socket->waitForReadyRead(s_defaultTimeout) ) {
        setError( "Command timed out.", Soprano::Error::ErrorTimeout );
        // We cannot recover from a timeout, thus we force a reconnect
        socket->close();
        return Statement();
    }

    Statement statement;
    Error::Error error;
    stream.readStatement( statement );
    stream.readError( error );

    setError( error );
    return statement;
}


int Soprano::Client::ClientConnection::queryIteratorType( int id )
{
    //qDebug() << this << QTime::currentTime().toString( "hh:mm:ss.zzz" ) << QThread::currentThreadId() << "(ClientConnection::queryIteratorType)";

    Socket* socket = getSocket();
    if ( !socket )
        return 0;
    SocketStream stream( socket );

    if (!stream.writeUnsignedInt16( COMMAND_ITERATOR_QUERY_TYPE ) ||
        !stream.writeUnsignedInt32( ( quint32 )id ) ) {
        setError( "Write error", Soprano::Error::ErrorTimeout );
        socket->close();
        return 0;
    }

    if ( !socket->waitForReadyRead(s_defaultTimeout) ) {
        setError( "Command timed out.", Soprano::Error::ErrorTimeout );
        // We cannot recover from a timeout, thus we force a reconnect
        socket->close();
        return 0;
    }

    quint8 type;
    Error::Error error;
    stream.readUnsignedInt8( type );
    stream.readError( error );

    setError( error );
    //qDebug() << this << QTime::currentTime().toString( "hh:mm:ss.zzz" ) << QThread::currentThreadId() << "(ClientConnection::queryIteratorType) end";
    return type;
}


bool Soprano::Client::ClientConnection::queryIteratorBoolValue( int id )
{
    //qDebug() << this << QTime::currentTime().toString( "hh:mm:ss.zzz" ) << QThread::currentThreadId() << "(ClientConnection::queryIteratorBoolValue)";

    Socket* socket = getSocket();
    if ( !socket )
        return false;
    SocketStream stream( socket );

    if (!stream.writeUnsignedInt16( COMMAND_ITERATOR_QUERY_BOOL_VALUE ) ||
        !stream.writeUnsignedInt32( ( quint32 )id ) ) {
        setError( "Write error", Soprano::Error::ErrorTimeout );
        socket->close();
        return false;
    }

    if ( !socket->waitForReadyRead(s_defaultTimeout) ) {
        setError( "Command timed out.", Soprano::Error::ErrorTimeout );
        // We cannot recover from a timeout, thus we force a reconnect
        socket->close();
        return false;
    }

    bool v;
    Error::Error error;
    stream.readBool( v );
    stream.readError( error );

    setError( error );
    return v;
}


void Soprano::Client::ClientConnection::iteratorClose( int id )
{
    //qDebug() << this << QTime::currentTime().toString( "hh:mm:ss.zzz" ) << QThread::currentThreadId() << "(ClientConnection::iteratorClose)";

    Socket* socket = getSocket();
    if ( !socket )
        return;
    SocketStream stream( socket );

    if (!stream.writeUnsignedInt16( COMMAND_ITERATOR_CLOSE ) ||
        !stream.writeUnsignedInt32( ( quint32 )id ) ) {
        setError( "Write error", Soprano::Error::ErrorTimeout );
        socket->close();
        return;
    }

    if ( !socket->waitForReadyRead(s_defaultTimeout) ) {
        setError( "Command timed out.", Soprano::Error::ErrorTimeout );
        // We cannot recover from a timeout, thus we force a reconnect
        socket->close();
        return;
    }

    Error::Error error;
    stream.readError( error );

    setError( error );
    //qDebug() << this << QTime::currentTime().toString( "hh:mm:ss.zzz" ) << QThread::currentThreadId() << "(ClientConnection::iteratorClose) end";
}


bool Soprano::Client::ClientConnection::checkProtocolVersion()
{
    Socket* socket = getSocket();
    if ( !socket )
        return false;
    SocketStream stream( socket );

    if (!stream.writeUnsignedInt16( COMMAND_SUPPORTS_PROTOCOL_VERSION ) ||
        !stream.writeUnsignedInt32( ( quint32 )PROTOCOL_VERSION ) ) {
        setError( "Write error", Soprano::Error::ErrorTimeout );
        socket->close();
        return false;
    }

    // wait for a reply, but not forever, in case we are connected to something unknown
    if ( !socket->waitForReadyRead(s_defaultTimeout) ) {
        setError( "Command timed out.", Soprano::Error::ErrorTimeout );
        // We cannot recover from a timeout, thus we force a reconnect
        socket->close();
        return false;
    }

    bool reply;
    stream.readBool( reply );

    if ( reply ) {
        clearError();
    }
    else {
        setError( QString( "Server does not support protocol version %1" ).arg( PROTOCOL_VERSION ) );
    }
    return reply;
}

#include "clientconnection.moc"
