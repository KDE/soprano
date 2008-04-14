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

#include "serverconnection.h"
#include "servercore.h"
#include "commands.h"
#include "randomgenerator.h"
#include "socketdevice.h"
#include "asyncmodel.h"
#include "datastream.h"

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
#include <QtNetwork/QTcpSocket>
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
    QIODevice* socket;

    QHash<quint32, Model*> modelIdMap;
    QHash<QString, quint32> modelNameMap;
    QHash<quint32, StatementIterator> openStatementIterators;
    QHash<quint32, NodeIterator> openNodeIterators;
    QHash<quint32, QueryResultIterator> openQueryIterators;

    void _s_readNextCommand();
    void _s_resultReady( Soprano::Util::AsyncResult* result );

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
Soprano::Server::ServerConnection::ServerConnection( ServerCore* core )
    : QObject( 0 ),
      d( new Private() )
{
    d->q = this;

    d->core = core;
    d->socket = 0;
}


Soprano::Server::ServerConnection::~ServerConnection()
{
    qDebug() << "Removing connection";
    delete d->socket;
    delete d;
}


void Soprano::Server::ServerConnection::close()
{
    d->socket->close();
}


void Soprano::Server::ServerConnection::start( QIODevice* socket )
{
    d->socket = socket;
    connect( socket, SIGNAL( readyRead() ),
             this, SLOT( _s_readNextCommand() ) );
    connect( socket, SIGNAL( disconnected() ),
             this, SIGNAL( finished() ) );
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


void Soprano::Server::ServerConnection::Private::_s_resultReady( Util::AsyncResult* result )
{
    // we only handle one client request at the same time
    // Thus, we simply communicate the result

    DataStream stream( socket );

    QVariant value = result->value();

    if( value.userType() == QVariant::Bool ) {
        stream.writeBool( value.toBool() );
    }
    else if ( value.userType() == QVariant::Int ) {
        stream.writeInt32( ( qint32 )value.toInt() );
    }
    else if ( value.userType() == qMetaTypeId<Node>() ) {
        stream.writeNode( value.value<Node>() );
    }
    else if ( value.userType() == qMetaTypeId<StatementIterator>() ) {
        stream.writeUnsignedInt32( mapIterator( value.value<StatementIterator>() ) );
    }
    else if ( value.userType() == qMetaTypeId<NodeIterator>() ) {
        stream.writeUnsignedInt32( mapIterator( value.value<NodeIterator>() ) );
    }
    else if ( value.userType() == qMetaTypeId<QueryResultIterator>() ) {
        stream.writeUnsignedInt32( mapIterator( value.value<QueryResultIterator>() ) );
    }
    else if ( value.userType() == qMetaTypeId<Error::ErrorCode>() ) {
        stream.writeErrorCode( value.value<Error::ErrorCode>() );
    }
    else {
        Q_ASSERT( false );
    }

    stream.writeError( result->lastError() );
}


Soprano::Model* Soprano::Server::ServerConnection::Private::getModel()
{
    DataStream stream( socket );

    quint32 id = 0;
    if ( stream.readUnsignedInt32( id ) ) {
        QHash<quint32, Model*>::iterator it = modelIdMap.find( id );
        if ( it != modelIdMap.end() ) {
            return *it;
        }
    }
    return 0;
}


quint32 Soprano::Server::ServerConnection::Private::generateUniqueId()
{
    quint32 id = 0;
    do {
        id = RandomGenerator::instance()->randomInt();
    } while ( modelIdMap.contains( id ) ||
              openStatementIterators.contains( id ) ||
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

    quint32 id = 0;

    // see if we already have that model
    QHash<QString, quint32>::const_iterator it = modelNameMap.find( name );
    if ( it != modelNameMap.end() ) {
        id = *it;
    }
    else {
        Model* model = core->model( name );
        if ( model ) {
            id = generateUniqueId();
            modelIdMap.insert( id, model );
            modelNameMap.insert( name, id );
        }
    }

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

    modelIdMap.remove( modelNameMap[name] );
    modelNameMap.remove( name );

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

        if ( Util::AsyncModel* am = qobject_cast<Util::AsyncModel*>( model ) ) {
            connect( am->addStatementAsync( s ), SIGNAL( resultReady( Soprano::Util::AsyncResult* ) ),
                     q, SLOT( _s_resultReady( Soprano::Util::AsyncResult* ) ) );
        }
        else {
            stream.writeErrorCode( model->addStatement( s ) );
            stream.writeError( model->lastError() );
        }
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

        if ( Util::AsyncModel* am = qobject_cast<Util::AsyncModel*>( model ) ) {
            connect( am->removeStatementAsync( s ), SIGNAL( resultReady( Soprano::Util::AsyncResult* ) ),
                     q, SLOT( _s_resultReady( Soprano::Util::AsyncResult* ) ) );
        }
        else {
            stream.writeErrorCode( model->removeStatement( s ) );
            stream.writeError( model->lastError() );
        }
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

        if ( Util::AsyncModel* am = qobject_cast<Util::AsyncModel*>( model ) ) {
            connect( am->removeAllStatementsAsync( s ), SIGNAL( resultReady( Soprano::Util::AsyncResult* ) ),
                     q, SLOT( _s_resultReady( Soprano::Util::AsyncResult* ) ) );
        }
        else {
            stream.writeErrorCode( model->removeAllStatements( s ) );
            stream.writeError( model->lastError() );
        }
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

        if ( Util::AsyncModel* am = qobject_cast<Util::AsyncModel*>( model ) ) {
            connect( am->listStatementsAsync( s ), SIGNAL( resultReady( Soprano::Util::AsyncResult* ) ),
                     q, SLOT( _s_resultReady( Soprano::Util::AsyncResult* ) ) );
        }
        else {
            StatementIterator it = model->listStatements( s );
            stream.writeUnsignedInt32( mapIterator( it ) );
            stream.writeError( model->lastError() );
        }
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

        if ( Util::AsyncModel* am = qobject_cast<Util::AsyncModel*>( model ) ) {
            connect( am->containsStatementAsync( s ), SIGNAL( resultReady( Soprano::Util::AsyncResult* ) ),
                     q, SLOT( _s_resultReady( Soprano::Util::AsyncResult* ) ) );
        }
        else {
            stream.writeBool( model->containsStatement( s ) );
            stream.writeError( model->lastError() );
        }
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

        if ( Util::AsyncModel* am = qobject_cast<Util::AsyncModel*>( model ) ) {
            connect( am->containsAnyStatementAsync( s ), SIGNAL( resultReady( Soprano::Util::AsyncResult* ) ),
                     q, SLOT( _s_resultReady( Soprano::Util::AsyncResult* ) ) );
        }
        else {
            stream.writeBool( model->containsAnyStatement( s ) );
            stream.writeError( model->lastError() );
        }
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
        if ( Util::AsyncModel* am = qobject_cast<Util::AsyncModel*>( model ) ) {
            connect( am->listContextsAsync(), SIGNAL( resultReady( Soprano::Util::AsyncResult* ) ),
                     q, SLOT( _s_resultReady( Soprano::Util::AsyncResult* ) ) );
        }
        else {
            NodeIterator it = model->listContexts();
            stream.writeUnsignedInt32( mapIterator( it ) );
            stream.writeError( model->lastError() );
        }
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

        if ( Util::AsyncModel* am = qobject_cast<Util::AsyncModel*>( model ) ) {
            connect( am->executeQueryAsync( queryString, ( Query::QueryLanguage )queryLang, userLang ), SIGNAL( resultReady( Soprano::Util::AsyncResult* ) ),
                     q, SLOT( _s_resultReady( Soprano::Util::AsyncResult* ) ) );
        }
        else {
            QueryResultIterator it = model->executeQuery( queryString, ( Query::QueryLanguage )queryLang, userLang );
            stream.writeUnsignedInt32( mapIterator( it ) );
            stream.writeError( model->lastError() );
        }
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
        if ( Util::AsyncModel* am = qobject_cast<Util::AsyncModel*>( model ) ) {
            connect( am->statementCountAsync(), SIGNAL( resultReady( Soprano::Util::AsyncResult* ) ),
                     q, SLOT( _s_resultReady( Soprano::Util::AsyncResult* ) ) );
        }
        else {
            qint32 count = model->statementCount();
            stream.writeInt32( count );
            stream.writeError( model->lastError() );
        }
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
        if ( Util::AsyncModel* am = qobject_cast<Util::AsyncModel*>( model ) ) {
            connect( am->isEmptyAsync(), SIGNAL( resultReady( Soprano::Util::AsyncResult* ) ),
                     q, SLOT( _s_resultReady( Soprano::Util::AsyncResult* ) ) );
        }
        else {
            stream.writeBool( model->isEmpty() );
            stream.writeError( model->lastError() );
        }
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
        if ( Util::AsyncModel* am = qobject_cast<Util::AsyncModel*>( model ) ) {
            connect( am->createBlankNodeAsync(), SIGNAL( resultReady( Soprano::Util::AsyncResult* ) ),
                     q, SLOT( _s_resultReady( Soprano::Util::AsyncResult* ) ) );
        }
        else {
            stream.writeNode( model->createBlankNode() );
            stream.writeError( model->lastError() );
        }
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
