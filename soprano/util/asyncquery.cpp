/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2009 Sebastian Trueg <trueg@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "asyncquery.h"
#include "statement.h"
#include "node.h"
#include "queryresultiterator.h"
#include "model.h"
#include "bindingset.h"

#include <QtCore/QThread>
#include <QtCore/QMutex>
#include <QtCore/QMutexLocker>
#include <QtCore/QWaitCondition>
#include <QtCore/QDebug>


namespace {
    enum ResultType {
        UnknownResult = 0,
        GraphResult,
        BindingResult,
        BooleanResult
    };
}

class Soprano::Util::AsyncQuery::Private : public QThread
{
public:
    Private()
        : m_closed(false),
          m_boolValue(false),
          m_type(UnknownResult) {
    }

    void run();
    void _s_finished();
    void _s_emitNextReady();

    Soprano::Model* m_model;
    QString m_query;
    Query::QueryLanguage m_queryLang;
    QString m_userQueryLang;

    QMutex m_closeMutex;
    QMutex m_mutex;
    QWaitCondition m_nextWaiter;

    bool m_closed;

    Statement m_currentStatement;
    BindingSet m_currentBindings;
    bool m_boolValue;
    QStringList m_bindingNames;
    ResultType m_type;

    AsyncQuery* q;
};


void Soprano::Util::AsyncQuery::Private::run()
{
    QueryResultIterator it = m_model->executeQuery( m_query, m_queryLang, m_userQueryLang );
    if( it.isValid() ) {
        // cache the type
        if( it.isGraph() )
            m_type = GraphResult;
        else if( it.isBinding() )
            m_type = BindingResult;
        else {
            m_type = BooleanResult;
            m_boolValue = it.boolValue();
        }

        if( m_type != BooleanResult ) {
            while ( !m_closed ) {
                bool nextReady = it.next();
                if( nextReady && !m_closed ) {
                    // lock the mutex before emitting to ensure consistency
                    m_mutex.lock();

                    // cache the next result
                    if( m_type == GraphResult )
                        m_currentStatement = it.currentStatement();
                    else
                        m_currentBindings = it.currentBindings();

                    // inform the client
                    QMetaObject::invokeMethod( q, "_s_emitNextReady", Qt::QueuedConnection );

                    // wait for the call to next()
                    m_nextWaiter.wait( &m_mutex );
                    m_mutex.unlock();
                }
                else {
                    // we are done, error is set below
                    break;
                }
            }
        }
    }

    // finished, set the error, finished will be emitted in _s_finished
    q->setError( m_model->lastError() );
}


void Soprano::Util::AsyncQuery::Private::_s_finished()
{
    emit q->finished( q );
    q->deleteLater();
}


void Soprano::Util::AsyncQuery::Private::_s_emitNextReady()
{
    emit q->nextReady( q );
}


Soprano::Util::AsyncQuery::AsyncQuery()
    : QObject(),
      d(new Private())
{
    d->q = this;
    connect( d, SIGNAL(finished()), this, SLOT(_s_finished()) );
}


Soprano::Util::AsyncQuery::~AsyncQuery()
{
    close();
    d->wait();
    delete d;
}


Soprano::Statement Soprano::Util::AsyncQuery::currentStatement() const
{
    QMutexLocker lock( &d->m_mutex );
    return d->m_currentStatement;
}


Soprano::BindingSet Soprano::Util::AsyncQuery::currentBindings() const
{
    QMutexLocker lock( &d->m_mutex );
    return d->m_currentBindings;
}


bool Soprano::Util::AsyncQuery::boolValue() const
{
    QMutexLocker lock( &d->m_mutex );
    return d->m_boolValue;
}


Soprano::Node Soprano::Util::AsyncQuery::binding( const QString& name ) const
{
    QMutexLocker lock( &d->m_mutex );
    return d->m_currentBindings[name];
}


Soprano::Node Soprano::Util::AsyncQuery::binding( int offset ) const
{
    QMutexLocker lock( &d->m_mutex );
    return d->m_currentBindings[offset];
}


int Soprano::Util::AsyncQuery::bindingCount() const
{
    QMutexLocker lock( &d->m_mutex );
    return d->m_bindingNames.count();
}


QStringList Soprano::Util::AsyncQuery::bindingNames() const
{
    QMutexLocker lock( &d->m_mutex );
    return d->m_bindingNames;
}


bool Soprano::Util::AsyncQuery::isGraph() const
{
    QMutexLocker lock( &d->m_mutex );
    return d->m_type == GraphResult;
}


bool Soprano::Util::AsyncQuery::isBinding() const
{
    QMutexLocker lock( &d->m_mutex );
    return d->m_type == BindingResult;
}


bool Soprano::Util::AsyncQuery::isBool() const
{
    QMutexLocker lock( &d->m_mutex );
    return d->m_type == BooleanResult;
}


bool Soprano::Util::AsyncQuery::next()
{
    if( d->isRunning() ) {
        d->m_mutex.lock();
        d->m_nextWaiter.wakeAll();
        d->m_mutex.unlock();
        return true;
    }
    else {
        // the thread is still working
        return false;
    }
}


void Soprano::Util::AsyncQuery::close()
{
    QMutexLocker lock( &d->m_mutex );
    d->m_closed = true;
    d->m_nextWaiter.wakeAll();
}


// static
Soprano::Util::AsyncQuery* Soprano::Util::AsyncQuery::executeQuery( Soprano::Model* model,
                                                                    const QString& query,
                                                                    Query::QueryLanguage language,
                                                                    const QString& userQueryLanguage )
{
    if( model ) {
        AsyncQuery* q = new AsyncQuery();
        q->d->m_model = model;
        q->d->m_query = query;
        q->d->m_queryLang = language;
        q->d->m_userQueryLang = userQueryLanguage;
        q->d->start();
        return q;
    }
    else {
        return 0;
    }
}

#include "asyncquery.moc"
