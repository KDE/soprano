/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2007-2010 Sebastian Trueg <trueg@kde.org>
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

#include "n3nodeparser.h"
#include "node.h"
#include "error.h"
#include "locator.h"
#include "vocabulary/rdf.h"

#include <QtCore/QRegExp>
#include <QtCore/QUrl>


namespace {
    // an empty delim means: look for space
    bool scanStream( QTextStream& s, QString* str, const QChar& delim = QChar() )
    {
        QChar c;
        int escapeCnt = 0;
        while ( 1 ) {
            if ( s.atEnd() )
                return false;
            s >> c;
            if ( ( delim.isNull() && c.isSpace() ) ||
                 ( c == delim && !escapeCnt ) ) {
                return true;
            }
            else if ( c == '\\') {
                escapeCnt = ( escapeCnt+1 )%2;
            }
            else {
                escapeCnt = 0;
            }
            str->append( c );
        }
    }

    QString& decodeLiteral( QString& s )
    {
        int pos = 0;
        while ( 1 ) {
            pos = s.indexOf( QLatin1String( "\\" ), pos );
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
            else {
                break;
            }
        }
        return s;
    }
}


class Soprano::N3NodeParser::Private
{
public:
    Private()
        : m_prefixedUriRx( "(\\w+)\\:(\\w+)" ) {
    }

    // cached prefix reg exp
    QRegExp m_prefixedUriRx;

    // prefixes that can be used to write something like: "rdf:type"
    QHash<QString, QUrl> m_prefixes;
};


Soprano::N3NodeParser::N3NodeParser()
    : d( new Private() )
{
}


Soprano::N3NodeParser::~N3NodeParser()
{
    delete d;
}


void Soprano::N3NodeParser::setNamespacePrefixes( const QHash<QString, QUrl>& prefixes )
{
    d->m_prefixes = prefixes;
}

void Soprano::N3NodeParser::addNamespacePrefix( const QString& bname, const QUrl& ns )
{
    d->m_prefixes.insert( bname, ns );
}


Soprano::Node Soprano::N3NodeParser::parseNode( QTextStream& s, Node::N3ParserFlags flags ) const
{
    clearError();

    s.skipWhiteSpace();

    Node node;

    // we treat the empty string as an empty node without an error
    if ( s.atEnd() ) {
        return Node();
    }

    QChar c;
    s >> c;

    // parser resource node
    // ============================================
    if ( c == '<' ) {
        QString str;
        if ( scanStream( s, &str, '>' ) ) {
            node = Soprano::Node( QUrl::fromEncoded( str.toAscii(), flags&Node::StrictUris ? QUrl::StrictMode : QUrl::TolerantMode ) );
        }
    }

    // parse blank node
    // ============================================
    else if ( c == '_' ) {
        s >> c;
        if ( c == ':' ) {
            // TODO: restrict the charset
            QString str;
            s >> str;
            if ( !str.isEmpty() )
                node = Soprano::Node::createBlankNode( str );
        }
    }

    // parse literal node
    // FIXME: also handle """ as delimiters
    // ============================================
    else if ( c == '"' ) {
        QString val;
        if ( scanStream( s, &val, '"' ) ) {
            decodeLiteral( val );
            if ( s.atEnd() ) {
                node = LiteralValue::createPlainLiteral( val );
            }
            else {
                QChar c;
                s >> c;
                if ( c.isSpace() ) {
                    node = LiteralValue::createPlainLiteral( val );
                }
                else if ( c == '@' ) {
                    QString lang;
                    s >> lang;
                    node = LiteralValue::createPlainLiteral( val, lang );
                }
                else if ( c == '^' ) {
                    s >> c;
                    if ( c == '^' ) {
                        // recursion: this is a bit evil
                        Node resNode;
                        s >> resNode;
                        if ( resNode.isResource() )
                            node = LiteralValue::fromString( val, resNode.uri() );
                    }
                }
            }
        }
    }

    // handle a few special strings
    else {
        QString token( c );
        scanStream( s, &token );

        // handle the "a" type
        if ( token == QLatin1String( "a" ) ) {
            node = Soprano::Vocabulary::RDF::type();
        }
        else if ( !( flags & Node::StrictLiteralTypes ) ) {
            // boolean values
            if ( token.toLower() == "false" )
                node = Soprano::LiteralValue( false );
            else if ( token.toLower() == "true" )
                node = Soprano::LiteralValue( true );

            // decimal values
            bool ok = false;
            int val = token.toInt( &ok );
            if ( ok ) {
                node = Soprano::LiteralValue( val );
            }
            double dVal = token.toDouble( &ok );
            if ( ok ) {
                node = Soprano::LiteralValue( dVal );
            }
            else {
                node = LiteralValue::createPlainLiteral( token );
            }
        }
        else if ( !( flags & Node::IgnorePrefixes ) &&
                  d->m_prefixedUriRx.exactMatch( token ) ) {
            const QString bname = d->m_prefixedUriRx.cap( 1 );
            const QUrl ns = d->m_prefixes[bname];
            if ( ns.isValid() ) {
                node = QUrl( ns.toString() + d->m_prefixedUriRx.cap( 2 ) );
            }
            else {
                setError( Error::ParserError( Error::Locator( 0, s.pos() ), QString::fromLatin1("Unknown namespace '%1'.").arg(bname) ) );
            }
        }
    }

    if ( !node.isValid() )
        setError( Error::ParserError( Error::Locator( 0, s.pos() ),
                                      QString::fromLatin1( "Got '%1'. Expected one of '<', '\"', '_', 'a'." ).arg( c ) ) );

    return node;
}
