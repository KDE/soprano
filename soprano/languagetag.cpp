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

#include "languagetag.h"

#include <QtCore/QStringList>
#include <QtCore/QHash>
#include <QtCore/QDebug>
#include <QtCore/QTextStream>

namespace {
    struct LanguageTagPrivateConstants
    {
        const QString matchWildcard;
        const QString privateUse;
        const QChar langDelimiter;

        LanguageTagPrivateConstants():
            matchWildcard("*"),
            privateUse("x"),
            langDelimiter('-')
        {
        }
    };

    Q_GLOBAL_STATIC(LanguageTagPrivateConstants, constants)
}


class Soprano::LanguageTag::Private : public QSharedData
{
public:
    QString normalizedTag;

    static QString normalizeString( const QString& str );
    static bool matchBasic( const QString& tag, const QString& range );
    static bool matchBasic( const QStringList& tagSub, const QStringList& rangeSub );
    static bool matchExtended( const LanguageTag& tag, const LanguageTag& range );
    static bool matchExtended( const QStringList& tagSub, const QStringList& rangeSub );

    static const unsigned char language_code_list[];
    static const unsigned char country_code_list[];

    static const unsigned char * langToPtr(QLocale::Language lang);
    static const unsigned char * countryToPtr(QLocale::Country country);

    static QString langToString(QLocale::Language lang);
    static QString countryToString(QLocale::Country country);
};


QString Soprano::LanguageTag::Private::normalizeString( const QString& str )
{
    return str.toLower();
}

bool Soprano::LanguageTag::Private::matchBasic( const QString& tag, const QString& range )
{
    // Wildcard match
    if (range == constants()->matchWildcard) return true;

    // Trivial reject
    if (range.size() > tag.size()) return false;

    // Trivial accept
    if (tag == range) return true;

    // Can't be a prefix if the sizes are the same
    if (range.size() == tag.size()) return false;

    if (tag.leftRef(range.size()) == range
        && tag.at(range.size()) == constants()->langDelimiter) {

        // range is a prefix of tag, and it is immediately followed by -
        return true;
    }

    return false;
}

bool Soprano::LanguageTag::Private::matchBasic( const QStringList& tagSub, const QStringList& rangeSub )
{
    // Invalid range
    if (rangeSub.isEmpty()) return false;

    // Wildcard match
    if (rangeSub.size() == 1 && rangeSub[0] == constants()->matchWildcard) return true;

    int rangeSize(rangeSub.size());

    // Trivial reject
    if (rangeSize > tagSub.size()) return false;

    for (int i = 0; i < rangeSize; ++i) {
        if (tagSub[i] != rangeSub[i]) return false;
    }

    return true;
}

bool Soprano::LanguageTag::Private::matchExtended( const LanguageTag& tag, const LanguageTag& range )
{
    // Trivial accept
    if (tag == range) return true;

    // Step 1 from RFC 4647 section 3.3.2.
    return matchExtended(tag.subTags(), range.subTags());
}

bool Soprano::LanguageTag::Private::matchExtended( const QStringList& tagSub, const QStringList& rangeSub )
{
    // The steps listed in the comments here come from RFC 4647 section 3.3.2.
    // Step 1 is taken above

    // Step 2
    if (tagSub[0] != rangeSub[0] && rangeSub[0] != constants()->matchWildcard) return false;

    int t(1), r(1);

    // Step 3
    while (r < rangeSub.size()) {
        QString curRangeSub(rangeSub[r]);

        // Step 3A
        if (curRangeSub == constants()->matchWildcard) {
            ++r;
            continue;
        }

        // Step 3B
        if (t >= tagSub.size()) return false;

        QString curTagSub(tagSub[t]);

        // Step 3C
        if (curTagSub == curRangeSub) {
            ++t;
            ++r;
            continue;
        }

        // Step 3D
        if (curTagSub.size() == 1) return false;

        // Step 3E
        ++t;
    }

    return true;
}

