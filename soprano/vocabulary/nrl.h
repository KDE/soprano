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

namespace Soprano {
    namespace Vocabulary {
	/**
	 * The Nepomuk Representation Language
	 */
	namespace NRL {
	    /**
	     * The NRL namespace (http://www.semanticdesktop.org/ontologies/2007/08/15/nrl/)
	     */
	    QUrl nrlNamespace();

	    QUrl minCardinality();
	    QUrl maxCardinality();
	    QUrl cardinality();
	    QUrl imports();
	    QUrl ruleLanguage();
	    QUrl InverseFunctionalProperty();
	    QUrl Graph();
	    QUrl hasSpecification();
	    QUrl DocumentGraph();
	    QUrl coreGraphMetadataFor();
	    QUrl TransitiveProperty();
	    QUrl FunctionalProperty();
	    QUrl KnowledgeBase();
	    QUrl externalRealizer();
	    QUrl subGraphOf();
	    QUrl Configuration();
	    QUrl ViewSpecification();
	    QUrl hasSemantics();
	    QUrl ExternalViewSpecification();
	    QUrl DefaultGraph();
	    QUrl updatable();
	    QUrl Semantics();
	    QUrl equivalentGraph();
	    QUrl ReflexiveProperty();
	    QUrl AsymmetricProperty();
	    QUrl viewOn();
	    QUrl SymmetricProperty();
	    QUrl superGraphOf();
	    QUrl Ontology();
	    QUrl realizes();
	    QUrl rule();
	    QUrl InstanceBase();
	    QUrl RuleViewSpecification();
	    QUrl semanticsDefinedBy();
	    QUrl GraphMetadata();
	    QUrl graphMetadataFor();
	    QUrl GraphView();
	    QUrl Data();
	    QUrl inverseProperty();
	    QUrl Schema();
	}
    }
}

#endif
