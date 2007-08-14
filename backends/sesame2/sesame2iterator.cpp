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

#include "sesame2iterator.h"
#include "sesame2types.h"
#include "jniwrapper.h"


class Soprano::Sesame2::Iterator::Private
{
public:
    Private( Iterator* parent )
        : m_parent( parent ),
          m_IDhasNext( 0 ),
          m_IDnext( 0 ) {
    }

    jmethodID IDhasNext() {
        if ( !m_IDhasNext ) {
            m_IDhasNext = m_parent->getMethodID( "hasNext", "()Z" );
            JNIWrapper::instance()->debugException();
        }
        return m_IDhasNext;
    }

    jmethodID IDnext() {
        if ( !m_IDnext ) {
            m_IDnext = m_parent->getMethodID( "next", "()L"JAVA_LANG_OBJECT";" );
            JNIWrapper::instance()->debugException();
        }
        return m_IDnext;
    }

private:
    Iterator* m_parent;

    jmethodID m_IDhasNext;
    jmethodID m_IDnext;
};


Soprano::Sesame2::Iterator::Iterator( jobject o )
    : JNIObjectWrapper( o ),
      d( new Private( this ) )
{
}


Soprano::Sesame2::Iterator::~Iterator()
{
    close();
    delete d;
}



bool Soprano::Sesame2::Iterator::hasNext()
{
    bool b = callBooleanMethod( d->IDhasNext() );
    if ( JNIWrapper::instance()->exceptionOccured() ) {
        JNIWrapper::instance()->debugException();
        b = false;
    }

    // there is one problem in the sesame2/soprano interface:
    // it seems the repository is blocked until the iterator is closed.
    // we close it here to at least catch some of the situations.
    // The only "real" solution I see ATM is to introduce close methods
    // in our iterators but that would be ugly.
    if ( !b ) {
        close();
    }

    return b;
}


jobject Soprano::Sesame2::Iterator::next()
{
    return callObjectMethod( d->IDnext() );
}


void Soprano::Sesame2::Iterator::close()
{
    // close the result (if this is a closable it)
    if ( isInstanceOf( JNIWrapper::instance()->env()->FindClass( INFO_ADUNA_ITERATION_CLOSABLEITERATION ) ) ) {
        if ( jmethodID closeID = getMethodID( "close", "()V" ) ) {
            callVoidMethod( closeID );
            JNIWrapper::instance()->debugException();
        }
    }
}
