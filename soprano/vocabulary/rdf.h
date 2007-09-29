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

#ifndef _SOPRANO_RDF_H_
#define _SOPRANO_RDF_H_

#include <QtCore/QUrl>
#include <soprano_export.h>

namespace Soprano {
    namespace Vocabulary {
	namespace RDF {
	    /**
	     * The RDF namespace (http://www.w3.org/1999/02/22-rdf-syntax-ns#)
	     */
	    SOPRANO_EXPORT QUrl rdfNamespace();
	    SOPRANO_EXPORT QUrl type();
	    SOPRANO_EXPORT QUrl Property();
	    SOPRANO_EXPORT QUrl Statement();
	    SOPRANO_EXPORT QUrl subject();
	    SOPRANO_EXPORT QUrl predicate();
	    SOPRANO_EXPORT QUrl object();
	    SOPRANO_EXPORT QUrl Bag();
	    SOPRANO_EXPORT QUrl Seq();
	    SOPRANO_EXPORT QUrl Alt();
	    SOPRANO_EXPORT QUrl value();
	    SOPRANO_EXPORT QUrl List();
	    SOPRANO_EXPORT QUrl nil();
	    SOPRANO_EXPORT QUrl first();
	    SOPRANO_EXPORT QUrl rest();
	    SOPRANO_EXPORT QUrl XMLLiteral();
	}
    }
}

#endif
