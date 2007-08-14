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
#include "sesame2queryresultiteratorbackend.h"
#include "jniwrapper.h"

#include <soprano/statementiterator.h>
#include <soprano/queryresultiterator.h>
#include <soprano/statement.h>
#include <soprano/query.h>

#include <QtCore/QDebug>


Soprano::Sesame2::Model::Model( RepositoryWrapper* repo )
    : Soprano::StorageModel(),
      m_repository( repo )
{
}


Soprano::Sesame2::Model::~Model()
{
    delete m_repository;
}


Soprano::ErrorCode Soprano::Sesame2::Model::addStatement( const Statement &statement )
{
    if ( jobject sesameStatement = m_repository->valueFactory()->convertStatement( statement ) ) {
        m_repository->repositoryConnection()->addStatement( sesameStatement );
        if ( JNIWrapper::instance()->exceptionOccured() ) {
            qDebug() << "(Soprano::Sesame2::Model::addStatements) failed.";
            JNIWrapper::instance()->debugException();
            return ERROR_UNKNOW;
        }
        else {
            return ERROR_NONE;
        }
    }
    else {
        return ERROR_UNKNOW;
    }
}


QList<Soprano::Node> Soprano::Sesame2::Model::listContexts() const
{
    QList<Soprano::Node> contexts;

    jobject ids = m_repository->repositoryConnection()->getContextIDs();
    if ( ids ) {
        Iterator it( ids );
        while ( it.hasNext() ) {
            jobject x = it.next();
            Q_ASSERT( x );
            contexts.append( convertNode( x ) );
        }
    }
    else {
        JNIWrapper::instance()->debugException();
    }

    return contexts;
}


Soprano::QueryResultIterator Soprano::Sesame2::Model::executeQuery( const Query &query ) const
{
    if ( query.type() != Soprano::Query::SPARQL ) {
        return QueryResultIterator();
    }

    jfieldID sparqlID = JNIWrapper::instance()->env()->GetStaticFieldID( JNIWrapper::instance()->env()->FindClass( ORG_OPENRDF_QUERY_QUERYLANGUAGE ),
                                                                         "SPARQL",
                                                                         "L"ORG_OPENRDF_QUERY_QUERYLANGUAGE";" );
    jobject sparqlQueryLang = JNIWrapper::instance()->env()->GetStaticObjectField( JNIWrapper::instance()->env()->FindClass( ORG_OPENRDF_QUERY_QUERYLANGUAGE ),
                                                                                   sparqlID );

    jobject queryResult = m_repository->repositoryConnection()->query( sparqlQueryLang,
                                                                       JNIWrapper::instance()->convertString( query.query() ) );

    if ( queryResult ) {
        return new QueryResultIteratorBackend( queryResult );
    }
    else {
        JNIWrapper::instance()->debugException();
        return QueryResultIterator();
    }
}


Soprano::StatementIterator Soprano::Sesame2::Model::listStatements( const Statement& statement ) const
{
    jobject results = m_repository->repositoryConnection()->getStatements( m_repository->valueFactory()->convertNode( statement.subject() ),
                                                                           m_repository->valueFactory()->convertNode( statement.predicate() ),
                                                                           m_repository->valueFactory()->convertNode( statement.object() ),
                                                                           m_repository->valueFactory()->convertNode( statement.context() ) );

    if ( JNIWrapper::instance()->exceptionOccured() ) {
        qDebug() << "(Soprano::Sesame2::Model::listStatements) failed.";
        JNIWrapper::instance()->debugException();
        return StatementIterator();
    }
    else {
        return new StatementIteratorBackend( results );
    }
}


Soprano::ErrorCode Soprano::Sesame2::Model::removeStatements( const Statement &statement )
{
    // we are not using convertStatement here since we support wildcards
    m_repository->repositoryConnection()->remove( m_repository->valueFactory()->convertNode( statement.subject() ),
                                                  m_repository->valueFactory()->convertNode( statement.predicate() ),
                                                  m_repository->valueFactory()->convertNode( statement.object() ),
                                                  m_repository->valueFactory()->convertNode( statement.context() ) );
    if ( JNIWrapper::instance()->exceptionOccured() ) {
        qDebug() << "(Soprano::Sesame2::Model::removeStatements) failed.";
        JNIWrapper::instance()->debugException();
        return ERROR_UNKNOW;
    }

    return ERROR_NONE;
}


int Soprano::Sesame2::Model::statementCount() const
{
    return m_repository->repositoryConnection()->size();
}


bool Soprano::Sesame2::Model::containsStatements( const Statement &statement ) const
{
    // we are not using convertStatement here since we support wildcards
    bool r = m_repository->repositoryConnection()->hasStatement( m_repository->valueFactory()->convertNode( statement.subject() ),
                                                                 m_repository->valueFactory()->convertNode( statement.predicate() ),
                                                                 m_repository->valueFactory()->convertNode( statement.object() ),
                                                                 m_repository->valueFactory()->convertNode( statement.context() ) );
    if ( JNIWrapper::instance()->exceptionOccured() ) {
        qDebug() << "(Soprano::Sesame2::Model::containsStatements) failed.";
        JNIWrapper::instance()->debugException();
        return false;
    }
    return r;
}
