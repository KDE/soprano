/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2007-2009 Sebastian Trueg <trueg@kde.org>
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

#include "serverconnection.h"
#include "servercore.h"
#include "commands.h"
#include "randomgenerator.h"
#include "socketdevice.h"
#include "datastream.h"
#include "modelpool.h"

#include "queryresultiterator.h"
#include "node.h"
#include "nodeiterator.h"
#include "literalvalue.h"
#include "statement.h"
#include "statementiterator.h"
#include "storagemodel.h"
#include "backend.h"
#include "error.h"
#include "bindingset.h"

#include <QtCore/QHash>
#include <QtCore/QDebug>
#include <QtCore/QThread>
#include <QtCore/QTime>

Q_DECLARE_METATYPE(Soprano::Error::ErrorCode)
Q_DECLARE_METATYPE(Soprano::Node)
Q_DECLARE_METATYPE(Soprano::StatementIterator)
Q_DECLARE_METATYPE(Soprano::NodeIterator)
Q_DECLARE_METATYPE(Soprano::QueryResultIterator)


class Soprano::Server::ServerConnection::Private
{
public:
    ServerCore* core;
    ModelPool* modelPool;
    QIODevice* socket;

    QHash<quint32, StatementIterator> openStatementIterators;
    QHash<quint32, NodeIterator> openNodeIterators;
    QHash<quint32, QueryResultIterator> openQueryIterators;

    void _s_readNextCommand();

    quint32 generateUniqueId();
    Soprano::Model* getModel();
    quint32 mapIterator( const StatementIterator& it );
    quint32 mapIterator( const NodeIterator& it );
    quint32 mapIterator( const QueryResultIterator& it );

    void supportsProtocolVersion();

    void createModel();
    void removeModel();
    void supportedFeatures();
    void addStatement();
    void removeStatement();
    void removeAllStatements();
    void listStatements();
    void containsStatement();
    void containsAnyStatement();
    void listContexts();
    void statementCount();
    void isEmpty();
    void query();
    void createBlankNode();

    void iteratorNext();
    void statementIteratorCurrent();
    void nodeIteratorCurrent();
    void queryIteratorCurrent();
    void iteratorClose();
    void queryIteratorCurrentStatement();
    void queryIteratorType();
    void queryIteratorBoolValue();

    ServerConnection* q;
};


// do NOT use core as parent. Otherwise ~QObject will crash us
// when we shut down the connections gracefully in ~ServerCore
Soprano::Server::ServerConnection::ServerConnection( ModelPool* pool, ServerCore* core )
    : QThread( 0 ),
      d( new Private() )
{
    d->q = this;

    d->core = core;
    d->modelPool = pool;
    d->socket = 0;
}


Soprano::Server::ServerConnection::~ServerConnection()
{
    qDebug() << Q_FUNC_INFO << "Removing connection";

    // d->socket deleted when quit()
    quit();
    wait();

    delete d;
}


void Soprano::Server::ServerConnection::close()
{
    d->socket->close();
}


void Soprano::Server::ServerConnection::start( QIODevice* socket )
{
    d->socket = socket;

    // "push" the socket to the new thread,
    // because QTcpSocket::write(...) seems to create children objects, in the new thread, and :
    // "QObject: Cannot create children for a parent that is in a different thread."
    d->socket->setParent( 0 );
    d->socket->moveToThread( this );

    // we move 'this' (ServerConnection object) to the new thread,
    // the one created by ourselves and it works!
    moveToThread( this );

    // start the thread (call run())
    QThread::start( QThread::InheritPriority );
}

void Soprano::Server::ServerConnection::run()
{
    // we are in the new thread

    connect( d->socket, SIGNAL( readyRead() ),
             this, SLOT( _s_readNextCommand() ) );

    // quit() emits the signal finished() when the thread is finished
    connect( d->socket, SIGNAL( disconnected() ),
             this, SLOT( quit() ) );

    // start the event loop
    exec();

    qDebug() << Q_FUNC_INFO << "thread done.";

    delete d->socket;
    d->socket = 0;
}