const unsigned char* Soprano::LanguageTag::Private::langToPtr(QLocale::Language lang)
{
    if (lang < 0 || lang > QLocale::LastLanguage) return 0;
    return &language_code_list[static_cast<int>(lang) * 3];
}

const unsigned char* Soprano::LanguageTag::Private::countryToPtr(QLocale::Country country)
{
    if (country < 0 || country > QLocale::LastCountry) return 0;
    return &country_code_list[static_cast<int>(country) * 2];
}

QString Soprano::LanguageTag::Private::langToString(QLocale::Language lang)
{
    const unsigned char * ptr = langToPtr(lang);
    if (!ptr) return QString();

    int i = 0;
    while (i < 3) {
        if (ptr[i] == 0) break;
        ++i;
    }
    return QString::fromLatin1(reinterpret_cast<const char *>(ptr), i);
}

QString Soprano::LanguageTag::Private::countryToString(QLocale::Country country)
{
    return QString::fromLatin1(reinterpret_cast<const char *>(countryToPtr(country)), 2);
}

Soprano::LanguageTag::LanguageTag():
    d(0)
{
}

Soprano::LanguageTag::LanguageTag( const LanguageTag &other ):
    d(other.d)
{
}

Soprano::LanguageTag::LanguageTag( const char * tag ):
    d(0)
{
    QString norm(Private::normalizeString(QString::fromLatin1(tag)));
    if (!norm.isEmpty()) {
        d = new Private;
        d->normalizedTag = norm;
    }
}

Soprano::LanguageTag::LanguageTag( const QLatin1String &tag ):
    d(0)
{
    QString norm(Private::normalizeString(tag));
    if (!norm.isEmpty()) {
        d = new Private;
        d->normalizedTag = norm;
    }
}

Soprano::LanguageTag::LanguageTag( const QString &tag ):
    d(0)
{
    QString norm(Private::normalizeString(tag));
    if (!norm.isEmpty()) {
        d = new Private;
        d->normalizedTag = norm;
    }
}

Soprano::LanguageTag::LanguageTag( const QLocale &locale ):
    d(0)
{
    if (locale.language() != QLocale::C) {
        QString norm(Private::normalizeString(locale.name().replace(QChar('_'), constants()->langDelimiter)));
        if (!norm.isEmpty()) {
            d = new Private;
            d->normalizedTag = norm;
        }
    }
}

Soprano::LanguageTag::LanguageTag( QLocale::Language lang, QLocale::Country country ):
    d(0)
{
    QString langStr(Private::langToString(lang));
    QString countryStr;

    if (!langStr.isEmpty() && langStr != QLatin1String("  ")) {
        if (country != QLocale::AnyCountry) {
            countryStr = Private::countryToString(country);
        }

        if (!countryStr.isEmpty()) {
            langStr.append(constants()->langDelimiter).append(countryStr);
        }

        d = new Private;
        d->normalizedTag = Private::normalizeString(langStr);
    }
}

Soprano::LanguageTag::~LanguageTag()
{
}

Soprano::LanguageTag& Soprano::LanguageTag::operator=( const LanguageTag& other )
{
    d = other.d;

    return *this;
}

bool Soprano::LanguageTag::operator==( const LanguageTag& other ) const
{
    if (!d && !other.d) return true;
    if (!d || !other.d) return false;

    return d->normalizedTag == other.d->normalizedTag;
}

bool Soprano::LanguageTag::operator!=( const LanguageTag& other ) const
{
    if (!d && !other.d) return false;
    if (!d || !other.d) return true;

    return d->normalizedTag != other.d->normalizedTag;
}

bool Soprano::LanguageTag::operator<( const LanguageTag& other ) const
{
    if (!d && !other.d) return false;
    if (!d)             return true;
    if (!other.d)       return false;

    return d->normalizedTag < other.d->normalizedTag;
}

