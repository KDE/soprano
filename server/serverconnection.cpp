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

#include "querylegacy.h"
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

Q_DECLARE_METATYPE( QAbstractSocket::SocketError )



class Soprano::Server::ServerConnection::Private
{
public:
    ServerCore* core;
    int socketDesc;
    QTcpSocket* socket;

    QHash<quint32, Model*> modelIdMap;
    QHash<quint32, StatementIterator> openStatementIterators;
    QHash<quint32, NodeIterator> openNodeIterators;
    QHash<quint32, QueryResultIterator> openQueryIterators;

    quint32 generateUniqueId();
    Soprano::Model* getModel( QDataStream& stream );
    quint32 mapIterator( const StatementIterator& it );
    quint32 mapIterator( const NodeIterator& it );
    quint32 mapIterator( const QueryResultIterator& it );

    void supportsProtocolVersion( QDataStream& stream );

    void createModel( QDataStream& stream );
    void supportedFeatures( QDataStream& );
    void addStatement( QDataStream& stream );
    void removeStatements( QDataStream& stream );
    void listStatements( QDataStream& stream );
    void containsStatements( QDataStream& stream );
    void listContexts( QDataStream& stream );
    void statementCount( QDataStream& stream );
    void isEmpty( QDataStream& stream );
    void query( QDataStream& );

    void iteratorNext( QDataStream& stream );
    void statementIteratorCurrent( QDataStream& stream );
    void nodeIteratorCurrent( QDataStream& stream );
    void queryIteratorCurrent( QDataStream& stream );
    void iteratorClose( QDataStream& stream );
    void queryIteratorCurrentStatement( QDataStream& stream );
    void queryIteratorType( QDataStream& stream );
    void queryIteratorBoolValue( QDataStream& stream );
};


Soprano::Server::ServerConnection::ServerConnection( ServerCore* core, int socket )
    : d( new Private() )
{
    qRegisterMetaType<QAbstractSocket::SocketError>();
    d->core = core;
    d->socketDesc = socket;
}


Soprano::Server::ServerConnection::~ServerConnection()
{
    qDebug() << "Removing connection";
    delete d;
}


void Soprano::Server::ServerConnection::close()
{
    d->socket->close();
}


void Soprano::Server::ServerConnection::slotError( QAbstractSocket::SocketError error )
{
    qDebug() << "Error: " << error;
    close();
}


void Soprano::Server::ServerConnection::run()
{
    d->socket = new QTcpSocket;
    d->socket->setSocketDescriptor( d->socketDesc );
    connect( d->socket, SIGNAL(error(QAbstractSocket::SocketError)),
             this, SLOT(slotError(QAbstractSocket::SocketError)) );

    while ( d->socket->isValid() ) {
        if ( d->socket->waitForReadyRead() ) {
            qDebug() << "ServerConnection: reading command.";
            QDataStream stream( d->socket );
            quint16 command = 0;
            stream >> command;
            switch( command ) {
            case COMMAND_SUPPORTS_PROTOCOL_VERSION:
                d->supportsProtocolVersion( stream );
                break;

            case COMMAND_CREATE_MODEL:
                d->createModel( stream );
                break;

            case COMMAND_SUPPORTED_FEATURES:
                d->supportedFeatures( stream );
                break;

            case COMMAND_MODEL_ADD_STATEMENT:
                d->addStatement( stream );
                break;

            case COMMAND_MODEL_REMOVE_STATEMENTS:
                d->removeStatements( stream );
                break;

            case COMMAND_MODEL_LIST_STATEMENTS:
                d->listStatements( stream );
                break;

            case COMMAND_MODEL_CONTAINS_STATEMENTS:
                d->containsStatements( stream );
                break;

            case COMMAND_MODEL_LIST_CONTEXTS:
                d->listContexts( stream );
                break;

            case COMMAND_MODEL_STATEMENT_COUNT:
                d->statementCount( stream );
                break;

            case COMMAND_MODEL_IS_EMPTY:
                d->isEmpty( stream );
                break;

            case COMMAND_MODEL_QUERY:
                d->query( stream );
                break;

            case COMMAND_ITERATOR_NEXT:
                d->iteratorNext( stream );
                break;

            case COMMAND_ITERATOR_CURRENT_STATEMENT:
                d->statementIteratorCurrent( stream );
                break;

            case COMMAND_ITERATOR_CURRENT_NODE:
                d->nodeIteratorCurrent( stream );
                break;

            case COMMAND_ITERATOR_CURRENT_BINDINGSET:
                d->queryIteratorCurrent( stream );
                break;

            case COMMAND_ITERATOR_CLOSE:
                d->iteratorClose( stream );
                break;

            case COMMAND_ITERATOR_QUERY_TYPE:
                d->queryIteratorType( stream );
                break;

            case COMMAND_ITERATOR_QUERY_BOOL_VALUE:
                d->queryIteratorBoolValue( stream );
                break;

            default:
                // FIXME: handle an error
                // for now we just close the connection on error.
                qDebug() << "Unknown command: " << command;
                d->socket->close();
            }
        }
    }

    delete d->socket;

    qDebug() << "(ServerConnection) done.";
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
    qDebug() << "(ServerConnection::createModel)";

    // extract options
    QString name;
    QList<BackendSetting> settings;
    stream >> name >> settings;

    // for now we ignore the settings

    // see if we already have that model
    Model* model = core->model( name );
    quint32 id = 0;
    if ( model ) {
        id = generateUniqueId();
        modelIdMap.insert( id, model );
    }

    stream << id << Error::Error();
    qDebug() << "(ServerConnection::createModel) done";
}


