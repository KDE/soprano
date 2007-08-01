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
    Private()
        : parent( 0 ) {
    }

    Model* parent;
};


Soprano::FilterModel::FilterModel()
    : Model(),
      d( new Private() )
{
}


Soprano::FilterModel::FilterModel( Model* parent )
    : Model(),
      d( new Private() )
{
    setParent( parent );
}


Soprano::FilterModel::~FilterModel()
{
    delete d;
}


Soprano::Model* Soprano::FilterModel::parentModel() const
{
    return d->parent;
}


void Soprano::FilterModel::setParentModel( Model* model )
{
    if ( model != d->parent ) {
        if ( d->parent ) {
            d->parent->disconnect( this );
        }
        d->parent = model;
        if ( d->parent ) {
            connect( d->parent, SIGNAL(statementsAdded()), this, SLOT(slotStatementsAdded()) );
            connect( d->parent, SIGNAL(statementsRemoved()), this, SLOT(slotStatementsRemoved()) );
        }
    }
}


Soprano::ErrorCode Soprano::FilterModel::add( const Model &model )
{
    Q_ASSERT( d->parent );
    return d->parent->add( model );
}


Soprano::ErrorCode Soprano::FilterModel::add( const StatementIterator &iter )
{
    Q_ASSERT( d->parent );
    return d->parent->add( iter );
}


Soprano::ErrorCode Soprano::FilterModel::add( const QList<Statement> &statements )
{
    Q_ASSERT( d->parent );
    return d->parent->add( statements );
}


Soprano::ErrorCode Soprano::FilterModel::add( const Statement &statement )
{
    Q_ASSERT( d->parent );
    return d->parent->add( statement );
}


bool Soprano::FilterModel::isEmpty() const
{
    Q_ASSERT( d->parent );
    return d->parent->isEmpty();
}


QList<Soprano::Node> Soprano::FilterModel::contexts() const
{
    Q_ASSERT( d->parent );
    return d->parent->contexts();
}


bool Soprano::FilterModel::contains( const Statement &statement ) const
{
    Q_ASSERT( d->parent );
    return d->parent->contains( statement );
}


bool Soprano::FilterModel::contains( const Node &context ) const
{
    Q_ASSERT( d->parent );
    return d->parent->contains( context );
}


Soprano::ResultSet Soprano::FilterModel::executeQuery( const Query &query ) const
{
    Q_ASSERT( d->parent );
    return d->parent->executeQuery( query );
}


Soprano::StatementIterator Soprano::FilterModel::listStatements() const
{
    Q_ASSERT( d->parent );
    return d->parent->listStatements();
}


Soprano::StatementIterator Soprano::FilterModel::listStatements( const Node &context ) const
{
    Q_ASSERT( d->parent );
    return d->parent->listStatements( context );
}


Soprano::StatementIterator Soprano::FilterModel::listStatements( const Statement &partial ) const
{
    Q_ASSERT( d->parent );
    return d->parent->listStatements( partial );
}


Soprano::ErrorCode Soprano::FilterModel::remove( const Statement &statement )
{
    Q_ASSERT( d->parent );
    return d->parent->remove( statement );
}


Soprano::ErrorCode Soprano::FilterModel::remove( const QList<Statement> &statements )
{
    Q_ASSERT( d->parent );
    return d->parent->remove( statements );
}


Soprano::ErrorCode Soprano::FilterModel::remove( const Node &context )
{
    Q_ASSERT( d->parent );
    return d->parent->remove( context );
}


Soprano::ErrorCode Soprano::FilterModel::removeAll( const Statement &statement )
{
    Q_ASSERT( d->parent );
    return d->parent->removeAll( statement );
}


Soprano::ErrorCode Soprano::FilterModel::removeAll()
{
    Q_ASSERT( d->parent );
    return d->parent->removeAll();
}


int Soprano::FilterModel::size() const
{
    Q_ASSERT( d->parent );
    return d->parent->size();
}


Soprano::ErrorCode Soprano::FilterModel::write( QTextStream &os ) const
{
    Q_ASSERT( d->parent );
    return d->parent->write( os );
}


Soprano::ErrorCode Soprano::FilterModel::print() const
{
    Q_ASSERT( d->parent );
    return d->parent->print();
}


void Soprano::FilterModel::slotStatementsAdded()
{
    emit statementsAdded();
}


void Soprano::FilterModel::slotStatementsRemoved()
{
    emit statementsRemoved();
}

#include "filtermodel.moc"
