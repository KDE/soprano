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

#include "multicallprotectionmodel.h"
#include "multicallprotectioniteratorbase.h"
#include "looplock.h"
#include "multicallprotectionstatementiteratorbackend.h"
#include "multicallprotectionnodeiteratorbackend.h"
#include "multicallprotectionqueryresultiteratorbackend.h"

#include <soprano/statementiterator.h>
#include <soprano/nodeiterator.h>
#include <soprano/queryresultiterator.h>

#include <QtCore/QReadWriteLock>
#include <QtCore/QMutex>


class Soprano::Util::MultiCallProtectionModel::Private
{
public:
    Private( MultiCallProtectionModel::ProtectionMode mode )
        : m_protectionMode( mode ) {
    }

    QList<MultiCallProtectionIteratorBase*> openIterators;

    void lockForWrite() {
        switch( m_protectionMode ) {
        case PlainMultiThreading:
            m_mutex.lock();
            break;
        case ReadWriteMultiThreading:
            m_msLock.lockForWrite();
            break;
        case ReadWriteSingleThreading:
            m_loopLock.lockForWrite();
            break;
        }
    }

    void lockForRead() {
        switch( m_protectionMode ) {
        case PlainMultiThreading:
            m_mutex.lock();
            break;
        case ReadWriteMultiThreading:
            m_msLock.lockForRead();
            break;
        case ReadWriteSingleThreading:
            m_loopLock.lockForRead();
            break;
        }
    }

    void unlock() {
        switch( m_protectionMode ) {
        case PlainMultiThreading:
            m_mutex.unlock();
            break;
        case ReadWriteMultiThreading:
            m_msLock.unlock();
            break;
        case ReadWriteSingleThreading:
            m_loopLock.unlock();
            break;
        }
    }

private:
    LoopLock m_loopLock;
    QReadWriteLock m_msLock;
    QMutex m_mutex;

    ProtectionMode m_protectionMode;
};


Soprano::Util::MultiCallProtectionModel::MultiCallProtectionModel( ProtectionMode mode, Model* parent )
    : FilterModel( parent ),
      d( new Private( mode ) )
{
}


Soprano::Util::MultiCallProtectionModel::~MultiCallProtectionModel()
{
    Q_FOREACH( MultiCallProtectionIteratorBase* it, d->openIterators ) {
        it->close();
    }

    delete d;
}


Soprano::Error::ErrorCode Soprano::Util::MultiCallProtectionModel::addStatement( const Statement &statement )
{
    d->lockForWrite();
    Error::ErrorCode c = FilterModel::addStatement( statement );
    d->unlock();
    return c;
}


Soprano::Error::ErrorCode Soprano::Util::MultiCallProtectionModel::removeStatement( const Statement &statement )
{
    d->lockForWrite();
    Error::ErrorCode c = FilterModel::removeStatement( statement );
    d->unlock();
    return c;
}


Soprano::Error::ErrorCode Soprano::Util::MultiCallProtectionModel::removeAllStatements( const Statement &statement )
{
    d->lockForWrite();
    Error::ErrorCode c = FilterModel::removeAllStatements( statement );
    d->unlock();
    return c;
}


Soprano::StatementIterator Soprano::Util::MultiCallProtectionModel::listStatements( const Statement &partial ) const
{
    d->lockForRead();
    StatementIterator it = FilterModel::listStatements( partial );
    if ( it.isValid() ) {
        MultiCallProtectionStatementIteratorBackend* b = new MultiCallProtectionStatementIteratorBackend( it, const_cast<MultiCallProtectionModel*>( this ) );
        d->openIterators.append( b );
        return b;
    }
    else {
        d->unlock();
        return it;
    }
}


Soprano::NodeIterator Soprano::Util::MultiCallProtectionModel::listContexts() const
{
    d->lockForRead();
    NodeIterator it = FilterModel::listContexts();
    if ( it.isValid() ) {
        MultiCallProtectionNodeIteratorBackend* b = new MultiCallProtectionNodeIteratorBackend( it, const_cast<MultiCallProtectionModel*>( this ) );
        d->openIterators.append( b );
        return b;
    }
    else {
        d->unlock();
        return it;
    }
}


Soprano::QueryResultIterator Soprano::Util::MultiCallProtectionModel::executeQuery( const QString& query,
                                                                      Query::QueryLanguage language,
                                                                      const QString& userQueryLanguage ) const
{
    d->lockForRead();
    QueryResultIterator it = FilterModel::executeQuery( query, language, userQueryLanguage );
    if ( it.isValid() ) {
        MultiCallProtectionQueryResultIteratorBackend* b = new MultiCallProtectionQueryResultIteratorBackend( it, const_cast<MultiCallProtectionModel*>( this ) );
        d->openIterators.append( b );
        return b;
    }
    else {
        d->unlock();
        return it;
    }
}


bool Soprano::Util::MultiCallProtectionModel::containsStatement( const Statement &statement ) const
{
    d->lockForRead();
    bool b = FilterModel::containsStatement( statement );
    d->unlock();
    return b;
}


bool Soprano::Util::MultiCallProtectionModel::containsAnyStatement( const Statement &statement ) const
{
    d->lockForRead();
    bool b = FilterModel::containsAnyStatement( statement );
    d->unlock();
    return b;
}


bool Soprano::Util::MultiCallProtectionModel::isEmpty() const
{
    d->lockForRead();
    bool b = FilterModel::isEmpty();
    d->unlock();
    return b;
}


int Soprano::Util::MultiCallProtectionModel::statementCount() const
{
    d->lockForRead();
    int cnt = FilterModel::statementCount();
    d->unlock();
    return cnt;
}


void Soprano::Util::MultiCallProtectionModel::removeIterator( MultiCallProtectionIteratorBase* it ) const
{
    d->openIterators.removeAll( it );
    d->unlock();
}

#include "multicallprotectionmodel.moc"
