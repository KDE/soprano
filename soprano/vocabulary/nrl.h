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
	     * The NRL namespace (http://www.semanticdesktop.org/ontologies/2007/08/15/nrl)
	     */
	    QUrl NAMESPACE();

	    QUrl MINCARDINALITY();
	    QUrl MAXCARDINALITY();
	    QUrl CARDINALITY();
	    QUrl IMPORTS();
	    QUrl RULELANGUAGE();
	    QUrl INVERSEFUNCTIONALPROPERTY();
	    QUrl GRAPH();
	    QUrl HASSPECIFICATION();
	    QUrl DOCUMENTGRAPH();
	    QUrl COREGRAPHMETADATAFOR();
	    QUrl TRANSITIVEPROPERTY();
	    QUrl FUNCTIONALPROPERTY();
	    QUrl KNOWLEDGEBASE();
	    QUrl EXTERNALREALIZER();
	    QUrl SUBGRAPHOF();
 	    QUrl CONFIGURATION();
	    QUrl VIEWSPECIFICATION();
	    QUrl HASSEMANTICS();
	    QUrl EXTERNALVIEWSPECIFICATION();
	    QUrl DEFAULTGRAPH();
	    QUrl UPDATABLE();
	    QUrl SEMANTICS();
	    QUrl EQUIVALENTGRAPH();
	    QUrl REFLEXIVEPROPERTY();
	    QUrl ASYMMETRICPROPERTY();
	    QUrl VIEWON();
	    QUrl SYMMETRICPROPERTY();
	    QUrl SUPERGRAPHOF();
	    QUrl ONTOLOGY();
	    QUrl REALIZES();
	    QUrl RULE();
	    QUrl INSTANCEBASE();
	    QUrl RULEVIEWSPECIFICATION();
	    QUrl SEMANTICSDEFINEDBY();
	    QUrl GRAPHMETADATA();
	    QUrl GRAPHMETADATAFOR();
	    QUrl GRAPHVIEW();
	    QUrl DATA();
	    QUrl INVERSEPROPERTY();
	    QUrl SCHEMA();
	}
    }
}

#endif
