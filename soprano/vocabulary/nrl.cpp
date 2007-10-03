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

class Nrl
{
public:
    Nrl()
        : nrlNamespace( NRL_NS ),
          minCardinality( NRL_NS"minCardinality" ),
          maxCardinality( NRL_NS"maxCardinality" ),
          cardinality( NRL_NS"cardinality" ),
          imports( NRL_NS"imports" ),
          ruleLanguage( NRL_NS"ruleLanguage" ),
          InverseFunctionalProperty( NRL_NS"InverseFunctionalProperty" ),
          Graph( NRL_NS"Graph" ),
          hasSpecification( NRL_NS"hasSpecification" ),
          DocumentGraph( NRL_NS"DocumentGraph" ),
          coreGraphMetadataFor( NRL_NS"coreGraphMetadataFor" ),
          TransitiveProperty( NRL_NS"TransitiveProperty" ),
          FunctionalProperty( NRL_NS"FunctionalProperty" ),
          KnowledgeBase( NRL_NS"KnowledgeBase" ),
          externalRealizer( NRL_NS"externalRealizer" ),
          subGraphOf( NRL_NS"subGraphOf" ),
          Configuration( NRL_NS"Configuration" ),
          ViewSpecification( NRL_NS"ViewSpecification" ),
          hasSemantics( NRL_NS"hasSemantics" ),
          ExternalViewSpecification( NRL_NS"ExternalViewSpecification" ),
          DefaultGraph( NRL_NS"DefaultGraph" ),
          updatable( NRL_NS"updatable" ),
          Semantics( NRL_NS"Semantics" ),
          equivalentGraph( NRL_NS"equivalentGraph" ),
          ReflexiveProperty( NRL_NS"ReflexiveProperty" ),
          AsymmetricProperty( NRL_NS"AsymmetricProperty" ),
          viewOn( NRL_NS"viewOn" ),
          SymmetricProperty( NRL_NS"SymmetricProperty" ),
          superGraphOf( NRL_NS"superGraphOf" ),
          Ontology( NRL_NS"Ontology" ),
          realizes( NRL_NS"realizes" ),
          rule( NRL_NS"rule" ),
          InstanceBase( NRL_NS"InstanceBase" ),
          RuleViewSpecification( NRL_NS"RuleViewSpecification" ),
          semanticsDefinedBy( NRL_NS"semanticsDefinedBy" ),
          GraphMetadata( NRL_NS"GraphMetadata" ),
          graphMetadataFor( NRL_NS"graphMetadataFor" ),
          GraphView( NRL_NS"GraphView" ),
          Data( NRL_NS"Data" ),
          inverseProperty( NRL_NS"inverseProperty" ),
          Schema( NRL_NS"Schema" ) {
    }

    QUrl nrlNamespace;
    QUrl minCardinality;
    QUrl maxCardinality;
    QUrl cardinality;
    QUrl imports;
    QUrl ruleLanguage;
    QUrl InverseFunctionalProperty;
    QUrl Graph;
    QUrl hasSpecification;
    QUrl DocumentGraph;
    QUrl coreGraphMetadataFor;
    QUrl TransitiveProperty;
    QUrl FunctionalProperty;
    QUrl KnowledgeBase;
    QUrl externalRealizer;
    QUrl subGraphOf;
    QUrl Configuration;
    QUrl ViewSpecification;
    QUrl hasSemantics;
    QUrl ExternalViewSpecification;
    QUrl DefaultGraph;
    QUrl updatable;
    QUrl Semantics;
    QUrl equivalentGraph;
    QUrl ReflexiveProperty;
    QUrl AsymmetricProperty;
    QUrl viewOn;
    QUrl SymmetricProperty;
    QUrl superGraphOf;
    QUrl Ontology;
    QUrl realizes;
    QUrl rule;
    QUrl InstanceBase;
    QUrl RuleViewSpecification;
    QUrl semanticsDefinedBy;
    QUrl GraphMetadata;
    QUrl graphMetadataFor;
    QUrl GraphView;
    QUrl Data;
    QUrl inverseProperty;
    QUrl Schema;
};


Q_GLOBAL_STATIC( Nrl, nrl )


QUrl Soprano::Vocabulary::NRL::nrlNamespace()
{
    return nrl()->nrlNamespace;
}


QUrl Soprano::Vocabulary::NRL::minCardinality()
{
    return nrl()->minCardinality;
}


QUrl Soprano::Vocabulary::NRL::maxCardinality()
{
    return nrl()->maxCardinality;
}


