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

#ifndef LITERAL_VALUE_H
#define LITERAL_VALUE_H

#include "soprano_export.h"

#include <QtCore/QVariant>
#include <QtCore/QSharedDataPointer>

#include "languagetag.h"


namespace Soprano
{
    /**
     * \class LiteralValue literalvalue.h Soprano/LiteralValue
     *
     * \brief Represents a literal value of an RDF Node.
     *
     * LiteralValue is based on QVariant to support
     * a subset of the XML Schema types that are compatible
     * with QT types.
     *
     * The following types are supported natively including
     * automatic type conversion. Other types are represented
     * as strings.
     *
     * \li int (Vocabulary::XMLSchema::xsdInt)
     * \li qlonglong (Vocabulary::XMLSchema::xsdLong)
     * \li unsigned int (Vocabulary::XMLSchema::unsignedInt)
     * \li qulonglong (Vocabulary::XMLSchema::unsignedLong)
     * \li bool (Vocabulary::XMLSchema::boolean)
     * \li double and float (Vocabulary::XMLSchema::xsdDouble) (float values are always converted to double)
     * \li QString (Vocabulary::XMLSchema::string or Vocabulary::RDF::XMLLiteral)
     * \li QDate (Vocabulary::XMLSchema::date)
     * \li QTime (Vocabulary::XMLSchema::time)
     * \li QDateTime (Vocabulary::XMLSchema::dateTime)
     * \li QByteArray (Vocabulary::XMLSchema::base64Binary)
     *
     * Literal values can be converted from strings via fromString().
     *
     * \sa Vocabulary::XMLSchema
     *
     * \author Sebastian Trueg <trueg@kde.org>
     */
    class SOPRANO_EXPORT LiteralValue
    {
    public:
        //@{
        /**
         * Create an empty literal value
         */
        LiteralValue();

        /**
         * Destructor
         */
        ~LiteralValue();

        /**
         * Default copy constructor
         */
        LiteralValue( const LiteralValue& other );

        /**
         * Creates a new LiteralValue from a QVariant.
         * User types are not supported. If v contains an
         * unsupported type an invalid LiteralValue is created.
         */
        LiteralValue( const QVariant& v );

        /**
         * Creates a literal value of type int (i.e.
         * http://www.w3.org/2001/XMLSchema#int)
         */
        LiteralValue( int i );

        /**
         * Creates a literal value of type long long (i.e.
         * http://www.w3.org/2001/XMLSchema#long)
         */
        LiteralValue( qlonglong i );

        /**
         * Creates a literal value of type unsigned int (i.e.
         * http://www.w3.org/2001/XMLSchema#unsignedInt)
         */
        LiteralValue( uint i );

        /**
         * Creates a literal value of type unsigned long long (i.e.
         * http://www.w3.org/2001/XMLSchema#unsignedLong)
         */
        LiteralValue( qulonglong i );

        /**
         * Creates a literal value of type bool (i.e.
         * http://www.w3.org/2001/XMLSchema#boolean)
         */
        LiteralValue( bool b );

        /**
         * Creates a literal value of type double (i.e.
         * http://www.w3.org/2001/XMLSchema#double)
         */
        LiteralValue( double d );

        /**
         * Creates a literal value of type QString (i.e.
         * http://www.w3.org/2001/XMLSchema#string)
         * \param string The value of the new literal interpreted
         * as UTF-8 encoded string.
         */
        LiteralValue( const char* string );

        /**
         * Creates a literal value of type QString (i.e.
         * http://www.w3.org/2001/XMLSchema#string)
         */
        LiteralValue( const QLatin1String& string );

        /**
         * Creates a literal value of type QString (i.e.
         * http://www.w3.org/2001/XMLSchema#string)
         */
        LiteralValue( const QString& string );

        /**
         * Creates a literal value of type QDate (i.e.
         * http://www.w3.org/2001/XMLSchema#date)
         */
        LiteralValue( const QDate& date );

        /**
         * Creates a literal value of type QTime (i.e.
         * http://www.w3.org/2001/XMLSchema#time)
         */
        LiteralValue( const QTime& time );

        /**
         * Creates a literal value of type QDateTime (i.e.
         * http://www.w3.org/2001/XMLSchema#dateTime)
         */
        LiteralValue( const QDateTime& datetime );