bool Soprano::LanguageTag::operator<=( const LanguageTag& other ) const
{
    if (!d && !other.d) return true;
    if (!d)             return true;
    if (!other.d)       return false;

    return d->normalizedTag <= other.d->normalizedTag;
}

bool Soprano::LanguageTag::operator>( const LanguageTag& other ) const
{
    if (!d && !other.d) return false;
    if (!d)             return false;
    if (!other.d)       return true;

    return d->normalizedTag > other.d->normalizedTag;
}

bool Soprano::LanguageTag::operator>=( const LanguageTag& other ) const
{
    if (!d && !other.d) return true;
    if (!d)             return false;
    if (!other.d)       return true;

    return d->normalizedTag >= other.d->normalizedTag;
}

bool Soprano::LanguageTag::matches( const LanguageTag& range, MatchFilter scheme ) const
{
    switch (scheme) {
    case MatchFilterBasic:
        return Private::matchBasic( toString(), range.toString() );
    case MatchFilterExtended:
        return Private::matchExtended( *this, range );
    default:
        return false;
    }
}

int Soprano::LanguageTag::lookup(
    const QList<LanguageTag> & choices,
    const QList<LanguageTag> & priority,
    LookupFlags flags,
    MatchFilter scheme
    )
{
    QList<QStringList> splitChoices;

    Q_FOREACH (const LanguageTag& choice, choices) {
        if (choice.toString() == constants()->matchWildcard) continue;
        splitChoices.append(choice.subTags());
    }

    int choiceCount(splitChoices.size());
    if (choiceCount == 0) return -1;


    Q_FOREACH (const LanguageTag& tag, priority) {
        QStringList tagSub(tag.subTags());

        do {
            for (int i = 0; i < choiceCount; ++i) {
                QStringList choiceSub(splitChoices[i]);

                bool matched;
                switch (scheme) {
                case MatchFilterBasic:
                    matched = Private::matchBasic( choiceSub, tagSub );
                    break;
                case MatchFilterExtended:
                    matched = Private::matchExtended( choiceSub, tagSub );
                    break;
                default:
                    // Always fails
                    return -1;
                }

                if (matched) return i;
            }

            if (flags & LookupFlagNoFallback) break;
            // Truncate the end
            do {
                tagSub.removeLast();
            } while (!tagSub.isEmpty() && tagSub.last().size() == 1);
        } while (!tagSub.isEmpty());
    }

    return -1;
}

int Soprano::LanguageTag::lookup(
    const QList<LanguageTag> & choices,
    const LanguageTag & priority,
    LookupFlags flags,
    MatchFilter scheme
    )
{
    return lookup(choices, QList<LanguageTag>() << priority, flags, scheme);
}

bool Soprano::LanguageTag::isEmpty() const
{
    return !d;
}

static inline bool isAlpha(QChar c)
{
    return (c >= 'A' && c <= 'Z')
        || (c >= 'a' && c <= 'z');
}

static inline bool isDigit(QChar c)
{
    return (c >= '0' && c <= '9');
}

static inline bool isAlphaNum(QChar c)
{
    return isAlpha(c) || isDigit(c);
}

bool Soprano::LanguageTag::isValid() const
{
    QStringList st(subTags());

    if (st.isEmpty()) return false;

    bool privateUse(false);
    int s, sMax(st.size());
    for (s = 0; s < sMax; ++s) {
        const QChar * data = st[s].constData();

        int i, iMax(st[s].size());
        if (iMax == 0 || iMax > 8) return false;

        if (s == 0) {
            // Letters only for the primary subtag
            for (i = 0; i < iMax; ++i) {
                if (!isAlpha(data[i])) return false;
            }
        } else {
            for (i = 0; i < iMax; ++i) {
                if (!isAlphaNum(data[i])) return false;
            }
        }

        if (!privateUse) {
            if (iMax == 1 && s == sMax - 1) return false;

            if (st[s] == constants()->privateUse) {
                privateUse = true;
            }
        }
    }

    return true;
}

