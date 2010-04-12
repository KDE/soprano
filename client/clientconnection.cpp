/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2007-2010 Sebastian Trueg <trueg@kde.org>
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
#include "datastream.h"

#include "node.h"
#include "statement.h"
#include "model.h"
#include "bindingset.h"
#include "backend.h"

#include <QtCore/QIODevice>
#include <QtCore/QThread>
#include <QtCore/QMutex>
#include <QtCore/QMutexLocker>
#include <QtCore/QTime>
#include <QtCore/QHash>


using namespace Soprano::Server;

namespace {
    const int s_defaultTimeout = 600000;
}


Soprano::Client::SocketHandler::SocketHandler( ClientConnectionPrivate* client, QIODevice* socket )
    : QObject(),
      m_client( client ),
      m_socket( socket )
{
}


Soprano::Client::SocketHandler::~SocketHandler()
{
    QMutexLocker lock( &m_client->socketMutex );
    m_client->sockets.removeAll( m_socket );
    delete m_socket;
}


Soprano::Client::ClientConnection::ClientConnection( QObject* parent )
    : QObject( parent ),
      d( new ClientConnectionPrivate() )
{
}


Soprano::Client::ClientConnection::~ClientConnection()
{
    d->socketMutex.lock();
    // the sockets need to be deleted in their respective threads.
    // this is what d->socketStorage does. We only close them here.
    // FIXME: QThreadStorage does NOT delete the local data in its destructor!
    foreach( QIODevice* socket, d->sockets ) {
        socket->close();
    }
    d->socketMutex.unlock();
    delete d;
}


QIODevice* Soprano::Client::ClientConnection::socketForCurrentThread()
{
    if ( isConnectedInCurrentThread() ) {
        return d->socketStorage.localData()->socket();
    }
    else if ( QIODevice* socket = newConnection() ) {
        d->socketMutex.lock();
        SocketHandler* cleaner = new SocketHandler( d, socket );
        d->sockets.append( socket );
        d->socketMutex.unlock();
        d->socketStorage.setLocalData( cleaner );
        return socket;
    }

    return 0;
}


