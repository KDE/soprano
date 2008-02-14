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

#include "mutexmodel.h"
#include "mutexiteratorbase.h"
#include "looplock.h"
#include "extreadwritelock.h"
#include "mutexstatementiteratorbackend.h"
#include "mutexnodeiteratorbackend.h"
#include "mutexqueryresultiteratorbackend.h"

#include "statementiterator.h"
#include "nodeiterator.h"
#include "queryresultiterator.h"

#include <QtCore/QMutex>


class Soprano::Util::MutexModel::Private
{
public:
    Private( MutexModel::ProtectionMode mode )
        : m_protectionMode( mode ) {
    }

    QList<MutexIteratorBase*> openIterators;

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
    ExtReadWriteLock m_msLock;
    QMutex m_mutex;

    ProtectionMode m_protectionMode;
};


Soprano::Util::MutexModel::MutexModel( ProtectionMode mode, Model* parent )
    : FilterModel( parent ),
      d( new Private( mode ) )
{
}


Soprano::Util::MutexModel::~MutexModel()
{
    Q_FOREACH( MutexIteratorBase* it, d->openIterators ) {
        it->setModelGone();
    }

    delete d;
}


Soprano::Error::ErrorCode Soprano::Util::MutexModel::addStatement( const Statement &statement )
{
    d->lockForWrite();
    Error::ErrorCode c = FilterModel::addStatement( statement );
    d->unlock();
    return c;
}


Soprano::Error::ErrorCode Soprano::Util::MutexModel::removeStatement( const Statement &statement )
{
    d->lockForWrite();
    Error::ErrorCode c = FilterModel::removeStatement( statement );
    d->unlock();
    return c;
}


Soprano::Error::ErrorCode Soprano::Util::MutexModel::removeAllStatements( const Statement &statement )
{
    d->lockForWrite();
    Error::ErrorCode c = FilterModel::removeAllStatements( statement );
    d->unlock();
    return c;
}


Soprano::StatementIterator Soprano::Util::MutexModel::listStatements( const Statement &partial ) const
{
    d->lockForRead();
    StatementIterator it = FilterModel::listStatements( partial );
    if ( it.isValid() ) {
        MutexStatementIteratorBackend* b = new MutexStatementIteratorBackend( it, const_cast<MutexModel*>( this ) );
        d->openIterators.append( b );
        return b;
    }
    else {
        d->unlock();
        return it;
    }
}


Soprano::NodeIterator Soprano::Util::MutexModel::listContexts() const
{
    d->lockForRead();
    NodeIterator it = FilterModel::listContexts();
    if ( it.isValid() ) {
        MutexNodeIteratorBackend* b = new MutexNodeIteratorBackend( it, const_cast<MutexModel*>( this ) );
        d->openIterators.append( b );
        return b;
    }
    else {
        d->unlock();
        return it;
    }
}


Soprano::QueryResultIterator Soprano::Util::MutexModel::executeQuery( const QString& query,
                                                                      Query::QueryLanguage language,
                                                                      const QString& userQueryLanguage ) const
{
    d->lockForRead();
    QueryResultIterator it = FilterModel::executeQuery( query, language, userQueryLanguage );
    if ( it.isValid() ) {
        MutexQueryResultIteratorBackend* b = new MutexQueryResultIteratorBackend( it, const_cast<MutexModel*>( this ) );
        d->openIterators.append( b );
        return b;
    }
    else {
        d->unlock();
        return it;
    }
}


bool Soprano::Util::MutexModel::containsStatement( const Statement &statement ) const
{
    d->lockForRead();
    bool b = FilterModel::containsStatement( statement );
    d->unlock();
    return b;
}


bool Soprano::Util::MutexModel::containsAnyStatement( const Statement &statement ) const
{
    d->lockForRead();
    bool b = FilterModel::containsAnyStatement( statement );
    d->unlock();
    return b;
}


bool Soprano::Util::MutexModel::isEmpty() const
{
    d->lockForRead();
    bool b = FilterModel::isEmpty();
    d->unlock();
    return b;
}


int Soprano::Util::MutexModel::statementCount() const
{
    d->lockForRead();
    int cnt = FilterModel::statementCount();
    d->unlock();
    return cnt;
}


void Soprano::Util::MutexModel::removeIterator( MutexIteratorBase* it ) const
{
    d->openIterators.removeAll( it );
    d->unlock();
}

#include "mutexmodel.moc"
