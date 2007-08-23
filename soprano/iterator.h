/*
 * This file is part of Soprano Project.
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

#ifndef SOPRANO_ITERATOR_H
#define SOPRANO_ITERATOR_H

#include <QtCore/QSharedDataPointer>
#include <QtCore/QList>

#include "soprano_export.h"
#include "iteratorbackend.h"

namespace Soprano {

    /**
     * \brief The basic %Soprano iterator class.
     *
     * Iteartors in %Soprano are very easy to use through two methods
     * next() and current(). Instead of the latter operator*() can also be used.
     * Both can be called subsequetially to retrieve the current element
     * until next() has been called again.
     *
     * \code
     * while( it.next() ) {
     *    doSomething( *it );
     *    doSomethingElse( it.current() );
     * }
     * \endcode
     *
     * Backends such as redland tend to invalidate the iterators if
     * the underlaying model is changed. Thus, it is always a good idea to cache
     * the results if they are to be used to modify the model:
     *
     * \code
     * StatementIterator it = model->listStatements();
     * QList<Statement> allStatements = it.allStatements();
     * Q_FOREACH( Statement s, allStatements ) {
     *    modifyTheModel( model, s );
     * }
     * \endcode
     *
     * Iterators in %Soprano may lock the underlying Model. Thus, they have to be closed.
     * This can either be achieved by deleting the iterator, finishing it (next() does return \p false),
     * or calling close(). Before that other operations on the Model may block.
     *
     * \warning Be aware that iterators in Soprano are shared objects which means
     * that copies of one iterator object work on the same data.
     * 
     * \author Daniele Galdi <daniele.galdi@gmail.com><br>Sebastian Trueg <trueg@kde.org>
     */
    template<typename T> class SOPRANO_EXPORT Iterator
    {
    public:
	/**
	 * Creates and empty, invalid iterator.
	 */
	Iterator();

	/**
	 * Create a new Iterator instance that uses sti as backend.
	 * Iterator will take ownership of the backend.
	 */
	Iterator( IteratorBackend<T> *sti );

	Iterator( const Iterator &sti );

	virtual ~Iterator();

	Iterator& operator=( const Iterator& );

	/**
	 * Close the iterator and release any locks on the underlying Model.
	 */
	void close();

	/**
	 * Advances to the next statement in the iterator.
	 *\return true if another Statement can be read from the iterator,
	 * false if the end has been reached.
	 */
	bool next();

	/**
	 *\return the current element, this is not valid until after
	 * the first call to next.
	 */
	T current() const;

	/**
	 * Retrieve the current element in the iterator.
	 *
	 * This is equivalent to current().
	 *
	 * \return The element the iterator currently points to or
	 * an invalid one if next has never been called.
	 */
	T operator*() const;

	/**
	 * \return \p true if the Iterator is valid, \p false otherwise, i.e.
	 * it has no backend .
	 */
	bool isValid() const;

	/**
	 * Convinience method which extracts all elements (this does not include the
	 * elements that have already been read from the iterator) from the iterator
	 * and returns them in a list.
	 *
	 * Be aware that after calling this method the iterator will be invalid.
	 *
	 * \return A list of all elements that rest in the iterator.
	 */
	QList<T> allElements();

    protected:
	/**
	 * Set the backend to read the actual data from.
	 * A previous backend will be deleted if there are no other StatementIterator
	 * instances using it.
	 */
	void setBackend( IteratorBackend<T>* b );

	IteratorBackend<T>* backend() const;

    private:
	class Private;
	QSharedDataPointer<Private> d;
    };
}


template<typename T> class Soprano::Iterator<T>::Private : public QSharedData
{
public:
    Private()
        : backend( 0 ) {
    }

    ~Private() {
        delete backend;
    }

    IteratorBackend<T>* backend;
};


template<typename T> Soprano::Iterator<T>::Iterator()
    : d( new Private() )
{
}

template<typename T> Soprano::Iterator<T>::Iterator( IteratorBackend<T> *sti )
    : d( new Private() )
{
    d->backend = sti;
}

template<typename T> Soprano::Iterator<T>::Iterator( const Iterator<T> &other )
    : d( other.d )
{
}

template<typename T> Soprano::Iterator<T>::~Iterator()
{
}

template<typename T> Soprano::Iterator<T>& Soprano::Iterator<T>::operator=( const Iterator<T>& other )
{
    d = other.d;
    return *this;
}

template<typename T> void Soprano::Iterator<T>::setBackend( IteratorBackend<T>* b )
{
    if ( d->backend != b ) {
        // now we want it to detach
        d->backend = b;
    }
}

template<typename T> Soprano::IteratorBackend<T>* Soprano::Iterator<T>::backend() const
{
    return d->backend;
}

template<typename T> void Soprano::Iterator<T>::close()
{
    // some evil hacking to avoid detachment of the shared data
    const Private* cd = d.constData();
    cd->backend->close();
}

template<typename T> bool Soprano::Iterator<T>::next()
{
    // some evil hacking to avoid detachment of the shared data
    const Private* cd = d.constData();
    return isValid() ? cd->backend->next() : false;
}

template<typename T> T Soprano::Iterator<T>::current() const
{
    return isValid() ? d->backend->current() : T();
}

template<typename T> T Soprano::Iterator<T>::operator*() const
{
    return current();
}

template<typename T> bool Soprano::Iterator<T>::isValid() const
{
    return d->backend != 0;
}


template<typename T> QList<T> Soprano::Iterator<T>::allElements()
{
    QList<T> sl;
    while ( next() ) {
        sl.append( current() );
    }
    return sl;
}

#endif

