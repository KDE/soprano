/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2007-2008 Sebastian Trueg <trueg@kde.org>
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

#include "sesame2backend.h"
#include "sesame2model.h"
#include "jniwrapper.h"
#include "sesame2repository.h"
#include "jobjectref.h"

#include <QtCore/QtPlugin>
#include <QtCore/QDebug>
#include <QtCore/QDir>


Q_EXPORT_PLUGIN2(soprano_sesame2backend, Soprano::Sesame2::BackendPlugin)

Soprano::Sesame2::BackendPlugin::BackendPlugin()
  : QObject(),
    Backend( "sesame2" ),
    m_jniWrapper( 0 )
{
}


Soprano::Sesame2::BackendPlugin::~BackendPlugin()
{
    delete m_jniWrapper;
}


bool Soprano::Sesame2::BackendPlugin::isAvailable() const
{
    if ( m_jniWrapper ) {
        return true;
    }

    // FIXME: there is some problem with JNI. If I delete it
    // nothing works.
    if ( JNIWrapper::instance() ) {
        //   delete JNIWrapper::instance();
        return true;
    }

    return false;
}


Soprano::StorageModel* Soprano::Sesame2::BackendPlugin::createModel( const BackendSettings& settings ) const
{
    m_mutex.lock();
    if ( !m_jniWrapper ) {
        m_jniWrapper = JNIWrapper::instance();

        // stupid java threading! Looks as if the query parsers need to be initialized in the main thread! Well, even this does not
        // guarantee that but at least it covers most cases.... :(
        JClassRef clazz = m_jniWrapper->env()->FindClass( "org/openrdf/query/parser/QueryParserRegistry" );
        jmethodID id = m_jniWrapper->env()->GetStaticMethodID( clazz, "getInstance", "()Lorg/openrdf/query/parser/QueryParserRegistry;" );
        m_jniWrapper->env()->CallStaticObjectMethod( clazz, id );
    }
    m_mutex.unlock();

    clearError();

    QString path;
    bool memory = false;

    // FIXME: support inference option

    Q_FOREACH( const BackendSetting& s, settings ) {
        if ( s.option() == BackendOptionUser ) {
            // no user options ATM
            qDebug() << "(Soprano::Sesame2::BackendPlugin) no user options supported.";
            setError( "No user options supported.", Error::ErrorInvalidArgument );
            return 0;
        }
        else if ( s.option() == BackendOptionStorageMemory ) {
            memory = s.value().toBool();
        }
        else if ( s.option() == BackendOptionStorageDir ) {
            path = s.value().toString();
        }
        else {
            qDebug() << "(Soprano::Sesame2::BackendPlugin) unsupported option: " << s.option();
            setError( QString( "Unsupported option: %1" ).arg( s.option() ), Error::ErrorInvalidArgument );
            return 0;
        }
    }

    if ( !path.isEmpty() && memory ) {
        qDebug() << "(Soprano::Sesame2::BackendPlugin) Path specified for memory model. Settings mismatch.";
        setError( "Cannot set storage path for a memory repository.", Error::ErrorInvalidArgument );
        return 0;
    }

    RepositoryWrapper* repo = 0;
    if ( memory || path.isEmpty() ) {
        repo = RepositoryWrapper::create();
    }
    else {
        repo = RepositoryWrapper::create( path );
    }

    if ( repo ) {
        if ( repo->initialize() ) {
            return new Model( this, repo );
        }
        else {
            setError( JNIWrapper::instance()->convertAndClearException() );
            delete repo;
        }
    }
    else {
        setError( JNIWrapper::instance()->convertAndClearException() );
    }

    return 0;
}


bool Soprano::Sesame2::BackendPlugin::deleteModelData( const BackendSettings& settings ) const
{
    QString path;
    Q_FOREACH( const BackendSetting& s, settings ) {
        if ( s.option() == BackendOptionStorageDir ) {
            path = s.value().toString();
            break;
        }
    }

    if ( path.isEmpty() ) {
        setError( "No storage path set.", Error::ErrorInvalidArgument );
        return false;
    }

    // FIXME: is there a way to get the actual list of files
    QDir dir( path );
    QStringList files = dir.entryList( QStringList() << "triples*" << "namespaces*" << "values*", QDir::Files );
    foreach( const QString& file, files ) {
        if ( !dir.remove( file ) ) {
            setError( "Failed to remove file '" + dir.filePath( file ), Error::ErrorUnknown );
            return false;
        }
    }

    clearError();
    return true;
}


Soprano::BackendFeatures Soprano::Sesame2::BackendPlugin::supportedFeatures() const
{
    return(  BackendFeatureStorageMemory|
             BackendFeatureAddStatement|
             BackendFeatureRemoveStatements|
             BackendFeatureListStatements|
             BackendFeatureQuery|
             BackendFeatureContext );
}

