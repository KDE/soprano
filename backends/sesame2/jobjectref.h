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

#ifndef _JOBJECT_REF_H_
#define _JOBJECT_REF_H_

#include <QtCore/QSharedDataPointer>
#include <QtCore/QSharedData>

#include <jni.h>

class JObjectRef
{
public:
    JObjectRef();
    JObjectRef( jobject object );
    JObjectRef( const JObjectRef& object );
    virtual ~JObjectRef();

    JObjectRef& operator=( const JObjectRef& );
    JObjectRef& operator=( jobject );

    jobject data() const;

    operator jobject() const;

    bool operator!() const;

    bool operator!=( const JObjectRef& ) const;
    bool operator==( const JObjectRef& ) const;
    
    bool isGlobal() const;

    JObjectRef toGlobalRef() const;

private:
    class Private;
    Private* d;
};


class JStringRef : public JObjectRef
{
public:
    JStringRef();
    JStringRef( jstring );
    JStringRef( const JObjectRef& );
    JStringRef( const QString& );
    JStringRef( const QByteArray& );

    jstring data() const;
    operator jstring() const;

    QString toQString() const;
    QByteArray toAscii() const;
};


class JClassRef : public JObjectRef
{
public:
    JClassRef();
    JClassRef( jclass clazz );
    JClassRef( const JObjectRef& );

    jclass data() const;
    operator jclass() const;
};

#endif
