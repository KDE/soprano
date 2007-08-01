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
    : d( new Private() )
{
}

Soprano::Model::~Model()
{
    delete d;
}



bool Soprano::Model::containsContext( const Node &context ) const
{
    return !listStatementsInContext( context ).isEmpty();
}

Soprano::StatementIterator Soprano::Model::listStatements() const
{
    return listStatements( Statement() );
}


Soprano::StatementIterator Soprano::Model::listStatementsInContext( const Node &context ) const
{
    return listStatements( Statement( Node(), Node(), Node(), context ) );
}


Soprano::ErrorCode Soprano::Model::removeStatements( const QList<Statement> &statements )
{
    for ( QList<Statement>::const_iterator it = statements.constBegin();
          it != statements.constEnd(); ++it ) {
        ErrorCode c = removeStatements( *it );
        if ( c != ERROR_NONE ) {
            return c;
        }
    }
    return ERROR_NONE;
}


Soprano::ErrorCode Soprano::Model::removeContext( const Node& context )
{
    return removeStatements( Statement( Node(), Node(), Node(), context ) );
}


Soprano::ErrorCode Soprano::Model::removeAllStatements()
{
    return removeStatements( Statement() );
}


Soprano::ErrorCode Soprano::Model::addModel( const Model &model )
{
    StatementIterator stmi = model.listStatements( Statement() );
    if ( !stmi.isValid() ) {
        return ERROR_UNKNOW;
    }

    while ( stmi.hasNext() ) {
        ErrorCode c = addStatement( stmi.next() );
        if ( ERROR_NONE != c ) {
            return c;
        }
    }

    return ERROR_NONE;
}

Soprano::ErrorCode Soprano::Model::addStatements( const StatementIterator &iter )
{
    if ( !iter.isValid() ) {
        return ERROR_UNKNOW;
    }

    while ( iter.hasNext() ) {
        ErrorCode c = addStatement( iter.next() );
        if ( ERROR_NONE != c ) {
            return c;
        }
    }

    return ERROR_NONE;
}

Soprano::ErrorCode Soprano::Model::addStatements( const QList<Statement> &statements )
{
    // FIXME: why not call add with an empty context here?

    QListIterator<Statement> iter(statements);
    while ( iter.hasNext() ) {
        ErrorCode c = addStatement( iter.next() );
        if ( ERROR_NONE != c ) {
            return c;
        }
    }

    return ERROR_NONE;
}


Soprano::ErrorCode Soprano::Model::write( QTextStream &os ) const
{
    StatementIterator it = listStatements();
    while ( it.hasNext() ) {
        os << it.next() << endl;
    }
}


Soprano::ErrorCode Soprano::Model::print() const
{
    QTextStream os( stdout );
    write( os );
}

#include "model.moc"