        /**
         * Creates a literal value of type QByteArray (i.e.
         * http://www.w3.org/2001/XMLSchema#base64Binary)
         */
        LiteralValue( const QByteArray& data );
        //@}

        //@{
        /**
         * Creates a copy of \a other
         */
        LiteralValue& operator=( const LiteralValue& other );

        /**
         * Assigns \a i to this literal value. The type will
         * be set to int (http://www.w3.org/2001/XMLSchema#int)
         * unless it is already set to a compatible type.
         *
         * Thus, a type http://www.w3.org/2001/XMLSchema#integer
         * or http://www.w3.org/2001/XMLSchema#decimal will not
         * be changed.
         *
         * Be aware that Soprano does not check the ranges of the
         * integer value yet.
         */
        LiteralValue& operator=( int i );

        /**
         * Assigns \a i to this literal value. The type will
         * be set to long (http://www.w3.org/2001/XMLSchema#long).
         */
        LiteralValue& operator=( qlonglong i );

        /**
         * Assigns \a i to this literal value. The type will
         * be set to uint (http://www.w3.org/2001/XMLSchema#unsignedInt)
         * unless it is already set to a compatible type.
         *
         * Thus, a type http://www.w3.org/2001/XMLSchema#unsignedShort
         * will not be changed.
         *
         * Be aware that Soprano does not check the ranges of the
         * unsigned value yet.
         */
        LiteralValue& operator=( uint i );

        /**
         * Assigns \a i to this literal value. The type will
         * be set to unsigned long (http://www.w3.org/2001/XMLSchema#unsignedLong).
         */
        LiteralValue& operator=( qulonglong i );

        /**
         * Assigns \a b to this literal value. The type will
         * be set to bool (http://www.w3.org/2001/XMLSchema#boolean).
         */
        LiteralValue& operator=( bool b );

        /**
         * Assigns \a d to this literal value. The type will
         * be set to double (http://www.w3.org/2001/XMLSchema#double).
         */
        LiteralValue& operator=( double d );

        /**
         * Assigns \a s to this literal value. The type will
         * be set to string (http://www.w3.org/2001/XMLSchema#string).
         */
        LiteralValue& operator=( const QString& s );

        /**
         * Assigns \a s to this literal value. The type will
         * be set to string (http://www.w3.org/2001/XMLSchema#string).
         */
        LiteralValue& operator=( const QLatin1String& s );

        /**
         * Assigns \a date to this literal value. The type will
         * be set to %data (http://www.w3.org/2001/XMLSchema#date).
         */
        LiteralValue& operator=( const QDate& date );

        /**
         * Assigns \a time to this literal value. The type will
         * be set to %time (http://www.w3.org/2001/XMLSchema#time).
         */
        LiteralValue& operator=( const QTime& time );

        /**
         * Assigns \a datetime to this literal value. The type will
         * be set to dateTime (http://www.w3.org/2001/XMLSchema#dateTime).
         */
        LiteralValue& operator=( const QDateTime& datetime );

        /**
         * Assigns \a datetime to this literal value. The type will
         * be set to ByteArray (http://www.w3.org/2001/XMLSchema#base64Binary).
         */
        LiteralValue& operator=( const QByteArray& data );
        //@}

        //@{
        bool operator==( const LiteralValue& other ) const;

        bool operator!=( const LiteralValue& other ) const;
        //@}

        //@{
        bool isValid() const;

        /**
          * Determines if this literal value is a plain literal.
          * Plain literals have no data type, but may have an optional language tag.
          *
          * \return \c true if this literal is plain
          */
        bool isPlain() const;

        bool isInt() const;
        bool isInt64() const;
        bool isUnsignedInt() const;
        bool isUnsignedInt64() const;
        bool isBool() const;
        bool isDouble() const;

        /**
         * Check if the literal contains a string value.
         * Be aware that unknown literal types are also
         * treated as strings. In that case compare
         * dataTypeUrl.
         */
        bool isString() const;
        bool isDate() const;
        bool isTime() const;
        bool isDateTime() const;
        bool isByteArray() const;
        //@}

        //@{
        int toInt() const;
        qlonglong toInt64() const;
        uint toUnsignedInt() const;
        qulonglong toUnsignedInt64() const;
        bool toBool() const;
        double toDouble() const;

