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
#include <soprano_export.h>

namespace Soprano {
    namespace Vocabulary {
	/**
	 * The Nepomuk Representation Language
	 */
	namespace NRL {
	    /**
	     * The NRL namespace (http://www.semanticdesktop.org/ontologies/2007/08/15/nrl/)
	     */
	    SOPRANO_EXPORT QUrl nrlNamespace();

	    SOPRANO_EXPORT QUrl minCardinality();
	    SOPRANO_EXPORT QUrl maxCardinality();
	    SOPRANO_EXPORT QUrl cardinality();
	    SOPRANO_EXPORT QUrl imports();
	    SOPRANO_EXPORT QUrl ruleLanguage();
	    SOPRANO_EXPORT QUrl InverseFunctionalProperty();
	    SOPRANO_EXPORT QUrl Graph();
	    SOPRANO_EXPORT QUrl hasSpecification();
	    SOPRANO_EXPORT QUrl DocumentGraph();
	    SOPRANO_EXPORT QUrl coreGraphMetadataFor();
	    SOPRANO_EXPORT QUrl TransitiveProperty();
	    SOPRANO_EXPORT QUrl FunctionalProperty();
	    SOPRANO_EXPORT QUrl KnowledgeBase();
	    SOPRANO_EXPORT QUrl externalRealizer();
	    SOPRANO_EXPORT QUrl subGraphOf();
	    SOPRANO_EXPORT QUrl Configuration();
	    SOPRANO_EXPORT QUrl ViewSpecification();
	    SOPRANO_EXPORT QUrl hasSemantics();
	    SOPRANO_EXPORT QUrl ExternalViewSpecification();
	    SOPRANO_EXPORT QUrl DefaultGraph();
	    SOPRANO_EXPORT QUrl updatable();
	    SOPRANO_EXPORT QUrl Semantics();
	    SOPRANO_EXPORT QUrl equivalentGraph();
	    SOPRANO_EXPORT QUrl ReflexiveProperty();
	    SOPRANO_EXPORT QUrl AsymmetricProperty();
	    SOPRANO_EXPORT QUrl viewOn();
	    SOPRANO_EXPORT QUrl SymmetricProperty();
	    SOPRANO_EXPORT QUrl superGraphOf();
	    SOPRANO_EXPORT QUrl Ontology();
	    SOPRANO_EXPORT QUrl realizes();
	    SOPRANO_EXPORT QUrl rule();
	    SOPRANO_EXPORT QUrl InstanceBase();
	    SOPRANO_EXPORT QUrl RuleViewSpecification();
	    SOPRANO_EXPORT QUrl semanticsDefinedBy();
	    SOPRANO_EXPORT QUrl GraphMetadata();
	    SOPRANO_EXPORT QUrl graphMetadataFor();
	    SOPRANO_EXPORT QUrl GraphView();
	    SOPRANO_EXPORT QUrl Data();
	    SOPRANO_EXPORT QUrl inverseProperty();
	    SOPRANO_EXPORT QUrl Schema();
	}
    }
}

#endif
