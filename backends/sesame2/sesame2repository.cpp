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

#include "sesame2repository.h"
#include "jniwrapper.h"
#include "sesame2types.h"
#include "sesame2valuefactory.h"
#include "sesame2repositoryconnection.h"

#include <soprano/node.h>
#include <soprano/literalvalue.h>


class Soprano::Sesame2::RepositoryWrapper::Private
{
public:
    Private( RepositoryWrapper* parent )
        : repository( 0 ),
          valueFactory( 0 ),
          repositoryConnection( 0 ),
          m_parent( parent ) {
    }

    jobject repository;
    ValueFactory* valueFactory;
    RepositoryConnection* repositoryConnection;

private:
    RepositoryWrapper* m_parent;
};


Soprano::Sesame2::RepositoryWrapper::RepositoryWrapper( jobject o )
    : JNIObjectWrapper( o ),
      d( new Private( this ) )
{
}


Soprano::Sesame2::RepositoryWrapper::~RepositoryWrapper()
{
    close();
    delete d;
}


Soprano::Sesame2::RepositoryWrapper* Soprano::Sesame2::RepositoryWrapper::create( const QString& path_ )
{
    jstring path = JNIWrapper::instance()->convertString( path_ );

    // put the path into a java.io.File
    jobject file = JNIWrapper::instance()->constructObject( JAVA_IO_FILE, "(L"JAVA_LANG_STRING";)V", path );
    if ( !file ) {
        return 0;
    }

    // create an instance of org.openrdf.sail.nativerdf.NativeStore;
    jobject store = JNIWrapper::instance()->constructObject( ORG_OPENRDF_SAIL_NATIVERDF_NATIVESTORE, "(L"JAVA_IO_FILE";)V", file );
    if ( !store ) {
        return 0;
    }

    // create an instance of org.openrdf.repository.sail.SailRepository
    jobject repository = JNIWrapper::instance()->constructObject( ORG_OPENRDF_REPOSITORY_SAIL_SAILREPOSITORY, "(L"ORG_OPENRDF_SAIL_SAIL";)V", store );
    if ( !repository ) {
        return 0;
    }

    return new RepositoryWrapper( repository );
}


Soprano::Sesame2::RepositoryWrapper* Soprano::Sesame2::RepositoryWrapper::create()
{
    // create an instance of org.openrdf.sail.;
    jobject store = JNIWrapper::instance()->constructObject( ORG_OPENRDF_SAIL_MEMORY_MEMORYSTORE );
    if ( !store ) {
        return 0;
    }

    // create an instance of org.openrdf.repository.sail.SailRepository
    jobject repository = JNIWrapper::instance()->constructObject( ORG_OPENRDF_REPOSITORY_SAIL_SAILREPOSITORY, "(L"ORG_OPENRDF_SAIL_SAIL";)V", store );
    if ( !repository ) {
        return 0;
    }

    return new RepositoryWrapper( repository );
}


void Soprano::Sesame2::RepositoryWrapper::close()
{
    delete d->valueFactory;

    if ( d->repository ) {
        jmethodID shutDownMethodId = getMethodID( "shutdown", "(void)V" );
        if ( shutDownMethodId ) {
            callVoidMethod( shutDownMethodId );
        }
    }

    // FIXME: do we delete the jobjects?

    d->repository = 0;

    setObject( 0 );
}




bool Soprano::Sesame2::RepositoryWrapper::initialize()
{
    // try to initialize
    callVoidMethod( getMethodID( "initialize", "()V" ) );
    if ( JNIWrapper::instance()->exceptionOccured() ) {
        JNIWrapper::instance()->debugException();
        return false;
    }
    else {
        return true;
    }
}


Soprano::Sesame2::ValueFactory* Soprano::Sesame2::RepositoryWrapper::valueFactory()
{
    if ( !d->valueFactory ) {
        jobject valueFactory = callObjectMethod( getMethodID( "getValueFactory", "()L"ORG_OPENRDF_MODEL_VALUEFACTORY";" ) );
        if ( !valueFactory ) {
            JNIWrapper::instance()->debugException();
            return 0;
        }
        d->valueFactory = new ValueFactory( valueFactory );
    }

    return d->valueFactory;
}


Soprano::Sesame2::RepositoryConnection* Soprano::Sesame2::RepositoryWrapper::repositoryConnection()
{
    if ( !d->repositoryConnection ) {
        jobject repositoryConnection = callObjectMethod( getMethodID( "getConnection", "()L"ORG_OPENRDF_REPOSITORY_SAIL_SAILREPOSITORYCONNECTION";" ) );
        if ( !repositoryConnection ) {
            JNIWrapper::instance()->debugException();
            return 0;
        }
        d->repositoryConnection = new RepositoryConnection( repositoryConnection );
    }

    return d->repositoryConnection;
}
