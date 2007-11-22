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

#include "../soprano/statementiterator.h"
#include "../soprano/queryresultiterator.h"
#include "../soprano/version.h"
#include "../soprano/pluginmanager.h"
#include "../soprano/parser.h"
#include "../soprano/serializer.h"

#include "../server/tcpclient.h"
#include "../server/dbus/dbusclient.h"
#include "../server/dbus/dbusmodel.h"

using namespace Soprano;

static void printStatementList( Soprano::StatementIterator it )
{
    QTextStream outStream( stdout );
    int cnt = 0;
    while ( it.next() ) {
        outStream << *it << endl;
        ++cnt;
    }
    if ( it.lastError() ) {
        outStream << "Error occured: " << it.lastError() << endl;
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
        if ( it.lastError() ) {
            outStream << "Error occured: " << it.lastError() << endl;
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
    else if ( s[0] == '_' && s.length() > 2 && s[1] == ':' ) {
        return Soprano::Node::createBlankNode( s.mid( 2 ) );
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


static bool importFile( Soprano::Model* model, const QString& fileName, const QString& serialization )
{
    const Soprano::Parser* parser = Soprano::PluginManager::instance()->discoverParserForSerialization( Soprano::mimeTypeToSerialization( serialization ), serialization );

    if ( parser ) {

        Soprano::StatementIterator it = parser->parseFile( fileName,
                                                           QUrl("http://dummybaseuri.org" ),
                                                           Soprano::mimeTypeToSerialization( serialization ), serialization );

        if ( parser->lastError() ) {
            QTextStream s( stderr );
            s << "Parsing failed: " << parser->lastError() << endl;
            delete model;
            return 2;
        }

        int cnt = 0;
        while ( it.next() ) {
            if ( model->addStatement( *it ) == Soprano::Error::ErrorNone ) {
                ++cnt;
            }
            else {
                QTextStream s( stderr );
                s << "Failed to import statement " << *it << endl;
                delete model;
                return 2;
            }
        }

        QTextStream s( stdout );
        s << "Imported " << cnt << " statements." << endl;
        delete model;
        return 0;
    }
    else {
        QTextStream s( stderr );
        s << "Could not find parser plugin for serialization " << serialization << endl;
        delete model;
        return 1;
    }
}


static bool exportFile( Soprano::Model* model, const QString& fileName, const QString& serialization )
{
    const Soprano::Serializer* serializer = Soprano::PluginManager::instance()->discoverSerializerForSerialization( Soprano::mimeTypeToSerialization( serialization ), serialization );

    if ( serializer ) {
        QFile file( fileName );
        if ( !file.open( QIODevice::WriteOnly ) ) {
            QTextStream s( stderr );
            s << "Could not open file for writing: " << fileName << endl;
            delete model;
            return 1;
        }

        QTextStream s( &file );

        if ( serializer->serialize( model->listStatements(), s, Soprano::mimeTypeToSerialization( serialization ), serialization ) ) {
            QTextStream s( stdout );
            s << "Successfully exported model." << endl;
            delete model;
            return 0;
        }
        else {
            QTextStream s( stderr );
            s << "Failed to export statements: " << serializer->lastError() << endl;
            delete model;
            return 2;
        }
    }
    else {
        QTextStream s( stderr );
        s << "Could not find serializer plugin for serialization " << serialization << endl;
        delete model;
        return 1;
    }
}



class CmdLineArgs
{
public:
    static bool parseCmdLine( CmdLineArgs& a, const QStringList& args, const QHash<QString, bool>& allowed ) {
        int i = 1;
        bool optionParsing = true;
        QTextStream errStream(stderr);
        while ( i < args.count() ) {
            if ( args[i].startsWith( "--" ) ) {
                if ( !optionParsing ) {
                    return false;
                }

                QString name = args[i].mid( 2 );
                if ( name.length() == 0 ) {
                    return false;
                }

                if ( allowed.contains( name ) ) {
                    if ( !allowed[name] ) {
                        a.m_options.append( name );
                    }
                    else if ( i+1 < args.count() ) {
                        if ( !args[i+1].startsWith( "--" ) ) {
                            a.m_settings[name] = args[++i];
                        }
                        else {
                            errStream << "Missing parameter: " << name << endl << endl;
                            return false;
                        }
                    }
                    else {
                        errStream << "Missing parameter: " << name << endl << endl;
                        return false;
                    }
                }
                else {
                    errStream << "Invalid option: " << name << endl << endl;
                    return false;
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

    bool hasSetting( const QString& name ) const {
        return m_settings.contains( name );
    }

    QString getSetting( const QString& name, const QString& defaultValue = QString() ) const {
        if ( m_settings.contains( name ) ) {
            return m_settings[name];
        }
        else {
            return defaultValue;
        }
    }

    bool optionSet( const QString& name ) const {
        return m_options.contains( name );
    }

    int count() const {
        return m_args.count();
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


#define VERSION "0.99"

int version()
{
    QTextStream s( stderr );
    s << "sopranocmd " << VERSION << " (using Soprano " << Soprano::versionString() << ")" << endl;
    s << "   Copyright (C) 2007 Sebastian Trueg <trueg@kde.org>" << endl;
    s << "   This program is free software; you can redistribute it and/or modify" << endl
      << "   it under the terms of the GNU General Public License as published by" << endl
      << "   the Free Software Foundation; either version 2 of the License, or" << endl
      << "   (at your option) any later version." << endl;

    return 0;
}

QStringList backendNames()
{
    QStringList names;
    QList<const Backend*> backends = PluginManager::instance()->allBackends();
    Q_FOREACH( const Backend* backend, backends ) {
        names << backend->pluginName();
    }
    return names;
}

int usage( const QString& error = QString() )
{
    version();

    QTextStream s( stderr );
    s << endl;
    s << "Usage:" << endl
      << "   sopranocmd [--version] [--help] [--port <port>] [--host <host>] [--model <name>] [--serialization <s>] [<command>] [<parameters>]" << endl
      << endl
      << "   --version           Print version information." << endl << endl
      << "   --help              Print this help." << endl << endl
      << "   --model <name>      The name of the Soprano model to perform the command on." << endl
      << "                       (only applicable when querying against the Soprano server.)" << endl << endl
      << "   --backend           The backend to use. If this option is not specified the Soprano server" << endl
      << "                       will be contacted. Possible backends are:" << endl
      << "                       " << backendNames().join( ", " ) << endl << endl
      << "   --dir               The storage directory. This only applies when specifying the backend. Defaults" << endl
      << "                       to current directory." << endl << endl
      << "   --dbus <service>    Contact the soprano server running on the specified service." << endl << endl
      << "   --port <port>       Specify the port the Soprano server is running on." << endl
      << "                       (only applicable when querying against the Soprano server.)" << endl << endl
      << "   --host <host>       Specify the host the Soprano server is running on (defaults to localhost)." << endl
      << "                       (only applicable when querying against the Soprano server.)" << endl << endl
      << "   --serialization <s> The serialization used for commands 'export' and 'import'. Defaults to 'application/x-trig'." << endl
      << "                       (only applicable with the mentioned commands.)" << endl << endl
      << "   <command>           The command to perform. Can be one of 'add', 'remove', 'list', 'query', 'import', or 'export'." << endl << endl
      << "   <parameters>        The parameters to the command." << endl
      << "                       - For command 'query' this is a SPARQL query string." << endl
      << "                       - For commands 'add' and 'remove' this is a list of 3 or 4 RDF node definitions." << endl
      << "                       - For command 'list' this is an optional list of one to four node definitions." << endl
      << "                       - For commands 'import' and 'export' this is a local file name to either parse or write to." << endl << endl;

    s << "   Nodes are defined in an N-Triples-like notation:" << endl
      << "   - Resouce nodes are defined in angle brackets." << endl
      << "     Example: <http://www.test.org#A>" << endl
      << "   - Blank nodes are defined as \"_:\" followed by their identifier." << endl
      << "     Example: _:a" << endl
      << "   - Literal nodes are defined as a combination of their string value and their datatype URI" << endl
      << "     or as a simple literal string:" << endl
      << "     Examples: \"Hello World\"^^<http://www.w3.org/2001/XMLSchema#string>" << endl
      << "               42" << endl
      << "               0.7" << endl
      << "   - An empty string evaluates to an empy node (\"\" does the trick)" << endl;

    if ( !error.isEmpty() ) {
        s << endl << error << endl;
        return 0;
    }
    else {
        return 1;
    }
}


int main( int argc, char *argv[] )
{
    QCoreApplication app( argc, argv );

    QHash<QString, bool> allowedCmdLineArgs;
    allowedCmdLineArgs.insert( "model", true );
    allowedCmdLineArgs.insert( "version", false );
    allowedCmdLineArgs.insert( "help", false );
    allowedCmdLineArgs.insert( "backend", true );
    allowedCmdLineArgs.insert( "dir", true );
    allowedCmdLineArgs.insert( "port", true );
    allowedCmdLineArgs.insert( "host", true );
    allowedCmdLineArgs.insert( "dbus", true );
    allowedCmdLineArgs.insert( "serialization", true );

    CmdLineArgs args;
    if ( !CmdLineArgs::parseCmdLine( args, app.arguments(), allowedCmdLineArgs ) ) {
        return usage();
    }

    if ( args.optionSet( "version" ) ) {
        return version();
    }
    if ( args.optionSet( "help" ) ) {
        return usage();
    }

    QString backendName = args.getSetting( "backend" );
    QString dir = args.getSetting( "dir" );
    QString command;
    QString modelName = args.getSetting( "model" );

    if ( modelName.isEmpty() && backendName.isEmpty() ) {
        return usage( "No model name specified." );
    }

    QTextStream errStream(stderr);
    Soprano::Client::TcpClient* tcpClient = 0;
    Soprano::Client::DBusClient* dbusClient = 0;
    Soprano::Model* model = 0;
    if ( backendName.isEmpty() ) {
        if ( args.hasSetting( "port" ) ) {
            QHostAddress host = QHostAddress::LocalHost;
            quint16 port = Soprano::Client::TcpClient::DEFAULT_PORT;
            port = args.getSetting( "port" ).toInt();
            if ( args.hasSetting( "host" ) ) {
                host = args.getSetting( "host" );
            }

            tcpClient = new Soprano::Client::TcpClient();

            if ( !tcpClient->connect( host, port ) ) {
                errStream << "Failed to connect to server on port " << port << endl;
                delete tcpClient;
                return 3;
            }
            if ( !( model = tcpClient->createModel( modelName ) ) ) {
                errStream << "Failed to create Model: " << tcpClient->lastError() << endl;
                delete tcpClient;
                return 2;
            }
        }
        else if ( args.hasSetting( "dbus" ) ) {
            QString dbusService = args.getSetting( "dbus" );
            dbusClient = new Soprano::Client::DBusClient( dbusService );
            if ( !dbusClient->isValid() ) {
                errStream << "Failed to contact Soprano dbus service at " << dbusService << endl;
                delete dbusClient;
                return 3;
            }
            if ( !( model = dbusClient->createModel( modelName ) ) ) {
                errStream << "Failed to create Model: " << dbusClient->lastError() << endl;
                delete dbusClient;
                return 2;
            }
        }
        else {
            errStream << "Please specify a backend to be used or details on how to contact" << endl
                      << "a soprano server." << endl;
            return 4;
        }
    }
    else {
        const Backend* backend = Soprano::PluginManager::instance()->discoverBackendByName( backendName );
        if ( !backend ) {
            errStream << "Failed to load backend " << backendName << endl;
            return 1;
        }
        if ( dir.isEmpty() ) {
            dir = QDir::currentPath();
        }
        QList<BackendSetting> settings;
        settings.append( BackendSetting( BackendOptionStorageDir, dir ) );
        if ( !( model = backend->createModel( settings ) ) ) {
            errStream << "Failed to create Model: " << backend->lastError() << endl;
            return 2;
        }
    }

    int firstArg = 0;
    if ( args.count() >= 1 ) {
        command = args[0];
        ++firstArg;
    }
    else {
        delete model;
        return usage();
    }

    int queryTime = 0;

    if ( command == "import" ||
         command == "export" ) {
        if ( firstArg != args.count()-1 ) {
            delete model;
            return usage();
        }

        QString fileName = args[firstArg];
        QString serialization = args.getSetting( "serialization", "application/x-trig" );

        if ( command == "import" ) {
            return importFile( model, fileName, serialization );
        }
        else {
            return exportFile( model, fileName, serialization );
        }
    }

    else if ( command == "query" ) {
        if ( firstArg >= args.count() ) {
            return usage();
        }

        QString query = args[firstArg];

        QTime time;
        time.start();

        Soprano::QueryResultIterator it = model->executeQuery( query, Soprano::Query::QueryLanguageSparql );
        queryTime = time.elapsed();
        printQueryResult( it );
    }

    else {
        // parse node commands (max 4)
        Soprano::Node n1, n2, n3, n4;
        if ( args.count() > 5 ) {
            delete model;
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
            errStream << "Subject needs to be a resource node." << endl;
            delete model;
            return 1;
        }
        if ( !n2.isResource() && !n2.isEmpty() ) {
            errStream << "Predicate needs to be a resource node." << endl;
            delete model;
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
                errStream << "At least subject, predicate, and object have to be defined." << endl;
                delete model;
                return 1;
            }
            model->addStatement( Soprano::Statement( n1, n2, n3, n4 ) );
            queryTime = time.elapsed();
        }
        else if ( command == "remove" ) {
            model->removeAllStatements( Soprano::Statement( n1, n2, n3, n4 ) );
            queryTime = time.elapsed();
        }
        else {
            errStream << "Unsupported command: " << command << endl;
            delete model;
            return 1;
        }
    }

    if ( !model->lastError() ) {
        errStream << "Query time: " << QTime().addMSecs( queryTime ).toString( "hh:mm:ss.z" ) << endl;
        delete model;
        return 0;
    }
    else {
        errStream << "Query failed: " << model->lastError() << endl;
        delete model;
        return 2;
    }
}