QStringList Soprano::LanguageTag::subTags() const
{
    return toString().split(constants()->langDelimiter);
}

QString Soprano::LanguageTag::toString() const
{
    return d ? d->normalizedTag : QString();
}

QString Soprano::LanguageTag::toPrettyString() const
{
    QStringList parts(subTags());

    parts[0] = parts[0].toLower();

    for (int i = 1; i < parts.size(); ++i) {
        if (parts[i].size() == 2) {
            parts[i] = parts[i].toUpper();
        } else if (parts[i].size() == 4) {
            parts[i] = parts[i].toLower();
            parts[i][0] = parts[i][0].toTitleCase();
        } else {
            parts[i] = parts[i].toLower();
        }
    }

    return parts.join(constants()->langDelimiter);
}

QLocale Soprano::LanguageTag::toLocale() const
{
    if (toString() == constants()->matchWildcard) {
        // Returns the default locale
        return QLocale();
    }

    QStringList parts(subTags());

    QString name(parts[0]), country;

    for (int i = 1; i < parts.size(); ++i) {
        if (parts[i] == constants()->privateUse) break;
        if (parts[i].size() == 2) {
            // Qt requires uppercase for country codes
            country = parts[i].toUpper();
            break;
        }
    }

    if (!country.isNull()) {
        name.append(QChar('_')).append(country);
    }
    return QLocale(name);
}

uint Soprano::qHash( const LanguageTag& tag )
{
    return ::qHash( tag.toString() );
}


QDebug operator<<( QDebug s, const Soprano::LanguageTag& tag )
{
    if (tag.isEmpty()) {
        return s << "(empty)";
    } else {
        return s << tag.toString();
    }
}

QTextStream& operator<<( QTextStream& s, const Soprano::LanguageTag& tag )
{
    if (tag.isEmpty()) {
        return s << "(empty)";
    } else {
        return s << tag.toString();
    }
}

/**
 * Copied from the Qt source qlocale_data_p.h until Qt exposes it.
 */