        /**
         * Each type can be converted to a string which means that
         * toString in combination with dataTypeUrl provides all the
         * information necessary to store this literal as RDF.
         *
         * The string value is cached so calling it multiple times in
         * a row is fast.
         *
         * \warning For historical reasons this is not a user-readable representation.
         *
         * \sa Node::toString
         */
        QString toString() const;
        QDate toDate() const;
        QTime toTime() const;
        QDateTime toDateTime() const;
        QByteArray toByteArray() const;
        //@}

        //@{
        /**
         * The XML Schema datatype URI.
         *
         * \return The URI of the XML Schema type referring to the
         * stored type or an empty QUrl if the LiteralValue is empty or
         * is a plain literal.
         */
        QUrl dataTypeUri() const;

        /**
         * The language tag.
         *
         * \return The language tag of the plain literal or an empty LanguageTag
         * if the LiteralValue has no language or it is a typed literal.
         */
        LanguageTag language() const;

        /**
         * The type of the data.
         *
         * \return The QVariant type of the stored data or QVariant::Invalid
         * if it is an empty value.
         */
        QVariant::Type type() const;

        /**
         * The literal value represented in a QVariant.
         * Be aware that the RDF typing information is lost
         * in the converted variant.
         */
        QVariant variant() const;
        //@}

        /**
         * Create a LiteralValue object by parsing string \a value based on \a type.
         * If \a type is unknown a simple string LiteralValue object is returned
         * containing the plain string \a value.
         *
         * \sa fromString(const QString&, const QUrl&)
         */
        static LiteralValue fromString( const QString& value, QVariant::Type type );

        /**
         * Create a LiteralValue object by parsing string \a value based on \a dataTypeUri.
         *
         * \param value The value of the literal. Might be converted based on \a dataTypeUri.
         *
         * \param dataTypeUri The data type URI. %Soprano can automatically convert all XML schema
         * types. All other (unknown) types will be stored as a string value with the plain
         * \a dataTypeUri as type. Thus, unknown literal types can still be used without
         * automatic type conversion. (Be aware though, that changing the value of a LiteralValue
         * instance will reset the type, ie. custom data type URIs will be lost.)
         *
         * Both an empty \a value and \a dataTypeUri will result in an invalid LiteralValue
         * instance but an empty \a value with a valid \a dataTypeUri is possible. A valid
         * \a value with an invalid \a dataTypeUri will result in a LiteralValue of type
         * Vocabulary::XMLSchema::string.
         *
         * \return A newly created LiteralValue instance based on the provided \a value and
         * \a dataTypeUri.
         *
         * \sa fromString(const QString&, QVariant::Type), Vocabulary::XMLSchema
         */
        static LiteralValue fromString( const QString& value, const QUrl& dataTypeUri );

        /**
         * Create a plain LiteralValue object with an optional language tag.
         *
         * \param value The value of the literal.
         *
         * \param lang The language tag.
         *
         * Both an empty \a value and \a lang will result in an invalid LiteralValue
         * instance but an empty \a value with a valid \a lang is possible. A valid
         * \a value with an empty \a lang will result in a plain, untyped literal with no
         * language tag.
         *
         * \return A newly created LiteralValue instance based on the provided \a value and
         * \a lang.
         */
        static LiteralValue createPlainLiteral( const QString& value, const LanguageTag& lang = LanguageTag() );

        /**
         * Convert an XML Schema URI into a QVariant::Type.
         * \return The QVariant::Type corresponding to dataTypeUri or QVariant::Invalid
         * if dataTypeUri is unknown.
         */
        static QVariant::Type typeFromDataTypeUri( const QUrl& dataTypeUri );

        /**
         * Convert a QVariant::Type into an XML Schema URI.
         * \return The XML Schema URI that corresponds to \p type or an empty QUrl if
         * the type os unknown, i.e. can not be mapped to an XML Schema type.
         */
        static QUrl dataTypeUriFromType( QVariant::Type type );

    private:
        class LiteralValueData;
        class PlainData;
        class TypedData;
        QSharedDataPointer<LiteralValueData> d;
    };

    SOPRANO_EXPORT uint qHash( const LiteralValue& lit );
}

SOPRANO_EXPORT QDebug operator<<( QDebug dbg, const Soprano::LiteralValue& );

#endif
