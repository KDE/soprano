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

#include "sesame2model.h"
#include "sesame2repository.h"
#include "sesame2repositoryconnection.h"
#include "sesame2utils.h"
#include "sesame2types.h"
#include "sesame2valuefactory.h"
#include "sesame2iterator.h"
#include "sesame2statementiteratorbackend.h"
#include "sesame2nodeiteratorbackend.h"
#include "sesame2queryresultiteratorbackend.h"
#include "jniwrapper.h"
#include "jobjectref.h"

#include <soprano/statementiterator.h>
#include <soprano/queryresultiterator.h>
#include <soprano/statement.h>
#include <soprano/nodeiterator.h>

#include <QtCore/QDebug>
#include <QtCore/QReadWriteLock>
#include <QtCore/QReadLocker>
#include <QtCore/QWriteLocker>


class Soprano::Sesame2::Model::Private {
public:
    RepositoryWrapper* repository;

    QReadWriteLock readWriteLock;

    QList<StatementIteratorBackend*> statementIterators;
    QList<NodeIteratorBackend*> nodeIterators;
    QList<QueryResultIteratorBackend*> queryIterators;
};


Soprano::Sesame2::Model::Model( const Backend* backend, RepositoryWrapper* repo )
    : Soprano::StorageModel( backend )
{
    d = new Private;
    d->repository = repo;
}


Soprano::Sesame2::Model::~Model()
{
    closeIterators();
    delete d->repository;
    delete d;
}


Soprano::Error::ErrorCode Soprano::Sesame2::Model::addStatement( const Statement &statement )
{
    d->readWriteLock.lockForWrite();

    clearError();

    if ( JObjectRef sesameStatement = d->repository->valueFactory()->convertStatement( statement ) ) {
        if ( JNIWrapper::instance()->exceptionOccured() ) {
            setError( JNIWrapper::instance()->convertAndClearException() );
            d->readWriteLock.unlock();
            return Error::ErrorUnknown;
        }
        d->repository->repositoryConnection()->addStatement( sesameStatement );
        if ( JNIWrapper::instance()->exceptionOccured() ) {
            qDebug() << "(Soprano::Sesame2::Model::addStatements) failed.";
            setError( JNIWrapper::instance()->convertAndClearException() );
            d->readWriteLock.unlock();
            return Error::ErrorUnknown;
        }
        else {
            d->readWriteLock.unlock();
            emit statementsAdded();
            return Error::ErrorNone;
        }
    }
    else {
        d->readWriteLock.unlock();
        return Error::ErrorUnknown;
    }
}


Soprano::NodeIterator Soprano::Sesame2::Model::listContexts() const
{
    d->readWriteLock.lockForRead();

    clearError();

    QList<Soprano::Node> contexts;

    JObjectRef ids = d->repository->repositoryConnection()->getContextIDs();

    if ( JNIWrapper::instance()->exceptionOccured() ) {
        qDebug() << "(Soprano::Sesame2::Model::listContexts) failed.";
        setError( JNIWrapper::instance()->convertAndClearException() );
        d->readWriteLock.unlock();
        return NodeIterator();
    }
    else {
        NodeIteratorBackend* it = new NodeIteratorBackend( ids, this );
        d->nodeIterators.append( it );
        return it;
    }
}


Soprano::QueryResultIterator Soprano::Sesame2::Model::executeQuery( const QString &query, Query::QueryLanguage language, const QString& userQueryLanguage ) const
{
    d->readWriteLock.lockForRead();

    clearError();

    if ( language != Soprano::Query::QUERY_LANGUAGE_SPARQL ) {
        setError( Error::Error( QString( "Unsupported query language %1." ).arg( Query::queryLanguageToString( language, userQueryLanguage ) ) ) );
        d->readWriteLock.unlock();
        return QueryResultIterator();
    }

    jfieldID sparqlID = JNIWrapper::instance()->env()->GetStaticFieldID( JNIWrapper::instance()->env()->FindClass( ORG_OPENRDF_QUERY_QUERYLANGUAGE ),
                                                                         "SPARQL",
                                                                         "L"ORG_OPENRDF_QUERY_QUERYLANGUAGE";" );
    JObjectRef sparqlQueryLang = JNIWrapper::instance()->env()->GetStaticObjectField( JNIWrapper::instance()->env()->FindClass( ORG_OPENRDF_QUERY_QUERYLANGUAGE ),
                                                                                      sparqlID );

    JObjectRef queryResult = d->repository->repositoryConnection()->query( sparqlQueryLang, JStringRef( query ) );

    if ( queryResult ) {
        QueryResultIteratorBackend* it = new QueryResultIteratorBackend( queryResult, this );
        d->queryIterators.append( it );
        return it;
    }
    else {
        setError( JNIWrapper::instance()->convertAndClearException() );
        d->readWriteLock.unlock();
        return QueryResultIterator();
    }
}


