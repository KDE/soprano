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

#include "xmls.h"


class XMLSchema
{
public:
    XMLSchema()
        : xmlsNamespace( "http://www.w3.org/2001/XMLSchema#" ),
          xmlsInt( "http://www.w3.org/2001/XMLSchema#int" ),
          xmlsInteger( "http://www.w3.org/2001/XMLSchema#integer" ),
          xmlsNegativeInteger( "http://www.w3.org/2001/XMLSchema#negativeInteger" ),
          xmlsDecimal( "http://www.w3.org/2001/XMLSchema#decimal" ),
          xmlsShort( "http://www.w3.org/2001/XMLSchema#short" ),
          xmlsLong( "http://www.w3.org/2001/XMLSchema#long" ),
          xmlsUnsignedInt( "http://www.w3.org/2001/XMLSchema#unsignedInt" ),
          xmlsUnsignedShort( "http://www.w3.org/2001/XMLSchema#unsignedShort" ),
          xmlsUnsignedLong( "http://www.w3.org/2001/XMLSchema#unsignedLong" ),
          xmlsBoolean( "http://www.w3.org/2001/XMLSchema#boolean" ),
          xmlsDouble( "http://www.w3.org/2001/XMLSchema#double" ),
          xmlsString( "http://www.w3.org/2001/XMLSchema#string" ),
          xmlsDate( "http://www.w3.org/2001/XMLSchema#date" ),
          xmlsTime( "http://www.w3.org/2001/XMLSchema#time" ),
          xmlsDateTime( "http://www.w3.org/2001/XMLSchema#dateTime" ) {
    }

    QUrl xmlsNamespace;
    QUrl xmlsInt;
    QUrl xmlsInteger;
    QUrl xmlsNegativeInteger;
    QUrl xmlsDecimal;
    QUrl xmlsShort;
    QUrl xmlsLong;
    QUrl xmlsUnsignedInt;
    QUrl xmlsUnsignedShort;
    QUrl xmlsUnsignedLong;
    QUrl xmlsBoolean;
    QUrl xmlsDouble;
    QUrl xmlsString;
    QUrl xmlsDate;
    QUrl xmlsTime;
    QUrl xmlsDateTime;
};


Q_GLOBAL_STATIC( XMLSchema, xmls );


QUrl Soprano::Vocabulary::XMLSchema::NAMESPACE()
{
    return xmls()->xmlsNamespace;
}


QUrl Soprano::Vocabulary::XMLSchema::INT()
{
    return xmls()->xmlsInt;
}


QUrl Soprano::Vocabulary::XMLSchema::INTEGER()
{
    return xmls()->xmlsInteger;
}


QUrl Soprano::Vocabulary::XMLSchema::NEGATIVEINTEGER()
{
    return xmls()->xmlsNegativeInteger;
}


QUrl Soprano::Vocabulary::XMLSchema::DECIMAL()
{
    return xmls()->xmlsDecimal;
}


QUrl Soprano::Vocabulary::XMLSchema::SHORT()
{
    return xmls()->xmlsShort;
}


QUrl Soprano::Vocabulary::XMLSchema::LONG()
{
    return xmls()->xmlsLong;
}


QUrl Soprano::Vocabulary::XMLSchema::UNSIGNEDINT()
{
    return xmls()->xmlsUnsignedInt;
}


QUrl Soprano::Vocabulary::XMLSchema::UNSIGNEDSHORT()
{
    return xmls()->xmlsUnsignedShort;
}


QUrl Soprano::Vocabulary::XMLSchema::UNSIGNEDLONG()
{
    return xmls()->xmlsUnsignedLong;
}


QUrl Soprano::Vocabulary::XMLSchema::STRING()
{
    return xmls()->xmlsString;
}


QUrl Soprano::Vocabulary::XMLSchema::DOUBLE()
{
    return xmls()->xmlsDouble;
}


QUrl Soprano::Vocabulary::XMLSchema::BOOLEAN()
{
    return xmls()->xmlsBoolean;
}


QUrl Soprano::Vocabulary::XMLSchema::DATE()
{
    return xmls()->xmlsDate;
}


QUrl Soprano::Vocabulary::XMLSchema::TIME()
{
    return xmls()->xmlsTime;
}


QUrl Soprano::Vocabulary::XMLSchema::DATETIME()
{
    return xmls()->xmlsDateTime;
}
