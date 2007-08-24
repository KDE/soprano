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
#include "sesame2-config.h"

#include <jni.h>

#include <stdarg.h>

#include <QtCore/QAtomic>
#include <QtCore/QGlobalStatic>
#include <QtCore/QDebug>
#include <QtCore/QThread>


class JNIWrapperFactory
{
public:
    JNIWrapper instance;
};


Q_GLOBAL_STATIC( JNIWrapperFactory, jniWrapperFactory );


class JNIWrapper::Private
{
public:
    JavaVM* jvm;
    JNIEnv* jniEnv;

    QHash<Qt::HANDLE, JNIEnv*> jniEnvMap;
};


JNIWrapper::JNIWrapper()
    : d( new Private() )
{
    // prepare the VM options
    JavaVMInitArgs vmArgs;
    JavaVMOption vmOptions[2];
    vmOptions[0].optionString = ( char* )"-Djava.class.path="SESAME2_CLASSPATH;
    vmOptions[1].optionString = ( char* )"-verbose:jni,gc,class";
    vmArgs.version = JNI_VERSION_1_6;
    vmArgs.options = vmOptions;
    vmArgs.nOptions = 2;

    // create the VM
    Q_ASSERT( JNI_CreateJavaVM( &d->jvm, reinterpret_cast<void**>( &d->jniEnv ), &vmArgs ) >= 0 );

    Q_ASSERT( d->jvm );
    Q_ASSERT( d->jniEnv );

    d->jniEnvMap[QThread::currentThreadId()] = d->jniEnv;
}


JNIWrapper::~JNIWrapper()
{
    d->jvm->DestroyJavaVM();
    delete d;
}


JNIWrapper* JNIWrapper::instance()
{
    return &jniWrapperFactory()->instance;
}


JNIEnv* JNIWrapper::env() const
{
    QHash<Qt::HANDLE, JNIEnv*>::const_iterator it = d->jniEnvMap.find( QThread::currentThreadId() );
    if ( it == d->jniEnvMap.constEnd() ) {
        JNIEnv* env = 0;
        Q_ASSERT( d->jvm->AttachCurrentThreadAsDaemon( ( void** )&env, 0 ) == 0 );
        d->jniEnvMap[QThread::currentThreadId()] = env;
        return env;
    }
    else {
        return *it;
    }
}


jobject JNIWrapper::constructObject( const char* className,  const char* constructorSig, ... )
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

    jobject newObject = env()->NewObjectV( clazz, constructorId, args );

    va_end( args );

    if ( !newObject ) {
        qDebug() << "Failed to create object of type:" << className;
        debugException();
    }

    return newObject;
}


QString JNIWrapper::convertString( jstring s )
{
    const jchar* chars = env()->GetStringChars( s, 0 );
    QString qs = QString::fromUtf16( chars );
    env()->ReleaseStringChars( s, chars );
    return qs;
}


jstring JNIWrapper::convertString( const QString& s )
{
    jstring js = env()->NewStringUTF( s.toUtf8().data() );
    if ( !js ) {
        qDebug() << "Failed to create string object with value:" << s;
        debugException();
    }
    return js;
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
