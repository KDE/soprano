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

#include <soprano/soprano.h>
#include "../server/serverbackend.h"


static void printStatementList( Soprano::StatementIterator it )
{
    QTextStream outStream( stdout );
    int cnt = 0;
    while ( it.next() ) {
        outStream << *it << endl;
        ++cnt;
    }
    outStream << "Total results: " << cnt << endl;
}


static void printQueryResult( Soprano::QueryResultIterator it )
{
    QTextStream outStream( stdout );
    if ( it.isBool() ) {
        outStream << it.boolValue() << endl;
    }
    else {
        bool graph = it.isGraph();
        int cnt = 0;
        while ( it.next() ) {
            if ( graph ) {
                outStream << it.currentStatement() << endl;
            }
            else {
                outStream << *it << endl;
            }
            ++cnt;
        }
        outStream << "Total results: " << cnt << endl;
    }
}

static Soprano::Node parseNode( const QString& s )
{
    if ( s.isEmpty() ) {
        return Soprano::Node();
    }
    else if ( s[0] == '<' && s[s.length()-1] == '>' ) {
        return Soprano::Node( QUrl( s.mid( 1, s.length()-2 ) ) );
    }
    else if ( s[0] == '"' ) {
        QString value = s;
        QString literalType;
        int pos = s.indexOf( "\"^^<" );
        if ( pos > 0 ) {
            literalType = s.mid( pos + 4, s.length() - pos - 5 );
            value = s.mid( 1, pos-1 );
        }
        else {
            value = s.mid( 1, s.length()-1 );
        }
        return Soprano::LiteralValue::fromString( value, QUrl( literalType ) );
    }
    else {
        // we only check for integer and double here
        bool ok = false;
        int val = s.toInt( &ok );
        if ( ok ) {
            return Soprano::LiteralValue( val );
        }
        double dVal = s.toDouble( &ok );
        if ( ok ) {
            return Soprano::LiteralValue( dVal );
        }
        else {
            return Soprano::LiteralValue( s );
        }
    }
}



class CmdLineArgs
{
public:
    static bool parseCmdLine( CmdLineArgs& a, const QStringList& args, const QHash<QString, bool>& allowed ) {
        int i = 1;
        bool optionParsing = true;
        while ( i < args.count() ) {
            if ( args[i].startsWith( "--" ) ) {
                if ( !optionParsing ) {
                    return false;
                }

                QString name = args[i].mid( 2 );
                if ( name.length() == 0 || !allowed.contains( name ) ) {
                    return false;
                }

                if ( i+1 < args.count() ) {
                    if ( !args[i+1].startsWith( "--" ) ) {
                        if ( allowed[name] ) {
                            a.m_settings[name] = args[i++];
                        }
                        else {
                            return false;
                        }
                    }
                    else {
                        if ( !allowed[name] ) {
                            a.m_options.append( name );
                        }
                        else {
                            return false;
                        }
                    }
                }
            }
            else {
                optionParsing = false;

                a.m_args.append( args[i] );
            }

            ++i;
        }

        return true;
    }

    QString getSetting( const QString& name ) const {
        return m_settings[name];
    }

    bool optionSet( const QString& name ) const {
        return m_options.contains( name );
    }

    int count() const {
        m_args.count();
    }

    QString operator[]( int i ) const {
        return m_args[i];
    }

    QString arg( int i ) const {
        return m_args[i];
    }

private:
    QHash<QString, QString> m_settings;
    QStringList m_args;
    QStringList m_options;
};


#define VERSION "0.1"

int usage( const QString& error = QString() )
{
    QTextStream s( stderr );
    s << "sopranocmd " << VERSION << " (using Soprano " << Soprano::versionString() << ")" << endl;
    s << "   Copyright (C) 2007 Sebastian Trueg <trueg@kde.org>" << endl;
    s << "   This software is released under the GNU General Public License version 2." << endl;
    s << endl;
    s << "Usage:" << endl
      << "   sopranod [--port <port>] --model <name> [<command>] <queryparameters>" << endl;

    if ( !error.isEmpty() ) {
        s << endl << error << endl;
    }

    return 1;
}


int main( int argc, char *argv[] )
{
    QCoreApplication app( argc, argv );

    QHash<QString, bool> allowedCmdLineArgs;
    allowedCmdLineArgs.insert( "port", true );
    allowedCmdLineArgs.insert( "model", true );

    CmdLineArgs args;
    if ( !CmdLineArgs::parseCmdLine( args, app.arguments(), allowedCmdLineArgs ) ) {
        return usage();
    }

    QString command;
    QString modelName = args.getSetting( "model" );

    if ( modelName.isEmpty() ) {
        return usage( "No model name specified." );
    }

    int firstArg = 0;
    if ( args.count() == 1 ) {
        command = "query";
    }
    else if ( args.count() > 1 ) {
        command = args[0];
        ++firstArg;
    }
    else {
        return usage();
    }

    Soprano::Server::ServerBackend backend;
    Soprano::Model* model = backend.createModel( modelName );
    if ( !model ) {
        QTextStream( stderr ) << "Failed to create Model: " << backend.lastError() << endl;
        return 2;
    }

    int queryTime = 0;

    if ( command == "query" ) {
        QString query = args[firstArg];

        QTime time;
        time.start();

        Soprano::QueryResultIterator it = model->executeQuery( Soprano::QueryLegacy( query, Soprano::QueryLegacy::SPARQL ) );
        queryTime = time.elapsed();
        printQueryResult( it );
    }

    else {
        // parse node commands (max 4)
        Soprano::Node n1, n2, n3, n4;
        if ( args.count() > 5 ) {
            return usage();
        }

        if ( args.count() > 1 ) {
            n1 = parseNode( args.arg( 1 ) );
        }
        if ( args.count() > 2 ) {
            n2 = parseNode( args.arg( 2 ) );
        }
        if ( args.count() > 3 ) {
            n3 = parseNode( args.arg( 3 ) );
        }
        if ( args.count() > 4 ) {
            n4 = parseNode( args.arg( 4 ) );
        }

        if ( !n1.isResource() && !n1.isEmpty() ) {
            QTextStream( stderr ) << "Subject needs to be a resource node." << endl;
            return 1;
        }
        if ( !n2.isResource() && !n2.isEmpty() ) {
            QTextStream( stderr ) << "Predicate needs to be a resource node." << endl;
            return 1;
        }

        QTime time;
        time.start();

        if ( command == "list" ) {
            Soprano::StatementIterator it = model->listStatements( Soprano::Statement( n1, n2, n3, n4 ) );
            queryTime = time.elapsed();
            printStatementList( it );
        }
        else if ( command == "add" ) {
            if ( n1.isEmpty() || n2.isEmpty() || n3.isEmpty() ) {
                QTextStream( stderr ) << "At least subject, predicate, and object have to be defined." << endl;
                return 1;
            }
            model->addStatement( Soprano::Statement( n1, n2, n3, n4 ) );
            queryTime = time.elapsed();
        }
        else if ( command == "remove" ) {
            model->removeStatements( Soprano::Statement( n1, n2, n3, n4 ) );
            queryTime = time.elapsed();
        }
        else {
            QTextStream( stderr ) << "Unsupported command: " << command << endl;
            return 1;
        }
    }

    if ( !model->lastError() ) {
        QTextStream( stderr ) << "Query time: " << QTime().addMSecs( queryTime ).toString( "hh:mm:ss.z" ) << endl;
        return 0;
    }
    else {
        QTextStream( stderr ) << "Query failed: " << model->lastError() << endl;
        return 2;
    }
}
