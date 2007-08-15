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

#include "sesame2repositoryconnection.h"
#include "sesame2types.h"
#include "jniwrapper.h"

#include <QtCore/QDebug>


class Soprano::Sesame2::RepositoryConnection::Private
{
public:
    Private( RepositoryConnection* parent )
        : m_parent( parent ),
          m_IDgetContextIDs( 0 ),
          m_IDaddStatement( 0 ),
          m_IDgetStatements( 0 ),
          m_IDhasStatement( 0 ),
          m_IDremove( 0 ),
          m_IDsize( 0 ),
          m_IDprepareQuery( 0 ),
          m_classResource( 0 ),
          m_classTupleQuery( 0 ),
          m_classGraphQuery( 0 ) {
    }

    jmethodID IDgetContextIDs() {
        if ( !m_IDgetContextIDs ) {
            m_IDgetContextIDs = m_parent->getMethodID( "getContextIDs", "()L"ORG_OPENRDF_REPOSITORY_REPOSITORYRESULT";" );
            JNIWrapper::instance()->debugException();
        }
        return m_IDgetContextIDs;
    }

    jmethodID IDaddStatement() {
        if ( !m_IDaddStatement ) {
            m_IDaddStatement = m_parent->getMethodID( "add", "(L"ORG_OPENRDF_MODEL_STATEMENT";[L"ORG_OPENRDF_MODEL_RESOURCE";)V" );
            JNIWrapper::instance()->debugException();
        }
        return m_IDaddStatement;
    }

    jmethodID IDgetStatements() {
        if ( !m_IDgetStatements ) {
            m_IDgetStatements = m_parent->getMethodID( "getStatements", "(L"ORG_OPENRDF_MODEL_RESOURCE";L"ORG_OPENRDF_MODEL_URI";L"ORG_OPENRDF_MODEL_VALUE";Z[L"ORG_OPENRDF_MODEL_RESOURCE";)L"ORG_OPENRDF_REPOSITORY_REPOSITORYRESULT";" );
            JNIWrapper::instance()->debugException();
        }
        return m_IDgetStatements;
    }

    jmethodID IDhasStatement() {
        if ( !m_IDhasStatement ) {
            m_IDhasStatement = m_parent->getMethodID( "hasStatement", "(L"ORG_OPENRDF_MODEL_RESOURCE";L"ORG_OPENRDF_MODEL_URI";L"ORG_OPENRDF_MODEL_VALUE";Z[L"ORG_OPENRDF_MODEL_RESOURCE";)Z" );
            JNIWrapper::instance()->debugException();
        }
        return m_IDhasStatement;
    }

    jmethodID IDremove() {
        if ( !m_IDremove ) {
            m_IDremove = m_parent->getMethodID( "remove", "(L"ORG_OPENRDF_MODEL_RESOURCE";L"ORG_OPENRDF_MODEL_URI";L"ORG_OPENRDF_MODEL_VALUE";[L"ORG_OPENRDF_MODEL_RESOURCE";)V" );
            JNIWrapper::instance()->debugException();
        }
        return m_IDremove;
    }

    jmethodID IDsize() {
        if ( !m_IDsize ) {
            m_IDsize = m_parent->getMethodID( "size", "([L"ORG_OPENRDF_MODEL_RESOURCE";)J" );
            JNIWrapper::instance()->debugException();
        }
        return m_IDsize;
    }

    jmethodID IDprepareQuery() {
        if ( !m_IDprepareQuery ) {
            m_IDprepareQuery = m_parent->getMethodID( "prepareQuery", "(L"ORG_OPENRDF_QUERY_QUERYLANGUAGE";L"JAVA_LANG_STRING";)L"ORG_OPENRDF_QUERY_QUERY";" );
            JNIWrapper::instance()->debugException();
        }
        return m_IDprepareQuery;
    }

    // caching class instance
    jclass classResource() {
        if ( !m_classResource ) {
            m_classResource = JNIWrapper::instance()->env()->FindClass( ORG_OPENRDF_MODEL_RESOURCE );
            JNIWrapper::instance()->debugException();
        }
        return m_classResource;
    }

    jclass classTupleQuery() {
        if ( !m_classTupleQuery ) {
            m_classTupleQuery = JNIWrapper::instance()->env()->FindClass( ORG_OPENRDF_QUERY_TUPLEQUERY );
            JNIWrapper::instance()->debugException();
        }
        return m_classTupleQuery;
    }

