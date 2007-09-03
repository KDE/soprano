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

#include "jniobjectwrapper.h"
#include "jniwrapper.h"

#include <stdarg.h>

#include <QtCore/QDebug>


JNIObjectWrapper::JNIObjectWrapper()
    : m_object( 0 )
{
}


JNIObjectWrapper::JNIObjectWrapper( const JObjectRef& object )
    : m_object( object )
{
}


JNIObjectWrapper::~JNIObjectWrapper()
{
}


jmethodID JNIObjectWrapper::getMethodID( const QString& name, const QString& signature ) const
{
    jmethodID id = JNIWrapper::instance()->env()->GetMethodID( JNIWrapper::instance()->env()->GetObjectClass( m_object ),
                                                               name.toUtf8().data(),
                                                               signature.toUtf8().data() );
    if ( !id ) {
        qDebug() << "Failed to get method id for method:" << name;
        JNIWrapper::instance()->debugException();
    }

    return id;
}


void JNIObjectWrapper::callVoidMethod( jmethodID methodId, ... )
{
    va_list args;
    va_start( args, methodId );
    JNIWrapper::instance()->env()->CallVoidMethodV( object(),
                                                    methodId,
                                                    args );
    va_end( args );
//    JNIWrapper::instance()->debugException();
}


bool JNIObjectWrapper::callBooleanMethod( jmethodID methodId, ... )
{
    bool returnVal;
    va_list args;
    va_start( args, methodId );
    returnVal = JNIWrapper::instance()->env()->CallBooleanMethodV( object(),
                                                                   methodId,
                                                                   args );
    va_end( args );
//    JNIWrapper::instance()->debugException();
    return returnVal;
}


long JNIObjectWrapper::callLongMethod( jmethodID methodId, ... )
{
    long returnVal;
    va_list args;
    va_start( args, methodId );
    returnVal = JNIWrapper::instance()->env()->CallLongMethodV( object(),
                                                                methodId,
                                                                args );
    va_end( args );
//    JNIWrapper::instance()->debugException();
    return returnVal;
}


JObjectRef JNIObjectWrapper::callObjectMethod( jmethodID methodId, ... )
{
    JObjectRef returnVal;
    va_list args;
    va_start( args, methodId );
    returnVal = JNIWrapper::instance()->env()->CallObjectMethodV( object(),
                                                                  methodId,
                                                                  args );
    va_end( args );
//    JNIWrapper::instance()->debugException();
    return returnVal;
}


jclass JNIObjectWrapper::objectClass()
{
    return JNIWrapper::instance()->env()->GetObjectClass( object() );
}


bool JNIObjectWrapper::isInstanceOf( jclass clazz ) const
{
    return JNIWrapper::instance()->env()->IsInstanceOf( object(), clazz );
}


void JNIObjectWrapper::setObject( const JObjectRef& o )
{
    m_object = o;
}