Soprano::StatementIterator Soprano::Sesame2::Model::listStatements( const Statement& statement ) const
{
    d->readWriteLock.lockForRead();

    clearError();

    // we are not using convertStatement here since we support wildcards
    JObjectRef subject = d->repository->valueFactory()->convertNode( statement.subject() );
    if ( JNIWrapper::instance()->exceptionOccured() ) {
        setError( JNIWrapper::instance()->convertAndClearException() );
        d->readWriteLock.unlock();
        return StatementIterator();
    }
    JObjectRef predicate = d->repository->valueFactory()->convertNode( statement.predicate() );
    if ( JNIWrapper::instance()->exceptionOccured() ) {
        setError( JNIWrapper::instance()->convertAndClearException() );
        d->readWriteLock.unlock();
        return StatementIterator();
    }
    JObjectRef object = d->repository->valueFactory()->convertNode( statement.object() );
    if ( JNIWrapper::instance()->exceptionOccured() ) {
        setError( JNIWrapper::instance()->convertAndClearException() );
        d->readWriteLock.unlock();
        return StatementIterator();
    }
    JObjectRef context = d->repository->valueFactory()->convertNode( statement.context() );
    if ( JNIWrapper::instance()->exceptionOccured() ) {
        setError( JNIWrapper::instance()->convertAndClearException() );
        d->readWriteLock.unlock();
        return StatementIterator();
    }

    JObjectRef results = d->repository->repositoryConnection()->getStatements( subject, predicate, object, context );
    if ( JNIWrapper::instance()->exceptionOccured() ) {
        qDebug() << "(Soprano::Sesame2::Model::listStatements) failed.";
        setError( JNIWrapper::instance()->convertAndClearException() );
        d->readWriteLock.unlock();
        return StatementIterator();
    }
    else {
        StatementIteratorBackend* it = new StatementIteratorBackend( results, this );
        d->statementIterators.append( it );
        return it;
    }
}


Soprano::Error::ErrorCode Soprano::Sesame2::Model::removeStatement( const Statement &statement )
{
    if ( !statement.isValid() ) {
        setError( "Invalid statement", Error::ErrorInvalidArgument );
        return Error::ErrorInvalidArgument;
    }

    // Sesame does not seems to support the "default context", i.e. no context, it is always used
    // as a wildcard, thus we have to do some serious hacking to get our way
    if ( statement.context().isEmpty() ) {
        QList<Statement> sl = listStatements( statement ).allStatements();
        // this may look weird but is perfectly ok since an empty context is used as
        // a wildcard in listStatements but not in QList.contains
        if ( sl.contains( statement ) ) {
            // we cannot use Model::removeStatements here since otherwise we might end
            // up in an endless loop if we have statements in the default context (i.e. empty)
            for ( QList<Statement>::const_iterator it = sl.constBegin();
                  it != sl.constEnd(); ++it ) {
                Error::ErrorCode c = removeAllStatements( *it );
                if ( lastError() ) {
                    return c;
                }
            }
            sl.removeAll( statement );
            return addStatements( sl );
        }
    }
    else {
        return removeAllStatements( statement );
    }

    // make gcc shut up
    return Error::ErrorNone;
}


Soprano::Error::ErrorCode Soprano::Sesame2::Model::removeAllStatements( const Statement &statement )
{
    d->readWriteLock.lockForWrite();

    clearError();

    // we are not using convertStatement here since we support wildcards
    JObjectRef subject = d->repository->valueFactory()->convertNode( statement.subject() );
    if ( JNIWrapper::instance()->exceptionOccured() ) {
        setError( JNIWrapper::instance()->convertAndClearException() );
        d->readWriteLock.unlock();
        return Error::ErrorUnknown;
    }
    JObjectRef predicate = d->repository->valueFactory()->convertNode( statement.predicate() );
    if ( JNIWrapper::instance()->exceptionOccured() ) {
        setError( JNIWrapper::instance()->convertAndClearException() );
        d->readWriteLock.unlock();
        return Error::ErrorUnknown;
    }
    JObjectRef object = d->repository->valueFactory()->convertNode( statement.object() );
    if ( JNIWrapper::instance()->exceptionOccured() ) {
        setError( JNIWrapper::instance()->convertAndClearException() );
        d->readWriteLock.unlock();
        return Error::ErrorUnknown;
    }
    JObjectRef context = d->repository->valueFactory()->convertNode( statement.context() );
    if ( JNIWrapper::instance()->exceptionOccured() ) {
        setError( JNIWrapper::instance()->convertAndClearException() );
        d->readWriteLock.unlock();
        return Error::ErrorUnknown;
    }

    d->repository->repositoryConnection()->remove( subject, predicate, object, context );
    if ( JNIWrapper::instance()->exceptionOccured() ) {
        qDebug() << "(Soprano::Sesame2::Model::removeAllStatements) failed.";
        setError( JNIWrapper::instance()->convertAndClearException() );
        d->readWriteLock.unlock();
        return Error::ErrorUnknown;
    }

    d->readWriteLock.unlock();

    emit statementsRemoved();

    return Error::ErrorNone;
}


