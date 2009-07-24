/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2008 Sebastian Trueg <trueg@kde.org>
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

#ifndef _SOPRANO_UTIL_ASYNC_ITERATOR_BACKEND_H_
#define _SOPRANO_UTIL_ASYNC_ITERATOR_BACKEND_H_

#include "iteratorbackend.h"
#include "iterator.h"
#include "asyncmodel_p.h"

#include <QtCore/QQueue>
#include <QtCore/QMutex>
#include <QtCore/QMutexLocker>
#include <QtCore/QWaitCondition>
#include <QtCore/QDebug>


namespace {
    const int s_cacheSize = 10;
}

namespace Soprano {
    namespace Util {
        /**
         * The sole purpose of this class is to provide a single
         * interface to all interators for the AsyncModel
         */
        class AsyncIteratorHandle
        {
        public:
            AsyncIteratorHandle( AsyncModelPrivate* d );
            virtual ~AsyncIteratorHandle();

            /**
             * Simply set m_asyncModelPrivate to 0. used
             * by the model in single thread mode.
             */
            void setModelGone();

        protected:
            /**
             * remove this iterator, ie. close it and remove it from the
             * model in single threaded mode
             */
            void remove();

            AsyncModelPrivate* modelPrivate() const { return m_asyncModelPrivate; }

        private:
            AsyncModelPrivate* m_asyncModelPrivate;
        };


        /**
         * Base class for the iterators which does the communications between the
         * main thread and the work thread to make sure iterators are only used
         * in the thread that created them.
         *
         * For that results are cached in a queue and communicated via the m_current
         * variable.
         */
        template<typename T> class AsyncIteratorBase : public AsyncIteratorHandle
        {
        public:
            AsyncIteratorBase( AsyncModelPrivate* d, const Iterator<T>& it )
                : AsyncIteratorHandle( d ),
                m_iterator( it ),
                m_atEnd(false) {
            }

            virtual ~AsyncIteratorBase() {}

            virtual void initWorkThread() {}

            // called in work thread by the AsyncCommand
            void iterate() {
                //qDebug() << "iterate";
                m_atEnd = false;
                while( !m_atEnd ) {
                    // wait for cache space to become avilable
                    m_mutex.lock();
                    if ( cacheFillState() >= s_cacheSize ) {
                        //qDebug() << "waiting for cache space";
                        m_iterateWc.wait( &m_mutex );
                    }
                    m_mutex.unlock();

                    // check if we have more data and if so, cache it
                    bool haveNext = false;
                    while ( m_iterator.next() ) {
                        QMutexLocker lock( &m_mutex );
                        //qDebug() << "iterate: filling cache";
                        enqueueCurrent();
                        if( m_iterator.lastError() ) {
                            //qDebug() << "iterate: error";
                            m_error = m_iterator.lastError();
                            haveNext = false;
                            break;
                        }
                        if ( cacheFillState() >= s_cacheSize ) {
                            haveNext = true;
                            break;
                        }
                    }

                    QMutexLocker lock( &m_mutex );
                    m_atEnd = !haveNext;

                    // tell the waiting thread that data is available
                    //qDebug() << "iterate: waking consumer";
                    m_nextWc.wakeAll();
                }

                // close the iterator
                m_iterator.close();
            }

        protected:
            // called in main thread by IteratorBackend::next
            bool getNext() {
                //qDebug() << "getNext";
                if( modelPrivate() ) {
                    if( modelPrivate()->mode == AsyncModel::MultiThreaded ) {
                        m_mutex.lock();

                        // wait for data to become available
                        if ( !cacheFillState() && !m_atEnd ) {
                            //qDebug() << "getNext: Waiting for data";
                            m_iterateWc.wakeAll();
                            m_nextWc.wait( &m_mutex );
                        }

                        if( m_error ) {
                            //qDebug() << "getNext: error";
                            m_mutex.unlock();
                            return false;
                        }

                        // get the new data
                        else if ( cacheFillState() != 0 ) {
                            //qDebug() << "getNext: got data";
                            dequeueFirst();
                            m_mutex.unlock();
                            return true;
                        }
                        else {
                            //qDebug() << "getNext: got no data";
                            m_mutex.unlock();
                            return false;
                        }
                    }
                    else {
                        return m_iterator.next();
                    }
                }
                else {
                    return false;
                }
            }

            void closeIterator() {
                //qDebug() << "closeIterator";
                if( modelPrivate() ) {
                    if( modelPrivate()->mode == AsyncModel::MultiThreaded ) {
                        stopIterating();
                    }
                    else {
                        m_iterator.close();
                        remove();
                    }
                }
            }

            // called in the main thread by IteratorBackend::close
            void stopIterating() {
                //qDebug() << "stopIterating";
                QMutexLocker lock( &m_mutex );
                m_atEnd = true;
                m_iterateWc.wakeAll();
            }

            // called in the main thread by IteratorBackend::current
            T getCurrent() const { 
                if( modelPrivate() ) {
                    if( modelPrivate()->mode == AsyncModel::MultiThreaded )
                        return m_current;
                    else
                        return m_iterator.current();
                }
                else {
                    return T();
                }
            }

            virtual int cacheFillState() const {
                return m_cache.size();
            }
            
            virtual void enqueueCurrent() {
                m_cache.enqueue( m_iterator.current() );
            }

            virtual void dequeueFirst() {
                m_current = m_cache.dequeue();
            }

            Iterator<T> m_iterator;
            Error::Error m_error;

        private:
            bool m_atEnd;
            QQueue<T> m_cache;
            T m_current;

            QMutex m_mutex;
            QWaitCondition m_nextWc;
            QWaitCondition m_iterateWc;
        };


        template<typename T> class AsyncIteratorBackend : public AsyncIteratorBase<T>, public IteratorBackend<T>
        {
        public:
            AsyncIteratorBackend( AsyncModelPrivate* d, const Iterator<T>& it )
                : AsyncIteratorBase<T>( d, it ) {
            }

            bool next() {
                return AsyncIteratorBase<T>::getNext();
            }
            
            T current() const {
                return AsyncIteratorBase<T>::getCurrent();
            }
            
            void close() {
                AsyncIteratorBase<T>::closeIterator();
            }

            Error::Error lastError() const {
                if( AsyncIteratorHandle::modelPrivate() && 
                    AsyncIteratorHandle::modelPrivate()->mode == AsyncModel::SingleThreaded )
                    return AsyncIteratorBase<T>::m_iterator.lastError();
                else
                    return AsyncIteratorBase<T>::m_error;
            }
        };
    }
}

#endif
