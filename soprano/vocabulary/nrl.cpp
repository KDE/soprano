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

#include "nrl.h"

#define NRL_NS "http://www.semanticdesktop.org/ontologies/2007/08/15/nrl/"

// FIXME: (trueg) do not use all-caps variables

class Nrl
{
public:
    Nrl()
        : nrlNamespace( NRL_NS ),
          MINCARDINALITY( NRL_NS"minCardinality" ),
          MAXCARDINALITY( NRL_NS"maxCardinality" ),
          CARDINALITY( NRL_NS"cardinality" ),
          IMPORTS( NRL_NS"imports" ),
          RULELANGUAGE( NRL_NS"ruleLanguage" ),
          INVERSEFUNCTIONALPROPERTY( NRL_NS"InverseFunctionalProperty" ),
          GRAPH( NRL_NS"Graph" ),
          HASSPECIFICATION( NRL_NS"hasSpecification" ),
          DOCUMENTGRAPH( NRL_NS"DocumentGraph" ),
          COREGRAPHMETADATAFOR( NRL_NS"coreGraphMetadataFor" ),
          TRANSITIVEPROPERTY( NRL_NS"TransitiveProperty" ),
          FUNCTIONALPROPERTY( NRL_NS"FunctionalProperty" ),
          KNOWLEDGEBASE( NRL_NS"KnowledgeBase" ),
          EXTERNALREALIZER( NRL_NS"externalRealizer" ),
          SUBGRAPHOF( NRL_NS"subGraphOf" ),
          CONFIGURATION( NRL_NS"Configuration" ),
          VIEWSPECIFICATION( NRL_NS"ViewSpecification" ),
          HASSEMANTICS( NRL_NS"hasSemantics" ),
          EXTERNALVIEWSPECIFICATION( NRL_NS"ExternalViewSpecification" ),
          DEFAULTGRAPH( NRL_NS"DefaultGraph" ),
          UPDATABLE( NRL_NS"updatable" ),
          SEMANTICS( NRL_NS"Semantics" ),
          EQUIVALENTGRAPH( NRL_NS"equivalentGraph" ),
          REFLEXIVEPROPERTY( NRL_NS"ReflexiveProperty" ),
          ASYMMETRICPROPERTY( NRL_NS"AsymmetricProperty" ),
          VIEWON( NRL_NS"viewOn" ),
          SYMMETRICPROPERTY( NRL_NS"SymmetricProperty" ),
          SUPERGRAPHOF( NRL_NS"superGraphOf" ),
          ONTOLOGY( NRL_NS"Ontology" ),
          REALIZES( NRL_NS"realizes" ),
          RULE( NRL_NS"rule" ),
          INSTANCEBASE( NRL_NS"InstanceBase" ),
          RULEVIEWSPECIFICATION( NRL_NS"RuleViewSpecification" ),
          SEMANTICSDEFINEDBY( NRL_NS"semanticsDefinedBy" ),
          GRAPHMETADATA( NRL_NS"GraphMetadata" ),
          GRAPHMETADATAFOR( NRL_NS"graphMetadataFor" ),
          GRAPHVIEW( NRL_NS"GraphView" ),
          DATA( NRL_NS"Data" ),
          INVERSEPROPERTY( NRL_NS"inverseProperty" ),
          SCHEMA( NRL_NS"Schema" ) {
    }

    QUrl nrlNamespace;
    QUrl MINCARDINALITY;
    QUrl MAXCARDINALITY;
    QUrl CARDINALITY;
    QUrl IMPORTS;
    QUrl RULELANGUAGE;
    QUrl INVERSEFUNCTIONALPROPERTY;
    QUrl GRAPH;
    QUrl HASSPECIFICATION;
    QUrl DOCUMENTGRAPH;
    QUrl COREGRAPHMETADATAFOR;
    QUrl TRANSITIVEPROPERTY;
    QUrl FUNCTIONALPROPERTY;
    QUrl KNOWLEDGEBASE;
    QUrl EXTERNALREALIZER;
    QUrl SUBGRAPHOF;
    QUrl CONFIGURATION;
    QUrl VIEWSPECIFICATION;
    QUrl HASSEMANTICS;
    QUrl EXTERNALVIEWSPECIFICATION;
    QUrl DEFAULTGRAPH;
    QUrl UPDATABLE;
    QUrl SEMANTICS;
    QUrl EQUIVALENTGRAPH;
    QUrl REFLEXIVEPROPERTY;
    QUrl ASYMMETRICPROPERTY;
    QUrl VIEWON;
    QUrl SYMMETRICPROPERTY;
    QUrl SUPERGRAPHOF;
    QUrl ONTOLOGY;
    QUrl REALIZES;
    QUrl RULE;
    QUrl INSTANCEBASE;
    QUrl RULEVIEWSPECIFICATION;
    QUrl SEMANTICSDEFINEDBY;
    QUrl GRAPHMETADATA;
    QUrl GRAPHMETADATAFOR;
    QUrl GRAPHVIEW;
    QUrl DATA;
    QUrl INVERSEPROPERTY;
    QUrl SCHEMA;
};


Q_GLOBAL_STATIC( Nrl, nrl )


QUrl Soprano::Vocabulary::NRL::nrlNamespace()
{
    return nrl()->nrlNamespace;
}


