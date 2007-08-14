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

#ifndef _JNI_OBJECT_WRAPPER_H_
#define _JNI_OBJECT_WRAPPER_H_

#include <QtCore/QString>

#include <jni.h>


class JNIObjectWrapper
{
public:
    JNIObjectWrapper();
    JNIObjectWrapper( jobject object );
    ~JNIObjectWrapper();

    void setObject( jobject o ) { m_object = o; }

    jobject object() const { return m_object; }

    jclass objectClass();

    bool isInstanceOf( jclass ) const;

    jmethodID getMethodID( const QString& name, const QString& signature ) const;

    void callVoidMethod( jmethodID methodId, ... );
    bool callBooleanMethod( jmethodID methodId, ... );
    long callLongMethod( jmethodID methodId, ... );
    jobject callObjectMethod( jmethodID methodId, ... );

    
private:
    jobject m_object;
};

#endif
