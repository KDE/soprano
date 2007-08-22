/* This file is part of Soprano
 *
 * Copyright (C) 2006 Daniele Galdi <daniele.galdi@gmail.com>
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

#include "querylegacy.h"

#include <QtCore/QString>


class Soprano::QueryLegacy::Private
{
public:
  Private() : limit(-1), offset(-1)
  {}
  int limit;
  int offset;
  QString query;
  QueryType type;
};

Soprano::QueryLegacy::QueryLegacy( const QString &query, QueryType type )
    : d( new Private() )
{
  d->query = query;
  d->type = type;
}

Soprano::QueryLegacy::QueryLegacy( const QString &query, QueryType type, int limit, int offset )
    : d( new Private() )
{
  d->query = query;
  d->type = type;
  d->limit = limit;
  d->offset = offset;
}

Soprano::QueryLegacy::QueryLegacy( const QueryLegacy &other )
    : d( new Private() )
{
  d->query = other.query();
  d->type = other.type();
  d->limit = other.limit();
  d->offset = other.offset();
}

Soprano::QueryLegacy::~QueryLegacy()
{
  delete d;
}

Soprano::QueryLegacy::QueryType Soprano::QueryLegacy::type() const
{
  return d->type;
}

QString Soprano::QueryLegacy::query() const
{
  return d->query;
}

int Soprano::QueryLegacy::limit() const
{
  return d->limit;
}

void Soprano::QueryLegacy::setLimit(int limit)
{
  d->limit = limit;
}

int Soprano::QueryLegacy::offset() const
{
  return d->offset;
}

void Soprano::QueryLegacy::setOffset(int offset)
{
  d->offset = offset;
}


QString Soprano::QueryLegacy::queryTypeToString( QueryType queryType )
{
    switch( queryType ) {
    case SPARQL:
        return "sparql";
    case RDQL:
        return "rdql";
    }
}


int Soprano::QueryLegacy::queryTypeFromString( const QString& s )
{
    QString sn( s.toLower() );
    if ( sn == "sparql" ) {
        return SPARQL;
    }
    else if ( sn == "rdql" ) {
        return RDQL;
    }
    else {
        return -1;
    }
}
