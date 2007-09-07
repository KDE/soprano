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

#include "indexqueryhit.h"
#include "node.h"

class Soprano::Index::QueryHit::Private : public QSharedData
{
    public:
    Private( const Node& res = Node(), double s = 0.0 )
        : resource( res ),
          score( s ) {
    }

    Node resource;
    double score;
};


Soprano::Index::QueryHit::QueryHit()
    : d( new Private() )
{
}


Soprano::Index::QueryHit::QueryHit( const Node& res,  double s )
    : d( new Private( res, s ) )
{
}


Soprano::Index::QueryHit::QueryHit( const QueryHit& other )
{
    d = other.d;
}


Soprano::Index::QueryHit::~QueryHit()
{
}


Soprano::Index::QueryHit& Soprano::Index::QueryHit::operator=( const QueryHit& other )
{
    d = other.d;
    return *this;
}


double Soprano::Index::QueryHit::score() const
{
    return d->score;
}


Soprano::Node Soprano::Index::QueryHit::resource() const
{
    return d->resource;
}
