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

#ifndef _SOPRANO_XMLS_H_
#define _SOPRANO_XMLS_H_

#include <QtCore/QUrl>

namespace Soprano {
    namespace Vocabulary {
	namespace XMLSchema {
            QUrl NAMESPACE();
            QUrl INT();
            QUrl INTEGER();
	    QUrl NEGATIVEINTEGER();
	    QUrl DECIMAL();
	    QUrl SHORT();
            QUrl LONG();
	    QUrl UNSIGNEDINT();
	    QUrl UNSIGNEDSHORT();
	    QUrl UNSIGNEDLONG();
            QUrl STRING();
            QUrl DOUBLE();
            QUrl BOOLEAN();
            QUrl DATE();
            QUrl DATETIME();
	    QUrl TIME();
        }
    }
}

#endif
