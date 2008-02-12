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

class NrlPrivate
{
public:
    NrlPrivate()
        : nrl_namespace( "http://semanticdesktop.org/ontologies/2007/08/15#" ),
          nrl_AsymmetricProperty( "http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#AsymmetricProperty" ),
          nrl_Configuration( "http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#Configuration" ),
          nrl_Data( "http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#Data" ),
          nrl_DefaultGraph( "http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#DefaultGraph" ),
          nrl_DocumentGraph( "http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#DocumentGraph" ),
          nrl_ExternalViewSpecification( "http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#ExternalViewSpecification" ),
          nrl_FunctionalProperty( "http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#FunctionalProperty" ),
          nrl_Graph( "http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#Graph" ),
          nrl_GraphMetadata( "http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#GraphMetadata" ),
          nrl_GraphView( "http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#GraphView" ),
          nrl_InstanceBase( "http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#InstanceBase" ),
          nrl_InverseFunctionalProperty( "http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#InverseFunctionalProperty" ),
          nrl_KnowledgeBase( "http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#KnowledgeBase" ),
          nrl_Ontology( "http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#Ontology" ),
          nrl_ReflexiveProperty( "http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#ReflexiveProperty" ),
          nrl_RuleViewSpecification( "http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#RuleViewSpecification" ),
          nrl_Schema( "http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#Schema" ),
          nrl_Semantics( "http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#Semantics" ),
          nrl_SymmetricProperty( "http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#SymmetricProperty" ),
          nrl_TransitiveProperty( "http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#TransitiveProperty" ),
          nrl_ViewSpecification( "http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#ViewSpecification" ),
          nrl_cardinality( "http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#cardinality" ),
          nrl_coreGraphMetadataFor( "http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#coreGraphMetadataFor" ),
          nrl_equivalentGraph( "http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#equivalentGraph" ),
          nrl_externalRealizer( "http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#externalRealizer" ),
          nrl_graphMetadataFor( "http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#graphMetadataFor" ),
          nrl_hasSemantics( "http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#hasSemantics" ),
          nrl_hasSpecification( "http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#hasSpecification" ),
          nrl_imports( "http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#imports" ),
          nrl_inverseProperty( "http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#inverseProperty" ),
          nrl_maxCardinality( "http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#maxCardinality" ),
          nrl_minCardinality( "http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#minCardinality" ),
          nrl_realizes( "http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#realizes" ),
          nrl_rule( "http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#rule" ),
          nrl_ruleLanguage( "http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#ruleLanguage" ),
          nrl_semanticsDefinedBy( "http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#semanticsDefinedBy" ),
          nrl_subGraphOf( "http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#subGraphOf" ),
          nrl_superGraphOf( "http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#superGraphOf" ),
          nrl_updatable( "http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#updatable" ),
          nrl_viewOn( "http://www.semanticdesktop.org/ontologies/2007/08/15/nrl#viewOn" ) {
    }

    QUrl nrl_namespace;
    QUrl nrl_AsymmetricProperty;
    QUrl nrl_Configuration;
    QUrl nrl_Data;
    QUrl nrl_DefaultGraph;
    QUrl nrl_DocumentGraph;
    QUrl nrl_ExternalViewSpecification;
    QUrl nrl_FunctionalProperty;
    QUrl nrl_Graph;
    QUrl nrl_GraphMetadata;
    QUrl nrl_GraphView;
    QUrl nrl_InstanceBase;
    QUrl nrl_InverseFunctionalProperty;
    QUrl nrl_KnowledgeBase;
    QUrl nrl_Ontology;
    QUrl nrl_ReflexiveProperty;
    QUrl nrl_RuleViewSpecification;
    QUrl nrl_Schema;
    QUrl nrl_Semantics;
    QUrl nrl_SymmetricProperty;
    QUrl nrl_TransitiveProperty;
    QUrl nrl_ViewSpecification;
    QUrl nrl_cardinality;
    QUrl nrl_coreGraphMetadataFor;
    QUrl nrl_equivalentGraph;
    QUrl nrl_externalRealizer;
    QUrl nrl_graphMetadataFor;
    QUrl nrl_hasSemantics;
    QUrl nrl_hasSpecification;
    QUrl nrl_imports;
    QUrl nrl_inverseProperty;
    QUrl nrl_maxCardinality;
    QUrl nrl_minCardinality;
    QUrl nrl_realizes;
    QUrl nrl_rule;
    QUrl nrl_ruleLanguage;
    QUrl nrl_semanticsDefinedBy;
    QUrl nrl_subGraphOf;
    QUrl nrl_superGraphOf;
    QUrl nrl_updatable;
    QUrl nrl_viewOn;
};

Q_GLOBAL_STATIC( NrlPrivate, s_nrl )

QUrl Soprano::Vocabulary::NRL::nrlNamespace()
{
    return s_nrl()->nrl_namespace;
}

QUrl Soprano::Vocabulary::NRL::AsymmetricProperty()
{
    return s_nrl()->nrl_AsymmetricProperty;
}

QUrl Soprano::Vocabulary::NRL::Configuration()
{
    return s_nrl()->nrl_Configuration;
}