void Soprano::Server::ServerConnection::Private::supportedFeatures( QDataStream& stream )
{
    qDebug() << "(ServerConnection::supportedFeatures)";

    quint32 features = 0;
    Error::Error error;
    if ( core->backend() ) {
        features = ( quint32 )core->backend()->supportedFeatures();
    }
    else {
        error = Error::Error( "No backend available" );
    }

    stream << features << error;
    qDebug() << "(ServerConnection::supportedFeatures) done";
}


void Soprano::Server::ServerConnection::Private::addStatement( QDataStream& stream )
{
    qDebug() << "(ServerConnection::addStatement)";
    Model* model = getModel( stream );
    if ( model ) {
        Statement s;
        stream >> s;

        stream << model->addStatement( s ) << model->lastError();
    }
    else {
        stream << Error::ERROR_INVALID_ARGUMENT << Error::Error( "Invalid model id" );
    }
    qDebug() << "(ServerConnection::addStatement) done";
}


void Soprano::Server::ServerConnection::Private::removeStatements( QDataStream& stream )
{
    qDebug() << "(ServerConnection::removeStatements)";
    Model* model = getModel( stream );
    if ( model ) {
        Statement s;
        stream >> s;

        stream << model->removeStatements( s ) << model->lastError();
    }
    else {
        stream << Error::ERROR_INVALID_ARGUMENT << Error::Error( "Invalid model id" );
    }
    qDebug() << "(ServerConnection::removeStatements) done";
}


void Soprano::Server::ServerConnection::Private::listStatements( QDataStream& stream )
{
    qDebug() << "(ServerConnection::listStatements)";
    Model* model = getModel( stream );
    if ( model ) {
        Statement s;
        stream >> s;

        StatementIterator it = model->listStatements( s );
        stream << mapIterator( it ) << model->lastError();
    }
    else {
        stream << Error::ERROR_INVALID_ARGUMENT << Error::Error( "Invalid model id" );
    }
    qDebug() << "(ServerConnection::listStatements) done";
}


void Soprano::Server::ServerConnection::Private::containsStatements( QDataStream& stream )
{
    qDebug() << "(ServerConnection::containsStatements)";
    Model* model = getModel( stream );
    if ( model ) {
        Statement s;
        stream >> s;

        bool b = model->containsStatements( s );
        stream << b << model->lastError();
    }
    else {
        stream << Error::ERROR_INVALID_ARGUMENT << Error::Error( "Invalid model id" );
    }
    qDebug() << "(ServerConnection::containsStatements) done";
}


void Soprano::Server::ServerConnection::Private::listContexts( QDataStream& stream )
{
    Model* model = getModel( stream );
    if ( model ) {
        NodeIterator it = model->listContexts();
        stream << mapIterator( it ) << model->lastError();
    }
    else {
        stream << Error::ERROR_INVALID_ARGUMENT << Error::Error( "Invalid model id" );
    }
}


void Soprano::Server::ServerConnection::Private::query( QDataStream& stream )
{
    Model* model = getModel( stream );
    if ( model ) {
        QString queryString;
        quint16 queryLang;
        stream >> queryString >> queryLang;
        QueryResultIterator it = model->executeQuery( QueryLegacy( queryString, ( QueryLegacy::QueryType )queryLang ) );
        stream << mapIterator( it ) << model->lastError();
    }
    else {
        stream << Error::ERROR_INVALID_ARGUMENT << Error::Error( "Invalid model id" );
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
        stream << Error::ERROR_INVALID_ARGUMENT << Error::Error( "Invalid model id" );
    }
}


void Soprano::Server::ServerConnection::Private::isEmpty( QDataStream& stream )
{
    Model* model = getModel( stream );
    if ( model ) {
        stream << model->isEmpty() << model->lastError();
    }
    else {
        stream << Error::ERROR_INVALID_ARGUMENT << Error::Error( "Invalid model id" );
    }
}