int Soprano::Client::ClientConnection::createModel( const QString& name, const QList<BackendSetting>& settings )
{
    //qDebug() << this << QTime::currentTime().toString( "hh:mm:ss.zzz" ) << QThread::currentThreadId() << "(ClientConnection::createModel)";

    QIODevice* socket = socketForCurrentThread();
    if ( !socket )
        return 0;
    DataStream stream( socket );

    stream.writeUnsignedInt16( COMMAND_CREATE_MODEL );
    stream.writeString( name );
    Q_UNUSED( settings );

    if ( !socket->waitForReadyRead(s_defaultTimeout) ) {
        setError( "Command timed out." );
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

    QIODevice* socket = socketForCurrentThread();
    if ( !socket )
        return;
    DataStream stream( socket );

    stream.writeUnsignedInt16( COMMAND_REMOVE_MODEL );
    stream.writeString( name );

    if ( !socket->waitForReadyRead(s_defaultTimeout) ) {
        setError( "Command timed out." );
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

    QIODevice* socket = socketForCurrentThread();
    if ( !socket )
        return BackendFeatureNone;
    DataStream stream( socket );

    stream.writeUnsignedInt16( COMMAND_SUPPORTED_FEATURES );

    if ( !socket->waitForReadyRead(s_defaultTimeout) ) {
        setError( "Command timed out." );
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

    QIODevice* socket = socketForCurrentThread();
    if ( !socket )
        return Error::convertErrorCode( lastError().code() );
    DataStream stream( socket );

    stream.writeUnsignedInt16( COMMAND_MODEL_ADD_STATEMENT );
    stream.writeUnsignedInt32( ( quint32 )modelId );
    stream.writeStatement( statement );

    if ( !socket->waitForReadyRead(s_defaultTimeout) ) {
        setError( "Command timed out." );
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

    QIODevice* socket = socketForCurrentThread();
    if ( !socket )
        return 0;
    DataStream stream( socket );

    stream.writeUnsignedInt16( COMMAND_MODEL_LIST_CONTEXTS );
    stream.writeUnsignedInt32( ( quint32 )modelId );

    if ( !socket->waitForReadyRead(s_defaultTimeout) ) {
        setError( "Command timed out." );
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

    QIODevice* socket = socketForCurrentThread();
    if ( !socket )
        return 0;
    DataStream stream( socket );

    stream.writeUnsignedInt16( COMMAND_MODEL_QUERY );
    stream.writeUnsignedInt32( ( quint32 )modelId );
    stream.writeString( query );
    stream.writeUnsignedInt16( ( quint16 )type );
    stream.writeString( userQueryLanguage );

    if ( !socket->waitForReadyRead(s_defaultTimeout) ) {
        setError( "Command timed out." );
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

    QIODevice* socket = socketForCurrentThread();
    if ( !socket )
        return 0;
    DataStream stream( socket );

    stream.writeUnsignedInt16( COMMAND_MODEL_LIST_STATEMENTS );
    stream.writeUnsignedInt32( ( quint32 )modelId );
    stream.writeStatement( partial );

    if ( !socket->waitForReadyRead(s_defaultTimeout) ) {
        setError( "Command timed out." );
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

    QIODevice* socket = socketForCurrentThread();
    if ( !socket )
        return Error::convertErrorCode( lastError().code() );
    DataStream stream( socket );

    stream.writeUnsignedInt16( COMMAND_MODEL_REMOVE_ALL_STATEMENTS );
    stream.writeUnsignedInt32( ( quint32 )modelId );
    stream.writeStatement( statement );

    if ( !socket->waitForReadyRead(s_defaultTimeout) ) {
        setError( "Command timed out." );
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

    QIODevice* socket = socketForCurrentThread();
    if ( !socket )
        return Error::convertErrorCode( lastError().code() );
    DataStream stream( socket );

    stream.writeUnsignedInt16( COMMAND_MODEL_REMOVE_STATEMENT );
    stream.writeUnsignedInt32( ( quint32 )modelId );
    stream.writeStatement( statement );

    if ( !socket->waitForReadyRead(s_defaultTimeout) ) {
        setError( "Command timed out." );
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

    QIODevice* socket = socketForCurrentThread();
    if ( !socket )
        return -1;
    DataStream stream( socket );

    stream.writeUnsignedInt16( COMMAND_MODEL_STATEMENT_COUNT );
    stream.writeUnsignedInt32( ( quint32 )modelId );

    if ( !socket->waitForReadyRead(s_defaultTimeout) ) {
        setError( "Command timed out." );
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

    QIODevice* socket = socketForCurrentThread();
    if ( !socket )
        return false;
    DataStream stream( socket );

    stream.writeUnsignedInt16( COMMAND_MODEL_CONTAINS_STATEMENT );
    stream.writeUnsignedInt32( ( quint32 )modelId );
    stream.writeStatement( statement );

    if ( !socket->waitForReadyRead(s_defaultTimeout) ) {
        setError( "Command timed out." );
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

    QIODevice* socket = socketForCurrentThread();
    if ( !socket )
        return false;
    DataStream stream( socket );

    stream.writeUnsignedInt16( COMMAND_MODEL_CONTAINS_ANY_STATEMENT );
    stream.writeUnsignedInt32( ( quint32 )modelId );
    stream.writeStatement( statement );

    if ( !socket->waitForReadyRead(s_defaultTimeout) ) {
        setError( "Command timed out." );
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

    QIODevice* socket = socketForCurrentThread();
    if ( !socket )
        return false;
    DataStream stream( socket );

    stream.writeUnsignedInt16( COMMAND_MODEL_IS_EMPTY );
    stream.writeUnsignedInt32( ( quint32 )modelId );

    if ( !socket->waitForReadyRead(s_defaultTimeout) ) {
        setError( "Command timed out." );
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

    QIODevice* socket = socketForCurrentThread();
    if ( !socket )
        return Node();
    DataStream stream( socket );

    stream.writeUnsignedInt16( COMMAND_MODEL_CREATE_BLANK_NODE );
    stream.writeUnsignedInt32( ( quint32 )modelId );

    if ( !socket->waitForReadyRead(s_defaultTimeout) ) {
        setError( "Command timed out." );
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

    QIODevice* socket = socketForCurrentThread();
    if ( !socket )
        return false;
    DataStream stream( socket );

    stream.writeUnsignedInt16( COMMAND_ITERATOR_NEXT );
    stream.writeUnsignedInt32( ( quint32 )id );

    if ( !socket->waitForReadyRead(s_defaultTimeout) ) {
        setError( "Command timed out." );
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

    QIODevice* socket = socketForCurrentThread();
    if ( !socket )
        return Node();
    DataStream stream( socket );

    stream.writeUnsignedInt16( COMMAND_ITERATOR_CURRENT_NODE );
    stream.writeUnsignedInt32( ( quint32 )id );

    if ( !socket->waitForReadyRead(s_defaultTimeout) ) {
        setError( "Command timed out." );
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

    QIODevice* socket = socketForCurrentThread();
    if ( !socket )
        return Statement();
    DataStream stream( socket );

    stream.writeUnsignedInt16( COMMAND_ITERATOR_CURRENT_STATEMENT );
    stream.writeUnsignedInt32( ( quint32 )id );

    if ( !socket->waitForReadyRead(s_defaultTimeout) ) {
        setError( "Command timed out." );
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

    QIODevice* socket = socketForCurrentThread();
    if ( !socket )
        return BindingSet();
    DataStream stream( socket );

    stream.writeUnsignedInt16( COMMAND_ITERATOR_CURRENT_BINDINGSET );
    stream.writeUnsignedInt32( ( quint32 )id );

    if ( !socket->waitForReadyRead(s_defaultTimeout) ) {
        setError( "Command timed out." );
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

    QIODevice* socket = socketForCurrentThread();
    if ( !socket )
        return Statement();
    DataStream stream( socket );

    stream.writeUnsignedInt16( COMMAND_ITERATOR_CURRENT_STATEMENT );
    stream.writeUnsignedInt32( ( quint32 )id );

    if ( !socket->waitForReadyRead(s_defaultTimeout) ) {
        setError( "Command timed out." );
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

    QIODevice* socket = socketForCurrentThread();
    if ( !socket )
        return 0;
    DataStream stream( socket );

    stream.writeUnsignedInt16( COMMAND_ITERATOR_QUERY_TYPE );
    stream.writeUnsignedInt32( ( quint32 )id );

    if ( !socket->waitForReadyRead(s_defaultTimeout) ) {
        setError( "Command timed out." );
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

    QIODevice* socket = socketForCurrentThread();
    if ( !socket )
        return false;
    DataStream stream( socket );

    stream.writeUnsignedInt16( COMMAND_ITERATOR_QUERY_BOOL_VALUE );
    stream.writeUnsignedInt32( ( quint32 )id );

    if ( !socket->waitForReadyRead(s_defaultTimeout) ) {
        setError( "Command timed out." );
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

    QIODevice* socket = socketForCurrentThread();
    if ( !socket )
        return;
    DataStream stream( socket );

    stream.writeUnsignedInt16( COMMAND_ITERATOR_CLOSE );
    stream.writeUnsignedInt32( ( quint32 )id );

    if ( !socket->waitForReadyRead(s_defaultTimeout) ) {
        setError( "Command timed out." );
        return;
    }

    Error::Error error;
    stream.readError( error );

    setError( error );
    //qDebug() << this << QTime::currentTime().toString( "hh:mm:ss.zzz" ) << QThread::currentThreadId() << "(ClientConnection::iteratorClose) end";
}


bool Soprano::Client::ClientConnection::checkProtocolVersion()
{
    QIODevice* socket = socketForCurrentThread();
    if ( !socket )
        return false;
    DataStream stream( socket );

    stream.writeUnsignedInt16( COMMAND_SUPPORTS_PROTOCOL_VERSION );
    stream.writeUnsignedInt32( ( quint32 )PROTOCOL_VERSION );

    // wait for a reply, but not forever, in case we are connected to something unknown
    if ( !socket->waitForReadyRead(s_defaultTimeout) ) {
        setError( "Command timed out." );
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


bool Soprano::Client::ClientConnection::connectInCurrentThread()
{
    return( socketForCurrentThread() != 0 );
}


bool Soprano::Client::ClientConnection::isConnectedInCurrentThread()
{
    return ( d->socketStorage.hasLocalData() &&
             isConnected( d->socketStorage.localData()->socket() ) );
}

#include "clientconnection.moc"
#include "clientconnection_p.moc"
