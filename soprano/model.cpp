/*
 * This file is part of Soprano Project
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

#include "model.h"

#include "node.h"
#include "statement.h"
#include "statementiterator.h"

#include <QtCore/QList>


class Soprano::Model::Private
{
public:
};

Soprano::Model::Model()
    : QObject( 0 ),
      d( new Private() )
{
}

Soprano::Model::~Model()
{
    delete d;
}

// private no-op constructor not meant to be used
Soprano::Model::Model( const Model& )
    : d( 0 )
{
}


// private no-op operator not meant to be used
Soprano::Model& Soprano::Model::operator=( const Model& )
{
    return *this;
}


bool Soprano::Model::containsContext( const Node &context ) const
{
    return listStatementsInContext( context ).next();
}

Soprano::StatementIterator Soprano::Model::listStatements() const
{
    return listStatements( Statement() );
}


Soprano::StatementIterator Soprano::Model::listStatementsInContext( const Node &context ) const
{
    return listStatements( Statement( Node(), Node(), Node(), context ) );
}


Soprano::Error::ErrorCode Soprano::Model::removeStatements( const QList<Statement> &statements )
{
    for ( QList<Statement>::const_iterator it = statements.constBegin();
          it != statements.constEnd(); ++it ) {
        Error::ErrorCode c = removeStatements( *it );
        if ( c != Error::ERROR_NONE ) {
            return c;
        }
    }
    return Error::ERROR_NONE;
}


Soprano::Error::ErrorCode Soprano::Model::removeContext( const Node& context )
{
    return removeStatements( Statement( Node(), Node(), Node(), context ) );
}


Soprano::Error::ErrorCode Soprano::Model::removeAllStatements()
{
    return removeStatements( Statement() );
}


Soprano::Error::ErrorCode Soprano::Model::addStatements( const QList<Statement> &statements )
{
    QListIterator<Statement> iter(statements);
    while ( iter.hasNext() ) {
        Error::ErrorCode c = addStatement( iter.next() );
        if ( Error::ERROR_NONE != c ) {
            return c;
        }
    }

    return Error::ERROR_NONE;
}


Soprano::Error::ErrorCode Soprano::Model::write( QTextStream &os ) const
{
    StatementIterator it = listStatements();
    while ( it.next() ) {
        os << *it << endl;
    }
}


Soprano::Error::ErrorCode Soprano::Model::print() const
{
    QTextStream os( stdout );
    write( os );
}

#include "model.moc"
