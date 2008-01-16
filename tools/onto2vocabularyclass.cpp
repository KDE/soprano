/*
 *
 * $Id: sourceheader 511311 2006-02-19 14:51:05Z trueg $
 *
 * This file is part of the Soprano project.
 * Copyright (C) 2007 Sebastian Trueg <trueg@kde.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * See the file "COPYING" for the exact licensing terms.
 */

#include <QtCore/QCoreApplication>
#include <QtCore/QTime>
#include <QtCore/QTextStream>
#include <QtCore/QDebug>
#include <QtCore/QList>
#include <QtCore/QDir>
#include <QtCore/QRegExp>

#include "../soprano/statementiterator.h"
#include "../soprano/nodeiterator.h"
#include "../soprano/version.h"
#include "../soprano/pluginmanager.h"
#include "../soprano/parser.h"
#include "../soprano/node.h"
#include "../soprano/vocabulary/rdfs.h"


using namespace Soprano;

static const char* LGPL_HEADER = "/*\n"
                                 " * This file is part of Soprano Project.\n"
                                 " *\n"
                                 " * Copyright (C) 2007 Sebastian Trueg <trueg@kde.org>\n"
                                 " *\n"
                                 " * This library is free software; you can redistribute it and/or\n"
                                 " * modify it under the terms of the GNU Library General Public\n"
                                 " * License as published by the Free Software Foundation; either\n"
                                 " * version 2 of the License, or (at your option) any later version.\n"
                                 " *\n"
                                 " * This library is distributed in the hope that it will be useful,\n"
                                 " * but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
                                 " * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU\n"
                                 " * Library General Public License for more details.\n"
                                 " *\n"
                                 " * You should have received a copy of the GNU Library General Public License\n"
                                 " * along with this library; see the file COPYING.LIB.  If not, write to\n"
                                 " * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,\n"
                                 " * Boston, MA 02110-1301, USA.\n"
                                 " */\n";


#define VERSION "0.1"

int version()
{
    QTextStream s( stderr );
    s << "onto2vocabularyclass " << VERSION << " (using Soprano " << Soprano::versionString() << ")" << endl;
    s << "   Copyright (C) 2007 Sebastian Trueg <trueg@kde.org>" << endl;
    s << "   This program is free software; you can redistribute it and/or modify" << endl
      << "   it under the terms of the GNU General Public License as published by" << endl
      << "   the Free Software Foundation; either version 2 of the License, or" << endl
      << "   (at your option) any later version." << endl;

    return 0;
}


int usage( const QString& error = QString() )
{
    version();

    QTextStream s( stderr );
    s << endl;
    s << "Usage:" << endl
      << "   onto2vocabularyclass --name <name> --encoding <encoding> [--namespace <ns>] [--no-visibility-export] <ontologyfile>" << endl;

    if ( !error.isEmpty() ) {
        s << endl << error << endl;
        return 0;
    }
    else {
        return 1;
    }
}


QString createIndent( int indent )
{
    QString s;
    for ( int i = 0; i < indent; ++i ) {
        s += "    ";
    }
    return s;
}


QString writeComment( const QString& comment, int indent )
{
    static const int maxLine = 50;

    QString s;

    if( !comment.isEmpty() ) {
        s += createIndent( indent );
        s += "/**\n";

        QStringList paragraphs = comment.split( '\n', QString::KeepEmptyParts );
        Q_FOREACH( QString paragraph, paragraphs ) {
            s += createIndent( indent ) + " * ";
            QStringList words = paragraph.split( QRegExp("\\s"), QString::SkipEmptyParts );
            int cnt = 0;
            for( int i = 0; i < words.count(); ++i ) {
                if( cnt >= maxLine ) {
                    s += '\n'
                         + createIndent( indent ) + " * ";
                    cnt = 0;
                }

                s += words[i] + ' ';
                cnt += words[i].length();
            }
            s += '\n';
        }


        s += createIndent( indent ) + " */";
    }

    return s;
}