    jclass classGraphQuery() {
        if ( !m_classGraphQuery ) {
            m_classGraphQuery = JNIWrapper::instance()->env()->FindClass( ORG_OPENRDF_QUERY_GRAPHQUERY );
            JNIWrapper::instance()->debugException();
        }
        return m_classGraphQuery;
    }

private:
    RepositoryConnection* m_parent;

    jmethodID m_IDgetContextIDs;
    jmethodID m_IDaddStatement;
    jmethodID m_IDgetStatements;
    jmethodID m_IDhasStatement;
    jmethodID m_IDremove;
    jmethodID m_IDsize;
    jmethodID m_IDprepareQuery;

    jclass m_classResource;
    jclass m_classTupleQuery;
    jclass m_classGraphQuery;
};


Soprano::Sesame2::RepositoryConnection::RepositoryConnection( jobject o )
    : JNIObjectWrapper( o ),
      d( new Private( this ) )
{
}


Soprano::Sesame2::RepositoryConnection::~RepositoryConnection()
{
    delete d;
}


jobject Soprano::Sesame2::RepositoryConnection::getContextIDs()
{
    return callObjectMethod( d->IDgetContextIDs() );
}


void Soprano::Sesame2::RepositoryConnection::addStatement( jobject statement )
{
    // sesame wants an empty array for the contexts
    static jobjectArray contexts = JNIWrapper::instance()->env()->NewObjectArray( 0, d->classResource(), 0 );
    callVoidMethod( d->IDaddStatement(), statement, contexts );
}


jobject Soprano::Sesame2::RepositoryConnection::getStatements( jobject subject, jobject predicate, jobject object, jobject context )
{
    jobjectArray contexts = JNIWrapper::instance()->env()->NewObjectArray( context ? 1 : 0,
                                                                           d->classResource(),
                                                                           context );
    return callObjectMethod( d->IDgetStatements(), subject, predicate, object, true, contexts );
}


bool Soprano::Sesame2::RepositoryConnection::hasStatement( jobject subject, jobject predicate, jobject object, jobject context )
{
    jobjectArray contexts = JNIWrapper::instance()->env()->NewObjectArray( context ? 1 : 0,
                                                                           d->classResource(),
                                                                           context );
    return callBooleanMethod( d->IDhasStatement(), subject, predicate, object, true, contexts );
}


void Soprano::Sesame2::RepositoryConnection::remove( jobject subject, jobject predicate, jobject object, jobject context )
{
    jobjectArray contexts = JNIWrapper::instance()->env()->NewObjectArray( context ? 1 : 0,
                                                                           d->classResource(),
                                                                           context );
    callVoidMethod( d->IDremove(), subject, predicate, object, contexts );
}


long Soprano::Sesame2::RepositoryConnection::size()
{
    // sesame wants an empty array for the contexts
    static jobjectArray contexts = JNIWrapper::instance()->env()->NewObjectArray( 0, d->classResource(), 0 );
    return callLongMethod( d->IDsize() );
}


jobject Soprano::Sesame2::RepositoryConnection::query( jobject queryLang, jstring queryString )
{
    // prepare the query
    jobject query = callObjectMethod( d->IDprepareQuery(), queryLang, queryString );
    if ( !query ) {
        qDebug() << "Failed to prepare query.";
        JNIWrapper::instance()->debugException();
        return 0;
    }

    // evaluate the query
    if ( JNIWrapper::instance()->env()->IsInstanceOf( query, d->classTupleQuery() ) ) {
        JNIObjectWrapper queryWrapper( query );
        jobject result = queryWrapper.callObjectMethod( queryWrapper.getMethodID( "evaluate", "()L"ORG_OPENRDF_QUERY_TUPLEQUERYRESULT";" ) );
        return result;
    }
    else if ( JNIWrapper::instance()->env()->IsInstanceOf( query, d->classGraphQuery() ) ) {
        JNIObjectWrapper queryWrapper( query );
        jobject result = queryWrapper.callObjectMethod( queryWrapper.getMethodID( "evaluate", "()L"ORG_OPENRDF_QUERY_GRAPHQUERYRESULT";" ) );
        return result;
    }
    else {
        qDebug() << "Uknown query type.";
        return 0;
    }
}