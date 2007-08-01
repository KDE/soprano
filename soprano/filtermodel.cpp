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

#include "filtermodel.h"
#include "statement.h"
#include "resultset.h"
#include "statementiterator.h"

#include <QtCore/QList>


class Soprano::FilterModel::Private
{
public:
    Model* parent;
};


Soprano::FilterModel::FilterModel( Model* parent )
    : Model(),
      d( new Private() )
{
    d->parent = parent;
}


Soprano::FilterModel::~FilterModel()
{
    delete d;
}



Soprano::Model* Soprano::FilterModel::parent() const
{
    return d->parent;
}


Soprano::ErrorCode Soprano::FilterModel::add( const Model &model )
{
    return d->parent->add( model );
}


Soprano::ErrorCode Soprano::FilterModel::add( const StatementIterator &iter )
{
    return d->parent->add( iter );
}


Soprano::ErrorCode Soprano::FilterModel::add( const QList<Statement> &statements )
{
    return d->parent->add( statements );
}


Soprano::ErrorCode Soprano::FilterModel::add( const Statement &statement )
{
    return d->parent->add( statement );
}


bool Soprano::FilterModel::isEmpty() const
{
    return d->parent->isEmpty();
}


QList<Soprano::Node> Soprano::FilterModel::contexts() const
{
    return d->parent->contexts();
}


bool Soprano::FilterModel::contains( const Statement &statement ) const
{
    return d->parent->contains( statement );
}


bool Soprano::FilterModel::contains( const Node &context ) const
{
    return d->parent->contains( context );
}


Soprano::ResultSet Soprano::FilterModel::executeQuery( const Query &query ) const
{
    return d->parent->executeQuery( query );
}


Soprano::StatementIterator Soprano::FilterModel::listStatements() const
{
    return d->parent->listStatements();
}


Soprano::StatementIterator Soprano::FilterModel::listStatements( const Node &context ) const
{
    return d->parent->listStatements( context );
}


Soprano::StatementIterator Soprano::FilterModel::listStatements( const Statement &partial ) const
{
    return d->parent->listStatements( partial );
}


Soprano::ErrorCode Soprano::FilterModel::remove( const Statement &statement )
{
    return d->parent->remove( statement );
}


Soprano::ErrorCode Soprano::FilterModel::remove( const QList<Statement> &statements )
{
    return d->parent->remove( statements );
}


Soprano::ErrorCode Soprano::FilterModel::remove( const Node &context )
{
    return d->parent->remove( context );
}


Soprano::ErrorCode Soprano::FilterModel::removeAll( const Statement &statement )
{
    return d->parent->removeAll( statement );
}


Soprano::ErrorCode Soprano::FilterModel::removeAll()
{
    return d->parent->removeAll();
}


int Soprano::FilterModel::size() const
{
    return d->parent->size();
}


Soprano::ErrorCode Soprano::FilterModel::write( QTextStream &os ) const
{
    return d->parent->write( os );
}


Soprano::ErrorCode Soprano::FilterModel::print() const
{
    return d->parent->print();
}


void Soprano::FilterModel::setupDefaultConnections()
{
    connect( d->parent, SIGNAL(statementsAdded()), this, SIGNAL(statementsAdded()) );
    connect( d->parent, SIGNAL(statementsRemoved()), this, SIGNAL(statementsRemoved()) );
}

#include "filtermodel.moc"
