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

#ifndef _SOPRANO_DATE_TIME_H_
#define _SOPRANO_DATE_TIME_H_

#include <QtCore/QString>
#include <QtCore/QDateTime>


namespace Soprano {
    namespace DateTime
	{
	    /**
	     * Parse a time string according to format hh:mm:ss.szzzzzz as defined in 
	     * XML Schema Part 2: Datatypes Second Edition: http://www.w3.org/TR/xmlschema-2
	     *
	     * Be aware that QTime has no notion of timezones and the returned value is 
	     * always in UTC.
	     */
	    QTime fromTimeString( const QString& );
	    QDate fromDateString( const QString& );
	    QDateTime fromDateTimeString( const QString& );

	    QString toString( const QTime& );
	    QString toString( const QDate& );
	    QString toString( const QDateTime& );
	}
}

#endif
