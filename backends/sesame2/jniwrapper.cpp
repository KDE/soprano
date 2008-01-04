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

#include "jniwrapper.h"
#include "jniobjectwrapper.h"
#include "sesame2-config.h"
#include "sesame2types.h"

#include <jni.h>

#include <stdarg.h>

#include <QtCore/QAtomic>
#include <QtCore/QGlobalStatic>
#include <QtCore/QDebug>
#include <QtCore/QThread>



JNIWrapper* JNIWrapper::s_instance = 0;

class JNIWrapper::Private
{
public:
    JavaVM* jvm;
    JNIEnv* jniEnv;

    QHash<QThread*, JNIEnv*> jniEnvMap;
};


JNIWrapper::JNIWrapper()
    : d( new Private() )
{
    s_instance = this;
}


JNIWrapper::~JNIWrapper()
{
    d->jvm->DestroyJavaVM();
    delete d;
    s_instance = 0;
}


JNIWrapper* JNIWrapper::instance()
{
    if ( !s_instance ) {
        // prepare the VM options
        JavaVMInitArgs vmArgs;
        JavaVMOption vmOptions[2];
        vmOptions[0].optionString = ( char* )"-Djava.class.path="SESAME2_CLASSPATH;
        vmOptions[1].optionString = ( char* )"-verbose:jni,gc,class";
        vmArgs.version = JNI_VERSION_1_4;
        vmArgs.options = vmOptions;
        vmArgs.nOptions = 2;

        // create the VM
        JavaVM* jvm = 0;
        JNIEnv* jniEnv = 0;
        if ( JNI_CreateJavaVM( &jvm, reinterpret_cast<void**>( &jniEnv ), &vmArgs ) >= 0 ) {
            Q_ASSERT( jvm );
            Q_ASSERT( jniEnv );
            s_instance = new JNIWrapper();
            s_instance->d->jvm = jvm;
            s_instance->d->jniEnv = jniEnv;
            s_instance->d->jniEnvMap[QThread::currentThread()] = jniEnv;
        }
        else {
            qDebug() << "Failed to create Java VM.";
        }
    }
    return s_instance;
}


JNIEnv* JNIWrapper::env() const
{
    QHash<QThread*, JNIEnv*>::const_iterator it = d->jniEnvMap.find( QThread::currentThread() );
    if ( it == d->jniEnvMap.constEnd() ) {
        JNIEnv* env = 0;
        Q_ASSERT( d->jvm->AttachCurrentThread( ( void** )&env, 0 ) == 0 );
        d->jniEnvMap[QThread::currentThread()] = env;

        connect( QThread::currentThread(), SIGNAL( finished() ),
                 this, SLOT( slotThreadFinished() ),
                 Qt::DirectConnection );

        return env;
    }
    else {
        return *it;
    }
}


void JNIWrapper::slotThreadFinished()
{
    QObject* o = sender();
    if ( o == QThread::currentThread() ) {
        qDebug() << "Detaching thread" << QThread::currentThread();
//        d->jvm->DetachCurrentThread();
        d->jniEnvMap.remove( QThread::currentThread() );
    }
}


JObjectRef JNIWrapper::constructObject( const char* className,  const char* constructorSig, ... )
{
    static const char* constructorName = "<init>";
    static const char* defaultConstructorSig = "()V";

    jclass clazz = env()->FindClass( className );
    if ( !clazz ) {
        debugException();
        return 0;
    }
    jmethodID constructorId = env()->GetMethodID( clazz,
                                                  constructorName,
                                                  constructorSig ? constructorSig : defaultConstructorSig );
    if ( !constructorId ) {
        qDebug() << "Failed to get constructor id for class:" << className << "and signature" << constructorSig;
        debugException();
        return 0;
    }

    va_list args;
    va_start( args, constructorSig );

    JObjectRef newObject = env()->NewObjectV( clazz, constructorId, args );

    va_end( args );

    if ( !newObject ) {
        qDebug() << "Failed to create object of type:" << className;
        debugException();
    }

    return newObject;
}


void JNIWrapper::debugException()
{
    if ( env()->ExceptionCheck() == JNI_TRUE ) {
        env()->ExceptionDescribe();
        env()->ExceptionClear();
    }
}


bool JNIWrapper::exceptionOccured()
{
    return env()->ExceptionCheck() == JNI_TRUE;
}


Soprano::Error::Error JNIWrapper::convertAndClearException()
{
    jthrowable exception = env()->ExceptionOccurred();
    if ( exception ) {
        env()->ExceptionDescribe();
        JNIObjectWrapper exWr( exception );
        jmethodID id = exWr.getMethodID( "getMessage", "()L"JAVA_LANG_STRING";" );
        QString message = JStringRef( exWr.callObjectMethod( id ) ).toQString();
        env()->ExceptionClear();
        return Soprano::Error::Error( "Sesame2 backend error: '" + message + "'", Soprano::Error::ErrorUnknown );
    }
    else {
        return Soprano::Error::Error();
    }
}

#include "jniwrapper.moc"