void Soprano::Server::ServerConnection::Private::iteratorNext( QDataStream& stream )
{
    qDebug() << "(ServerConnection::iteratorNext)";
    quint32 id = 0;
    stream >> id;

    QHash<quint32, StatementIterator>::iterator it1 = openStatementIterators.find( id );
    if ( it1 != openStatementIterators.end() ) {
        stream << it1.value().next() << it1.value().lastError();
        return;
    }

    QHash<quint32, NodeIterator>::iterator it2 = openNodeIterators.find( id );
    if ( it2 != openNodeIterators.end() ) {
        stream << it2.value().next() << it2.value().lastError();
        return;
    }

    QHash<quint32, QueryResultIterator>::iterator it3 = openQueryIterators.find( id );
    if ( it3 != openQueryIterators.end() ) {
        stream << it3.value().next() << it3.value().lastError();
        return;
    }

    stream << false << Error::Error( "Invalid iterator ID." );
    qDebug() << "(ServerConnection::iteratorNext) done";
}


void Soprano::Server::ServerConnection::Private::statementIteratorCurrent( QDataStream& stream )
{
    qDebug() << "(ServerConnection::statementIteratorCurrent)";
    quint32 id = 0;
    stream >> id;

    QHash<quint32, StatementIterator>::iterator it = openStatementIterators.find( id );
    if ( it != openStatementIterators.end() ) {
        stream << it.value().current() << it.value().lastError();
        return;
    }

    // could be a graph query iterator
    QHash<quint32, QueryResultIterator>::iterator it2 = openQueryIterators.find( id );
    if ( it2 != openQueryIterators.end() ) {
        stream << it2.value().currentStatement() << it2.value().lastError();
        return;
    }

    stream << Statement() << Error::Error( "Invalid iterator ID." );
    qDebug() << "(ServerConnection::statementIteratorCurrent) done";
}


void Soprano::Server::ServerConnection::Private::nodeIteratorCurrent( QDataStream& stream )
{
    qDebug() << "(ServerConnection::nodeIteratorCurrent)";
    quint32 id = 0;
    stream >> id;

    QHash<quint32, NodeIterator>::iterator it = openNodeIterators.find( id );
    if ( it != openNodeIterators.end() ) {
        stream << it.value().current() << it.value().lastError();
        return;
    }
    stream << Node() << Error::Error( "Invalid iterator ID." );
    qDebug() << "(ServerConnection::nodeIteratorCurrent) done";
}


void Soprano::Server::ServerConnection::Private::queryIteratorCurrent( QDataStream& stream )
{
    qDebug() << "(ServerConnection::queryIteratorCurrent)";
    quint32 id = 0;
    stream >> id;

    QHash<quint32, QueryResultIterator>::iterator it = openQueryIterators.find( id );
    if ( it != openQueryIterators.end() ) {
        stream << it.value().current() << it.value().lastError();
        return;
    }

    stream << BindingSet() << Error::Error( "Invalid iterator ID." );
    qDebug() << "(ServerConnection::queryIteratorCurrent) done";
}


void Soprano::Server::ServerConnection::Private::iteratorClose( QDataStream& stream )
{
    qDebug() << "(ServerConnection::iteratorClose)";
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
    qDebug() << "(ServerConnection::iteratorClose) done";
}


void Soprano::Server::ServerConnection::Private::queryIteratorType( QDataStream& stream )
{
    qDebug() << "(ServerConnection::queryIteratorType)";
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
        return;
    }

    stream << Statement() << Error::Error( "Invalid iterator ID." );
    qDebug() << "(ServerConnection::queryIteratorType) done";
}


void Soprano::Server::ServerConnection::Private::queryIteratorBoolValue( QDataStream& stream )
{
    qDebug() << "(ServerConnection::queryIteratorBoolValue)";
    quint32 id = 0;
    stream >> id;

    QHash<quint32, QueryResultIterator>::iterator it = openQueryIterators.find( id );
    if ( it != openQueryIterators.end() ) {
        stream << it.value().boolValue() << it.value().lastError();
        return;
    }

    stream << false << Error::Error( "Invalid iterator ID." );
    qDebug() << "(ServerConnection::queryIteratorBoolValue) done";
}


void Soprano::Server::ServerConnection::Private::supportsProtocolVersion( QDataStream& stream )
{
    qDebug() << "(ServerConnection::supportsProtocolVersion)";
    quint32 requestedVersion;
    stream >> requestedVersion;

    stream << ( requestedVersion <= PROTOCOL_VERSION ? true : false );
    qDebug() << "(ServerConnection::supportsProtocolVersion) done";
}

#include "serverconnection.moc"