int main( int argc, char *argv[] )
{
    QCoreApplication app( argc, argv );

    QStringList args = app.arguments();
    QString fileName;
    QString className;
    QString namespaceName;
    QString encoding;
    bool visibilityExport = true;
    int i = 1;
    while ( i < args.count() ) {
        if ( args[i] == "--encoding" ) {
            ++i;
            if ( i < args.count() ) {
                encoding = args[i];
            }
            else {
                return usage();
            }
        }
        else if ( args[i] == "--name" ) {
            ++i;
            if ( i < args.count() ) {
                className = args[i];
            }
            else {
                return usage();
            }
        }
        else if ( args[i] == "--namespace" ) {
            ++i;
            if ( i < args.count() ) {
                namespaceName = args[i];
            }
            else {
                return usage();
            }
        }
        else if ( args[i] == "--no-visibility-export" ) {
            visibilityExport = false;
        }
        else if ( i == args.count()-1 ) {
            fileName = args[i];
        }
        else {
            QTextStream s( stderr );
            s << "Unknown argument:" << args[i] << endl << endl;
            return usage();
        }
        ++i;
    }

    if ( fileName.isEmpty() ) {
        return usage();
    }
    if ( encoding.isEmpty() ) {
        return usage();
    }

    if ( !QFile::exists( fileName ) ) {
        QTextStream s( stderr );
        s << "Could not find file" << fileName << endl;
        return 1;
    }

    const Parser* parser = PluginManager::instance()->discoverParserForSerialization( mimeTypeToSerialization( encoding ), encoding );
    if ( !parser ) {
        QTextStream s( stderr );
        s << "Could not find parser plugin for encoding " << encoding << endl;
        return 1;
    }

    StatementIterator it = parser->parseFile( fileName, QUrl( "http://dummybaseuri.org" ), mimeTypeToSerialization( encoding ), encoding );
    if ( parser->lastError() ) {
        QTextStream s( stderr );
        s << "Failed to parse file" << fileName << "(" << parser->lastError() << ")" << endl;
        return 1;
    }

    QFile headerFile( className.toLower() + ".h" );
    QFile sourceFile( className.toLower() + ".cpp" );

    if ( !headerFile.open( QIODevice::WriteOnly ) ) {
        QTextStream s( stderr );
        s << "Failed to open file" << headerFile.fileName() << endl;
        return 1;
    }

    if ( !sourceFile.open( QIODevice::WriteOnly ) ) {
        QTextStream s( stderr );
        s << "Failed to open file" << sourceFile.fileName() << endl;
        return 1;
    }

    QTextStream headerStream( &headerFile );
    QTextStream sourceStream( &sourceFile );

    QList<Statement> resources = it.allStatements();

    qDebug() << "parsed" << resources.count() << "resources.";

    // create entries
    // ----------------------------------------------------
    QMap<QString, QPair<QString, QString> > normalizedResources;
    QStringList done;
    foreach( Statement resource, resources ) {
        QString uri = resource.subject().uri().toString();
        QString name = resource.subject().uri().fragment();
        if ( name.isEmpty() && !uri.contains( '#' ) ) {
            name = resource.subject().uri().path().section( "/", -1 );
        }

        if ( !name.isEmpty() && !done.contains( name ) ) {
            normalizedResources.insert( uri, qMakePair( name, QString() ) );
            done += name;
        }
    }

    // extract comments
    foreach( Statement resource, resources ) {
        if ( resource.predicate().uri() == Soprano::Vocabulary::RDFS::comment() ) {
            if ( normalizedResources.contains( resource.subject().toString() ) ) {
                normalizedResources[resource.subject().toString()].second = resource.object().literal().toString();
            }
        }
    }

    if ( normalizedResources.isEmpty() ) {
        QTextStream s( stderr );
        s << "Nothing found to export." << endl;
        return 1;
    }

    // We simplify and take it as granted that all resources have the same NS
    QString ontoNamespace;
    QUrl namespaceUri( normalizedResources.begin().key() );
    if ( namespaceUri.hasFragment() ) {
        namespaceUri.setFragment( QString() );
        ontoNamespace = namespaceUri.toString() + "#";
    }
    else {
        ontoNamespace = namespaceUri.toString().section( "/", 0, -2 ) + "/";
    }
    qDebug() << "namespace: " << ontoNamespace;
    // ----------------------------------------------------


    // write the header
    // ----------------------------------------------------
    headerStream << LGPL_HEADER << endl;

    headerStream << "#ifndef _SOPRANO_" << className.toUpper() << "_H_" << endl
                 << "#define _SOPRANO_" << className.toUpper() << "_H_" << endl << endl;

    headerStream << "#include <QtCore/QUrl>" << endl;

    if ( visibilityExport )
        headerStream << "#include \"soprano_export.h\"" << endl << endl;

    int indent = 0;
    if ( !namespaceName.isEmpty() ) {
        QStringList tokens = namespaceName.split( "::" );
        for ( int i = 0; i < tokens.count(); ++i ) {
            headerStream << createIndent( indent++ ) << "namespace " << tokens[i] << " {" << endl;
        }
    }

    // the class name
    headerStream << createIndent( indent++ ) << "namespace " << className << " {" << endl;

    // the onto namespace
    headerStream << createIndent( indent ) << "/**" << endl
                 << createIndent( indent ) << " * " << ontoNamespace << endl
                 << createIndent( indent ) << " */" << endl;

    headerStream << createIndent( indent );
    if ( visibilityExport )
        headerStream << "SOPRANO_EXPORT ";
    headerStream << "QUrl " << className.toLower() << "Namespace();" << endl << endl;

    for( QMap<QString, QPair<QString, QString> >::const_iterator it = normalizedResources.begin();
         it != normalizedResources.end(); ++it ) {
        QString uri = it.key();
        QString name = it.value().first;
        QString comment = it.value().second;

        if ( comment.isEmpty() ) {
            headerStream << writeComment( uri, indent ) << endl;
        }
        else {
            headerStream << writeComment( uri + "\n\n" + comment, indent ) << endl;
        }
        headerStream << createIndent( indent ) << "SOPRANO_EXPORT QUrl " << name << "();" << endl;

        ++it;
        if ( it != normalizedResources.end() ) {
            headerStream << endl;
        }
        --it;
    }

    // close the namespaces
    while ( indent > 0 ) {
        headerStream << createIndent( --indent ) << "}" << endl;
    }

    headerStream << endl << "#endif" << endl;
    // ----------------------------------------------------


    // write source
    // ----------------------------------------------------
    sourceStream << LGPL_HEADER << endl;
    sourceStream << "#include \"" << headerFile.fileName() << "\"" << endl << endl;

    QString privateClassName = className[0].toUpper() + className.mid( 1 ).toLower() + "Private";
    QString singletonName = "s_" + className.toLower();

    sourceStream << "class " << privateClassName << endl
                 << "{" << endl
                 << "public:" << endl
                 << createIndent( 1 ) << privateClassName << "()" << endl
                 << createIndent( 2 ) << ": ";

    sourceStream << className.toLower() << "_namespace( \"" << ontoNamespace << "\" )," << endl;

    for( QMap<QString, QPair<QString, QString> >::const_iterator it = normalizedResources.begin();
         it != normalizedResources.end(); ++it ) {
        QString uri = it.key();
        QString name = it.value().first;

        sourceStream << createIndent( 2 ) << "  " << className.toLower() << "_" << name << "( \"" << uri << "\" )";

        ++it;
        if ( it != normalizedResources.end() ) {
            sourceStream << "," << endl;
        }
        --it;
    }

    sourceStream << " {" << endl
                 << createIndent( 1 ) << "}" << endl << endl;

    sourceStream << createIndent( 1 ) << "QUrl " << className.toLower() << "_namespace;" << endl;

    for( QMap<QString, QPair<QString, QString> >::const_iterator it = normalizedResources.begin();
         it != normalizedResources.end(); ++it ) {
        QString name = it.value().first;
        sourceStream << createIndent( 1 ) << "QUrl " << className.toLower() << "_" << name << ";" << endl;
    }
    sourceStream << "};" << endl << endl;

    sourceStream << "Q_GLOBAL_STATIC( " << privateClassName << ", " << singletonName << " )" << endl << endl;

    sourceStream << "QUrl ";
    if ( !namespaceName.isEmpty() ) {
        sourceStream << namespaceName << "::";
    }
    sourceStream << className << "::" << className.toLower() << "Namespace()" << endl
                 << "{" << endl
                 << createIndent( 1 ) << "return " << singletonName << "()->" << className.toLower() << "_namespace;" << endl
                 << "}" << endl << endl;

    for( QMap<QString, QPair<QString, QString> >::const_iterator it = normalizedResources.begin();
         it != normalizedResources.end(); ++it ) {
        QString name = it.value().first;

        sourceStream << "QUrl ";

        if ( !namespaceName.isEmpty() ) {
            sourceStream << namespaceName << "::";
        }

        sourceStream << className << "::" << name << "()" << endl
                     << "{" << endl
                     << createIndent( 1 ) << "return " << singletonName << "()->" << className.toLower() << "_" << name << ";" << endl
                     << "}" << endl;

        ++it;
        if ( it != normalizedResources.end() ) {
            sourceStream << endl;
        }
        --it;
    }

    // ----------------------------------------------------


    return 0;
}
