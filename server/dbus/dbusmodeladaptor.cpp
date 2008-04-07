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

#include "dbusmodeladaptor.h"
#include "dbusutil.h"
#include "dbusexportiterator.h"
#include "dbusstatementiteratoradaptor.h"
#include "dbusnodeiteratoradaptor.h"
#include "dbusqueryresultiteratoradaptor.h"
#include "dbusoperators.h"
#include "dbusexportmodel.h"
#include "statementiterator.h"
#include "nodeiterator.h"
#include "queryresultiterator.h"
#include "asyncmodel.h"

#include <QtCore/QObject>
#include <QtCore/QMetaObject>
#include <QtCore/QVariant>
#include <QtDBus/QtDBus>
#include <QtCore/QList>
#include <QtCore/QStringList>
#include <QtCore/QMutex>
#include <QtCore/QPointer>
#include <QtCore/QDebug>

#include "soprano/model.h"
#include "soprano/node.h"


Q_DECLARE_METATYPE(Soprano::Error::ErrorCode)
Q_DECLARE_METATYPE(Soprano::StatementIterator)
Q_DECLARE_METATYPE(Soprano::NodeIterator)
Q_DECLARE_METATYPE(Soprano::QueryResultIterator)


class Soprano::Server::DBusModelAdaptor::Private
{
public:
    Private( DBusModelAdaptor* parent )
        : m_iteratorCount( 0 ),
          q( parent ) {
    }

    DBusExportModel* model;

    // map of delayed results in case we use an AsyncModel
    QHash<Util::AsyncResult*, QDBusMessage> delayedResultsHash;

    QString createUniqueIteratorDBusObjectPath() {
        return QString( "%1/iterator%2" ).arg( model->dbusObjectPath() ).arg( ++m_iteratorCount );
    }

    QString registerIterator( const StatementIterator& it, const QString& service );
    QString registerIterator( const NodeIterator& it, const QString& service );
    QString registerIterator( const QueryResultIterator& it, const QString& service );

    void _s_delayedResultReady( Soprano::Util::AsyncResult* );

private:
    int m_iteratorCount;
    DBusModelAdaptor* q;
};


QString Soprano::Server::DBusModelAdaptor::Private::registerIterator( const StatementIterator& it, const QString& service )
{
    DBusExportIterator* itW = new DBusExportIterator( it, q );
    itW->setDeleteOnClose( true );
    QString objectPath = createUniqueIteratorDBusObjectPath();
    itW->registerIterator( objectPath, service );
    return objectPath;
}


QString Soprano::Server::DBusModelAdaptor::Private::registerIterator( const NodeIterator& it, const QString& service )
{
    DBusExportIterator* itW = new DBusExportIterator( it, q );
    itW->setDeleteOnClose( true );
    QString objectPath = createUniqueIteratorDBusObjectPath();
    itW->registerIterator( objectPath, service );
    return objectPath;
}


QString Soprano::Server::DBusModelAdaptor::Private::registerIterator( const QueryResultIterator& it, const QString& service )
{
    DBusExportIterator* itW = new DBusExportIterator( it, q );
    itW->setDeleteOnClose( true );
    QString objectPath = createUniqueIteratorDBusObjectPath();
    itW->registerIterator( objectPath, service );
    return objectPath;
}


void Soprano::Server::DBusModelAdaptor::Private::_s_delayedResultReady( Util::AsyncResult* result )
{
    Q_ASSERT( delayedResultsHash.contains( result ) );
    QDBusMessage& msg = delayedResultsHash[result];

    if ( result->lastError() ) {
        DBus::sendErrorReply( msg, result->lastError() );
    }
    else {
        QVariant value = result->value();
        QVariant replyValue;

        if( value.userType() == QVariant::Bool ||
            value.userType() == QVariant::Int ||
            value.userType() == qMetaTypeId<Node>() ) {
            replyValue = value;
        }
        else if ( value.userType() == qMetaTypeId<Error::ErrorCode>() ) {
            replyValue = ( int )value.value<Error::ErrorCode>();
        }
        else if ( value.userType() == qMetaTypeId<StatementIterator>() ) {
            replyValue = registerIterator( value.value<StatementIterator>(), msg.service() );
        }
        else if ( value.userType() == qMetaTypeId<NodeIterator>() ) {
            replyValue = registerIterator( value.value<NodeIterator>(), msg.service() );
        }
        else if ( value.userType() == qMetaTypeId<QueryResultIterator>() ) {
            replyValue = registerIterator( value.value<QueryResultIterator>(), msg.service() );
        }
        else {
            Q_ASSERT( false );
        }

        QDBusMessage reply = msg.createReply( replyValue );
        QDBusConnection::sessionBus().send( reply );
    }
    delayedResultsHash.remove( result );
}


