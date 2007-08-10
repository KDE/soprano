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

#include "inferenceruleparser.h"
#include "inferencerule.h"
#include "nodepattern.h"
#include "statementpattern.h"
#include "node.h"

#include <QtCore/QStringList>
#include <QtCore/QTextStream>
#include <QtCore/QFile>
#include <QtCore/QMap>
#include <QtCore/QList>
#include <QtCore/QDebug>
#include <QtCore/QUrl>


class Soprano::Inference::RuleParser::Private
{
public:
    Private()
        : prefixLine( "(?:PREFIX|prefix)\\s+(\\S+)\\s+<(\\S+)>" ),
          ruleLine( "\\[" "\\s*" "(\\w+)\\:" "\\s*" "(\\([^\\)]+\\))" "(?:\\s*\\,\\s*(\\([^\\)]+\\)))*" "\\s*" "\\-\\>" "\\s*" "(\\([^\\)]+\\))" "\\s*" "\\]" ),
          statementPattern( "\\(" "(\\?\\w|\\<\\S+\\>|[^\\<\\>\\s]+)" "\\s*" "(\\?\\w|\\<\\S+\\>|[^\\<\\>\\s]+)" "\\s*" "(\\?\\w|\\<\\S+\\>|[^\\<\\>\\s]+)" "\\)" ) {
    }


    NodePattern parseNodePattern( const QString& s, bool* success ) {
        if ( s[0] == '?' ) {
            *success = true;
            return Soprano::Inference::NodePattern( s.mid( 1 ) );
        }
        else if ( s[0] == '<' ) {
            *success = true;
            return Soprano::Inference::NodePattern( Soprano::Node( QUrl( s.mid( 1, s.length()-2 ) ) ) );
        }
        else {
            QString prefix = s.left( s.indexOf( ':' )+1 );
            if ( !prefixes.contains( prefix ) ) {
                qDebug() << "Could not find prefix" << prefix;
                *success = false;
                return Soprano::Inference::NodePattern();
            }
            else {
                *success = true;
                return Soprano::Inference::NodePattern( Node( QUrl( prefixes[prefix] + s.mid( s.indexOf( ':' )+1 ) ) ) );
            }
        }
    }

    StatementPattern parseMatchedStatementPattern( bool* success ) {
        QString subject = statementPattern.cap( 1 );
        QString predicate = statementPattern.cap( 2 );
        QString object = statementPattern.cap( 3 );

//        qDebug() << "Parsed statement pattern: " << subject << predicate << object;

        bool s1, s2, s3;
        Soprano::Inference::NodePattern subjectPattern = parseNodePattern( subject, &s1 );
        Soprano::Inference::NodePattern predicatePattern = parseNodePattern( predicate, &s2 );
        Soprano::Inference::NodePattern objectPattern = parseNodePattern( object, &s3 );

        *success = s1 && s2 && s3;

        return Soprano::Inference::StatementPattern( subjectPattern,
                                                     predicatePattern,
                                                     objectPattern );
    }

    QList<Rule> rules;
    QMap<QString, QString> prefixes;

    QRegExp prefixLine;
    QRegExp ruleLine;
    QRegExp statementPattern;
};


Soprano::Inference::RuleParser::RuleParser()
    : d( new Private() )
{
}


Soprano::Inference::RuleParser::~RuleParser()
{
    delete d;
}


bool Soprano::Inference::RuleParser::parseFile( const QString& path )
{
    // cleanup first
    d->rules.clear();
    d->prefixes.clear();

    QFile file( path );
    if ( file.open( QIODevice::ReadOnly ) ) {
        QTextStream s( &file );
        QString line;
        while ( !( line = s.readLine().trimmed() ).isNull() ) {
            if ( line.startsWith( '#' ) ||
                 line.isEmpty() ) {
                // do nothing
            }
            else if ( d->prefixLine.exactMatch( line ) ) {
                qDebug() << "Found prefix line:" << d->prefixLine.cap( 1 ) << "->" << d->prefixLine.cap( 2 );
                d->prefixes.insert( d->prefixLine.cap( 1 ), d->prefixLine.cap( 2 ) );
            }
            else if ( d->ruleLine.exactMatch( line ) ) {
                Rule newRule;
                bool success = true;

                // start with the effect statement pattern
                int effectPos = d->statementPattern.lastIndexIn( line );
                newRule.setEffect( d->parseMatchedStatementPattern( &success ) );

                // get out all the condition patterns
                int pos = 0;
                while ( ( pos = d->statementPattern.indexIn( line, pos ) ) != -1 &&
                        pos < effectPos ) {
                    newRule.addPrecondition( d->parseMatchedStatementPattern( &success ) );
                    if ( !success ) {
                        return false;
                    }
                    pos += d->statementPattern.matchedLength();
                }

                d->rules.append( newRule );
            }
            else {
                qDebug() << "Failed to parse line: " << line;
                return false;
            }
        }

        return true;
    }
    else {
        qDebug() << "Failed to open rule file" << path;
        return false;
    }
}


QList<Soprano::Inference::Rule> Soprano::Inference::RuleParser::rules() const
{
    return d->rules;
}
