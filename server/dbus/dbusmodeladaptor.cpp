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
#include "iteratorwrapper.h"
#include "dbusstatementiteratoradaptor.h"
#include "dbusnodeiteratoradaptor.h"
#include "dbusqueryresultiteratoradaptor.h"
#include "dbusoperators.h"

#include <QtCore/QObject>
#include <QtCore/QMetaObject>
#include <QtCore/QVariant>
#include <QtDBus/QtDBus>
#include <QtCore/QList>
#include <QtCore/QStringList>
#include <QtCore/QMutex>
#include <QtCore/QPointer>
#include <QtCore/QMultiHash>
#include <QtCore/QDebug>

#include "soprano/model.h"
#include "soprano/node.h"

#include "mutexmodel.h"


class Soprano::Server::DBusModelAdaptor::Private
{
public:
    Private()
        : m_iteratorCount( 0 ) {
    }

    Model* origModel;
    Model* model;
    QMultiHash<QString, QPointer<IteratorWrapper> > openIterators;

    QString dbusObjectPath;

    QString createUniqueIteratorDBusObjectPath() {
        return QString( "%1/iterator%2" ).arg( dbusObjectPath ).arg( ++m_iteratorCount );
    }

private:
    int m_iteratorCount;
};


Soprano::Server::DBusModelAdaptor::DBusModelAdaptor( Model* model, QObject* parent, const QString& dbusObjectPath )
    : QDBusAbstractAdaptor( parent ),
      d( new Private() )
{
    qDBusRegisterMetaType<Soprano::Node>();
    qDBusRegisterMetaType<Soprano::Statement>();
    qDBusRegisterMetaType<Soprano::BindingSet>();

    d->dbusObjectPath = dbusObjectPath;
    d->origModel = model;
    d->model = new Util::MutexModel( Util::MutexModel::ReadWriteSingleThreading, model );

    // we cannot use setAutoRelaySignals here since that would connect (non-existing)
    // signals from parent instead of model
    connect( model, SIGNAL( statementsAdded() ),
             this, SIGNAL( statementsAdded() ) );
    connect( model, SIGNAL( statementsRemoved() ),
             this, SIGNAL( statementsRemoved() ) );

    connect( QDBusConnection::sessionBus().interface(), SIGNAL(serviceOwnerChanged(const QString&, const QString&, const QString&)),
             this, SLOT(slotServiceOwnerChanged(const QString&, const QString&, const QString&)) );
}

Soprano::Server::DBusModelAdaptor::~DBusModelAdaptor()
{
    delete d->model;
    delete d;
}

int Soprano::Server::DBusModelAdaptor::addStatement( const Soprano::Statement& statement, const QDBusMessage& m )
{
    // handle method call org.soprano.Model.addStatement
    int errorCode = ( int )d->model->addStatement( statement );
    if ( d->model->lastError() ) {
        DBus::sendErrorReply( m, d->model->lastError() );
    }
    return errorCode;
}

bool Soprano::Server::DBusModelAdaptor::containsAnyStatement( const Soprano::Statement& statement, const QDBusMessage& m )
{
    // handle method call org.soprano.Model.containsAnyStatement
    bool r = d->model->containsAnyStatement( statement );
    if ( d->model->lastError() ) {
        DBus::sendErrorReply( m, d->model->lastError() );
    }
    return r;
}

bool Soprano::Server::DBusModelAdaptor::containsStatement( const Soprano::Statement& statement, const QDBusMessage& m )
{
    // handle method call org.soprano.Model.containsStatement
    bool r = d->model->containsStatement( statement );
    if ( d->model->lastError() ) {
        DBus::sendErrorReply( m, d->model->lastError() );
    }
    return r;
}

Soprano::Node Soprano::Server::DBusModelAdaptor::createBlankNode( const QDBusMessage& m )
{
    // handle method call org.soprano.Model.createBlankNode
    Soprano::Node node = d->model->createBlankNode();
    if ( d->model->lastError() ) {
        DBus::sendErrorReply( m, d->model->lastError() );
    }
    return node;
}

