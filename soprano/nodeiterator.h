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

#ifndef SOPRANO_NODE_ITERATOR_H
#define SOPRANO_NODE_ITERATOR_H

#include "iterator.h"
#include "node.h"
#include "soprano_export.h"

#include <QtCore/QList>


namespace Soprano {

    class NodeIteratorBackend;

    /**
     * \class NodeIterator nodeiterator.h Soprano/NodeIterator
     *
     * \brief An iterator that provides a stream of Nodes.
     *
     * The most common use of %NodeIterator is through Model::listContexts().
     *
     * Many backends do lock the underlying Model during iteration. Thus, 
     * it is always a good idea to cache the results if they are to be used
     * to modify the model to prevent a deadlock:
     *
     * \code
     * Soprano::NodeIterator it = model->listContexts();
     * QList<Node> allNodes = it.allElements();
     * Q_FOREACH( Soprano::Node n, allNodes ) {
     *    modifyTheModel( model, n );
     * }
     * \endcode
     *
     * Iterators have to be closed.
     * This can either be achieved by deleting the iterator, finishing it (next() does return \p false),
     * or calling close(). Before that other operations on the Model may block.
     *
     * Iterators are not thread-safe. Two threads using the same iterator may result
     * in undefined behaviour and even crashes. An iterator needs to be closed by the
     * same thread that opened it (except if the iterator contains special code to handle such
     * a situation.)
     *
     * \warning Be aware that iterators in Soprano are shared objects which means
     * that copies of one iterator object work on the same data.
     *
     * For further details on %Soprano iterators see Iterator.
     * 
     * \author Sebastian Trueg <trueg@kde.org>
     */
    class SOPRANO_EXPORT NodeIterator : public Iterator<Node>
    {
    public:
        /**
         * Creates and empty, invalid iterator.
         */
        NodeIterator();

        /**
         * Create a new NodeIterator instance that uses sti as backend.
         * NodeIterator will take ownership of the backend.
         */
        NodeIterator( IteratorBackend<Node> *sti );

        NodeIterator( const NodeIterator &sti );

        virtual ~NodeIterator();

        NodeIterator& operator=( const NodeIterator& );

        /**
         * Convenience method which extracts all nodes (this does not include the
         * nodes that have already been read from the iterator) from the iterator
         * and returns them in a list.
         *
         * Be aware that after calling this method the iterator will be invalid.
         *
         * \return A list of all nodes that rest in the iterator.
         */
        QList<Node> allNodes() { return allElements(); }
    };
}

#endif // SOPRANO_NODE_ITERATOR_H