QUrl Soprano::Vocabulary::NRL::Data()
{
    return s_nrl()->nrl_Data;
}

QUrl Soprano::Vocabulary::NRL::DefaultGraph()
{
    return s_nrl()->nrl_DefaultGraph;
}

QUrl Soprano::Vocabulary::NRL::DocumentGraph()
{
    return s_nrl()->nrl_DocumentGraph;
}

QUrl Soprano::Vocabulary::NRL::ExternalViewSpecification()
{
    return s_nrl()->nrl_ExternalViewSpecification;
}

QUrl Soprano::Vocabulary::NRL::FunctionalProperty()
{
    return s_nrl()->nrl_FunctionalProperty;
}

QUrl Soprano::Vocabulary::NRL::Graph()
{
    return s_nrl()->nrl_Graph;
}

QUrl Soprano::Vocabulary::NRL::GraphMetadata()
{
    return s_nrl()->nrl_GraphMetadata;
}

QUrl Soprano::Vocabulary::NRL::GraphView()
{
    return s_nrl()->nrl_GraphView;
}

QUrl Soprano::Vocabulary::NRL::InstanceBase()
{
    return s_nrl()->nrl_InstanceBase;
}

QUrl Soprano::Vocabulary::NRL::InverseFunctionalProperty()
{
    return s_nrl()->nrl_InverseFunctionalProperty;
}

QUrl Soprano::Vocabulary::NRL::KnowledgeBase()
{
    return s_nrl()->nrl_KnowledgeBase;
}

QUrl Soprano::Vocabulary::NRL::Ontology()
{
    return s_nrl()->nrl_Ontology;
}

QUrl Soprano::Vocabulary::NRL::ReflexiveProperty()
{
    return s_nrl()->nrl_ReflexiveProperty;
}

QUrl Soprano::Vocabulary::NRL::RuleViewSpecification()
{
    return s_nrl()->nrl_RuleViewSpecification;
}

QUrl Soprano::Vocabulary::NRL::Schema()
{
    return s_nrl()->nrl_Schema;
}

QUrl Soprano::Vocabulary::NRL::Semantics()
{
    return s_nrl()->nrl_Semantics;
}

QUrl Soprano::Vocabulary::NRL::SymmetricProperty()
{
    return s_nrl()->nrl_SymmetricProperty;
}

QUrl Soprano::Vocabulary::NRL::TransitiveProperty()
{
    return s_nrl()->nrl_TransitiveProperty;
}

QUrl Soprano::Vocabulary::NRL::ViewSpecification()
{
    return s_nrl()->nrl_ViewSpecification;
}

QUrl Soprano::Vocabulary::NRL::cardinality()
{
    return s_nrl()->nrl_cardinality;
}

QUrl Soprano::Vocabulary::NRL::coreGraphMetadataFor()
{
    return s_nrl()->nrl_coreGraphMetadataFor;
}

QUrl Soprano::Vocabulary::NRL::equivalentGraph()
{
    return s_nrl()->nrl_equivalentGraph;
}

QUrl Soprano::Vocabulary::NRL::externalRealizer()
{
    return s_nrl()->nrl_externalRealizer;
}

QUrl Soprano::Vocabulary::NRL::graphMetadataFor()
{
    return s_nrl()->nrl_graphMetadataFor;
}

QUrl Soprano::Vocabulary::NRL::hasSemantics()
{
    return s_nrl()->nrl_hasSemantics;
}

QUrl Soprano::Vocabulary::NRL::hasSpecification()
{
    return s_nrl()->nrl_hasSpecification;
}

QUrl Soprano::Vocabulary::NRL::imports()
{
    return s_nrl()->nrl_imports;
}

QUrl Soprano::Vocabulary::NRL::inverseProperty()
{
    return s_nrl()->nrl_inverseProperty;
}

QUrl Soprano::Vocabulary::NRL::maxCardinality()
{
    return s_nrl()->nrl_maxCardinality;
}

QUrl Soprano::Vocabulary::NRL::minCardinality()
{
    return s_nrl()->nrl_minCardinality;
}

QUrl Soprano::Vocabulary::NRL::realizes()
{
    return s_nrl()->nrl_realizes;
}

QUrl Soprano::Vocabulary::NRL::rule()
{
    return s_nrl()->nrl_rule;
}

QUrl Soprano::Vocabulary::NRL::ruleLanguage()
{
    return s_nrl()->nrl_ruleLanguage;
}

QUrl Soprano::Vocabulary::NRL::semanticsDefinedBy()
{
    return s_nrl()->nrl_semanticsDefinedBy;
}

QUrl Soprano::Vocabulary::NRL::subGraphOf()
{
    return s_nrl()->nrl_subGraphOf;
}

QUrl Soprano::Vocabulary::NRL::superGraphOf()
{
    return s_nrl()->nrl_superGraphOf;
}

QUrl Soprano::Vocabulary::NRL::updatable()
{
    return s_nrl()->nrl_updatable;
}

QUrl Soprano::Vocabulary::NRL::viewOn()
{
    return s_nrl()->nrl_viewOn;
}
