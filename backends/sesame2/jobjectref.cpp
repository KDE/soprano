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
#include <QtCore/QSharedData>


class JObjectRef::Private : public QSharedData
{
public:
    Private( jobject o = 0, bool g = false )
        : object( o ),
          global( g ) {
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

    jobject object;
    bool global;
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
}


JObjectRef::~JObjectRef()
{
}


JObjectRef& JObjectRef::operator=( const JObjectRef& other )
{
    d = other.d;
    return *this;
}


JObjectRef& JObjectRef::operator=( jobject o )
{
    d->object = o;
    d->global = false;
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


JStringRef::JStringRef( const QByteArray& data )
    : JObjectRef()
{
    jstring js = JNIWrapper::instance()->env()->NewStringUTF( data.data() );
    if ( js ) {
        JObjectRef::operator=( js );
    }
    else {
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


QByteArray JStringRef::toAscii() const
{
    QByteArray a;
    if ( data() ) {
        const jchar* chars = JNIWrapper::instance()->env()->GetStringChars( JStringRef::data(), 0 );
        int len = JNIWrapper::instance()->env()->GetStringLength( JStringRef::data() );

        a.resize( len );
        // java strings are always big endian
        for ( int i = 0; i < len; ++i ) {
            Q_ASSERT( chars[i]>>8 == 0 );
            a[i] = ( char )chars[i];
        }
        JNIWrapper::instance()->env()->ReleaseStringChars( JStringRef::data(), chars );
    }
    return a;
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