Soprano::Server::DBusModelAdaptor::DBusModelAdaptor( DBusExportModel* dbusModel )
    : QDBusAbstractAdaptor( dbusModel ),
      d( new Private( this ) )
{
    qDBusRegisterMetaType<Soprano::Node>();
    qDBusRegisterMetaType<Soprano::Statement>();
    qDBusRegisterMetaType<Soprano::BindingSet>();

    d->model = dbusModel;

    // we cannot use setAutoRelaySignals here since that would connect (non-existing)
    // signals from parent instead of model
    connect( dbusModel->parentModel(), SIGNAL( statementsAdded() ),
             this, SIGNAL( statementsAdded() ) );
    connect( dbusModel->parentModel(), SIGNAL( statementsRemoved() ),
             this, SIGNAL( statementsRemoved() ) );
    connect( dbusModel->parentModel(), SIGNAL( statementAdded(const Soprano::Statement&) ),
             this, SIGNAL( statementAdded(const Soprano::Statement&) ) );
    connect( dbusModel->parentModel(), SIGNAL( statementRemoved(const Soprano::Statement&) ),
             this, SIGNAL( statementRemoved(const Soprano::Statement&) ) );
}

Soprano::Server::DBusModelAdaptor::~DBusModelAdaptor()
{
    delete d;
}

int Soprano::Server::DBusModelAdaptor::addStatement( const Soprano::Statement& statement, const QDBusMessage& m )
{
    // handle method call org.soprano.Model.addStatement
    if ( Util::AsyncModel* am = qobject_cast<Util::AsyncModel*>( d->model->parentModel() ) ) {
        Util::AsyncResult* result = am->addStatementAsync( statement );
        connect( result, SIGNAL( resultReady( Soprano::Util::AsyncResult* ) ),
                 this, SLOT( _s_delayedResultReady( Soprano::Util::AsyncResult* ) ) );

        // create a delayed dummy result
        m.setDelayedReply( true );
        d->delayedResultsHash.insert( result, m );
        return 0;
    }
    else {
        int errorCode = ( int )d->model->addStatement( statement );
        if ( d->model->lastError() ) {
            DBus::sendErrorReply( m, d->model->lastError() );
        }
        return errorCode;
    }
}

bool Soprano::Server::DBusModelAdaptor::containsAnyStatement( const Soprano::Statement& statement, const QDBusMessage& m )
{
    // handle method call org.soprano.Model.containsAnyStatement
    if ( Util::AsyncModel* am = qobject_cast<Util::AsyncModel*>( d->model->parentModel() ) ) {
        Util::AsyncResult* result = am->containsAnyStatementAsync( statement );
        connect( result, SIGNAL( resultReady( Soprano::Util::AsyncResult* ) ),
                 this, SLOT( _s_delayedResultReady( Soprano::Util::AsyncResult* ) ) );

        // create a delayed dummy result
        m.setDelayedReply( true );
        d->delayedResultsHash.insert( result, m );
        return false;
    }
    else {
        bool r = d->model->containsAnyStatement( statement );
        if ( d->model->lastError() ) {
            DBus::sendErrorReply( m, d->model->lastError() );
        }
        return r;
    }
}