QUrl Soprano::Vocabulary::NRL::cardinality()
{
    return nrl()->cardinality;
}


QUrl Soprano::Vocabulary::NRL::imports()
{
    return nrl()->imports;
}


QUrl Soprano::Vocabulary::NRL::ruleLanguage()
{
    return nrl()->ruleLanguage;
}


QUrl Soprano::Vocabulary::NRL::InverseFunctionalProperty()
{
    return nrl()->InverseFunctionalProperty;
}


QUrl Soprano::Vocabulary::NRL::Graph()
{
    return nrl()->Graph;
}


QUrl Soprano::Vocabulary::NRL::hasSpecification()
{
    return nrl()->hasSpecification;
}


QUrl Soprano::Vocabulary::NRL::DocumentGraph()
{
    return nrl()->DocumentGraph;
}


QUrl Soprano::Vocabulary::NRL::coreGraphMetadataFor()
{
    return nrl()->coreGraphMetadataFor;
}


QUrl Soprano::Vocabulary::NRL::TransitiveProperty()
{
    return nrl()->TransitiveProperty;
}


QUrl Soprano::Vocabulary::NRL::FunctionalProperty()
{
    return nrl()->FunctionalProperty;
}


QUrl Soprano::Vocabulary::NRL::KnowledgeBase()
{
    return nrl()->KnowledgeBase;
}


QUrl Soprano::Vocabulary::NRL::externalRealizer()
{
    return nrl()->externalRealizer;
}


QUrl Soprano::Vocabulary::NRL::subGraphOf()
{
    return nrl()->subGraphOf;
}


QUrl Soprano::Vocabulary::NRL::Configuration()
{
    return nrl()->Configuration;
}


QUrl Soprano::Vocabulary::NRL::ViewSpecification()
{
    return nrl()->ViewSpecification;
}


QUrl Soprano::Vocabulary::NRL::hasSemantics()
{
    return nrl()->hasSemantics;
}


QUrl Soprano::Vocabulary::NRL::ExternalViewSpecification()
{
    return nrl()->ExternalViewSpecification;
}


QUrl Soprano::Vocabulary::NRL::DefaultGraph()
{
    return nrl()->DefaultGraph;
}


QUrl Soprano::Vocabulary::NRL::updatable()
{
    return nrl()->updatable;
}


QUrl Soprano::Vocabulary::NRL::Semantics()
{
    return nrl()->Semantics;
}


QUrl Soprano::Vocabulary::NRL::equivalentGraph()
{
    return nrl()->equivalentGraph;
}


QUrl Soprano::Vocabulary::NRL::ReflexiveProperty()
{
    return nrl()->ReflexiveProperty;
}


QUrl Soprano::Vocabulary::NRL::AsymmetricProperty()
{
    return nrl()->AsymmetricProperty;
}


QUrl Soprano::Vocabulary::NRL::viewOn()
{
    return nrl()->viewOn;
}


QUrl Soprano::Vocabulary::NRL::SymmetricProperty()
{
    return nrl()->SymmetricProperty;
}


QUrl Soprano::Vocabulary::NRL::superGraphOf()
{
    return nrl()->superGraphOf;
}


QUrl Soprano::Vocabulary::NRL::Ontology()
{
    return nrl()->Ontology;
}


QUrl Soprano::Vocabulary::NRL::realizes()
{
    return nrl()->realizes;
}


QUrl Soprano::Vocabulary::NRL::rule()
{
    return nrl()->rule;
}


QUrl Soprano::Vocabulary::NRL::InstanceBase()
{
    return nrl()->InstanceBase;
}


QUrl Soprano::Vocabulary::NRL::RuleViewSpecification()
{
    return nrl()->RuleViewSpecification;
}


QUrl Soprano::Vocabulary::NRL::semanticsDefinedBy()
{
    return nrl()->semanticsDefinedBy;
}


QUrl Soprano::Vocabulary::NRL::GraphMetadata()
{
    return nrl()->GraphMetadata;
}


QUrl Soprano::Vocabulary::NRL::graphMetadataFor()
{
    return nrl()->graphMetadataFor;
}


QUrl Soprano::Vocabulary::NRL::GraphView()
{
    return nrl()->GraphView;
}


QUrl Soprano::Vocabulary::NRL::Data()
{
    return nrl()->Data;
}


QUrl Soprano::Vocabulary::NRL::inverseProperty()
{
    return nrl()->inverseProperty;
}


QUrl Soprano::Vocabulary::NRL::Schema()
{
    return nrl()->Schema;
}