const unsigned char Soprano::LanguageTag::Private::language_code_list[] =
    "  \0" // Unused
    "  \0" // C
    "ab\0" // Abkhazian
    "om\0" // Afan
    "aa\0" // Afar
    "af\0" // Afrikaans
    "sq\0" // Albanian
    "am\0" // Amharic
    "ar\0" // Arabic
    "hy\0" // Armenian
    "as\0" // Assamese
    "ay\0" // Aymara
    "az\0" // Azerbaijani
    "ba\0" // Bashkir
    "eu\0" // Basque
    "bn\0" // Bengali
    "dz\0" // Bhutani
    "bh\0" // Bihari
    "bi\0" // Bislama
    "br\0" // Breton
    "bg\0" // Bulgarian
    "my\0" // Burmese
    "be\0" // Byelorussian
    "km\0" // Cambodian
    "ca\0" // Catalan
    "zh\0" // Chinese
    "co\0" // Corsican
    "hr\0" // Croatian
    "cs\0" // Czech
    "da\0" // Danish
    "nl\0" // Dutch
    "en\0" // English
    "eo\0" // Esperanto
    "et\0" // Estonian
    "fo\0" // Faroese
    "fj\0" // Fiji
    "fi\0" // Finnish
    "fr\0" // French
    "fy\0" // Frisian
    "gd\0" // Gaelic
    "gl\0" // Galician
    "ka\0" // Georgian
    "de\0" // German
    "el\0" // Greek
    "kl\0" // Greenlandic
    "gn\0" // Guarani
    "gu\0" // Gujarati
    "ha\0" // Hausa
    "he\0" // Hebrew
    "hi\0" // Hindi
    "hu\0" // Hungarian
    "is\0" // Icelandic
    "id\0" // Indonesian
    "ia\0" // Interlingua
    "ie\0" // Interlingue
    "iu\0" // Inuktitut
    "ik\0" // Inupiak
    "ga\0" // Irish
    "it\0" // Italian
    "ja\0" // Japanese
    "jv\0" // Javanese
    "kn\0" // Kannada
    "ks\0" // Kashmiri
    "kk\0" // Kazakh
    "rw\0" // Kinyarwanda
    "ky\0" // Kirghiz
    "ko\0" // Korean
    "ku\0" // Kurdish
    "rn\0" // Kurundi
    "lo\0" // Laothian
    "la\0" // Latin
    "lv\0" // Latvian
    "ln\0" // Lingala
    "lt\0" // Lithuanian
    "mk\0" // Macedonian
    "mg\0" // Malagasy
    "ms\0" // Malay
    "ml\0" // Malayalam
    "mt\0" // Maltese
    "mi\0" // Maori
    "mr\0" // Marathi
    "mo\0" // Moldavian
    "mn\0" // Mongolian
    "na\0" // Nauru
    "ne\0" // Nepali
    "nb\0" // Norwegian
    "oc\0" // Occitan
    "or\0" // Oriya
    "ps\0" // Pashto
    "fa\0" // Persian
    "pl\0" // Polish
    "pt\0" // Portuguese
    "pa\0" // Punjabi
    "qu\0" // Quechua
    "rm\0" // RhaetoRomance
    "ro\0" // Romanian
    "ru\0" // Russian
    "sm\0" // Samoan
    "sg\0" // Sangho
    "sa\0" // Sanskrit
    "sr\0" // Serbian
    "sh\0" // SerboCroatian
    "st\0" // Sesotho
    "tn\0" // Setswana
    "sn\0" // Shona
    "sd\0" // Sindhi
    "si\0" // Singhalese
    "ss\0" // Siswati
    "sk\0" // Slovak
    "sl\0" // Slovenian
    "so\0" // Somali
    "es\0" // Spanish
    "su\0" // Sundanese
    "sw\0" // Swahili
    "sv\0" // Swedish
    "tl\0" // Tagalog
    "tg\0" // Tajik
    "ta\0" // Tamil
    "tt\0" // Tatar
    "te\0" // Telugu
    "th\0" // Thai
    "bo\0" // Tibetan
    "ti\0" // Tigrinya
    "to\0" // Tonga
    "ts\0" // Tsonga
    "tr\0" // Turkish
    "tk\0" // Turkmen
    "tw\0" // Twi
    "ug\0" // Uigur
    "uk\0" // Ukrainian
    "ur\0" // Urdu
    "uz\0" // Uzbek
    "vi\0" // Vietnamese
    "vo\0" // Volapuk
    "cy\0" // Welsh
    "wo\0" // Wolof
    "xh\0" // Xhosa
    "yi\0" // Yiddish
    "yo\0" // Yoruba
    "za\0" // Zhuang
    "zu\0" // Zulu
    "nn\0" // Nynorsk
    "bs\0" // Bosnian
    "dv\0" // Divehi
    "gv\0" // Manx
    "kw\0" // Cornish
    "ak\0" // Akan
    "kok" // Konkani
    "gaa" // Ga
    "ig\0" // Igbo
    "kam" // Kamba
    "syr" // Syriac
    "byn" // Blin
    "gez" // Geez
    "kfo" // Koro
    "sid" // Sidamo
    "cch" // Atsam
    "tig" // Tigre
    "kaj" // Jju
    "fur" // Friulian
    "ve\0" // Venda
    "ee\0" // Ewe
    "wa\0" // Walamo
    "haw" // Hawaiian
    "kcg" // Tyap
    "ny\0" // Chewa
    ;

/**
 * Copied from the Qt source qlocale_data_p.h until Qt exposes it.
 */