bool Soprano::Server::DBusModelAdaptor::containsStatement( const Soprano::Statement& statement, const QDBusMessage& m )
{
    // handle method call org.soprano.Model.containsStatement
    if ( Util::AsyncModel* am = qobject_cast<Util::AsyncModel*>( d->model->parentModel() ) ) {
        Util::AsyncResult* result = am->containsStatementAsync( statement );
        connect( result, SIGNAL( resultReady( Soprano::Util::AsyncResult* ) ),
                 this, SLOT( _s_delayedResultReady( Soprano::Util::AsyncResult* ) ) );

        // create a delayed dummy result
        m.setDelayedReply( true );
        d->delayedResultsHash.insert( result, m );
        return false;
    }
    else {
        bool r = d->model->containsStatement( statement );
        if ( d->model->lastError() ) {
            DBus::sendErrorReply( m, d->model->lastError() );
        }
        return r;
    }
}

Soprano::Node Soprano::Server::DBusModelAdaptor::createBlankNode( const QDBusMessage& m )
{
    // handle method call org.soprano.Model.createBlankNode
    if ( Util::AsyncModel* am = qobject_cast<Util::AsyncModel*>( d->model->parentModel() ) ) {
        Util::AsyncResult* result = am->createBlankNodeAsync();
        connect( result, SIGNAL( resultReady( Soprano::Util::AsyncResult* ) ),
                 this, SLOT( _s_delayedResultReady( Soprano::Util::AsyncResult* ) ) );

        // create a delayed dummy result
        m.setDelayedReply( true );
        d->delayedResultsHash.insert( result, m );
        return Node();
    }
    else {
        Soprano::Node node = d->model->createBlankNode();
        if ( d->model->lastError() ) {
            DBus::sendErrorReply( m, d->model->lastError() );
        }
        return node;
    }
}

QString Soprano::Server::DBusModelAdaptor::executeQuery( const QString &query, const QString& queryLanguage, const QDBusMessage& m )
{
    // handle method call org.soprano.Model.executeQuery
    if ( Util::AsyncModel* am = qobject_cast<Util::AsyncModel*>( d->model->parentModel() ) ) {
        Util::AsyncResult* result = am->executeQueryAsync( query, Query::queryLanguageFromString( queryLanguage ), queryLanguage );
        connect( result, SIGNAL( resultReady( Soprano::Util::AsyncResult* ) ),
                 this, SLOT( _s_delayedResultReady( Soprano::Util::AsyncResult* ) ) );

        // create a delayed dummy result
        m.setDelayedReply( true );
        d->delayedResultsHash.insert( result, m );
        return QString();
    }
    else {
        QueryResultIterator it = d->model->executeQuery( query, Query::queryLanguageFromString( queryLanguage ), queryLanguage );
        if ( it.isValid() ) {
            return d->registerIterator( it, m.service() );
        }
        else {
            DBus::sendErrorReply( m, d->model->lastError() );
            return QString();
        }
    }
}

bool Soprano::Server::DBusModelAdaptor::isEmpty( const QDBusMessage& m )
{
    // handle method call org.soprano.Model.isEmpty
    if ( Util::AsyncModel* am = qobject_cast<Util::AsyncModel*>( d->model->parentModel() ) ) {
        Util::AsyncResult* result = am->isEmptyAsync();
        connect( result, SIGNAL( resultReady( Soprano::Util::AsyncResult* ) ),
                 this, SLOT( _s_delayedResultReady( Soprano::Util::AsyncResult* ) ) );

        // create a delayed dummy result
        m.setDelayedReply( true );
        d->delayedResultsHash.insert( result, m );
        return false;
    }
    else {
        bool r = d->model->isEmpty();
        if ( d->model->lastError() ) {
            DBus::sendErrorReply( m, d->model->lastError() );
        }
        return r;
    }
}


QString Soprano::Server::DBusModelAdaptor::listContexts( const QDBusMessage& m )
{
    // handle method call org.soprano.Model.listContexts
    if ( Util::AsyncModel* am = qobject_cast<Util::AsyncModel*>( d->model->parentModel() ) ) {
        Util::AsyncResult* result = am->listContextsAsync();
        connect( result, SIGNAL( resultReady( Soprano::Util::AsyncResult* ) ),
                 this, SLOT( _s_delayedResultReady( Soprano::Util::AsyncResult* ) ) );

        // create a delayed dummy result
        m.setDelayedReply( true );
        d->delayedResultsHash.insert( result, m );
        return QString();
    }
    else {
        NodeIterator it = d->model->listContexts();
        if ( it.isValid() ) {
            return d->registerIterator( it, m.service() );
        }
        else {
            DBus::sendErrorReply( m, d->model->lastError() );
            return QString();
        }
    }
}

