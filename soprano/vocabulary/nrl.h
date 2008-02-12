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

#ifndef _SOPRANO_NRL_H_
#define _SOPRANO_NRL_H_

#include <QtCore/QUrl>
#include "soprano_export.h"

namespace Soprano {
    namespace Vocabulary {
        namespace NRL {
            /**
             * http://semanticdesktop.org/ontologies/2007/08/15#
             */
            SOPRANO_EXPORT QUrl nrlNamespace();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#AsymmetricProperty 
             * 
             * A marker class to identify asymmetric properties 
             */
            SOPRANO_EXPORT QUrl AsymmetricProperty();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#Configuration 
             * 
             * Represents a named graph containing configuration data 
             */
            SOPRANO_EXPORT QUrl Configuration();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#Data 
             * 
             * An abstract class representing all named graph roles 
             */
            SOPRANO_EXPORT QUrl Data();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#DefaultGraph 
             * 
             * Represents the default graph, the graph which contains any 
             * triple that does not belong to any other named graph 
             */
            SOPRANO_EXPORT QUrl DefaultGraph();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#DocumentGraph 
             * 
             * A marker class to identify named graphs that exist within a physical 
             * document 
             */
            SOPRANO_EXPORT QUrl DocumentGraph();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#ExternalViewSpecification 
             * 
             * Represents an external view specification, this usually being 
             * a program which automatically generates the required view 
             * for an input graph 
             */
            SOPRANO_EXPORT QUrl ExternalViewSpecification();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#FunctionalProperty 
             * 
             * A marker class to identify functional properties 
             */
            SOPRANO_EXPORT QUrl FunctionalProperty();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#Graph 
             * 
             * Represents a named graph 
             */
            SOPRANO_EXPORT QUrl Graph();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#GraphMetadata 
             * 
             * Represents a special named graph that contains metadata for 
             * another graph 
             */
            SOPRANO_EXPORT QUrl GraphMetadata();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#GraphView 
             * 
             * Identifies a graph which is itself a view of another named graph 
             */
            SOPRANO_EXPORT QUrl GraphView();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#InstanceBase 
             * 
             * Represents a named graph containing instance data 
             */
            SOPRANO_EXPORT QUrl InstanceBase();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#InverseFunctionalProperty 
             * 
             * A marker class to identify inverse functional properties 
             */
            SOPRANO_EXPORT QUrl InverseFunctionalProperty();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#KnowledgeBase 
             * 
             * Represents a named graph containing both schematic and instance 
             * data 
             */
            SOPRANO_EXPORT QUrl KnowledgeBase();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#Ontology 
             * 
             * Represents a named graph having the role of an Ontology 
             */
            SOPRANO_EXPORT QUrl Ontology();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#ReflexiveProperty 
             * 
             * A marker class to identify reflexive properties 
             */
            SOPRANO_EXPORT QUrl ReflexiveProperty();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#RuleViewSpecification 
             * 
             * Represents a view specification that is composed of a set of 
             * rules which generate the required view from the input graph 
             * upon firing 
             */
            SOPRANO_EXPORT QUrl RuleViewSpecification();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#Schema 
             * 
             * Represents a named graph containing schematic data 
             */
            SOPRANO_EXPORT QUrl Schema();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#Semantics 
             * 
             * Represents some declarative semantics 
             */
            SOPRANO_EXPORT QUrl Semantics();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#SymmetricProperty 
             * 
             * A marker class to identify symmetric properties 
             */
            SOPRANO_EXPORT QUrl SymmetricProperty();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#TransitiveProperty 
             * 
             * A marker class to identify transitive properties 
             */
            SOPRANO_EXPORT QUrl TransitiveProperty();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#ViewSpecification 
             * 
             * Represents a specification of the means to achieve a transformation 
             * of an input graph into the required graph view 
             */
            SOPRANO_EXPORT QUrl ViewSpecification();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#cardinality 
             * 
             * Specifies the precise value cardinality for a specific property 
             */
            SOPRANO_EXPORT QUrl cardinality();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#coreGraphMetadataFor 
             * 
             * Links a metadata graph to the graph for which it specifies the 
             * core graph properties including the semantics and the graph 
             * namespace. A graph can have only one unique core metadata graph 
             */
            SOPRANO_EXPORT QUrl coreGraphMetadataFor();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#equivalentGraph 
             * 
             * Links two equivalent named graphs. A symmetric property 
             */
            SOPRANO_EXPORT QUrl equivalentGraph();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#externalRealizer 
             * 
             * Points to the location of the realizer for the external view 
             * specification 
             */
            SOPRANO_EXPORT QUrl externalRealizer();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#graphMetadataFor 
             * 
             * Links a metadata graph to the graph that is being described. 
             * A unique value is compulsory 
             */
            SOPRANO_EXPORT QUrl graphMetadataFor();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#hasSemantics 
             * 
             * Points to a representation of the declarative semantics for 
             * a graph role 
             */
            SOPRANO_EXPORT QUrl hasSemantics();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#hasSpecification 
             * 
             * Points to the representation of the view specification required 
             * to generate the graph view in question 
             */
            SOPRANO_EXPORT QUrl hasSpecification();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#imports 
             * 
             * Models a subsumption relationship between two graphs, stating 
             * that the object graph is imported and included in the subject 
             * graph 
             */
            SOPRANO_EXPORT QUrl imports();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#inverseProperty 
             * 
             * Links two properties and specifies their inverse behaviour 
             */
            SOPRANO_EXPORT QUrl inverseProperty();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#maxCardinality 
             * 
             * Specifies a maximum value cardinality for a specific property 
             */
            SOPRANO_EXPORT QUrl maxCardinality();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#minCardinality 
             * 
             * Specifies a minimum value cardinality for a specific property 
             */
            SOPRANO_EXPORT QUrl minCardinality();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#realizes 
             * 
             * Points to a representation of the declarative semantics that 
             * the view specification realizes 
             */
            SOPRANO_EXPORT QUrl realizes();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#rule 
             * 
             * Specifies rules for a view specification that is driven by rules 
             */
            SOPRANO_EXPORT QUrl rule();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#ruleLanguage 
             * 
             * Specifies the rule language for a view specification that is 
             * driven by rules 
             */
            SOPRANO_EXPORT QUrl ruleLanguage();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#semanticsDefinedBy 
             * 
             * Points to the human readable specifications for a representation 
             * of some declarative semantics 
             */
            SOPRANO_EXPORT QUrl semanticsDefinedBy();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#subGraphOf 
             * 
             * Specifies a containment relationship between two graphs, 
             * meaning that the subject graph is included in the object graph 
             */
            SOPRANO_EXPORT QUrl subGraphOf();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#superGraphOf 
             * 
             * Specifies a subsumption relationship between two graphs, 
             * meaning that the object graph is included in the subject graph 
             */
            SOPRANO_EXPORT QUrl superGraphOf();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#updatable 
             * 
             * A core graph metadata property, this defines whether a graph 
             * can be freely updated '1' or otherwise '0' 
             */
            SOPRANO_EXPORT QUrl updatable();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#viewOn 
             * 
             * Points to a graph view over the subject named graph 
             */
            SOPRANO_EXPORT QUrl viewOn();
        }
    }
}

#endif