void Soprano::Server::ServerConnection::Private::_s_readNextCommand()
{
    DataStream stream( socket );
    quint16 command = 0;
    stream.readUnsignedInt16( command );
    switch( command ) {
    case COMMAND_SUPPORTS_PROTOCOL_VERSION:
        supportsProtocolVersion();
        break;

    case COMMAND_CREATE_MODEL:
        createModel();
        break;

    case COMMAND_REMOVE_MODEL:
        removeModel();
        break;

    case COMMAND_SUPPORTED_FEATURES:
        supportedFeatures();
        break;

    case COMMAND_MODEL_ADD_STATEMENT:
        addStatement();
        break;

    case COMMAND_MODEL_REMOVE_STATEMENT:
        removeStatement();
        break;

    case COMMAND_MODEL_REMOVE_ALL_STATEMENTS:
        removeAllStatements();
        break;

    case COMMAND_MODEL_LIST_STATEMENTS:
        listStatements();
        break;

    case COMMAND_MODEL_CONTAINS_STATEMENT:
        containsStatement();
        break;

    case COMMAND_MODEL_CONTAINS_ANY_STATEMENT:
        containsAnyStatement();
        break;

    case COMMAND_MODEL_LIST_CONTEXTS:
        listContexts();
        break;

    case COMMAND_MODEL_STATEMENT_COUNT:
        statementCount();
        break;

    case COMMAND_MODEL_IS_EMPTY:
        isEmpty();
        break;

    case COMMAND_MODEL_QUERY:
        query();
        break;

    case COMMAND_ITERATOR_NEXT:
        iteratorNext();
        break;

    case COMMAND_ITERATOR_CURRENT_STATEMENT:
        statementIteratorCurrent();
        break;

    case COMMAND_ITERATOR_CURRENT_NODE:
        nodeIteratorCurrent();
        break;

    case COMMAND_ITERATOR_CURRENT_BINDINGSET:
        queryIteratorCurrent();
        break;

    case COMMAND_ITERATOR_CLOSE:
        iteratorClose();
        break;

    case COMMAND_ITERATOR_QUERY_TYPE:
        queryIteratorType();
        break;

    case COMMAND_ITERATOR_QUERY_BOOL_VALUE:
        queryIteratorBoolValue();
        break;

    case COMMAND_MODEL_CREATE_BLANK_NODE:
        createBlankNode();
        break;

    default:
        // FIXME: handle an error
        // for now we just close the connection on error.
        qDebug() << "Unknown command: " << command << "closing connection";
        q->close();
        break;
    }
}


Soprano::Model* Soprano::Server::ServerConnection::Private::getModel()
{
    DataStream stream( socket );

    quint32 id = 0;
    if ( stream.readUnsignedInt32( id ) ) {
        return modelPool->modelById( id );
    }
    return 0;
}


quint32 Soprano::Server::ServerConnection::Private::generateUniqueId()
{
    quint32 id = 0;
    do {
        id = RandomGenerator::instance()->randomInt();
    } while ( openStatementIterators.contains( id ) ||
              openNodeIterators.contains( id ) ||
              openQueryIterators.contains( id ) );
    return id;
}


quint32 Soprano::Server::ServerConnection::Private::mapIterator( const StatementIterator& it )
{
    quint32 id = generateUniqueId();
    openStatementIterators.insert( id, it );
    return id;
}


quint32 Soprano::Server::ServerConnection::Private::mapIterator( const NodeIterator& it )
{
    quint32 id = generateUniqueId();
    openNodeIterators.insert( id, it );
    return id;
}


quint32 Soprano::Server::ServerConnection::Private::mapIterator( const QueryResultIterator& it )
{
    quint32 id = generateUniqueId();
    openQueryIterators.insert( id, it );
    return id;
}


void Soprano::Server::ServerConnection::Private::createModel()
{
    //qDebug() << "(ServerConnection::createModel)";

    DataStream stream( socket );

    // extract options
    QString name;
    stream.readString( name );

    // for now we ignore the settings

    quint32 id = modelPool->idForModelName( name );

    stream.writeUnsignedInt32( id );
    stream.writeError( Error::Error() );
    //qDebug() << "(ServerConnection::createModel) done";
}


void Soprano::Server::ServerConnection::Private::removeModel()
{
    //qDebug() << "(ServerConnection::createModel)";

    DataStream stream( socket );

    // extract options
    QString name;
    stream.readString( name );

    modelPool->removeModel( name );
    core->removeModel( name );

    stream.writeError( Error::Error() );
    //qDebug() << "(ServerConnection::createModel) done";
}


