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
#include "sesame2sopranowrapper.h"

#include "node.h"
#include "literalvalue.h"


class Soprano::Sesame2::RepositoryWrapper::Private
{
public:
    Private()
        : valueFactory( 0 ),
          repositoryConnection( 0 ),
          sopranoWrapper( 0 ) {
    }

    ValueFactory* valueFactory;
    RepositoryConnection* repositoryConnection;
    SopranoWrapper* sopranoWrapper;
};


Soprano::Sesame2::RepositoryWrapper::RepositoryWrapper( const JObjectRef& o )
    : JNIObjectWrapper( o ),
      d( new Private() )
{
}


Soprano::Sesame2::RepositoryWrapper::~RepositoryWrapper()
{
    close();
    delete d;
}


Soprano::Sesame2::RepositoryWrapper* Soprano::Sesame2::RepositoryWrapper::create( const QString& path_ )
{
    JStringRef path( path_ );

    // put the path into a java.io.File
    JObjectRef file = JNIWrapper::instance()->constructObject( JAVA_IO_FILE,
                                                               "(L"JAVA_LANG_STRING";)V",
                                                               path.data() );
    if ( !file ) {
        return 0;
    }

    // triple indexes: default indexes + index to query on graphs which we do often in Nepomuk
    // FIXME: it would be great to make this configurable through Soprano
    JStringRef tripleIndexes( QString::fromLatin1( "spoc,posc,opsc,cspo" ) );

    // create an instance of org.openrdf.sail.nativerdf.NativeStore;
    JObjectRef store = JNIWrapper::instance()->constructObject( ORG_OPENRDF_SAIL_NATIVERDF_NATIVESTORE,
                                                                "(L"JAVA_IO_FILE";L"JAVA_LANG_STRING";)V",
                                                                file.data(),
                                                                tripleIndexes.data() );
    if ( !store ) {
        return 0;
    }

    // create an instance of org.openrdf.repository.sail.SailRepository
    JObjectRef repository = JNIWrapper::instance()->constructObject( ORG_OPENRDF_REPOSITORY_SAIL_SAILREPOSITORY,
                                                                     "(L"ORG_OPENRDF_SAIL_SAIL";)V",
                                                                     store.data() );
    if ( !repository ) {
        return 0;
    }

    return new RepositoryWrapper( repository.toGlobalRef() );
}


Soprano::Sesame2::RepositoryWrapper* Soprano::Sesame2::RepositoryWrapper::create()
{
    // create an instance of org.openrdf.sail.;
    JObjectRef store = JNIWrapper::instance()->constructObject( ORG_OPENRDF_SAIL_MEMORY_MEMORYSTORE );
    if ( !store ) {
        return 0;
    }

    // create an instance of org.openrdf.repository.sail.SailRepository
    JObjectRef repository = JNIWrapper::instance()->constructObject( ORG_OPENRDF_REPOSITORY_SAIL_SAILREPOSITORY, "(L"ORG_OPENRDF_SAIL_SAIL";)V", store.data() );
    if ( !repository ) {
        return 0;
    }

    return new RepositoryWrapper( repository.toGlobalRef() );
}


void Soprano::Sesame2::RepositoryWrapper::close()
{
    delete d->valueFactory;
    delete d->repositoryConnection;
    d->valueFactory = 0;
    d->repositoryConnection = 0;

    if ( object() ) {
        jmethodID shutDownMethodId = getMethodID( "shutDown", "()V" );
        if ( shutDownMethodId ) {
            callVoidMethod( shutDownMethodId );
            JNIWrapper::instance()->debugException();
        }
    }

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
        JObjectRef valueFactory = callObjectMethod( getMethodID( "getValueFactory", "()L"ORG_OPENRDF_MODEL_VALUEFACTORY";" ) );
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
        JObjectRef repositoryConnection = callObjectMethod( getMethodID( "getConnection", "()L"ORG_OPENRDF_REPOSITORY_SAIL_SAILREPOSITORYCONNECTION";" ) );
        if ( !repositoryConnection ) {
            JNIWrapper::instance()->debugException();
            return 0;
        }
        d->repositoryConnection = new RepositoryConnection( repositoryConnection );
    }

    return d->repositoryConnection;
}


Soprano::Sesame2::SopranoWrapper* Soprano::Sesame2::RepositoryWrapper::sopranoWrapper()
{
    if ( !d->sopranoWrapper ) {
        JObjectRef sopranoWrapper = JNIWrapper::instance()->constructObject( "SopranoSesame2Wrapper",
                                                                             "(Lorg/openrdf/repository/RepositoryConnection;)V",
                                                                             repositoryConnection()->object().data() );
        if ( !sopranoWrapper ) {
            JNIWrapper::instance()->debugException();
            return 0;
        }
        d->sopranoWrapper = new SopranoWrapper( sopranoWrapper );
    }

    return d->sopranoWrapper;
}
