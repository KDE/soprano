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

// FIXME: add all the missing URIs
// FIXME: should we put the different namespaces into differnt fiels like rdf.h, rdfs.h, and so on?
namespace Soprano {
    namespace Vocabulary {
	namespace RDF {
	    /**
	     * The RDF namespace (http://www.w3.org/1999/02/22-rdf-syntax-ns#)
	     */
	    const char* NAMESPACE = "http://www.w3.org/1999/02/22-rdf-syntax-ns#";
            static const char* TYPE = "http://www.w3.org/1999/02/22-rdf-syntax-ns#type";
            static const char* PROPERTY = "http://www.w3.org/1999/02/22-rdf-syntax-ns#Property";
	}

	namespace RDFS {
	    const char* NAMESPACE = "http://www.w3.org/2000/01/rdf-schema#";
	    const char* CLASS = "http://www.w3.org/2000/01/rdf-schema#Class";
	    const char* COMMENT = "http://www.w3.org/2000/01/rdf-schema#comment";
	    const char* DATATYPE = "http://www.w3.org/2000/01/rdf-schema#Datatype";
	    const char* DOMAIN = "http://www.w3.org/2000/01/rdf-schema#domain";
	    const char* RANGE = "http://www.w3.org/2000/01/rdf-schema#range";
	    const char* LABEL = "http://www.w3.org/2000/01/rdf-schema#label";
	    const char* SUBCLASSOF = "http://www.w3.org/2000/01/rdf-schema#subClassOf";
	    const char* SUBPROPERTYOF = "http://www.w3.org/2000/01/rdf-schema#subPropertyOf";
	    const char* SEEALSO = "http://www.w3.org/2000/01/rdf-schema#seeAlso";
	    const char* RESOURCE = "http://www.w3.org/2000/01/rdf-schema#Resource";
	    const char* MEMBER = "http://www.w3.org/2000/01/rdf-schema#member";
	    const char* LITERAL = "http://www.w3.org/2000/01/rdf-schema#Literal";
	    const char* ISDEFINEDBY = "http://www.w3.org/2000/01/rdf-schema#isDefinedBy";
	    const char* CONTAINER = "http://www.w3.org/2000/01/rdf-schema#Container";
	    const char* CONTAINERMEMBERSHIPPROPERTY = "http://www.w3.org/2000/01/rdf-schema#ContainerMembershipProperty";
	}

	namespace XMLSchema {
            static const char* NAMESPACE = "http://www.w3.org/2001/XMLSchema#";
            static const char* INT = "http://www.w3.org/2001/XMLSchema#int";
            static const char* LONG = "http://www.w3.org/2001/XMLSchema#long";
            static const char* STRING = "http://www.w3.org/2001/XMLSchema#string";
            static const char* DOUBLE = "http://www.w3.org/2001/XMLSchema#double";
            static const char* BOOLEAN = "http://www.w3.org/2001/XMLSchema#boolean";
            static const char* DATE = "http://www.w3.org/2001/XMLSchema#date";
            static const char* DATETIME = "http://www.w3.org/2001/XMLSchema#dateTime";
        }
    }
}

#endif
