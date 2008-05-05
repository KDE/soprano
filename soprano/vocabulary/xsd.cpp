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
        : xsdNamespace( QUrl::fromEncoded( "http://www.w3.org/2001/XMLSchema#", QUrl::StrictMode ) ),
          xsdInt( QUrl::fromEncoded( "http://www.w3.org/2001/XMLSchema#int", QUrl::StrictMode ) ),
          xsdInteger( QUrl::fromEncoded( "http://www.w3.org/2001/XMLSchema#integer", QUrl::StrictMode ) ),
          xsdNegativeInteger( QUrl::fromEncoded( "http://www.w3.org/2001/XMLSchema#negativeInteger", QUrl::StrictMode ) ),
          xsdNonNegativeInteger( QUrl::fromEncoded( "http://www.w3.org/2001/XMLSchema#nonNegativeInteger", QUrl::StrictMode ) ),
          xsdDecimal( QUrl::fromEncoded( "http://www.w3.org/2001/XMLSchema#decimal", QUrl::StrictMode ) ),
          xsdShort( QUrl::fromEncoded( "http://www.w3.org/2001/XMLSchema#short", QUrl::StrictMode ) ),
          xsdLong( QUrl::fromEncoded( "http://www.w3.org/2001/XMLSchema#long", QUrl::StrictMode ) ),
          xsdUnsignedInt( QUrl::fromEncoded( "http://www.w3.org/2001/XMLSchema#unsignedInt", QUrl::StrictMode ) ),
          xsdUnsignedShort( QUrl::fromEncoded( "http://www.w3.org/2001/XMLSchema#unsignedShort", QUrl::StrictMode ) ),
          xsdUnsignedLong( QUrl::fromEncoded( "http://www.w3.org/2001/XMLSchema#unsignedLong", QUrl::StrictMode ) ),
          xsdBoolean( QUrl::fromEncoded( "http://www.w3.org/2001/XMLSchema#boolean", QUrl::StrictMode ) ),
          xsdDouble( QUrl::fromEncoded( "http://www.w3.org/2001/XMLSchema#double", QUrl::StrictMode ) ),
          xsdFloat( QUrl::fromEncoded( "http://www.w3.org/2001/XMLSchema#float", QUrl::StrictMode ) ),
          xsdString( QUrl::fromEncoded( "http://www.w3.org/2001/XMLSchema#string", QUrl::StrictMode ) ),
          xsdDate( QUrl::fromEncoded( "http://www.w3.org/2001/XMLSchema#date", QUrl::StrictMode ) ),
          xsdTime( QUrl::fromEncoded( "http://www.w3.org/2001/XMLSchema#time", QUrl::StrictMode ) ),
          xsdDateTime( QUrl::fromEncoded( "http://www.w3.org/2001/XMLSchema#dateTime", QUrl::StrictMode ) ),
          xsdBase64Binary( QUrl::fromEncoded( "http://www.w3.org/2001/XMLSchema#base64Binary", QUrl::StrictMode ) ) {
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
    QUrl xsdFloat;
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


QUrl Soprano::Vocabulary::XMLSchema::xsdFloat()
{
    return xsd()->xsdFloat;
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
