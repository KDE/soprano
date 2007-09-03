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
#include <soprano/querylegacy.h>
#include <soprano/nodeiterator.h>

#include <QtCore/QDebug>



Soprano::Sesame2::Model::Model( const Backend* backend, RepositoryWrapper* repo )
    : Soprano::StorageModel( backend ),
      m_repository( repo )
{
}


Soprano::Sesame2::Model::~Model()
{
    closeIterators();
    delete m_repository;
}


Soprano::Error::ErrorCode Soprano::Sesame2::Model::addStatement( const Statement &statement )
{
    clearError();
    if ( JObjectRef sesameStatement = m_repository->valueFactory()->convertStatement( statement ) ) {
        if ( JNIWrapper::instance()->exceptionOccured() ) {
            setError( JNIWrapper::instance()->convertAndClearException() );
            return Error::ERROR_UNKNOWN;
        }
        m_repository->repositoryConnection()->addStatement( sesameStatement );
        if ( JNIWrapper::instance()->exceptionOccured() ) {
            qDebug() << "(Soprano::Sesame2::Model::addStatements) failed.";
            setError( JNIWrapper::instance()->convertAndClearException() );
            return Error::ERROR_UNKNOWN;
        }
        else {
            return Error::ERROR_NONE;
        }
    }
    else {
        return Error::ERROR_UNKNOWN;
    }
}


Soprano::NodeIterator Soprano::Sesame2::Model::listContexts() const
{
    clearError();

    QList<Soprano::Node> contexts;

    JObjectRef ids = m_repository->repositoryConnection()->getContextIDs();

    if ( JNIWrapper::instance()->exceptionOccured() ) {
        qDebug() << "(Soprano::Sesame2::Model::listContexts) failed.";
        setError( JNIWrapper::instance()->convertAndClearException() );
        return NodeIterator();
    }
    else {
        m_openIterators.append( ids );
        return new NodeIteratorBackend( ids );
    }
}


Soprano::QueryResultIterator Soprano::Sesame2::Model::executeQuery( const QueryLegacy &query ) const
{
    clearError();

    if ( query.type() != Soprano::QueryLegacy::SPARQL ) {
        setError( Error::Error( "Unsupported query language." ) );
        return QueryResultIterator();
    }

    jfieldID sparqlID = JNIWrapper::instance()->env()->GetStaticFieldID( JNIWrapper::instance()->env()->FindClass( ORG_OPENRDF_QUERY_QUERYLANGUAGE ),
                                                                         "SPARQL",
                                                                         "L"ORG_OPENRDF_QUERY_QUERYLANGUAGE";" );
    JObjectRef sparqlQueryLang = JNIWrapper::instance()->env()->GetStaticObjectField( JNIWrapper::instance()->env()->FindClass( ORG_OPENRDF_QUERY_QUERYLANGUAGE ),
                                                                                      sparqlID );

    JObjectRef queryResult = m_repository->repositoryConnection()->query( sparqlQueryLang, JStringRef( query.query() ) );

    if ( queryResult ) {
        m_openIterators.append( queryResult );
        return new QueryResultIteratorBackend( queryResult );
    }
    else {
        setError( JNIWrapper::instance()->convertAndClearException() );
        return QueryResultIterator();
    }
}


Soprano::StatementIterator Soprano::Sesame2::Model::listStatements( const Statement& statement ) const
{
    clearError();

    // we are not using convertStatement here since we support wildcards
    JObjectRef subject = m_repository->valueFactory()->convertNode( statement.subject() );
    if ( JNIWrapper::instance()->exceptionOccured() ) {
        setError( JNIWrapper::instance()->convertAndClearException() );
        return StatementIterator();
    }
    JObjectRef predicate = m_repository->valueFactory()->convertNode( statement.predicate() );
    if ( JNIWrapper::instance()->exceptionOccured() ) {
        setError( JNIWrapper::instance()->convertAndClearException() );
        return StatementIterator();
    }
    JObjectRef object = m_repository->valueFactory()->convertNode( statement.object() );
    if ( JNIWrapper::instance()->exceptionOccured() ) {
        setError( JNIWrapper::instance()->convertAndClearException() );
        return StatementIterator();
    }
    JObjectRef context = m_repository->valueFactory()->convertNode( statement.context() );
    if ( JNIWrapper::instance()->exceptionOccured() ) {
        setError( JNIWrapper::instance()->convertAndClearException() );
        return StatementIterator();
    }

    JObjectRef results = m_repository->repositoryConnection()->getStatements( subject, predicate, object, context );
    if ( JNIWrapper::instance()->exceptionOccured() ) {
        qDebug() << "(Soprano::Sesame2::Model::listStatements) failed.";
        setError( JNIWrapper::instance()->convertAndClearException() );
        return StatementIterator();
    }
    else {
        m_openIterators.append( results );
        return new StatementIteratorBackend( results );
    }
}


