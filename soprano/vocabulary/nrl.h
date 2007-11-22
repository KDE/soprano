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
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#
             */
            SOPRANO_EXPORT QUrl nrlNamespace();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#imports
             */
            SOPRANO_EXPORT QUrl imports();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#ruleLanguage
             */
            SOPRANO_EXPORT QUrl ruleLanguage();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#cardinality
             */
            SOPRANO_EXPORT QUrl cardinality();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#InverseFunctionalProperty
             */
            SOPRANO_EXPORT QUrl InverseFunctionalProperty();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#Graph
             */
            SOPRANO_EXPORT QUrl Graph();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#hasSpecification
             */
            SOPRANO_EXPORT QUrl hasSpecification();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#DocumentGraph
             */
            SOPRANO_EXPORT QUrl DocumentGraph();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#coreGraphMetadataFor
             */
            SOPRANO_EXPORT QUrl coreGraphMetadataFor();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#TransitiveProperty
             */
            SOPRANO_EXPORT QUrl TransitiveProperty();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#FunctionalProperty
             */
            SOPRANO_EXPORT QUrl FunctionalProperty();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#KnowledgeBase
             */
            SOPRANO_EXPORT QUrl KnowledgeBase();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#externalRealizer
             */
            SOPRANO_EXPORT QUrl externalRealizer();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#subGraphOf
             */
            SOPRANO_EXPORT QUrl subGraphOf();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#Configuration
             */
            SOPRANO_EXPORT QUrl Configuration();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#ViewSpecification
             */
            SOPRANO_EXPORT QUrl ViewSpecification();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#hasSemantics
             */
            SOPRANO_EXPORT QUrl hasSemantics();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#ExternalViewSpecification
             */
            SOPRANO_EXPORT QUrl ExternalViewSpecification();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#DefaultGraph
             */
            SOPRANO_EXPORT QUrl DefaultGraph();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#updatable
             */
            SOPRANO_EXPORT QUrl updatable();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#Semantics
             */
            SOPRANO_EXPORT QUrl Semantics();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#equivalentGraph
             */
            SOPRANO_EXPORT QUrl equivalentGraph();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#ReflexiveProperty
             */
            SOPRANO_EXPORT QUrl ReflexiveProperty();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#AsymmetricProperty
             */
            SOPRANO_EXPORT QUrl AsymmetricProperty();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#maxCardinality
             */
            SOPRANO_EXPORT QUrl maxCardinality();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#viewOn
             */
            SOPRANO_EXPORT QUrl viewOn();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#SymmetricProperty
             */
            SOPRANO_EXPORT QUrl SymmetricProperty();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#superGraphOf
             */
            SOPRANO_EXPORT QUrl superGraphOf();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#Ontology
             */
            SOPRANO_EXPORT QUrl Ontology();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#realizes
             */
            SOPRANO_EXPORT QUrl realizes();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#minCardinality
             */
            SOPRANO_EXPORT QUrl minCardinality();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#rule
             */
            SOPRANO_EXPORT QUrl rule();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#InstanceBase
             */
            SOPRANO_EXPORT QUrl InstanceBase();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#RuleViewSpecification
             */
            SOPRANO_EXPORT QUrl RuleViewSpecification();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#semanticsDefinedBy
             */
            SOPRANO_EXPORT QUrl semanticsDefinedBy();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#GraphMetadata
             */
            SOPRANO_EXPORT QUrl GraphMetadata();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#graphMetadataFor
             */
            SOPRANO_EXPORT QUrl graphMetadataFor();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#GraphView
             */
            SOPRANO_EXPORT QUrl GraphView();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#Data
             */
            SOPRANO_EXPORT QUrl Data();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#inverseProperty
             */
            SOPRANO_EXPORT QUrl inverseProperty();

            /**
             * http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#Schema
             */
            SOPRANO_EXPORT QUrl Schema();
        }
    }
}

#endif