void Soprano::Server::ServerConnection::Private::supportedFeatures()
{
    //qDebug() << "(ServerConnection::supportedFeatures)";

    DataStream stream( socket );

    quint32 features = 0;
    Error::Error error;
    if ( core->backend() ) {
        features = ( quint32 )core->backend()->supportedFeatures();
    }
    else {
        error = Error::Error( "No backend available" );
    }

    stream.writeUnsignedInt32( features );
    stream.writeError( error );
    //qDebug() << "(ServerConnection::supportedFeatures) done";
}


void Soprano::Server::ServerConnection::Private::addStatement()
{
    //qDebug() << "(ServerConnection::addStatement)";
    DataStream stream( socket );

    Model* model = getModel();
    if ( model ) {
        Statement s;
        stream.readStatement( s );

        stream.writeErrorCode( model->addStatement( s ) );
        stream.writeError( model->lastError() );
    }
    else {
        stream.writeErrorCode( Error::ErrorInvalidArgument );
        stream.writeError( Error::Error( "Invalid model id" ) );
    }
    //qDebug() << "(ServerConnection::addStatement) done";
}


void Soprano::Server::ServerConnection::Private::removeStatement()
{
    //qDebug() << "(ServerConnection::removeStatement)";
    DataStream stream( socket );

    Model* model = getModel();
    if ( model ) {
        Statement s;
        stream.readStatement( s );

        stream.writeErrorCode( model->removeStatement( s ) );
        stream.writeError( model->lastError() );
    }
    else {
        stream.writeErrorCode( Error::ErrorInvalidArgument );
        stream.writeError( Error::Error( "Invalid model id" ) );
    }
    //qDebug() << "(ServerConnection::removeStatement) done";
}


void Soprano::Server::ServerConnection::Private::removeAllStatements()
{
    //qDebug() << "(ServerConnection::removeAllStatements)";
    DataStream stream( socket );

    Model* model = getModel();
    if ( model ) {
        Statement s;
        stream.readStatement( s );

        stream.writeErrorCode( model->removeAllStatements( s ) );
        stream.writeError( model->lastError() );
    }
    else {
        stream.writeErrorCode( Error::ErrorInvalidArgument );
        stream.writeError( Error::Error( "Invalid model id" ) );
    }
    //qDebug() << "(ServerConnection::removeAllStatements) done";
}


void Soprano::Server::ServerConnection::Private::listStatements()
{
    //qDebug() << "(ServerConnection::listStatements)";
    DataStream stream( socket );

    Model* model = getModel();
    if ( model ) {
        Statement s;
        stream.readStatement( s );

        StatementIterator it = model->listStatements( s );
        stream.writeUnsignedInt32( it.isValid() ? mapIterator( it ) : quint32(0) );
        stream.writeError( model->lastError() );
    }
    else {
        stream.writeUnsignedInt32( 0 );
        stream.writeError( Error::Error( "Invalid model id" ) );
    }
    //qDebug() << "(ServerConnection::listStatements) done";
}


void Soprano::Server::ServerConnection::Private::containsStatement()
{
    //qDebug() << "(ServerConnection::containsStatement)";
    DataStream stream( socket );

    Model* model = getModel();
    if ( model ) {
        Statement s;
        stream.readStatement( s );

        stream.writeBool( model->containsStatement( s ) );
        stream.writeError( model->lastError() );
    }
    else {
        stream.writeBool( false );
        stream.writeError( Error::Error( "Invalid model id" ) );
    }
    //qDebug() << "(ServerConnection::containsStatement) done";
}


void Soprano::Server::ServerConnection::Private::containsAnyStatement()
{
    //qDebug() << "(ServerConnection::containsAnyStatement)";
    DataStream stream( socket );

    Model* model = getModel();
    if ( model ) {
        Statement s;
        stream.readStatement( s );

        stream.writeBool( model->containsAnyStatement( s ) );
        stream.writeError( model->lastError() );
    }
    else {
        stream.writeBool( false );
        stream.writeError( Error::Error( "Invalid model id" ) );
    }
    //qDebug() << "(ServerConnection::containsAnyStatement) done";
}


void Soprano::Server::ServerConnection::Private::listContexts()
{
    DataStream stream( socket );

    Model* model = getModel();
    if ( model ) {
        NodeIterator it = model->listContexts();
        stream.writeUnsignedInt32( it.isValid() ? mapIterator( it ) : quint32(0) );
        stream.writeError( model->lastError() );
    }
    else {
        stream.writeUnsignedInt32( 0 );
        stream.writeError( Error::Error( "Invalid model id" ) );
    }
}


