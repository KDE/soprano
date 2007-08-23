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

class Soprano::Locator::Private: public QSharedData {
public:
    Private() {};

    int line;
    int column;
    int byte;
    QString fileName;
};

Soprano::Locator::Locator()
{
    d = new Private();
}

Soprano::Locator::Locator( const Locator &other )
{
    d = other.d;
}

Soprano::Locator::~Locator()
{       
}

Soprano::Locator Soprano::Locator::operator=( const Locator &other )
{
    d = other.d;
    return *this;
}

int Soprano::Locator::line() const
{
    return d->line;
}

void Soprano::Locator::setLine( int line )
{
    d->line = line;
}

int Soprano::Locator::column() const
{
    return d->column;
}
    
void Soprano::Locator::setColumn( int column )
{
    d->column = column;
}

int Soprano::Locator::byte() const
{
    return d->byte;
}
    
void Soprano::Locator::setByte( int byte )
{
   d->byte = byte;
}

const QString Soprano::Locator::fileName() const
{
    return d->fileName;
}
    
void Soprano::Locator::setFileName( const QString& fileName )
{
    d->fileName = fileName;
}