QUrl Soprano::Vocabulary::NRL::minCardinality()
{
    return nrl()->MINCARDINALITY;
}


QUrl Soprano::Vocabulary::NRL::maxCardinality()
{
    return nrl()->MAXCARDINALITY;
}


QUrl Soprano::Vocabulary::NRL::cardinality()
{
    return nrl()->CARDINALITY;
}


QUrl Soprano::Vocabulary::NRL::imports()
{
    return nrl()->IMPORTS;
}


QUrl Soprano::Vocabulary::NRL::ruleLanguage()
{
    return nrl()->RULELANGUAGE;
}


QUrl Soprano::Vocabulary::NRL::InverseFunctionalProperty()
{
    return nrl()->INVERSEFUNCTIONALPROPERTY;
}


QUrl Soprano::Vocabulary::NRL::Graph()
{
    return nrl()->GRAPH;
}


QUrl Soprano::Vocabulary::NRL::hasSpecification()
{
    return nrl()->HASSPECIFICATION;
}


QUrl Soprano::Vocabulary::NRL::DocumentGraph()
{
    return nrl()->DOCUMENTGRAPH;
}


QUrl Soprano::Vocabulary::NRL::coreGraphMetadataFor()
{
    return nrl()->COREGRAPHMETADATAFOR;
}


QUrl Soprano::Vocabulary::NRL::TransitiveProperty()
{
    return nrl()->TRANSITIVEPROPERTY;
}


QUrl Soprano::Vocabulary::NRL::FunctionalProperty()
{
    return nrl()->FUNCTIONALPROPERTY;
}


QUrl Soprano::Vocabulary::NRL::KnowledgeBase()
{
    return nrl()->KNOWLEDGEBASE;
}


QUrl Soprano::Vocabulary::NRL::externalRealizer()
{
    return nrl()->EXTERNALREALIZER;
}


QUrl Soprano::Vocabulary::NRL::subGraphOf()
{
    return nrl()->SUBGRAPHOF;
}


QUrl Soprano::Vocabulary::NRL::Configuration()
{
    return nrl()->CONFIGURATION;
}


QUrl Soprano::Vocabulary::NRL::ViewSpecification()
{
    return nrl()->VIEWSPECIFICATION;
}


QUrl Soprano::Vocabulary::NRL::hasSemantics()
{
    return nrl()->HASSEMANTICS;
}


QUrl Soprano::Vocabulary::NRL::ExternalViewSpecification()
{
    return nrl()->EXTERNALVIEWSPECIFICATION;
}


QUrl Soprano::Vocabulary::NRL::DefaultGraph()
{
    return nrl()->DEFAULTGRAPH;
}


QUrl Soprano::Vocabulary::NRL::updatable()
{
    return nrl()->UPDATABLE;
}


QUrl Soprano::Vocabulary::NRL::Semantics()
{
    return nrl()->SEMANTICS;
}


QUrl Soprano::Vocabulary::NRL::equivalentGraph()
{
    return nrl()->EQUIVALENTGRAPH;
}


QUrl Soprano::Vocabulary::NRL::ReflexiveProperty()
{
    return nrl()->REFLEXIVEPROPERTY;
}


QUrl Soprano::Vocabulary::NRL::AsymmetricProperty()
{
    return nrl()->ASYMMETRICPROPERTY;
}


QUrl Soprano::Vocabulary::NRL::viewOn()
{
    return nrl()->VIEWON;
}


QUrl Soprano::Vocabulary::NRL::SymmetricProperty()
{
    return nrl()->SYMMETRICPROPERTY;
}


QUrl Soprano::Vocabulary::NRL::superGraphOf()
{
    return nrl()->SUPERGRAPHOF;
}


QUrl Soprano::Vocabulary::NRL::Ontology()
{
    return nrl()->ONTOLOGY;
}


QUrl Soprano::Vocabulary::NRL::realizes()
{
    return nrl()->REALIZES;
}


QUrl Soprano::Vocabulary::NRL::rule()
{
    return nrl()->RULE;
}


QUrl Soprano::Vocabulary::NRL::InstanceBase()
{
    return nrl()->INSTANCEBASE;
}


QUrl Soprano::Vocabulary::NRL::RuleViewSpecification()
{
    return nrl()->RULEVIEWSPECIFICATION;
}


QUrl Soprano::Vocabulary::NRL::semanticsDefinedBy()
{
    return nrl()->SEMANTICSDEFINEDBY;
}


QUrl Soprano::Vocabulary::NRL::GraphMetadata()
{
    return nrl()->GRAPHMETADATA;
}


QUrl Soprano::Vocabulary::NRL::graphMetadataFor()
{
    return nrl()->GRAPHMETADATAFOR;
}


QUrl Soprano::Vocabulary::NRL::GraphView()
{
    return nrl()->GRAPHVIEW;
}


QUrl Soprano::Vocabulary::NRL::Data()
{
    return nrl()->DATA;
}


QUrl Soprano::Vocabulary::NRL::inverseProperty()
{
    return nrl()->INVERSEPROPERTY;
}


QUrl Soprano::Vocabulary::NRL::Schema()
{
    return nrl()->SCHEMA;
}
