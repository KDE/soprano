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

#include "wstring.h"

#include <wchar.h>

#include <QDebug>



// looks as if QT has a bug in toWCharArray, so we use the method from strigi
void utf8toucs2( const char* p, const char* e, wchar_t* buf )
{
    int i = 0;
    char nb = 0;
    while (p < e) {
        wchar_t w = 0;
        char c = *p;
        if (nb--) {
            w = (w<<6) + (c & 0x3f);
        } else if ((0xE0 & c) == 0xC0) {
            w = c & 0x1F;
            nb = 0;
        } else if ((0xF0 & c) == 0xE0) {
            w = c & 0x0F;
            nb = 1;
        } else if ((0xF8 & c) == 0xF0) {
            w = c & 0x07;
            nb = 2;
        } else {
            w = (w<<6) + (c&0x7F);
            buf[i++] = w;
            w = 0;
            nb = 0;
        }
        p++;
    }
    buf[i] = L'\0';
}


class WString::Private : public QSharedData
{
public:
    Private()
        : data( 0 ),
          wrap( false ) {
    }

    ~Private() {
        if ( !wrap ) {
            delete [] data;
        }
    }

    wchar_t* data;
    bool wrap;
};


WString::WString()
    : d( new Private() )
{
}


WString::WString( const WString& s )
{
    d = s.d;
}


WString::WString( const wchar_t* s, bool wrap )
    : d( new Private() )
{
    d->wrap = wrap;
    if ( wrap ) {
        d->data = const_cast<wchar_t*>( s );
    }
    else {
        d->data = new wchar_t[ wcslen(s) + 1];
        wcscpy( d->data, s );
    }
}


WString::WString( const QString& s )
    : d( new Private() )
{
    d->data = new wchar_t[s.length()+1];

    // looks as if QT has a bug in toWCharArray, so we use the method from strigi
    QByteArray utf8 = s.toUtf8();
    utf8toucs2( utf8.data(), utf8.data()+utf8.length(), d->data );
//    s.toWCharArray( d->data );
}


WString::~WString()
{
}


WString& WString::operator=( const WString& s )
{
    d = s.d;
    return *this;
}


WString& WString::operator=( const wchar_t* s )
{
    d->data = new wchar_t[ wcslen(s) + 1 ];
    wcscpy( d->data, s );
    return *this;
}


WString& WString::operator=( const QString& s )
{
    d->data = new wchar_t[s.length()+1];
    s.toWCharArray( d->data );
    return *this;
}


bool WString::isEmpty() const
{
    return d->data == 0;
}


int WString::length() const
{
    return wcslen( d->data );
}


const wchar_t* WString::data() const
{
    return d->data;
}


WString::operator QString() const
{
    return toQString();
}


QString WString::toQString() const
{
    return QString::fromWCharArray( d->data );
}


bool WString::operator==( const WString& other ) const
{
    return wcscmp( d->data, other.d->data ) == 0;
}


bool WString::operator==( const QString& other ) const
{
    return toQString() == other;
}


bool WString::operator!=( const WString& other ) const
{
    return !operator==( other );
}


bool WString::operator!=( const QString& other ) const
{
    return toQString() != other;
}