QString Soprano::Server::DBusModelAdaptor::listStatements( const Soprano::Statement& statement, const QDBusMessage& m )
{
    // handle method call org.soprano.Model.listStatements
    if ( Util::AsyncModel* am = qobject_cast<Util::AsyncModel*>( d->model->parentModel() ) ) {
        Util::AsyncResult* result = am->listStatementsAsync( statement );
        connect( result, SIGNAL( resultReady( Soprano::Util::AsyncResult* ) ),
                 this, SLOT( _s_delayedResultReady( Soprano::Util::AsyncResult* ) ) );

        // create a delayed dummy result
        m.setDelayedReply( true );
        d->delayedResultsHash.insert( result, m );
        return QString();
    }
    else {
        StatementIterator it = d->model->listStatements( statement );
        if ( it.isValid() ) {
            return d->registerIterator( it, m.service() );
        }
        else {
            DBus::sendErrorReply( m, d->model->lastError() );
            return QString();
        }
    }
}

int Soprano::Server::DBusModelAdaptor::removeAllStatements( const Soprano::Statement& statement, const QDBusMessage& m )
{
    // handle method call org.soprano.Model.removeAllStatements
    if ( Util::AsyncModel* am = qobject_cast<Util::AsyncModel*>( d->model->parentModel() ) ) {
        Util::AsyncResult* result = am->removeAllStatementsAsync( statement );
        connect( result, SIGNAL( resultReady( Soprano::Util::AsyncResult* ) ),
                 this, SLOT( _s_delayedResultReady( Soprano::Util::AsyncResult* ) ) );

        // create a delayed dummy result
        m.setDelayedReply( true );
        d->delayedResultsHash.insert( result, m );
        return 0;
    }
    else {
        int errorCode = ( int )d->model->removeAllStatements( statement );
        if ( d->model->lastError() ) {
            DBus::sendErrorReply( m, d->model->lastError() );
        }
        return errorCode;
    }
}

int Soprano::Server::DBusModelAdaptor::removeStatement( const Soprano::Statement& statement, const QDBusMessage& m )
{
    // handle method call org.soprano.Model.removeStatement
    if ( Util::AsyncModel* am = qobject_cast<Util::AsyncModel*>( d->model->parentModel() ) ) {
        Util::AsyncResult* result = am->removeStatementAsync( statement );
        connect( result, SIGNAL( resultReady( Soprano::Util::AsyncResult* ) ),
                 this, SLOT( _s_delayedResultReady( Soprano::Util::AsyncResult* ) ) );

        // create a delayed dummy result
        m.setDelayedReply( true );
        d->delayedResultsHash.insert( result, m );
        return 0;
    }
    else {
        int errorCode = ( int )d->model->removeStatement( statement );
        if ( d->model->lastError() ) {
            DBus::sendErrorReply( m, d->model->lastError() );
        }
        return errorCode;
    }
}

int Soprano::Server::DBusModelAdaptor::statementCount( const QDBusMessage& m )
{
    // handle method call org.soprano.Model.statementCount
    if ( Util::AsyncModel* am = qobject_cast<Util::AsyncModel*>( d->model->parentModel() ) ) {
        Util::AsyncResult* result = am->statementCountAsync();
        connect( result, SIGNAL( resultReady( Soprano::Util::AsyncResult* ) ),
                 this, SLOT( _s_delayedResultReady( Soprano::Util::AsyncResult* ) ) );

        // create a delayed dummy result
        m.setDelayedReply( true );
        d->delayedResultsHash.insert( result, m );
        return 0;
    }
    else {
        int cnt = d->model->statementCount();
        if ( d->model->lastError() ) {
            DBus::sendErrorReply( m, d->model->lastError() );
        }
        return cnt;
    }
}

#include "dbusmodeladaptor.moc"