const unsigned char Soprano::LanguageTag::Private::country_code_list[] =
    "  " // AnyCountry
    "AF" // Afghanistan
    "AL" // Albania
    "DZ" // Algeria
    "AS" // AmericanSamoa
    "AD" // Andorra
    "AO" // Angola
    "AI" // Anguilla
    "AQ" // Antarctica
    "AG" // AntiguaAndBarbuda
    "AR" // Argentina
    "AM" // Armenia
    "AW" // Aruba
    "AU" // Australia
    "AT" // Austria
    "AZ" // Azerbaijan
    "BS" // Bahamas
    "BH" // Bahrain
    "BD" // Bangladesh
    "BB" // Barbados
    "BY" // Belarus
    "BE" // Belgium
    "BZ" // Belize
    "BJ" // Benin
    "BM" // Bermuda
    "BT" // Bhutan
    "BO" // Bolivia
    "BA" // BosniaAndHerzegowina
    "BW" // Botswana
    "BV" // BouvetIsland
    "BR" // Brazil
    "IO" // BritishIndianOceanTerritory
    "BN" // BruneiDarussalam
    "BG" // Bulgaria
    "BF" // BurkinaFaso
    "BI" // Burundi
    "KH" // Cambodia
    "CM" // Cameroon
    "CA" // Canada
    "CV" // CapeVerde
    "KY" // CaymanIslands
    "CF" // CentralAfricanRepublic
    "TD" // Chad
    "CL" // Chile
    "CN" // China
    "CX" // ChristmasIsland
    "CC" // CocosIslands
    "CO" // Colombia
    "KM" // Comoros
    "CD" // DemocraticRepublicOfCongo
    "CG" // PeoplesRepublicOfCongo
    "CK" // CookIslands
    "CR" // CostaRica
    "CI" // IvoryCoast
    "HR" // Croatia
    "CU" // Cuba
    "CY" // Cyprus
    "CZ" // CzechRepublic
    "DK" // Denmark
    "DJ" // Djibouti
    "DM" // Dominica
    "DO" // DominicanRepublic
    "TL" // EastTimor
    "EC" // Ecuador
    "EG" // Egypt
    "SV" // ElSalvador
    "GQ" // EquatorialGuinea
    "ER" // Eritrea
    "EE" // Estonia
    "ET" // Ethiopia
    "FK" // FalklandIslands
    "FO" // FaroeIslands
    "FJ" // Fiji
    "FI" // Finland
    "FR" // France
    "FX" // MetropolitanFrance
    "GF" // FrenchGuiana
    "PF" // FrenchPolynesia
    "TF" // FrenchSouthernTerritories
    "GA" // Gabon
    "GM" // Gambia
    "GE" // Georgia
    "DE" // Germany
    "GH" // Ghana
    "GI" // Gibraltar
    "GR" // Greece
    "GL" // Greenland
    "GD" // Grenada
    "GP" // Guadeloupe
    "GU" // Guam
    "GT" // Guatemala
    "GN" // Guinea
    "GW" // GuineaBissau
    "GY" // Guyana
    "HT" // Haiti
    "HM" // HeardAndMcDonaldIslands
    "HN" // Honduras
    "HK" // HongKong
    "HU" // Hungary
    "IS" // Iceland
    "IN" // India
    "ID" // Indonesia
    "IR" // Iran
    "IQ" // Iraq
    "IE" // Ireland
    "IL" // Israel
    "IT" // Italy
    "JM" // Jamaica
    "JP" // Japan
    "JO" // Jordan
    "KZ" // Kazakhstan
    "KE" // Kenya
    "KI" // Kiribati
    "KP" // DemocraticRepublicOfKorea
    "KR" // RepublicOfKorea
    "KW" // Kuwait
    "KG" // Kyrgyzstan
    "LA" // Lao
    "LV" // Latvia
    "LB" // Lebanon
    "LS" // Lesotho
    "LR" // Liberia
    "LY" // LibyanArabJamahiriya
    "LI" // Liechtenstein
    "LT" // Lithuania
    "LU" // Luxembourg
    "MO" // Macau
    "MK" // Macedonia
    "MG" // Madagascar
    "MW" // Malawi
    "MY" // Malaysia
    "MV" // Maldives
    "ML" // Mali
    "MT" // Malta
    "MH" // MarshallIslands
    "MQ" // Martinique
    "MR" // Mauritania
    "MU" // Mauritius
    "YT" // Mayotte
    "MX" // Mexico
    "FM" // Micronesia
    "MD" // Moldova
    "MC" // Monaco
    "MN" // Mongolia
    "MS" // Montserrat
    "MA" // Morocco
    "MZ" // Mozambique
    "MM" // Myanmar
    "NA" // Namibia
    "NR" // Nauru
    "NP" // Nepal
    "NL" // Netherlands
    "AN" // NetherlandsAntilles
    "NC" // NewCaledonia
    "NZ" // NewZealand
    "NI" // Nicaragua
    "NE" // Niger
    "NG" // Nigeria
    "NU" // Niue
    "NF" // NorfolkIsland
    "MP" // NorthernMarianaIslands
    "NO" // Norway
    "OM" // Oman
    "PK" // Pakistan
    "PW" // Palau
    "PS" // PalestinianTerritory
    "PA" // Panama
    "PG" // PapuaNewGuinea
    "PY" // Paraguay
    "PE" // Peru
    "PH" // Philippines
    "PN" // Pitcairn
    "PL" // Poland
    "PT" // Portugal
    "PR" // PuertoRico
    "QA" // Qatar
    "RE" // Reunion
    "RO" // Romania
    "RU" // RussianFederation
    "RW" // Rwanda
    "KN" // SaintKittsAndNevis
    "LC" // StLucia
    "VC" // StVincentAndTheGrenadines
    "WS" // Samoa
    "SM" // SanMarino
    "ST" // SaoTomeAndPrincipe
    "SA" // SaudiArabia
    "SN" // Senegal
    "SC" // Seychelles
    "SL" // SierraLeone
    "SG" // Singapore
    "SK" // Slovakia
    "SI" // Slovenia
    "SB" // SolomonIslands
    "SO" // Somalia
    "ZA" // SouthAfrica
    "GS" // SouthGeorgiaAndTheSouthSandwichIslands
    "ES" // Spain
    "LK" // SriLanka
    "SH" // StHelena
    "PM" // StPierreAndMiquelon
    "SD" // Sudan
    "SR" // Suriname
    "SJ" // SvalbardAndJanMayenIslands
    "SZ" // Swaziland
    "SE" // Sweden
    "CH" // Switzerland
    "SY" // SyrianArabRepublic
    "TW" // Taiwan
    "TJ" // Tajikistan
    "TZ" // Tanzania
    "TH" // Thailand
    "TG" // Togo
    "TK" // Tokelau
    "TO" // Tonga
    "TT" // TrinidadAndTobago
    "TN" // Tunisia
    "TR" // Turkey
    "TM" // Turkmenistan
    "TC" // TurksAndCaicosIslands
    "TV" // Tuvalu
    "UG" // Uganda
    "UA" // Ukraine
    "AE" // UnitedArabEmirates
    "GB" // UnitedKingdom
    "US" // UnitedStates
    "UM" // UnitedStatesMinorOutlyingIslands
    "UY" // Uruguay
    "UZ" // Uzbekistan
    "VU" // Vanuatu
    "VA" // VaticanCityState
    "VE" // Venezuela
    "VN" // VietNam
    "VG" // BritishVirginIslands
    "VI" // USVirginIslands
    "WF" // WallisAndFutunaIslands
    "EH" // WesternSahara
    "YE" // Yemen
    "YU" // Yugoslavia
    "ZM" // Zambia
    "ZW" // Zimbabwe
    "CS" // SerbiaAndMontenegro
    ;
