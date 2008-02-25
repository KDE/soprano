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

#ifndef _SOPRANO_RDFS_H_
#define _SOPRANO_RDFS_H_

#include <QtCore/QUrl>
#include "soprano_export.h"

namespace Soprano {
    namespace Vocabulary {
        namespace RDFS {
            /**
             * http://www.w3.org/2000/01/rdf-schema#
             */
            SOPRANO_EXPORT QUrl rdfsNamespace();

            /**
             * http://www.w3.org/2000/01/rdf-schema#Class 
             * 
             * The class of classes. 
             */
            SOPRANO_EXPORT QUrl Class();

            /**
             * http://www.w3.org/2000/01/rdf-schema#Container 
             * 
             * The class of RDF containers. 
             */
            SOPRANO_EXPORT QUrl Container();

            /**
             * http://www.w3.org/2000/01/rdf-schema#ContainerMembershipProperty 
             * 
             * The class of container membership properties, rdf:_1, rdf:_2, 
             * ..., 
             * all of which are sub-properties of 'member'. 
             */
            SOPRANO_EXPORT QUrl ContainerMembershipProperty();

            /**
             * http://www.w3.org/2000/01/rdf-schema#Datatype 
             * 
             * The class of RDF datatypes. 
             */
            SOPRANO_EXPORT QUrl Datatype();

            /**
             * http://www.w3.org/2000/01/rdf-schema#Literal 
             * 
             * The class of literal values, eg. textual strings and integers. 
             */
            SOPRANO_EXPORT QUrl Literal();

            /**
             * http://www.w3.org/2000/01/rdf-schema#Resource 
             * 
             * The class resource, everything. 
             */
            SOPRANO_EXPORT QUrl Resource();

            /**
             * http://www.w3.org/2000/01/rdf-schema#comment 
             * 
             * A description of the subject resource. 
             */
            SOPRANO_EXPORT QUrl comment();

            /**
             * http://www.w3.org/2000/01/rdf-schema#domain 
             * 
             * A domain of the subject property. 
             */
            SOPRANO_EXPORT QUrl domain();

            /**
             * http://www.w3.org/2000/01/rdf-schema#isDefinedBy 
             * 
             * The defininition of the subject resource. 
             */
            SOPRANO_EXPORT QUrl isDefinedBy();

            /**
             * http://www.w3.org/2000/01/rdf-schema#label 
             * 
             * A human-readable name for the subject. 
             */
            SOPRANO_EXPORT QUrl label();

            /**
             * http://www.w3.org/2000/01/rdf-schema#member 
             * 
             * A member of the subject resource. 
             */
            SOPRANO_EXPORT QUrl member();

            /**
             * http://www.w3.org/2000/01/rdf-schema#range 
             * 
             * A range of the subject property. 
             */
            SOPRANO_EXPORT QUrl range();

            /**
             * http://www.w3.org/2000/01/rdf-schema#seeAlso 
             * 
             * Further information about the subject resource. 
             */
            SOPRANO_EXPORT QUrl seeAlso();

            /**
             * http://www.w3.org/2000/01/rdf-schema#subClassOf 
             * 
             * The subject is a subclass of a class. 
             */
            SOPRANO_EXPORT QUrl subClassOf();

            /**
             * http://www.w3.org/2000/01/rdf-schema#subPropertyOf 
             * 
             * The subject is a subproperty of a property. 
             */
            SOPRANO_EXPORT QUrl subPropertyOf();
        }
    }
}

#endif