int Soprano::Sesame2::Model::statementCount() const
{
    QReadLocker lock( &d->readWriteLock );

    clearError();
    int size = d->repository->repositoryConnection()->size();
    if ( JNIWrapper::instance()->exceptionOccured() ) {
        setError( JNIWrapper::instance()->convertAndClearException() );
        return -1;
    }
    else {
        return size;
    }
}


bool Soprano::Sesame2::Model::isEmpty() const
{
    QReadLocker lock( &d->readWriteLock );

    clearError();
    bool empty = d->repository->repositoryConnection()->isEmpty();
    if ( JNIWrapper::instance()->exceptionOccured() ) {
        setError( JNIWrapper::instance()->convertAndClearException() );
        return true;
    }
    else {
        return empty;
    }
}


bool Soprano::Sesame2::Model::containsStatement( const Statement &statement ) const
{
    // As it seems sesame cannot handle the default graph in RepositoryConnection.hasStatement
    // it is always used as a wildcard. Thus, we have to use the default implementation based
    // on listStatements
    return StorageModel::containsStatement( statement );
}


bool Soprano::Sesame2::Model::containsAnyStatement( const Statement &statement ) const
{
    QReadLocker lock( &d->readWriteLock );

    clearError();

    // we are not using convertStatement here since we support wildcards
    JObjectRef subject = d->repository->valueFactory()->convertNode( statement.subject() );
    if ( JNIWrapper::instance()->exceptionOccured() ) {
        setError( JNIWrapper::instance()->convertAndClearException() );
        return false;
    }
    JObjectRef predicate = d->repository->valueFactory()->convertNode( statement.predicate() );
    if ( JNIWrapper::instance()->exceptionOccured() ) {
        setError( JNIWrapper::instance()->convertAndClearException() );
        return false;
    }
    JObjectRef object = d->repository->valueFactory()->convertNode( statement.object() );
    if ( JNIWrapper::instance()->exceptionOccured() ) {
        setError( JNIWrapper::instance()->convertAndClearException() );
        return false;
    }
    JObjectRef context = d->repository->valueFactory()->convertNode( statement.context() );
    if ( JNIWrapper::instance()->exceptionOccured() ) {
        setError( JNIWrapper::instance()->convertAndClearException() );
        return false;
    }

    bool r = d->repository->repositoryConnection()->hasStatement( subject, predicate, object, context );
    if ( JNIWrapper::instance()->exceptionOccured() ) {
        qDebug() << "(Soprano::Sesame2::Model::containsAnyStatements) failed.";
        setError( JNIWrapper::instance()->convertAndClearException() );
        return false;
    }
    return r;
}


Soprano::Node Soprano::Sesame2::Model::createBlankNode()
{
    JObjectRef bnode = d->repository->valueFactory()->createBNode();
    setError( JNIWrapper::instance()->convertAndClearException() );
    return convertNode( bnode );
}


void Soprano::Sesame2::Model::closeIterators()
{
    for ( QList<StatementIteratorBackend*>::iterator it = d->statementIterators.begin();
          it != d->statementIterators.end(); ++it ) {
        ( *it )->close();
    }
    for ( QList<NodeIteratorBackend*>::iterator it = d->nodeIterators.begin();
          it != d->nodeIterators.end(); ++it ) {
        ( *it )->close();
    }
    for ( QList<QueryResultIteratorBackend*>::iterator it = d->queryIterators.begin();
          it != d->queryIterators.end(); ++it ) {
        ( *it )->close();
    }
}


void Soprano::Sesame2::Model::removeIterator( StatementIteratorBackend* it ) const
{
    d->statementIterators.removeAll( it );
    d->readWriteLock.unlock();
}


void Soprano::Sesame2::Model::removeIterator( NodeIteratorBackend* it ) const
{
    d->nodeIterators.removeAll( it );
    d->readWriteLock.unlock();
}


void Soprano::Sesame2::Model::removeIterator( QueryResultIteratorBackend* r ) const
{
    d->queryIterators.removeAll( r );
    d->readWriteLock.unlock();
}