void Soprano::Server::ServerConnection::Private::query()
{
    DataStream stream( socket );

    Model* model = getModel();
    if ( model ) {
        QString queryString;
        quint16 queryLang;
        QString userLang;
        stream.readString( queryString );
        stream.readUnsignedInt16( queryLang );
        stream.readString( userLang );

        QueryResultIterator it = model->executeQuery( queryString, ( Query::QueryLanguage )queryLang, userLang );
        stream.writeUnsignedInt32( it.isValid() ? mapIterator( it ) : quint32(0) );
        stream.writeError( model->lastError() );
    }
    else {
        stream.writeUnsignedInt32( 0 );
        stream.writeError( Error::Error( "Invalid model id" ) );
    }
}


void Soprano::Server::ServerConnection::Private::statementCount()
{
    DataStream stream( socket );

    Model* model = getModel();
    if ( model ) {
        qint32 count = model->statementCount();
        stream.writeInt32( count );
        stream.writeError( model->lastError() );
    }
    else {
        stream.writeInt32( -1 );
        stream.writeError( Error::Error( "Invalid model id" ) );
    }
}


void Soprano::Server::ServerConnection::Private::isEmpty()
{
    DataStream stream( socket );

    Model* model = getModel();
    if ( model ) {
        stream.writeBool( model->isEmpty() );
        stream.writeError( model->lastError() );
    }
    else {
        stream.writeBool( false );
        stream.writeError( Error::Error( "Invalid model id" ) );
    }
}


void Soprano::Server::ServerConnection::Private::createBlankNode()
{
    DataStream stream( socket );

    Model* model = getModel();
    if ( model ) {
        stream.writeNode( model->createBlankNode() );
        stream.writeError( model->lastError() );
    }
    else {
        stream.writeNode( Node() );
        stream.writeError( Error::Error( "Invalid model id" ) );
    }
}


void Soprano::Server::ServerConnection::Private::iteratorNext()
{
    DataStream stream( socket );

    //qDebug() << "(ServerConnection::iteratorNext)";
    quint32 id = 0;
    stream.readUnsignedInt32( id );

    QHash<quint32, StatementIterator>::iterator it1 = openStatementIterators.find( id );
    if ( it1 != openStatementIterators.end() ) {
        stream.writeBool( it1.value().next() );
        stream.writeError( it1.value().lastError() );
        return;
    }

    QHash<quint32, NodeIterator>::iterator it2 = openNodeIterators.find( id );
    if ( it2 != openNodeIterators.end() ) {
        stream.writeBool( it2.value().next() );
        stream.writeError( it2.value().lastError() );
        return;
    }

    QHash<quint32, QueryResultIterator>::iterator it3 = openQueryIterators.find( id );
    if ( it3 != openQueryIterators.end() ) {
        stream.writeBool( it3.value().next() );
        stream.writeError( it3.value().lastError() );
        return;
    }

    stream.writeBool( false );
    stream.writeError( Error::Error( "Invalid iterator ID." ) );
    //qDebug() << "(ServerConnection::iteratorNext) done";
}


void Soprano::Server::ServerConnection::Private::statementIteratorCurrent()
{
    DataStream stream( socket );

    //qDebug() << "(ServerConnection::statementIteratorCurrent)";
    quint32 id = 0;
    stream.readUnsignedInt32( id );

    QHash<quint32, StatementIterator>::iterator it = openStatementIterators.find( id );
    if ( it != openStatementIterators.end() ) {
        stream.writeStatement( it.value().current() );
        stream.writeError( it.value().lastError() );
        return;
    }

    // could be a graph query iterator
    QHash<quint32, QueryResultIterator>::iterator it2 = openQueryIterators.find( id );
    if ( it2 != openQueryIterators.end() ) {
        stream.writeStatement( it2.value().currentStatement() );
        stream.writeError( it2.value().lastError() );
        return;
    }

    stream.writeStatement( Statement() );
    stream.writeError( Error::Error( "Invalid iterator ID." ) );
    //qDebug() << "(ServerConnection::statementIteratorCurrent) done";
}


void Soprano::Server::ServerConnection::Private::nodeIteratorCurrent()
{
    DataStream stream( socket );

    //qDebug() << "(ServerConnection::nodeIteratorCurrent)";
    quint32 id = 0;
    stream.readUnsignedInt32( id );

    QHash<quint32, NodeIterator>::iterator it = openNodeIterators.find( id );
    if ( it != openNodeIterators.end() ) {
        stream.writeNode( it.value().current() );
        stream.writeError( it.value().lastError() );
    }
    else {
        stream.writeNode( Node() );
        stream.writeError( Error::Error( "Invalid iterator ID." ) );
    }
    //qDebug() << "(ServerConnection::nodeIteratorCurrent) done";
}


