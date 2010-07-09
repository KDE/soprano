/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2009 Greg Beauchesne <greg_b@vision-play.com>
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

#ifndef SOPRANO_LANGUAGETAG_H
#define SOPRANO_LANGUAGETAG_H

#include <QtCore/QSharedDataPointer>
#include <QtCore/QTextStream>
#include <QtCore/QLocale>
#include <QtCore/QFlags>
#include <QtCore/QMetaType>

#include "soprano_export.h"

namespace Soprano
{
    /**
     * \class LanguageTag languagetag.h Soprano/LanguageTag
     *
     * \brief A LanguageTag represents a language according to RFC 3066 and RFC 4646.
     *
     * \author Greg Beauchesne <greg_b@vision-play.com>
     *
     * \since 2.3
     */
    class SOPRANO_EXPORT LanguageTag
    {
    public:
        enum MatchFilter {
            /**
             * Represents basic language filtering as described in RFC 3066 section 2.5
             * (or RFC 4647 section 3.3.1). Basic language filtering checks only that the
             * range string is a prefix of a language tag and that the prefix is followed
             * by "-" or the end of the string. Additionally, the language range "*"
             * matches all tags.
             */
            MatchFilterBasic = 0,

            /**
             * Represents extended language filtering as described in RFC 4647 section 3.3.2.
             * Extended language filtering breaks tags up into subtags and is permitted to
             * skip subtags in the language tag in order to match parts of the range string.
             */
            MatchFilterExtended = 1
        };

        /**
         * Flags for performing LanguageTag lookups.
         *
         * \sa lookup()
         */
        enum LookupFlag {
            LookupFlagNone = 0,
            /// Disables fallback behavior for lookup ranges
            LookupFlagNoFallback = 1
        };
        Q_DECLARE_FLAGS(LookupFlags, LookupFlag)

    public:
        /**
         * \name Constructors
         */
        //@{
        /**
         * Default constructor.
         * Creates an empty language tag.
         */
        LanguageTag();

        /**
         * Copy constructor.
         *
         * \param other The language tag from which to copy
         */
        LanguageTag( const LanguageTag &other );

        /**
         * Creates a language tag.
         *
         * \param tag The language tag string
         */
        LanguageTag( const char * tag );

        /**
         * Creates a language tag.
         *
         * \param tag The language tag string
         */
        LanguageTag( const QLatin1String &tag );

        /**
         * Creates a language tag.
         *
         * \param tag The language tag string
         */
        LanguageTag( const QString &tag );

        /**
         * Creates a language tag from a QLocale. If the language is
         * QLocale::C, then an empty LanguageTag is produced.
         *
         * \param locale the locale
         */
        explicit LanguageTag( const QLocale &locale );

        /**
         * Creates a language tag from a language and country. If \a lang is
         * QLocale::C, then an empty LanguageTag is produced.
         *
         * \param lang the language code
         * \param country the country code
         */
        LanguageTag( QLocale::Language lang, QLocale::Country country = QLocale::AnyCountry );

        /**
         * Destructor.
         */
        ~LanguageTag();
        //@}

        /**
         * \name Operators
         */
        //@{
        /**
         * Assigns \a other to this tag.
         */
        LanguageTag& operator=( const LanguageTag& other );

        /**
         * Comparison operator. Language tag comparisons are
         * case-insensitive.
         *
         * \return \c true if this tag and \p other are equal.
         */
        bool operator==( const LanguageTag& other ) const;

        /**
         * Comparison operator. Language tag comparisons are
         * case-insensitive.
         * \return \c true if this tag and \p other differ.
         */
        bool operator!=( const LanguageTag& other ) const;

        /**
         * Comparison operator.
         * \return \c true if this tag is lexically less than \p other.
         */
        bool operator<( const LanguageTag& other ) const;

        /**
         * Comparison operator.
         * \return \c true if this tag is lexically less than or equal to \p other.
         */
        bool operator<=( const LanguageTag& other ) const;

        /**
         * Comparison operator.
         * \return \c true if this tag is lexically greater than \p other.
         */
        bool operator>( const LanguageTag& other ) const;

        /**
         * Comparison operator.
         * \return \c true if this tag is lexically greater than or equal to \p other.
         */
        bool operator>=( const LanguageTag& other ) const;

        /**
         * Match this language tag against \a range using the specified
         * filtering scheme.
         *
         * \param range the range against which to match
         * \param scheme the filtering scheme to use
         *
         * \return \c true if this tag matches \a pattern, \c false if not.
         */
        bool matches( const LanguageTag& range, MatchFilter scheme = MatchFilterBasic ) const;
        //@}

        /**
         * \name Information
         */
        //@{

        /**
         * \return \c true if this language tag is empty.
         */
        bool isEmpty() const;

        /**
         * Determines if this language tag is valid. A valid language
         * tag is divided into subtags that are divided by "-". The tag
         * must adhere the following rules:
         * - Each subtag is composed only of characters in the ASCII range
         *   [a-zA-Z0-9] (letters and digits), with the exception of the
         *   first subtag, which must be composed of letters only
         * - No subtags are empty
         * - No subtags are longer than 8 characters
         * - Single-character subtags cannot end the tag unless after a
         *   private use ("x") subtag (e.g. "en-x-i" and "en-x-abcd-i" are
         *   legal; "en-x", "en-i", and "en-i-abcd-g" are illegal)
         *
         * \return \c true if this language tag is valid
         */
        bool isValid() const;

