/* 
 * This file is part of Soprano Project
 *
 * Copyright (C) 2006 Duncan Mac-Vicar <duncan@kde.org>
 * Copyright (C) 2007-2008 Sebastian Trueg <trueg@kde.org>
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

#ifndef _SOPRANO_RAPTOR_PARSER_LEGACY_H_
#define _SOPRANO_RAPTOR_PARSER_LEGACY_H_

#include <QtCore/QUrl>
#include <QtCore/QObject>

#include "parser.h"

#include <raptor.h>


namespace Soprano {
    namespace Raptor {
        class ParserLegacy : public QObject, public Soprano::Parser
        {
            Q_OBJECT
            Q_INTERFACES(Soprano::Parser)

        public:
            ParserLegacy();
            ~ParserLegacy();

            RdfSerializations supportedSerializations() const;

            StatementIterator parseFile( const QString& filename, 
                                         const QUrl& baseUri, 
                                         RdfSerialization serialization,
                                         const QString& userSerialization = QString() ) const;
            StatementIterator parseString( const QString& data, 
                                           const QUrl& baseUri, 
                                           RdfSerialization serialization,
                                           const QString& userSerialization = QString() ) const;
            StatementIterator parseStream( QTextStream&, 
                                           const QUrl& baseUri, 
                                           RdfSerialization serialization,
                                           const QString& userSerialization = QString() ) const;
        };
    }
}

#endif