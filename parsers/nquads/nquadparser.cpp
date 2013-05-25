/*
 * This file is part of Soprano Project
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

#include "nquadparser.h"

#include "node.h"
#include "statement.h"
#include "statementiterator.h"
#include "sopranotypes.h"
#include "util/simplestatementiterator.h"
#include "locator.h"

#include <QtCore/QtPlugin>
#include <QtCore/QRegExp>
#include <QtCore/QTextStream>


Q_EXPORT_PLUGIN2(soprano_nquadparser, Soprano::NQuadParser)


Soprano::NQuadParser::NQuadParser()
    : QObject(),
      Parser( "nquads" )
{
}


Soprano::NQuadParser::~NQuadParser()
{
}


Soprano::RdfSerializations Soprano::NQuadParser::supportedSerializations() const
{
    return SerializationNQuads;
}


Soprano::StatementIterator Soprano::NQuadParser::parseStream( QTextStream& stream,
                                                              const QUrl& baseUri,
                                                              RdfSerialization serialization,
                                                              const QString& userSerialization ) const
{
    Q_UNUSED( baseUri );

    clearError();

    if ( serialization == SerializationNQuads ) {
        QList<Statement> sl; // IMPROVEME: it would be better to let an iterator parse step by step
        QString line;
        int row = 1;
        while ( !( line = stream.readLine() ).isNull() ) {
            // skip comments
            if ( !line.startsWith( '#' ) ) {
                Statement s = parseLine( line.trimmed(), row );
                if ( s.isValid() ) {
                    sl += s;
                }
                else {
                    return 0;
                }
            }
            ++row;
        }

        return Util::SimpleStatementIterator( sl );
    }
    else {
        setError( "Unsupported serialization " + serializationMimeType( serialization, userSerialization ),
                  Error::ErrorInvalidArgument );
        return 0;
    }
}


Soprano::Statement Soprano::NQuadParser::parseLine( const QString& line, int row ) const
{
    int offset = 0;

    // parse subject
    Node subject = parseNode( line, offset );
    if ( !subject.isResource() && !subject.isBlank() ) {
        setError( Error::ParserError( Error::Locator( row, offset+1 ), "Subject has to be a resource or blank node" ) );
        return Statement();
    }

    // skip whitespace
    while ( offset < line.length() && line[offset].isSpace() ) {
        ++offset;
    }

    // parse predicate
    Node predicate = parseNode( line, offset );
    if ( !predicate.isResource() ) {
        setError( Error::ParserError( Error::Locator( row, offset+1 ), "Predicate has to be a resource node" ) );
        return Statement();
    }

    // skip whitespace
    while ( offset < line.length() && line[offset].isSpace() ) {
        ++offset;
    }

    // parse object
    Node object = parseNode( line, offset );
    if ( object.isEmpty() ) {
        setError( Error::ParserError( Error::Locator( row, offset+1 ), "Need to have a valid object node" ) );
        return Statement();
    }

    // check if we have a context node
    Node context;
    while ( offset < line.length() && line[offset].isSpace() ) {
        ++offset;
    }
    if ( offset >= line.length() ) {
        setError( Error::ParserError( Error::Locator( row, offset ), "Unexpected end of line" ) );
        return Statement();
    }
    if ( line[offset] != '.' ) {
        context = parseNode( line, offset );
        if ( !context.isResource() ) {
            setError( Error::ParserError( Error::Locator( row, offset+1 ), "Context has to be a resource node" ) );
            return Statement();
        }
    }

    // search for the final dot
    while ( offset < line.length() && line[offset].isSpace() ) {
        ++offset;
    }
    if ( offset >= line.length() ) {
        setError( Error::ParserError( Error::Locator( row, offset ), "Unexpected end of line" ) );
        return Statement();
    }
    else if ( line[offset] != '.' ) {
        setError( Error::ParserError( Error::Locator( row, offset+1 ), "Expected '.' instead of " + line.mid( offset, 1 ) ) );
        return Statement();
    }

    return Statement( subject, predicate, object, context );
}


namespace {
    bool isEscaped( const QString& s, int pos )
    {
        int bsc = 0;
        while ( pos >= 1 ) {
            if ( s[--pos] == '\\' ) {
                bsc++;
            }
            else {
                break;
            }
        }
        return(  bsc % 2 > 0 );
    }

    int findLiteralEnd( const QString& s, int offset, bool* lang )
    {
        int pos = offset;
        do {
            pos = s.indexOf( "\"", pos+1 );
        } while ( pos < s.length() && isEscaped( s, pos ) );

        if ( pos+1 < s.length() ) {
            if ( s[pos+1] == '@' ) {
                *lang = true;
                return pos;
            }
            else {
                *lang = false;
                if ( pos+3 < s.length() ) {
                    if ( s.mid( pos+1, 3 ) == "^^<" ||
                         s[pos+1].isSpace() ) {
                        return pos;
                    }
                }
            }
        }

        return -1;
    }

    QString& decodeLiteral( QString& s )
    {
        int pos = 0;
        while ( pos >= 0 && pos < s.length() ) {
            pos = s.indexOf( "\\", pos );
            if ( pos != -1 && s.length() > pos+1 ) {
                if ( s[pos+1] == '\\' ) {
                    s.remove( pos, 1 );
                }
                else if ( s[pos+1] == 'n' ) {
                    s.replace( pos, 2, '\n' );
                }
                else if ( s[pos+1] == 'r' ) {
                    s.replace( pos, 2, '\r' );
                }
                else if ( s[pos+1] == '\"' ) {
                    s.remove( pos, 1 );
                }
                ++pos;
            }
        }
        return s;
    }
}

Soprano::Node Soprano::NQuadParser::parseNode( const QString& s, int& offset ) const
{
    Node node;

    if ( s.length() > offset ) {

        // parser resource node
        if ( s[offset] == '<' ) {
            int pos = s.indexOf( '>', offset+1 );
            if ( pos > 0 ) {
                node = Soprano::Node( QUrl::fromEncoded( s.mid( offset+1, pos-offset-1 ).toLatin1() ) );
                offset = pos+1;
            }
        }

        // parse blank node
        else if ( s[offset] == '_' && s.length() > offset+2 && s[offset+1] == ':' ) {
            int pos = s.indexOf( QRegExp( "\\s" ), offset+2 );
            if ( pos > 0 ) {
                node = Soprano::Node::createBlankNode( s.mid( offset+2, pos-offset-1 ) );
                offset = pos;
            }
        }

        // parse literal node
        else if ( s[offset] == '"' ) {
            bool hasLangTag = false;
            int literalEndPos = findLiteralEnd( s, offset, &hasLangTag );

            if ( literalEndPos > 0 ) {
                QString value = s.mid( offset+1, literalEndPos-offset-1 );
                decodeLiteral( value );

                if ( hasLangTag ) {
                    int langEnd = s.indexOf( QRegExp( "\\s" ), literalEndPos+1 );
                    if ( langEnd > 0 ) {
                        QString lang = s.mid( literalEndPos+2, langEnd-literalEndPos-2 );
                        node = Node( LiteralValue::createPlainLiteral( value, lang ) );

                        offset = langEnd;
                    }
                }
                else if ( s.length() > literalEndPos+1 && !s[literalEndPos+1].isSpace() ) {
                    int literalTypeEnd = s.indexOf( ">", literalEndPos + 4 );
                    if ( literalTypeEnd > 0 ) {
                        QString literalType = s.mid( literalEndPos + 4, literalTypeEnd - literalEndPos - 4 );
                        node = Soprano::LiteralValue::fromString( value, QUrl::fromEncoded( literalType.toLatin1() ) );

                        offset = literalTypeEnd+1;
                    }
                }
                else {
                    node = Node( LiteralValue::createPlainLiteral( value ) );
                    offset = literalEndPos+1;
                }
            }
        }
    }

    return node;
}