        /**
         * \return the subtag parts that make up this language tag
         */
        QStringList subTags() const;

        //@}

        /**
         * \name Conversion
         */
        //@{
        /**
         * Converts this LanguageTag to a string.
         *
         * \return the string representation of this LanguageTag
         */
        QString toString() const;

        /**
         * Converts this LanguageTag to a string formatted according to the
         * case rules in RFC 4647 section 4.4 (page 44).
         *
         * \return the pretty string representation of this LanguageTag
         */
        QString toPrettyString() const;

        /**
         * Converts this LanguageTag to a locale, using the primary language
         * tag and first two-letter language subtag, if any.
         * If the conversion could not be performed, then the "C"
         * QLocale is returned.
         *
         * \return the converted locale
         *
         * \note QLocale restricts the countries that can be paired with a
         * particular language. If the language-country pair in this tag
         * is not in the QLocale database, or if this language tag does not
         * contain country information, then the most appropriate country
         * will be used for the locale. For example, "de-US" (German as
         * spoken in the United States) is not a valid combination, and the
         * country would be changed to Germany.
         */
        QLocale toLocale() const;

        /**
         * A convenience operator to convert this LanguageTag to a string.
         *
         * \return the string representation of this LanguageTag
         *
         * \sa toString()
         */
        inline operator QString() const { return toString(); }
        //@}


        /**
         * Compares a language tag against a list of language tags and
         * selects the most suitable tag according to the rules of RFC 4647
         * Section 3.4.
         *
         * The arguments consist of \a choices, a list of tags from which
         * to select, and \a priority, a list of acceptable language ranges.
         * Both lists must be sorted in descending order of preference.
         *
         * The matching algorithm works as follows:
         * <ol>
         * <li>For each item in the \a priority list, the \a choices list is
         * scanned for matches. The wildcard value "*" is ignored in
         * lookup operations, as it would match any tag. If a match occurs,
         * the lookup is complete and the matching \a choices list index is
         * returned.</li>
         * <li>If no match has been found, the last subtag on the current
         * \a priority item is truncated, and the \a choices list is scanned
         * again. Single-character subtags are also truncated if they are at
         * the end of the tag. This repeats until there are no subtags
         * remaining. This fallback behavior can be skipped by specifying the
         * LookupFlagsNoFallback flag.</li>
         * <li>If neither the current \a priority item nor any of its
         * fallback values matched the \a choices list, then the next item
         * in the \a priority list is considered according to the above
         * steps.</li>
         * <li>If none of the \a priority items match the \a choices list,
         * then a value < 0 is returned.</li>
         * </ol>
         *
         * \param choices the list of available language choices
         * \param priority the language priority list
         * \param flags flags for the lookup operation
         * \param scheme the matching scheme to use. RFC 4647 specifies
         *            that basic (\link LanguageTag::MatchFilterBasic MatchFilterBasic\endlink)
         *            processing should be used.
         *
         * \return the index of the matching language tag in \a choices, or
         *         < 0 if no tag matched.
         */
        static int lookup(
            const QList<LanguageTag> & choices,
            const QList<LanguageTag> & priority,
            LookupFlags flags = LookupFlagNone,
            MatchFilter scheme = MatchFilterBasic
        );

        /**
         * A convenience method to select a language tag based on a single
         * search pattern.
         *
         * \param choices the list of available language choices
         * \param priority the language range against which to match
         * \param flags flags for the lookup operation
         * \param scheme the matching scheme to use. RFC 4647 specifies
         *            that basic (\link LanguageTag::MatchFilterBasic MatchFilterBasic\endlink)
         *            processing should be used.
         *
         * \return the index of the matching language tag in \a choices, or
         *         < 0 if no tag matched.
         *
         * \sa lookup(const QList<LanguageTag> &, const QList<LanguageTag> &, LookupFlags, MatchFilter)
         */
        static int lookup(
            const QList<LanguageTag> & choices,
            const LanguageTag & priority,
            LookupFlags flags = LookupFlagNone,
            MatchFilter scheme = MatchFilterBasic
        );

    private:
        class Private;
        QSharedDataPointer<Private> d;
    };

    SOPRANO_EXPORT uint qHash( const LanguageTag& node );
}

Q_DECLARE_METATYPE( Soprano::LanguageTag )
Q_DECLARE_TYPEINFO( Soprano::LanguageTag, Q_MOVABLE_TYPE );

Q_DECLARE_OPERATORS_FOR_FLAGS(Soprano::LanguageTag::LookupFlags)

SOPRANO_EXPORT QDebug operator<<( QDebug s, const Soprano::LanguageTag& );
SOPRANO_EXPORT QTextStream& operator<<( QTextStream& s, const Soprano::LanguageTag& );

#endif // SOPRANO_LANGUAGETAG_H
