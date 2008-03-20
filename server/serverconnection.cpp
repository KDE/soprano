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
#include "operators.h"
#include "socketdevice.h"
#include "mutexmodel.h"

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

    void _k_readNextCommand();

    quint32 generateUniqueId();
    Soprano::Model* getModel( QDataStream& stream );
    quint32 mapIterator( const StatementIterator& it );
    quint32 mapIterator( const NodeIterator& it );
    quint32 mapIterator( const QueryResultIterator& it );

    void supportsProtocolVersion( QDataStream& stream );

    void createModel( QDataStream& stream );
    void removeModel( QDataStream& stream );
    void supportedFeatures( QDataStream& );
    void addStatement( QDataStream& stream );
    void removeStatement( QDataStream& stream );
    void removeAllStatements( QDataStream& stream );
    void listStatements( QDataStream& stream );
    void containsStatement( QDataStream& stream );
    void containsAnyStatement( QDataStream& stream );
    void listContexts( QDataStream& stream );
    void statementCount( QDataStream& stream );
    void isEmpty( QDataStream& stream );
    void query( QDataStream& );
    void createBlankNode( QDataStream& );

    void iteratorNext( QDataStream& stream );
    void statementIteratorCurrent( QDataStream& stream );
    void nodeIteratorCurrent( QDataStream& stream );
    void queryIteratorCurrent( QDataStream& stream );
    void iteratorClose( QDataStream& stream );
    void queryIteratorCurrentStatement( QDataStream& stream );
    void queryIteratorType( QDataStream& stream );
    void queryIteratorBoolValue( QDataStream& stream );

    ServerConnection* q;
};


Soprano::Server::ServerConnection::ServerConnection( ServerCore* core )
    : QObject( core ),
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
             this, SLOT( _k_readNextCommand() ) );
    connect( socket, SIGNAL( disconnected() ),
             this, SIGNAL( finished() ) );
}


void Soprano::Server::ServerConnection::Private::_k_readNextCommand()
{
    QDataStream stream( socket );
    quint16 command = 0;
    stream >> command;
    switch( command ) {
    case COMMAND_SUPPORTS_PROTOCOL_VERSION:
        supportsProtocolVersion( stream );
        break;

    case COMMAND_CREATE_MODEL:
        createModel( stream );
        break;

    case COMMAND_REMOVE_MODEL:
        removeModel( stream );
        break;

    case COMMAND_SUPPORTED_FEATURES:
        supportedFeatures( stream );
        break;

    case COMMAND_MODEL_ADD_STATEMENT:
        addStatement( stream );
        break;

    case COMMAND_MODEL_REMOVE_STATEMENT:
        removeStatement( stream );
        break;

    case COMMAND_MODEL_REMOVE_ALL_STATEMENTS:
        removeAllStatements( stream );
        break;

    case COMMAND_MODEL_LIST_STATEMENTS:
        listStatements( stream );
        break;

    case COMMAND_MODEL_CONTAINS_STATEMENT:
        containsStatement( stream );
        break;

    case COMMAND_MODEL_CONTAINS_ANY_STATEMENT:
        containsAnyStatement( stream );
        break;

    case COMMAND_MODEL_LIST_CONTEXTS:
        listContexts( stream );
        break;

    case COMMAND_MODEL_STATEMENT_COUNT:
        statementCount( stream );
        break;

    case COMMAND_MODEL_IS_EMPTY:
        isEmpty( stream );
        break;

    case COMMAND_MODEL_QUERY:
        query( stream );
        break;

    case COMMAND_ITERATOR_NEXT:
        iteratorNext( stream );
        break;

    case COMMAND_ITERATOR_CURRENT_STATEMENT:
        statementIteratorCurrent( stream );
        break;

    case COMMAND_ITERATOR_CURRENT_NODE:
        nodeIteratorCurrent( stream );
        break;

    case COMMAND_ITERATOR_CURRENT_BINDINGSET:
        queryIteratorCurrent( stream );
        break;

    case COMMAND_ITERATOR_CLOSE:
        iteratorClose( stream );
        break;

    case COMMAND_ITERATOR_QUERY_TYPE:
        queryIteratorType( stream );
        break;

    case COMMAND_ITERATOR_QUERY_BOOL_VALUE:
        queryIteratorBoolValue( stream );
        break;

    case COMMAND_MODEL_CREATE_BLANK_NODE:
        createBlankNode( stream );
        break;

    default:
        // FIXME: handle an error
        // for now we just close the connection on error.
        qDebug() << "Unknown command: " << command;
        break;
    }
}