QString Soprano::Server::DBusModelAdaptor::executeQuery( const QString &query, const QString& queryLanguage, const QDBusMessage& m )
{
    // handle method call org.soprano.Model.executeQuery
    QueryResultIterator it = d->model->executeQuery( query, Query::queryLanguageFromString( queryLanguage ), queryLanguage );
    if ( it.isValid() ) {
        IteratorWrapper* itW = new IteratorWrapper( it, this );
        d->openIterators.insert( m.service(), itW );
        ( void )new DBusQueryResultIteratorAdaptor( itW );
        QString objectPath = d->createUniqueIteratorDBusObjectPath();
        QDBusConnection::sessionBus().registerObject( objectPath, itW );
        return objectPath;
    }
    else {
        DBus::sendErrorReply( m, d->model->lastError() );
        return QString();
    }
}

bool Soprano::Server::DBusModelAdaptor::isEmpty( const QDBusMessage& m )
{
    // handle method call org.soprano.Model.isEmpty
    bool r = d->model->isEmpty();
    if ( d->model->lastError() ) {
        DBus::sendErrorReply( m, d->model->lastError() );
    }
    return r;
}


QString Soprano::Server::DBusModelAdaptor::listContexts( const QDBusMessage& m )
{
    // handle method call org.soprano.Model.listContexts
    NodeIterator it = d->model->listContexts();
    if ( it.isValid() ) {
        IteratorWrapper* itW = new IteratorWrapper( it, this );
        d->openIterators.insert( m.service(), itW );
        ( void )new DBusNodeIteratorAdaptor( itW );
        QString objectPath = d->createUniqueIteratorDBusObjectPath();
        QDBusConnection::sessionBus().registerObject( objectPath, itW );
        return objectPath;
    }
    else {
        DBus::sendErrorReply( m, d->model->lastError() );
        return QString();
    }
}

QString Soprano::Server::DBusModelAdaptor::listStatements( const Soprano::Statement& statement, const QDBusMessage& m )
{
    // handle method call org.soprano.Model.listStatements
    StatementIterator it = d->model->listStatements( statement );
    if ( it.isValid() ) {
        IteratorWrapper* itW = new IteratorWrapper( it, this );
        d->openIterators.insert( m.service(), itW );
        ( void )new DBusStatementIteratorAdaptor( itW );
        QString objectPath = d->createUniqueIteratorDBusObjectPath();
        QDBusConnection::sessionBus().registerObject( objectPath, itW );
        return objectPath;
    }
    else {
        DBus::sendErrorReply( m, d->model->lastError() );
        return QString();
    }
}

int Soprano::Server::DBusModelAdaptor::removeAllStatements( const Soprano::Statement& statement, const QDBusMessage& m )
{
    // handle method call org.soprano.Model.removeAllStatements
    int errorCode = ( int )d->model->removeAllStatements( statement );
    if ( d->model->lastError() ) {
        DBus::sendErrorReply( m, d->model->lastError() );
    }
    return errorCode;
}

int Soprano::Server::DBusModelAdaptor::removeStatement( const Soprano::Statement& statement, const QDBusMessage& m )
{
    // handle method call org.soprano.Model.removeStatement
    int errorCode = ( int )d->model->removeStatement( statement );
    if ( d->model->lastError() ) {
        DBus::sendErrorReply( m, d->model->lastError() );
    }
    return errorCode;
}

int Soprano::Server::DBusModelAdaptor::statementCount( const QDBusMessage& m )
{
    // handle method call org.soprano.Model.statementCount
    int cnt = d->model->statementCount();
    if ( d->model->lastError() ) {
        DBus::sendErrorReply( m, d->model->lastError() );
    }
    return cnt;
}


void Soprano::Server::DBusModelAdaptor::slotServiceOwnerChanged( const QString& name, const QString&, const QString& )
{
    // delete all iterators that were opened by that client
    QList<QPointer<IteratorWrapper> > its = d->openIterators.values( name );
    for ( int i = 0; i < its.count(); ++i ) {
        if ( its[i] ) {
            delete its[i];
        }
    }
    d->openIterators.remove( name );
}

#include "dbusmodeladaptor.moc"
