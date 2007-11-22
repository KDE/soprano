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

using namespace Soprano;

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
      << "   onto2vocabularyclass --name <name> --encoding <encoding> [--namespace <ns>] <ontologyfile>" << endl;

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


int main( int argc, char *argv[] )
{
    QCoreApplication app( argc, argv );

    QStringList args = app.arguments();
    QString fileName;
    QString className;
    QString namespaceName;
    QString encoding;
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
        s << "Could not find parser plugin for encoding" << encoding << endl;
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

    QList<Node> resources = it.iterateSubjects().allNodes();

    qDebug() << "parsed" << resources.count() << "resources.";

    // create entries
    // ----------------------------------------------------
    QList<QPair<QString,QString> > normalizedResoruces;
    QStringList done;
    foreach( Node resource, resources ) {
        QString uri = resource.uri().toString();
        QString name = resource.uri().fragment();
        if ( name.isEmpty() && !uri.contains( '#' ) ) {
            name = resource.uri().path().section( "/", -1 );
        }

        if ( !name.isEmpty() && !done.contains( name ) ) {
            normalizedResoruces.append( qMakePair( uri, name ) );
            done += name;
        }
    }

    // We simplify and take it as granted that all resources have the same NS
    QString ontoNamespace;
    if ( QUrl( normalizedResoruces[0].first ).hasFragment() ) {
        QUrl uri = normalizedResoruces[0].first;
        uri.setFragment( QString() );
        ontoNamespace = uri.toString() + "#";
    }
    else {
        ontoNamespace = normalizedResoruces[0].first.section( "/", 0, -2 ) + "/";
    }
    qDebug() << "namespace: " << ontoNamespace;
    // ----------------------------------------------------


    // write the header
    // ----------------------------------------------------
    headerStream << "#ifndef _SOPRANO_" << className.toUpper() << "_H_" << endl
                 << "#define _SOPRANO_" << className.toUpper() << "_H_" << endl << endl;

    headerStream << "#include <QtCore/QUrl>" << endl
                 << "#include \"soprano_export.h\"" << endl << endl;

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
    headerStream << createIndent( indent ) << "SOPRANO_EXPORT QUrl " << className.toLower() << "Namespace();" << endl << endl;

    for( int i = 0; i < normalizedResoruces.count(); ++i ) {
        QString uri = normalizedResoruces[i].first;
        QString name = normalizedResoruces[i].second;

        headerStream << createIndent( indent ) << "/**" << endl
                     << createIndent( indent ) << " * " << uri << endl
                     << createIndent( indent ) << " */" << endl;
        headerStream << createIndent( indent ) << "SOPRANO_EXPORT QUrl " << name << "();" << endl;

        if ( i < normalizedResoruces.count()-1 ) {
            headerStream << endl;
        }
    }

    // close the namespaces
    while ( indent > 0 ) {
        headerStream << createIndent( --indent ) << "}" << endl;
    }

    headerStream << endl << "#endif" << endl;
    // ----------------------------------------------------


    // write source
    // ----------------------------------------------------
    sourceStream << "#include \"" << headerFile.fileName() << "\"" << endl << endl;

    QString privateClassName = className[0].toUpper() + className.mid( 1 ).toLower() + "Private";
    QString singletonName = "s_" + className.toLower();

    sourceStream << "class " << privateClassName << endl
                 << "{" << endl
                 << "public:" << endl
                 << createIndent( 1 ) << privateClassName << "()" << endl
                 << createIndent( 2 ) << ": ";

    sourceStream << className.toLower() << "_namespace( \"" << ontoNamespace << "\" )," << endl;

    for( int i = 0; i < normalizedResoruces.count(); ++i ) {
        QString uri = normalizedResoruces[i].first;
        QString name = normalizedResoruces[i].second;

        sourceStream << createIndent( 2 ) << "  " << className.toLower() << "_" << name << "( \"" << uri << "\" )";
        if ( i < normalizedResoruces.count()-1 ) {
            sourceStream << "," << endl;
        }
    }

    sourceStream << " {" << endl
                 << createIndent( 1 ) << "}" << endl << endl;

    sourceStream << createIndent( 1 ) << "QUrl " << className.toLower() << "_namespace;" << endl;

    for( int i = 0; i < normalizedResoruces.count(); ++i ) {
        QString name = normalizedResoruces[i].second;

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

    for( int i = 0; i < normalizedResoruces.count(); ++i ) {
        QString name = normalizedResoruces[i].second;

        sourceStream << "QUrl ";
        if ( !namespaceName.isEmpty() ) {
            sourceStream << namespaceName << "::";
        }
        sourceStream << className << "::" << name << "()" << endl
                     << "{" << endl
                     << createIndent( 1 ) << "return " << singletonName << "()->" << className.toLower() << "_" << name << ";" << endl
                     << "}" << endl;

        if ( i < normalizedResoruces.count()-1 ) {
            sourceStream << endl;
        }
    }

    // ----------------------------------------------------


    return 0;
}