Soprano::Model* Soprano::Server::ServerConnection::Private::getModel( QDataStream& stream )
{
    quint32 id = 0;
    stream >> id;

    QHash<quint32, Model*>::iterator it = modelIdMap.find( id );
    if ( it != modelIdMap.end() ) {
        return *it;
    }
    else {
        return 0;
    }
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


void Soprano::Server::ServerConnection::Private::createModel( QDataStream& stream )
{
    //qDebug() << "(ServerConnection::createModel)";

    // extract options
    QString name;
    QList<BackendSetting> settings;
    stream >> name >> settings;

    // for now we ignore the settings

    // see if we already have that model
    Model* model = core->model( name );
    quint32 id = 0;
    if ( model ) {
        // Use a mutexmodel for single thread since otherwise one client
        // could lock via an iterator. If then another tries to access the model
        // the whole server locks down
        Util::MutexModel* mm = new Util::MutexModel( Util::MutexModel::ReadWriteSingleThreading, model );
        mm->setParent( q ); // memory management

        id = generateUniqueId();
        modelIdMap.insert( id, mm );
        modelNameMap.insert( name, id );
    }

    stream << id << Error::Error();
    //qDebug() << "(ServerConnection::createModel) done";
}


void Soprano::Server::ServerConnection::Private::removeModel( QDataStream& stream )
{
    //qDebug() << "(ServerConnection::createModel)";

    // extract options
    QString name;
    stream >> name;

    // delete the mutex model created above
    delete modelIdMap[modelNameMap[name]];
    modelIdMap.remove( modelNameMap[name] );
    modelNameMap.remove( name );

    core->removeModel( name );

    stream << Error::Error();
    //qDebug() << "(ServerConnection::createModel) done";
}


void Soprano::Server::ServerConnection::Private::supportedFeatures( QDataStream& stream )
{
    //qDebug() << "(ServerConnection::supportedFeatures)";

    quint32 features = 0;
    Error::Error error;
    if ( core->backend() ) {
        features = ( quint32 )core->backend()->supportedFeatures();
    }
    else {
        error = Error::Error( "No backend available" );
    }

    stream << features << error;
    //qDebug() << "(ServerConnection::supportedFeatures) done";
}


void Soprano::Server::ServerConnection::Private::addStatement( QDataStream& stream )
{
    //qDebug() << "(ServerConnection::addStatement)";
    Model* model = getModel( stream );
    if ( model ) {
        Statement s;
        stream >> s;

        stream << model->addStatement( s );
        stream << model->lastError();
    }
    else {
        stream << Error::ErrorInvalidArgument << Error::Error( "Invalid model id" );
    }
    //qDebug() << "(ServerConnection::addStatement) done";
}


void Soprano::Server::ServerConnection::Private::removeStatement( QDataStream& stream )
{
    //qDebug() << "(ServerConnection::removeStatement)";
    Model* model = getModel( stream );
    if ( model ) {
        Statement s;
        stream >> s;

        stream << model->removeStatement( s );
        stream << model->lastError();
    }
    else {
        stream << Error::ErrorInvalidArgument << Error::Error( "Invalid model id" );
    }
    //qDebug() << "(ServerConnection::removeStatement) done";
}


void Soprano::Server::ServerConnection::Private::removeAllStatements( QDataStream& stream )
{
    //qDebug() << "(ServerConnection::removeAllStatements)";
    Model* model = getModel( stream );
    if ( model ) {
        Statement s;
        stream >> s;

        stream << model->removeAllStatements( s );
        stream << model->lastError();
    }
    else {
        stream << Error::ErrorInvalidArgument << Error::Error( "Invalid model id" );
    }
    //qDebug() << "(ServerConnection::removeAllStatements) done";
}


void Soprano::Server::ServerConnection::Private::listStatements( QDataStream& stream )
{
    //qDebug() << "(ServerConnection::listStatements)";
    Model* model = getModel( stream );
    if ( model ) {
        Statement s;
        stream >> s;

        StatementIterator it = model->listStatements( s );
        stream << mapIterator( it ) << model->lastError();
    }
    else {
        stream << Error::ErrorInvalidArgument << Error::Error( "Invalid model id" );
    }
    //qDebug() << "(ServerConnection::listStatements) done";
}


void Soprano::Server::ServerConnection::Private::containsStatement( QDataStream& stream )
{
    //qDebug() << "(ServerConnection::containsStatement)";
    Model* model = getModel( stream );
    if ( model ) {
        Statement s;
        stream >> s;

        stream << model->containsStatement( s );
        stream << model->lastError();
    }
    else {
        stream << Error::ErrorInvalidArgument << Error::Error( "Invalid model id" );
    }
    //qDebug() << "(ServerConnection::containsStatement) done";
}


void Soprano::Server::ServerConnection::Private::containsAnyStatement( QDataStream& stream )
{
    //qDebug() << "(ServerConnection::containsAnyStatement)";
    Model* model = getModel( stream );
    if ( model ) {
        Statement s;
        stream >> s;

        stream << model->containsAnyStatement( s );
        stream << model->lastError();
    }
    else {
        stream << Error::ErrorInvalidArgument << Error::Error( "Invalid model id" );
    }
    //qDebug() << "(ServerConnection::containsAnyStatement) done";
}


void Soprano::Server::ServerConnection::Private::listContexts( QDataStream& stream )
{
    Model* model = getModel( stream );
    if ( model ) {
        NodeIterator it = model->listContexts();
        stream << mapIterator( it ) << model->lastError();
    }
    else {
        stream << Error::ErrorInvalidArgument << Error::Error( "Invalid model id" );
    }
}


void Soprano::Server::ServerConnection::Private::query( QDataStream& stream )
{
    Model* model = getModel( stream );
    if ( model ) {
        QString queryString;
        quint16 queryLang;
        QString userLang;
        stream >> queryString >> queryLang >> userLang;
        QueryResultIterator it = model->executeQuery( queryString, ( Query::QueryLanguage )queryLang, userLang );
        stream << mapIterator( it ) << model->lastError();
    }
    else {
        stream << Error::ErrorInvalidArgument << Error::Error( "Invalid model id" );
    }
}


void Soprano::Server::ServerConnection::Private::statementCount( QDataStream& stream )
{
    Model* model = getModel( stream );
    if ( model ) {
        qint32 count = model->statementCount();
        stream << count << model->lastError();
    }
    else {
        stream << Error::ErrorInvalidArgument << Error::Error( "Invalid model id" );
    }
}


void Soprano::Server::ServerConnection::Private::isEmpty( QDataStream& stream )
{
    Model* model = getModel( stream );
    if ( model ) {
        stream << model->isEmpty();
        stream << model->lastError();
    }
    else {
        stream << Error::ErrorInvalidArgument << Error::Error( "Invalid model id" );
    }
}


void Soprano::Server::ServerConnection::Private::createBlankNode( QDataStream& stream )
{
    Model* model = getModel( stream );
    if ( model ) {
        stream << model->createBlankNode();
        stream << model->lastError();
    }
    else {
        stream << Error::ErrorInvalidArgument << Error::Error( "Invalid model id" );
    }
}


void Soprano::Server::ServerConnection::Private::iteratorNext( QDataStream& stream )
{
    //qDebug() << "(ServerConnection::iteratorNext)";
    quint32 id = 0;
    stream >> id;

    QHash<quint32, StatementIterator>::iterator it1 = openStatementIterators.find( id );
    if ( it1 != openStatementIterators.end() ) {
        stream << it1.value().next();
        stream << it1.value().lastError();
        return;
    }

    QHash<quint32, NodeIterator>::iterator it2 = openNodeIterators.find( id );
    if ( it2 != openNodeIterators.end() ) {
        stream << it2.value().next();
        stream << it2.value().lastError();
        return;
    }

    QHash<quint32, QueryResultIterator>::iterator it3 = openQueryIterators.find( id );
    if ( it3 != openQueryIterators.end() ) {
        stream << it3.value().next();
        stream << it3.value().lastError();
        return;
    }

    stream << false << Error::Error( "Invalid iterator ID." );
    //qDebug() << "(ServerConnection::iteratorNext) done";
}


void Soprano::Server::ServerConnection::Private::statementIteratorCurrent( QDataStream& stream )
{
    //qDebug() << "(ServerConnection::statementIteratorCurrent)";
    quint32 id = 0;
    stream >> id;

    QHash<quint32, StatementIterator>::iterator it = openStatementIterators.find( id );
    if ( it != openStatementIterators.end() ) {
        stream << it.value().current();
        stream << it.value().lastError();
        return;
    }

    // could be a graph query iterator
    QHash<quint32, QueryResultIterator>::iterator it2 = openQueryIterators.find( id );
    if ( it2 != openQueryIterators.end() ) {
        stream << it2.value().currentStatement();
        stream << it2.value().lastError();
        return;
    }

    stream << Statement() << Error::Error( "Invalid iterator ID." );
    //qDebug() << "(ServerConnection::statementIteratorCurrent) done";
}


void Soprano::Server::ServerConnection::Private::nodeIteratorCurrent( QDataStream& stream )
{
    //qDebug() << "(ServerConnection::nodeIteratorCurrent)";
    quint32 id = 0;
    stream >> id;

    QHash<quint32, NodeIterator>::iterator it = openNodeIterators.find( id );
    if ( it != openNodeIterators.end() ) {
        stream << it.value().current();
        stream << it.value().lastError();
    }
    else {
        stream << Node() << Error::Error( "Invalid iterator ID." );
    }
    //qDebug() << "(ServerConnection::nodeIteratorCurrent) done";
}


void Soprano::Server::ServerConnection::Private::queryIteratorCurrent( QDataStream& stream )
{
    //qDebug() << "(ServerConnection::queryIteratorCurrent)";
    quint32 id = 0;
    stream >> id;

    QHash<quint32, QueryResultIterator>::iterator it = openQueryIterators.find( id );
    if ( it != openQueryIterators.end() ) {
        stream << it.value().current();
        stream << it.value().lastError();
    }
    else {
        stream << BindingSet() << Error::Error( "Invalid iterator ID." );
    }
    //qDebug() << "(ServerConnection::queryIteratorCurrent) done";
}


void Soprano::Server::ServerConnection::Private::iteratorClose( QDataStream& stream )
{
    //qDebug() << "(ServerConnection::iteratorClose)";
    quint32 id = 0;
    stream >> id;

    QHash<quint32, StatementIterator>::iterator it1 = openStatementIterators.find( id );
    if ( it1 != openStatementIterators.end() ) {
        it1.value().close();
        stream << it1.value().lastError();
        openStatementIterators.erase( it1 );
        return;
    }

    QHash<quint32, NodeIterator>::iterator it2 = openNodeIterators.find( id );
    if ( it2 != openNodeIterators.end() ) {
        it2.value().close();
        stream << it2.value().lastError();
        openNodeIterators.erase( it2 );
        return;
    }

    QHash<quint32, QueryResultIterator>::iterator it3 = openQueryIterators.find( id );
    if ( it3 != openQueryIterators.end() ) {
        it3.value().close();
        stream << it3.value().lastError();
        openQueryIterators.erase( it3 );
        return;
    }

    stream << Error::Error( "Invalid iterator ID." );
    //qDebug() << "(ServerConnection::iteratorClose) done";
}


void Soprano::Server::ServerConnection::Private::queryIteratorType( QDataStream& stream )
{
    //qDebug() << "(ServerConnection::queryIteratorType)";
    quint32 id = 0;
    stream >> id;

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
        stream << type << it.value().lastError();
    }
    else {
        stream << ( quint8 )0 << Error::Error( "Invalid iterator ID." );
    }
    //qDebug() << "(ServerConnection::queryIteratorType) done";
}


void Soprano::Server::ServerConnection::Private::queryIteratorBoolValue( QDataStream& stream )
{
    //qDebug() << "(ServerConnection::queryIteratorBoolValue)";
    quint32 id = 0;
    stream >> id;

    QHash<quint32, QueryResultIterator>::iterator it = openQueryIterators.find( id );
    if ( it != openQueryIterators.end() ) {
        stream << it.value().boolValue();
        stream << it.value().lastError();
    }
    else {
        stream << false << Error::Error( "Invalid iterator ID." );
    }
    //qDebug() << "(ServerConnection::queryIteratorBoolValue) done";
}


void Soprano::Server::ServerConnection::Private::supportsProtocolVersion( QDataStream& stream )
{
    //qDebug() << "(ServerConnection::supportsProtocolVersion)";
    quint32 requestedVersion;
    stream >> requestedVersion;

    stream << ( requestedVersion <= PROTOCOL_VERSION ? true : false );
    //qDebug() << "(ServerConnection::supportsProtocolVersion) done";
}

#include "serverconnection.moc"
