/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2007 Daniele Galdi <daniele.galdi@gmail.com>
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

#include "locator.h"

class Soprano::Error::Locator::Private: public QSharedData {
public:
    Private()
        : line(-1),
          column(-1),
          byte(-1)
    {};

    int line;
    int column;
    int byte;
    QString fileName;
};

Soprano::Error::Locator::Locator()
{
    d = new Private();
}

Soprano::Error::Locator::Locator( const Locator &other )
{
    d = other.d;
}

Soprano::Error::Locator::Locator( int line, int column, int byte, const QString& filename )
    : d( new Private() )
{
    d->line = line;
    d->column = column;
    d->byte = byte;
    d->fileName = filename;
}

Soprano::Error::Locator::~Locator()
{
}

Soprano::Error::Locator& Soprano::Error::Locator::operator=( const Locator &other )
{
    d = other.d;
    return *this;
}

int Soprano::Error::Locator::line() const
{
    return d->line;
}

void Soprano::Error::Locator::setLine( int line )
{
    d->line = line;
}

int Soprano::Error::Locator::column() const
{
    return d->column;
}

void Soprano::Error::Locator::setColumn( int column )
{
    d->column = column;
}

int Soprano::Error::Locator::byte() const
{
    return d->byte;
}

void Soprano::Error::Locator::setByte( int byte )
{
   d->byte = byte;
}

QString Soprano::Error::Locator::fileName() const
{
    return d->fileName;
}

void Soprano::Error::Locator::setFileName( const QString& fileName )
{
    d->fileName = fileName;
}
