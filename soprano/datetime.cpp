/*
 *
 * $Id: sourceheader 511311 2006-02-19 14:51:05Z trueg $
 *
 * This file is part of the Nepomuk KDE project.
 * Copyright (C) 2006 Sebastian Trueg <trueg@kde.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * See the file "COPYING" for the exact licensing terms.
 */

#include "datetime.h"

#include <QtCore/QDebug>
#include <QtCore/QLocale>

#include <stdlib.h>

// from QT 4.3
#ifndef Q_FUNC_INFO
#if defined(Q_CC_GNU) || defined(Q_CC_HPACC)
#  define Q_FUNC_INFO __PRETTY_FUNCTION__
#elif defined(_MSC_VER) && _MSC_VER > 1300
#  define Q_FUNC_INFO __FUNCSIG__
#else
    /* These two macros makes it possible to turn the builtin line expander into a
     * string literal. */
#   define QT_STRINGIFY2(x) #x
#   define QT_STRINGIFY(x) QT_STRINGIFY2(x)
#   define Q_FUNC_INFO __FILE__ ":" QT_STRINGIFY(__LINE__)
/*
   The MIPSpro compiler postpones macro expansion, and therefore macros must be in scope
   when being used
*/
#   if !defined(Q_CC_MIPS)
#       undef QT_STRINGIFY2
#       undef QT_STRINGIFY
#   endif
#endif
#endif


QTime Soprano::DateTime::fromTimeString( const QString& s )
{
    // ensure the format
    if( s[2] != ':' ||
        s[5] != ':' ) {
        qDebug() << Q_FUNC_INFO << " invalid formatted time string: " << s << endl;
        return QTime();
    }

    bool ok = true;

    int hh = s.mid( 0, 2 ).toInt( &ok );
    if( !ok ) {
        qDebug() << Q_FUNC_INFO << " invalid formatted time string: " << s << endl;
        return QTime();
    }

    int mm = s.mid( 3, 2 ).toInt( &ok );
    if( !ok ) {
        qDebug() << Q_FUNC_INFO << " invalid formatted time string: " << s << endl;
        return QTime();
    }

    int ss = s.mid( 6, 2 ).toInt( &ok );
    if( !ok ) {
        qDebug() << Q_FUNC_INFO << " invalid formatted time string: " << s << endl;
        return QTime();
    }

    int pos = 8;

    // parse the fraction of seconds
    int z = 0;
    if( s[8] == '.' || s[8] == ',' ) {
        ++pos;
        while( s[pos].isDigit() )
            ++pos;
        z = s.mid(9, pos-9).leftJustified( 3, '0' ).toInt(&ok);
        if( !ok ) {
            qDebug() << Q_FUNC_INFO << " invalid formatted time string: " << s << endl;
            return QTime();
        }
    }

    // finally create the time object
    QTime t( hh, mm, ss, z );

    // parse the timezone
    if( s[pos] == 'Z' ) {
        return t;
    }
    else {
        if( s.length() != pos+6 ) {
            qDebug() << Q_FUNC_INFO << " invalid formatted timezone string: " << s << endl;
            return QTime();
        }

        bool add = true;
        if( s[pos] == '+' )
            add = true;
        else if( s[pos] == '-' )
            add = false;
        else {
            qDebug() << Q_FUNC_INFO << " invalid formatted timezone string: " << s << endl;
            return QTime();
        }

        ++pos;

        hh = s.mid(pos, 2).toInt( &ok );
        if( !ok ) {
            qDebug() << Q_FUNC_INFO << " invalid formatted timezone string: " << s << endl;
            return QTime();
        }

        pos += 3;

        mm = s.mid(pos, 2).toInt( &ok );
        if( !ok ) {
            qDebug() << Q_FUNC_INFO << " invalid formatted timezone string: " << s << endl;
            return QTime();
        }

        int secs = 60*( 60*hh + mm );
        if( add )
            return t.addSecs( -1*secs );
        else
            return t.addSecs( secs );
    }

    return t;
}


QDate Soprano::DateTime::fromDateString( const QString& s )
{
    bool ok = true;

    int pos = 0;

    int yLen = s.indexOf( '-', 1 );
    int y = s.mid(0, yLen).toInt( &ok );
    if( !ok ) {
        qDebug() << Q_FUNC_INFO << " invalid formatted date string: " << s << endl;
        return QDate();
    }
    pos += yLen+1;

    int m = s.mid(pos,2).toInt( &ok );
    if( !ok ) {
        qDebug() << Q_FUNC_INFO << " invalid formatted date string: " << s << endl;
        return QDate();
    }
    pos += 3;
    int d = s.mid(pos,2).toInt( &ok );
    if( !ok ) {
        qDebug() << Q_FUNC_INFO << " invalid formatted date string: " << s << endl;
        return QDate();
    }

    return QDate( y, m, d );
}


QDateTime Soprano::DateTime::fromDateTimeString( const QString& s )
{
    int pos = s.indexOf('T');
    if( pos > 0 ) {
        QDate date = fromDateString( s.mid( 0, pos ) );
        if( !date.isValid() )
            return QDateTime();
        QTime time = fromTimeString( s.mid( pos+1 ) );
        if( !time.isValid() )
            return QDateTime();
        return QDateTime( date, time, Qt::UTC );
    }
    else {
        qDebug() << Q_FUNC_INFO << " invalid formatted datetime string: " << s << endl;
        return QDateTime();
    }
}


QString Soprano::DateTime::toString( const QTime& t )
{
    QString frac;
    if( t.msec() > 0 ) {
        frac.sprintf( "%03d", t.msec() );
        // remove trailing zeros
        while( frac.endsWith( "0" ) )
            frac.truncate( frac.length() -1 );
    }

    // Comma is preferred by ISO8601 as the decimal point symbol,
    // so use it unless '.' is the symbol used in this locale.
    return t.toString( "HH:mm:ss" )
        + ( QLocale::system().decimalPoint() == QChar( '.' ) ? QChar( '.' ) : QChar( ',' ) )
        + frac
        + 'Z';
}


QString Soprano::DateTime::toString( const QDate& d )
{
    // QDate::toString does not work for dates BC
    // Also QDate treats years between 0 and 99 as 1900 - 1999
    // we have to treat the - sign extra since QString::arg does the filling before it
    return QString("%1%2-%3-%4")
        .arg( d.year() < 0 ? QString("-") : QString() )
        .arg( abs(d.year()), 4, 10, QChar('0') )
        .arg( d.month(), 2, 10, QChar('0') )
        .arg( d.day(), 2, 10, QChar('0') );
}


QString Soprano::DateTime::toString( const QDateTime& dt )
{
    return toString( dt.date() ) + 'T' + toString( dt.toUTC().time() );
}
