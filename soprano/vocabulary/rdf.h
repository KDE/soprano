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

#ifndef _SOPRANO_RDF_H_
#define _SOPRANO_RDF_H_

#include <QtCore/QUrl>
#include "soprano_export.h"

namespace Soprano {
    namespace Vocabulary {
        namespace RDF {
            /**
             * http://www.w3.org/1999/02/22-rdf-syntax-ns#
             */
            SOPRANO_EXPORT QUrl rdfNamespace();

            /**
             * http://www.w3.org/1999/02/22-rdf-syntax-ns#type
             */
            SOPRANO_EXPORT QUrl type();

            /**
             * http://www.w3.org/1999/02/22-rdf-syntax-ns#Property
             */
            SOPRANO_EXPORT QUrl Property();

            /**
             * http://www.w3.org/1999/02/22-rdf-syntax-ns#Statement
             */
            SOPRANO_EXPORT QUrl Statement();

            /**
             * http://www.w3.org/1999/02/22-rdf-syntax-ns#subject
             */
            SOPRANO_EXPORT QUrl subject();

            /**
             * http://www.w3.org/1999/02/22-rdf-syntax-ns#predicate
             */
            SOPRANO_EXPORT QUrl predicate();

            /**
             * http://www.w3.org/1999/02/22-rdf-syntax-ns#object
             */
            SOPRANO_EXPORT QUrl object();

            /**
             * http://www.w3.org/1999/02/22-rdf-syntax-ns#Bag
             */
            SOPRANO_EXPORT QUrl Bag();

            /**
             * http://www.w3.org/1999/02/22-rdf-syntax-ns#Seq
             */
            SOPRANO_EXPORT QUrl Seq();

            /**
             * http://www.w3.org/1999/02/22-rdf-syntax-ns#Alt
             */
            SOPRANO_EXPORT QUrl Alt();

            /**
             * http://www.w3.org/1999/02/22-rdf-syntax-ns#value
             */
            SOPRANO_EXPORT QUrl value();

            /**
             * http://www.w3.org/1999/02/22-rdf-syntax-ns#List
             */
            SOPRANO_EXPORT QUrl List();

            /**
             * http://www.w3.org/1999/02/22-rdf-syntax-ns#nil
             */
            SOPRANO_EXPORT QUrl nil();

            /**
             * http://www.w3.org/1999/02/22-rdf-syntax-ns#first
             */
            SOPRANO_EXPORT QUrl first();

            /**
             * http://www.w3.org/1999/02/22-rdf-syntax-ns#rest
             */
            SOPRANO_EXPORT QUrl rest();

            /**
             * http://www.w3.org/1999/02/22-rdf-syntax-ns#XMLLiteral
             */
            SOPRANO_EXPORT QUrl XMLLiteral();
        }
    }
}

#endif