Soprano::Error::ErrorCode Soprano::Sesame2::Model::removeStatements( const Statement &statement )
{
    clearError();

    // we are not using convertStatement here since we support wildcards
    JObjectRef subject = m_repository->valueFactory()->convertNode( statement.subject() );
    if ( JNIWrapper::instance()->exceptionOccured() ) {
        setError( JNIWrapper::instance()->convertAndClearException() );
        return Error::ERROR_UNKNOWN;
    }
    JObjectRef predicate = m_repository->valueFactory()->convertNode( statement.predicate() );
    if ( JNIWrapper::instance()->exceptionOccured() ) {
        setError( JNIWrapper::instance()->convertAndClearException() );
        return Error::ERROR_UNKNOWN;
    }
    JObjectRef object = m_repository->valueFactory()->convertNode( statement.object() );
    if ( JNIWrapper::instance()->exceptionOccured() ) {
        setError( JNIWrapper::instance()->convertAndClearException() );
        return Error::ERROR_UNKNOWN;
    }
    JObjectRef context = m_repository->valueFactory()->convertNode( statement.context() );
    if ( JNIWrapper::instance()->exceptionOccured() ) {
        setError( JNIWrapper::instance()->convertAndClearException() );
        return Error::ERROR_UNKNOWN;
    }

    m_repository->repositoryConnection()->remove( subject, predicate, object, context );
    if ( JNIWrapper::instance()->exceptionOccured() ) {
        qDebug() << "(Soprano::Sesame2::Model::removeStatements) failed.";
        setError( JNIWrapper::instance()->convertAndClearException() );
        return Error::ERROR_UNKNOWN;
    }

    return Error::ERROR_NONE;
}


int Soprano::Sesame2::Model::statementCount() const
{
    clearError();
    int size = m_repository->repositoryConnection()->size();
    if ( JNIWrapper::instance()->exceptionOccured() ) {
        setError( JNIWrapper::instance()->convertAndClearException() );
        return -1;
    }
    else {
        return size;
    }
}


bool Soprano::Sesame2::Model::containsStatements( const Statement &statement ) const
{
    clearError();

    // we are not using convertStatement here since we support wildcards
    JObjectRef subject = m_repository->valueFactory()->convertNode( statement.subject() );
    if ( JNIWrapper::instance()->exceptionOccured() ) {
        setError( JNIWrapper::instance()->convertAndClearException() );
        return false;
    }
    JObjectRef predicate = m_repository->valueFactory()->convertNode( statement.predicate() );
    if ( JNIWrapper::instance()->exceptionOccured() ) {
        setError( JNIWrapper::instance()->convertAndClearException() );
        return false;
    }
    JObjectRef object = m_repository->valueFactory()->convertNode( statement.object() );
    if ( JNIWrapper::instance()->exceptionOccured() ) {
        setError( JNIWrapper::instance()->convertAndClearException() );
        return false;
    }
    JObjectRef context = m_repository->valueFactory()->convertNode( statement.context() );
    if ( JNIWrapper::instance()->exceptionOccured() ) {
        setError( JNIWrapper::instance()->convertAndClearException() );
        return false;
    }

    bool r = m_repository->repositoryConnection()->hasStatement( subject, predicate, object, context );
    if ( JNIWrapper::instance()->exceptionOccured() ) {
        qDebug() << "(Soprano::Sesame2::Model::containsStatements) failed.";
        setError( JNIWrapper::instance()->convertAndClearException() );
        return false;
    }
    return r;
}


void Soprano::Sesame2::Model::closeIterators()
{
    Q_FOREACH( JObjectRef it, m_openIterators ) {
        Iterator( it ).close();
    }
    m_openIterators.clear();
}
