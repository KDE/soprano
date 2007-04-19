/* 
 * This file is part of Soprano Project
 *
 * Copyright (C) 2006 Duncan Mac-Vicar <duncan@kde.org>
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

#ifndef SOPRANO_BACKEND_REDLAND_PARSER_H
#define SOPRANO_BACKEND_REDLAND_PARSER_H

#include <QtCore/QUrl>

#include "parser.h"

namespace Soprano {
    namespace Redland {
	class RedlandParser: public Soprano::Parser
	    {
	    public:
		RedlandParser();
		Model* parseFile( const QString& filename, 
				  const QUrl& baseUri, 
				  RdfSerialization serialization = AUTO_DETECTION ) const;
		Model* parseString( const QString& data, 
				    const QUrl& baseUri, 
				    RdfSerialization serialization = AUTO_DETECTION ) const;
		Model* parseStream( QTextStream*, 
				    const QUrl& baseUri, 
				    RdfSerialization serialization = AUTO_DETECTION ) const;
	    };
    }
}

#endif // SOPRANO_BACKEND_REDLAND_PARSER_H

