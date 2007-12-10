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

#include "xsd.h"


class XMLSchema
{
public:
    XMLSchema()
        : xsdNamespace( "http://www.w3.org/2001/XMLSchema#" ),
          xsdInt( "http://www.w3.org/2001/XMLSchema#int" ),
          xsdInteger( "http://www.w3.org/2001/XMLSchema#integer" ),
          xsdNegativeInteger( "http://www.w3.org/2001/XMLSchema#negativeInteger" ),
          xsdNonNegativeInteger( "http://www.w3.org/2001/XMLSchema#nonNegativeInteger" ),
          xsdDecimal( "http://www.w3.org/2001/XMLSchema#decimal" ),
          xsdShort( "http://www.w3.org/2001/XMLSchema#short" ),
          xsdLong( "http://www.w3.org/2001/XMLSchema#long" ),
          xsdUnsignedInt( "http://www.w3.org/2001/XMLSchema#unsignedInt" ),
          xsdUnsignedShort( "http://www.w3.org/2001/XMLSchema#unsignedShort" ),
          xsdUnsignedLong( "http://www.w3.org/2001/XMLSchema#unsignedLong" ),
          xsdBoolean( "http://www.w3.org/2001/XMLSchema#boolean" ),
          xsdDouble( "http://www.w3.org/2001/XMLSchema#double" ),
          xsdString( "http://www.w3.org/2001/XMLSchema#string" ),
          xsdDate( "http://www.w3.org/2001/XMLSchema#date" ),
          xsdTime( "http://www.w3.org/2001/XMLSchema#time" ),
          xsdDateTime( "http://www.w3.org/2001/XMLSchema#dateTime" ),
          xsdBase64Binary( "http://www.w3.org/2001/XMLSchema#base64Binary" ) {
    }

    QUrl xsdNamespace;
    QUrl xsdInt;
    QUrl xsdInteger;
    QUrl xsdNegativeInteger;
    QUrl xsdNonNegativeInteger;
    QUrl xsdDecimal;
    QUrl xsdShort;
    QUrl xsdLong;
    QUrl xsdUnsignedInt;
    QUrl xsdUnsignedShort;
    QUrl xsdUnsignedLong;
    QUrl xsdBoolean;
    QUrl xsdDouble;
    QUrl xsdString;
    QUrl xsdDate;
    QUrl xsdTime;
    QUrl xsdDateTime;
    QUrl xsdBase64Binary;
};


Q_GLOBAL_STATIC( XMLSchema, xsd )


QUrl Soprano::Vocabulary::XMLSchema::xsdNamespace()
{
    return xsd()->xsdNamespace;
}


QUrl Soprano::Vocabulary::XMLSchema::xsdInt()
{
    return xsd()->xsdInt;
}


QUrl Soprano::Vocabulary::XMLSchema::integer()
{
    return xsd()->xsdInteger;
}


QUrl Soprano::Vocabulary::XMLSchema::negativeInteger()
{
    return xsd()->xsdNegativeInteger;
}


QUrl Soprano::Vocabulary::XMLSchema::nonNegativeInteger()
{
    return xsd()->xsdNonNegativeInteger;
}


QUrl Soprano::Vocabulary::XMLSchema::decimal()
{
    return xsd()->xsdDecimal;
}


QUrl Soprano::Vocabulary::XMLSchema::xsdShort()
{
    return xsd()->xsdShort;
}


QUrl Soprano::Vocabulary::XMLSchema::xsdLong()
{
    return xsd()->xsdLong;
}


QUrl Soprano::Vocabulary::XMLSchema::unsignedInt()
{
    return xsd()->xsdUnsignedInt;
}


QUrl Soprano::Vocabulary::XMLSchema::unsignedShort()
{
    return xsd()->xsdUnsignedShort;
}


QUrl Soprano::Vocabulary::XMLSchema::unsignedLong()
{
    return xsd()->xsdUnsignedLong;
}


QUrl Soprano::Vocabulary::XMLSchema::string()
{
    return xsd()->xsdString;
}


QUrl Soprano::Vocabulary::XMLSchema::xsdDouble()
{
    return xsd()->xsdDouble;
}


QUrl Soprano::Vocabulary::XMLSchema::boolean()
{
    return xsd()->xsdBoolean;
}


QUrl Soprano::Vocabulary::XMLSchema::date()
{
    return xsd()->xsdDate;
}


QUrl Soprano::Vocabulary::XMLSchema::time()
{
    return xsd()->xsdTime;
}


QUrl Soprano::Vocabulary::XMLSchema::dateTime()
{
    return xsd()->xsdDateTime;
}


QUrl Soprano::Vocabulary::XMLSchema::base64Binary()
{
    return xsd()->xsdBase64Binary;
}
