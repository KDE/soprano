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

#include "dummymodel.h"
#include "statement.h"
#include "queryresultiterator.h"
#include "statementiterator.h"
#include "nodeiterator.h"
#include "query/query.h"


class Soprano::Util::DummyModel::Private
{
public:
    Private()
        : defaultError( "Invalid model", Error::ErrorNotSupported ) {
    }
    Error::Error defaultError;
};

Soprano::Util::DummyModel::DummyModel()
    : Model(),
      d( new Private() )
{
}


Soprano::Util::DummyModel::~DummyModel()
{
    delete d;
}


Soprano::Error::ErrorCode Soprano::Util::DummyModel::addStatement( const Statement& )
{
    setError( d->defaultError );
    return Error::ErrorNotSupported;
}


Soprano::Error::ErrorCode Soprano::Util::DummyModel::addStatement( const Node&, const Node&, const Node&, const Node& )
{
    setError( d->defaultError );
    return Error::ErrorNotSupported;
}


bool Soprano::Util::DummyModel::isEmpty() const
{
    setError( d->defaultError );
    return true;
}


Soprano::NodeIterator Soprano::Util::DummyModel::listContexts() const
{
    setError( d->defaultError );
    return NodeIterator();
}


bool Soprano::Util::DummyModel::containsStatement( const Statement& ) const
{
    setError( d->defaultError );
    return false;
}


bool Soprano::Util::DummyModel::containsStatement( const Node&, const Node&, const Node&, const Node& ) const
{
    setError( d->defaultError );
    return false;
}


bool Soprano::Util::DummyModel::containsAnyStatement( const Statement& ) const
{
    setError( d->defaultError );
    return false;
}


bool Soprano::Util::DummyModel::containsAnyStatement( const Node&, const Node&, const Node&, const Node& ) const
{
    setError( d->defaultError );
    return false;
}


// Soprano::QueryResultIterator Soprano::Util::DummyModel::executeQuery( const Query::Query& ) const
// {
//     setError( d->defaultError );
//     return QueryResultIterator();
// }


Soprano::QueryResultIterator Soprano::Util::DummyModel::executeQuery( const QString&, Query::QueryLanguage, const QString& ) const
{
    setError( d->defaultError );
    return QueryResultIterator();
}


Soprano::StatementIterator Soprano::Util::DummyModel::listStatements( const Statement& ) const
{
    setError( d->defaultError );
    return StatementIterator();
}


Soprano::StatementIterator Soprano::Util::DummyModel::listStatements( const Node&, const Node&, const Node&, const Node& ) const
{
    setError( d->defaultError );
    return StatementIterator();
}


Soprano::Error::ErrorCode Soprano::Util::DummyModel::removeStatement( const Statement& )
{
    setError( d->defaultError );
    return Error::ErrorNotSupported;
}


Soprano::Error::ErrorCode Soprano::Util::DummyModel::removeStatement( const Node&, const Node&, const Node&, const Node& )
{
    setError( d->defaultError );
    return Error::ErrorNotSupported;
}


Soprano::Error::ErrorCode Soprano::Util::DummyModel::removeAllStatements( const Statement& )
{
    setError( d->defaultError );
    return Error::ErrorNotSupported;
}


int Soprano::Util::DummyModel::statementCount() const
{
    setError( d->defaultError );
    return 0;
}


Soprano::Node Soprano::Util::DummyModel::createBlankNode()
{
    setError( d->defaultError );
    return Node();
}


Soprano::Error::ErrorCode Soprano::Util::DummyModel::write( QTextStream& ) const
{
    setError( d->defaultError );
    return Error::ErrorNotSupported;
}

