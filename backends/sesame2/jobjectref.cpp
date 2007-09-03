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

#include "jobjectref.h"

#include "jniwrapper.h"

#include <QtCore/QDebug>
#include <QtCore/QString>


class JObjectRef::Private
{
public:
    Private( jobject o = 0, bool g = false )
        : object( o ),
          global( g ),
          m_ref( 1 ) {
    }

    ~Private() {
        if ( object ) {
            if ( global ) {
                JNIWrapper::instance()->env()->DeleteGlobalRef( object );
            }
            else {
                JNIWrapper::instance()->env()->DeleteLocalRef( object );
            }
        }
    }

    int ref() {
        return ++m_ref;
    }

    int unref() {
        return --m_ref;
    }

    jobject object;
    bool global;

private:
    int m_ref;
};

JObjectRef::JObjectRef()
    : d( new Private() )
{
}


JObjectRef::JObjectRef( jobject object )
    : d( new Private( object ) )
{
}


JObjectRef::JObjectRef( const JObjectRef& other )
{
    d = other.d;
    d->ref();
}


JObjectRef::~JObjectRef()
{
    if ( !d->unref() ) {
        delete d;
    }
}


JObjectRef& JObjectRef::operator=( const JObjectRef& other )
{
    if ( d != other.d ) {
        if ( !d->unref() ) {
            delete d;
        }
        d = other.d;
        d->ref();
    }

    return *this;
}


JObjectRef& JObjectRef::operator=( jobject o )
{
    if ( !d->unref() ) {
        delete d;
    }
    d = new Private( o );
    d->ref();
    return *this;
}


jobject JObjectRef::data() const
{
    return d->object;
}


JObjectRef::operator jobject() const
{
    return d->object;
}


bool JObjectRef::operator!() const
{
    return !d->object;
}


bool JObjectRef::operator!=( const JObjectRef& other ) const
{
    return d != other.d && d->object != other.d->object;
}


bool JObjectRef::operator==( const JObjectRef& other ) const
{
    return d == other.d || d->object == other.d->object;
}


bool JObjectRef::isGlobal() const
{
    return d->global;
}


JObjectRef JObjectRef::toGlobalRef() const
{
    JObjectRef ref;
    if ( d->object ) {
        ref.d->object = JNIWrapper::instance()->env()->NewGlobalRef( d->object );
        ref.d->global = true;
    }
    return ref;
}


JStringRef::JStringRef()
    : JObjectRef()
{
}


JStringRef::JStringRef( jstring s )
    : JObjectRef( s )
{
}


JStringRef::JStringRef( const JObjectRef& other )
    : JObjectRef( other )
{
}


JStringRef::JStringRef( const QString& s )
    : JObjectRef()
{
    jstring js = JNIWrapper::instance()->env()->NewStringUTF( s.toUtf8().data() );
    if ( js ) {
        JObjectRef::operator=( js );
    }
    else {
        qDebug() << "Failed to create string object with value:" << s;
        JNIWrapper::instance()->debugException();
    }
}


jstring JStringRef::data() const
{
    return reinterpret_cast<jstring>( JObjectRef::data() );
}


JStringRef::operator jstring() const
{
    return JStringRef::data();
}


QString JStringRef::toQString() const
{
    if ( data() ) {
        const jchar* chars = JNIWrapper::instance()->env()->GetStringChars( JStringRef::data(), 0 );
        QString qs = QString::fromUtf16( chars );
        JNIWrapper::instance()->env()->ReleaseStringChars( JStringRef::data(), chars );
        return qs;
    }
    else {
        return QString();
    }
}


JClassRef::JClassRef()
    : JObjectRef()
{
}


JClassRef::JClassRef( jclass clazz )
    : JObjectRef( clazz )
{
}


JClassRef::JClassRef( const JObjectRef& ref )
    : JObjectRef( ref )
{
}

jclass JClassRef::data() const
{
    return reinterpret_cast<jclass>( JObjectRef::data() );
}


JClassRef::operator jclass() const
{
    return data();
}
