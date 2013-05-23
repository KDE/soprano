/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2008-2009 Sebastian Trueg <trueg@kde.org>
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

#include "asyncresult.h"
#include "node.h"
#include "nodeiterator.h"
#include "statementiterator.h"
#include "queryresultiterator.h"


Q_DECLARE_METATYPE( Soprano::Error::ErrorCode )
Q_DECLARE_METATYPE( Soprano::Node )
Q_DECLARE_METATYPE( Soprano::StatementIterator )
Q_DECLARE_METATYPE( Soprano::NodeIterator )
Q_DECLARE_METATYPE( Soprano::QueryResultIterator )
Q_DECLARE_METATYPE( Soprano::Util::AsyncResult* )


Soprano::Util::AsyncResult* Soprano::Util::AsyncResult::createResult()
{
    return new AsyncResult();
}


Soprano::Util::AsyncResult::AsyncResult()
    : QObject( 0 )
{
    qRegisterMetaType<Soprano::Util::AsyncResult*>();
}


Soprano::Util::AsyncResult::~AsyncResult()
{
}


void Soprano::Util::AsyncResult::setResult( const QVariant& result, const Error::Error& error )
{
    m_result = result;
    setError( error );
    // ensure that results are delivered in the calling thread
    QMetaObject::invokeMethod( this, "slotResultReady", Qt::QueuedConnection );
}


void Soprano::Util::AsyncResult::slotResultReady()
{
    emit resultReady( this );
    delete this;
}


Soprano::Error::ErrorCode Soprano::Util::AsyncResult::errorCode() const
{
    return value().value<Error::ErrorCode>();
}


Soprano::StatementIterator Soprano::Util::AsyncResult::statementIterator() const
{
    return value().value<StatementIterator>();
}


Soprano::NodeIterator Soprano::Util::AsyncResult::nodeIterator() const
{
    return value().value<NodeIterator>();
}


Soprano::QueryResultIterator Soprano::Util::AsyncResult::queryResultIterator() const
{
    return value().value<QueryResultIterator>();
}


Soprano::Node Soprano::Util::AsyncResult::node() const
{
    return value().value<Node>();
}

