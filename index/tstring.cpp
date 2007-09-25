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

#include "tstring.h"

#include <QDebug>



namespace {
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
}


class TString::Private : public QSharedData
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

    TCHAR* data;
    bool wrap;
};


TString::TString()
    : d( new Private() )
{
}


TString::TString( const TString& s )
{
    d = s.d;
}


TString::TString( const TCHAR* s, bool wrap )
    : d( new Private() )
{
    d->wrap = wrap;
    if ( wrap ) {
        d->data = const_cast<TCHAR*>( s );
    }
    else {
        d->data = new TCHAR[_tcslen(s) + 1];
        _tcscpy( d->data, s );
    }
}


TString::TString( const QString& s )
    : d( new Private() )
{
    operator=( s );
}


TString::~TString()
{
}


TString& TString::operator=( const TString& s )
{
    d = s.d;
    return *this;
}


TString& TString::operator=( const TCHAR* s )
{
    d->data = new TCHAR[ _tcslen(s) + 1 ];
    _tcscpy( d->data, s );
    return *this;
}


TString& TString::operator=( const QString& s )
{
    // looks as if QT has a bug in toWCharArray, so we use the method from strigi
    QByteArray utf8 = s.toUtf8();

#ifdef _UCS2
    d->data = new TCHAR[s.length()+1];
    utf8toucs2( utf8.data(), utf8.data()+utf8.length(), d->data );
//    s.toWCharArray( d->data );
#else
    d->data = strdup( utf8.data() );
#endif
    return *this;
}


bool TString::isEmpty() const
{
    return d->data == 0;
}


int TString::length() const
{
    return _tcslen( d->data );
}


const TCHAR* TString::data() const
{
    return d->data;
}


TString::operator QString() const
{
    return toQString();
}


QString TString::toQString() const
{
#ifdef _UCS2
    return QString::fromWCharArray( d->data );
#else
    return QString::fromUtf8( d->data );
#endif
}


bool TString::operator==( const TString& other ) const
{
    return _tcscmp( d->data, other.d->data ) == 0;
}


bool TString::operator==( const QString& other ) const
{
    return toQString() == other;
}


bool TString::operator!=( const TString& other ) const
{
    return !operator==( other );
}


bool TString::operator!=( const QString& other ) const
{
    return toQString() != other;
}


TString TString::fromUtf8( const char* data )
{
    TString s;
#ifdef _UCS2
    s.d->data = new TCHAR[ strlen( data ) + 1 ]; // like this we are on the safe side
    utf8toucs2( data, data + strlen( data ), s.d->data );
#else
    s.d->data = strdup( data );
#endif
    return s;
}