void Soprano::Server::ServerConnection::Private::queryIteratorCurrent()
{
    DataStream stream( socket );

    //qDebug() << "(ServerConnection::queryIteratorCurrent)";
    quint32 id = 0;
    stream.readUnsignedInt32( id );

    QHash<quint32, QueryResultIterator>::iterator it = openQueryIterators.find( id );
    if ( it != openQueryIterators.end() ) {
        stream.writeBindingSet( it.value().current() );
        stream.writeError( it.value().lastError() );
    }
    else {
        stream.writeBindingSet( BindingSet() );
        stream.writeError( Error::Error( "Invalid iterator ID." ) );
    }
    //qDebug() << "(ServerConnection::queryIteratorCurrent) done";
}


void Soprano::Server::ServerConnection::Private::iteratorClose()
{
    DataStream stream( socket );

    //qDebug() << "(ServerConnection::iteratorClose)";
    quint32 id = 0;
    stream.readUnsignedInt32( id );

    QHash<quint32, StatementIterator>::iterator it1 = openStatementIterators.find( id );
    if ( it1 != openStatementIterators.end() ) {
        it1.value().close();
        stream.writeError( it1.value().lastError() );
        openStatementIterators.erase( it1 );
        return;
    }

    QHash<quint32, NodeIterator>::iterator it2 = openNodeIterators.find( id );
    if ( it2 != openNodeIterators.end() ) {
        it2.value().close();
        stream.writeError( it2.value().lastError() );
        openNodeIterators.erase( it2 );
        return;
    }

    QHash<quint32, QueryResultIterator>::iterator it3 = openQueryIterators.find( id );
    if ( it3 != openQueryIterators.end() ) {
        it3.value().close();
        stream.writeError( it3.value().lastError() );
        openQueryIterators.erase( it3 );
        return;
    }

    stream.writeError( Error::Error( "Invalid iterator ID." ) );
    //qDebug() << "(ServerConnection::iteratorClose) done";
}


void Soprano::Server::ServerConnection::Private::queryIteratorType()
{
    DataStream stream( socket );

    //qDebug() << "(ServerConnection::queryIteratorType)";
    quint32 id = 0;
    stream.readUnsignedInt32( id );

    QHash<quint32, QueryResultIterator>::iterator it = openQueryIterators.find( id );
    if ( it != openQueryIterators.end() ) {
        quint8 type = 0;
        if ( it.value().isGraph() ) {
            type = 1;
        }
        else if ( it.value().isBool() ) {
            type = 2;
        }
        else {
            type = 3;
        }
        stream.writeUnsignedInt8( type );
        stream.writeError( it.value().lastError() );
    }
    else {
        stream.writeUnsignedInt8( 0 );
        stream.writeError( Error::Error( "Invalid iterator ID." ) );
    }
    //qDebug() << "(ServerConnection::queryIteratorType) done";
}


void Soprano::Server::ServerConnection::Private::queryIteratorBoolValue()
{
    DataStream stream( socket );

    //qDebug() << "(ServerConnection::queryIteratorBoolValue)";
    quint32 id = 0;
    stream.readUnsignedInt32( id );

    QHash<quint32, QueryResultIterator>::iterator it = openQueryIterators.find( id );
    if ( it != openQueryIterators.end() ) {
        stream.writeBool( it.value().boolValue() );
        stream.writeError( it.value().lastError() );
    }
    else {
        stream.writeBool( false );
        stream.writeError( Error::Error( "Invalid iterator ID." ) );
    }
    //qDebug() << "(ServerConnection::queryIteratorBoolValue) done";
}


void Soprano::Server::ServerConnection::Private::supportsProtocolVersion()
{
    DataStream stream( socket );

    //qDebug() << "(ServerConnection::supportsProtocolVersion)";
    quint32 requestedVersion;
    stream.readUnsignedInt32( requestedVersion );

    // Since version 3 we are not backwards compatible anymore!
    stream.writeBool( ( requestedVersion == PROTOCOL_VERSION ? true : false ) );
    //qDebug() << "(ServerConnection::supportsProtocolVersion) done";
}

#include "serverconnection.moc"
