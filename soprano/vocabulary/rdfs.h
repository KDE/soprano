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

#include <QtCore/QUrl>
#include <soprano_export.h>

namespace Soprano {
    namespace Vocabulary {
	namespace RDFS {
	    SOPRANO_EXPORT QUrl rdfsNamespace();
	    SOPRANO_EXPORT QUrl Class();
	    SOPRANO_EXPORT QUrl comment();
	    SOPRANO_EXPORT QUrl Datatype();
	    SOPRANO_EXPORT QUrl domain();
	    SOPRANO_EXPORT QUrl range();
	    SOPRANO_EXPORT QUrl label();
	    SOPRANO_EXPORT QUrl subClassOf();
	    SOPRANO_EXPORT QUrl subPropertyOf();
	    SOPRANO_EXPORT QUrl seeAlso();
	    SOPRANO_EXPORT QUrl Resource();
	    SOPRANO_EXPORT QUrl member();
	    SOPRANO_EXPORT QUrl Literal();
	    SOPRANO_EXPORT QUrl isDefinedBy();
	    SOPRANO_EXPORT QUrl Container();
	    SOPRANO_EXPORT QUrl ContainerMembershipProperty();
	}
    }
}

#endif
