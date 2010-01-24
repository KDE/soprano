/*
 * This file is part of Soprano Project
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

#include <QtCore/QCoreApplication>
#include <QtCore/QTime>
#include <QtCore/QTextStream>
#include <QtCore/QDebug>
#include <QtCore/QList>
#include <QtCore/QDir>
#include <QtCore/QRegExp>
#include <QtCore/QSharedPointer>

#include "soprano-tools-config.h"

#include "modelmonitor.h"

#include "../soprano/statementiterator.h"
#include "../soprano/queryresultiterator.h"
#include "../soprano/version.h"
#include "../soprano/pluginmanager.h"
#include "../soprano/parser.h"
#include "../soprano/serializer.h"
#include "../soprano/storagemodel.h"
#include "../soprano/vocabulary.h"
#define USING_SOPRANO_NRLMODEL_UNSTABLE_API
#include "../soprano/nrlmodel.h"

#ifdef BUILD_CLUCENE_INDEX
#include "../index/indexfiltermodel.h"
#endif

#include "../client/tcpclient.h"
#include "../client/localsocketclient.h"
#ifdef BUILD_DBUS_SUPPORT
#include "../client/dbus/dbusclient.h"
#include "../client/dbus/dbusmodel.h"
#endif
#include "../client/sparql/sparqlmodel.h"

#include <signal.h>

using namespace Soprano;

namespace {
    Soprano::Model* s_model = 0;
    ModelMonitor* s_monitor = 0;

    /// true if the output should be human-readable
    bool s_interactive = true;

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

    /// The global CmdLineArgs object. Defined here so the methods below can use
    /// the command line args, too
    CmdLineArgs args;

    int serializeData( Soprano::StatementIterator data, QTextStream& stream, const QString& serialization )
    {
        const Soprano::Serializer* serializer
            = Soprano::PluginManager::instance()->discoverSerializerForSerialization( Soprano::mimeTypeToSerialization( serialization ),
                                                                                      serialization );

        if ( serializer ) {
            //
            // As we support a bunch of default prefixes in queries, we do so with serialization
            //
            serializer->addPrefix( "rdf", Soprano::Vocabulary::RDF::rdfNamespace() );
            serializer->addPrefix( "rdfs", Soprano::Vocabulary::RDFS::rdfsNamespace() );
            serializer->addPrefix( "xsd", Soprano::Vocabulary::XMLSchema::xsdNamespace() );
            serializer->addPrefix( "nrl", Soprano::Vocabulary::NRL::nrlNamespace() );
            serializer->addPrefix( "nao", Soprano::Vocabulary::NAO::naoNamespace() );

            if ( serializer->serialize( data, stream, Soprano::mimeTypeToSerialization( serialization ), serialization ) ) {
                QTextStream s( stderr );
                s << "Successfully exported model." << endl;
                return 0;
            }
            else {
                QTextStream s( stderr );
                s << "Failed to export statements: " << serializer->lastError() << endl;
                return 2;
            }
        }
        else {
            QTextStream s( stderr );
            s << "Could not find serializer plugin for serialization " << serialization << endl;
            return 1;
        }
    }


    int printStatementList( Soprano::StatementIterator it )
    {
        QTextStream outStream( stdout );
        QTextStream errStream(stderr);

        if ( !args.hasSetting( "file" ) &&
             args.hasSetting( "serialization" ) ) {
            return serializeData( it, outStream, args.getSetting( "serialization" ) );
        }
        else {
            int cnt = 0;
            while ( it.next() ) {
                outStream << *it << endl;
                ++cnt;
            }

            if ( it.lastError() ) {
                errStream << "Error occured: " << it.lastError() << endl;
                return 2;
            }
            errStream << "Total results: " << cnt << endl;
            return 0;
        }
    }


    int printQueryResult( Soprano::QueryResultIterator it )
    {
        QTextStream outStream( stdout );
        QTextStream errStream( stderr );

        if ( it.isBool() ) {
            outStream << ( it.boolValue() ? "true" : "false" ) << endl;
        }

        else if ( it.isGraph() ) {
            return printStatementList( it.iterateStatements() );
        }

        else if ( it.isBinding() &&
                  args.hasSetting( "graphselect" ) ) {
            QStringList vars = args.getSetting( "graphselect" ).split( ';', QString::SkipEmptyParts );
            if ( vars.count() != 4 ) {
                errStream << "Need 4 variables to construct statements from bindings." << endl;
                return 1;
            }
            return printStatementList( it.iterateStatementsFromBindings(vars[0], vars[1], vars[2], vars[3]) );
        }

        else {
            int cnt = 0;
            while ( it.next() ) {
                if( s_interactive ) {
                    outStream << *it << endl;
                }
                else {
                    const BindingSet set = *it;
                    for( int i = 0; i < set.count(); ++i ) {
                        outStream << set[i];
                        if( i == set.count()-1 )
                            outStream << endl;
                        else
                            outStream << " ";
                    }
                }
                ++cnt;
            }
            QTextStream stderrStream(stderr);
            if ( it.lastError() ) {
                stderrStream << "Error occured: " << it.lastError() << endl;
                return 2;
            }
            stderrStream << "Total results: " << cnt << endl;
        }

        return 0;
    }

    QUrl parseUri( const QString& s ) {
        // try to be a little smart about user input
        if ( s.contains( '%' ) ) {
            QByteArray b = s.toAscii();
            if ( !b.isEmpty() ) {
                return QUrl::fromEncoded( b );
            }
        }
        return QUrl( s );
    }

    QRegExp s_prefixedUriRx( "(\\w+)\\:(\\w+)" );

    Soprano::Node parseNode( const QString& s )
    {
        if ( s.isEmpty() ) {
            return Soprano::Node();
        }
        else if ( s[0] == '<' && s[s.length()-1] == '>' ) {
            return Soprano::Node( parseUri( s.mid( 1, s.length()-2 ) ) );
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
                return Soprano::LiteralValue::fromString( value, QUrl( literalType ) );
            }
            else {
                QString lang;
                pos = s.indexOf( "\"@" );
                int len = s.length()-2;
                if ( pos > 0 ) {
                    lang = s.mid( pos+2, s.length() - pos - 2 );
                    len -= lang.length() + 1;
                }
                value = s.mid( 1, len );
                return Soprano::LiteralValue::createPlainLiteral( value, lang );
            }
        }
        else if ( s_prefixedUriRx.exactMatch( s ) ) {
            QUrl ns;
            Soprano::NRLModel* nrlModel = qobject_cast<Soprano::NRLModel*>( s_model );
            if ( nrlModel ) {
                ns = nrlModel->queryPrefixes()[s_prefixedUriRx.cap( 1 )];
            }
            if ( ns.isValid() )
                return QUrl( ns.toString() + s_prefixedUriRx.cap( 2 ) );
            else
                return Soprano::Node();
        }
        else if ( s == QLatin1String( "a" ) ) {
            return Soprano::Vocabulary::RDF::type();
        }
        else {
            // we only check for boolean, integer and double here
            if ( s.toLower() == "false" )
                return Soprano::LiteralValue( false );
            else if ( s.toLower() == "true" )
                return Soprano::LiteralValue( true );

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


    int importFile( Soprano::Model* model, const QString& fileName, const QString& serialization )
    {
        Soprano::NRLModel* nrlModel = qobject_cast<Soprano::NRLModel*>( model );
        QUrl graph;

        const Soprano::Parser* parser = Soprano::PluginManager::instance()->discoverParserForSerialization( Soprano::mimeTypeToSerialization( serialization ), serialization );

        if ( parser ) {

            Soprano::StatementIterator it = parser->parseFile( fileName,
                                                               QUrl("http://dummybaseuri.org" ),
                                                               Soprano::mimeTypeToSerialization( serialization ), serialization );

            if ( parser->lastError() ) {
                QTextStream s( stderr );
                s << "Parsing failed: " << parser->lastError() << endl;
                return 2;
            }

            int cnt = 0;
            while ( it.next() ) {
                //
                // In NRL mode we make sure each statement is in a proper graph
                //
                Statement statement( *it );
                if ( !statement.context().isValid() &&
                     nrlModel ) {
                    if ( graph.isEmpty() ) {
                        graph = nrlModel->createGraph( Soprano::Vocabulary::NRL::KnowledgeBase() );
                        QTextStream s( stderr );
                        s << "Generated new NRL context for imported statements: " << graph.toString() << endl;
                    }
                    statement.setContext( graph );
                }

                if ( model->addStatement( statement ) == Soprano::Error::ErrorNone ) {
                    ++cnt;
                }
                else {
                    QTextStream s( stderr );
                    s << "Failed to import statement " << *it << ": " << model->lastError() << endl;
                    return 2;
                }
            }

            QTextStream s( stderr );
            s << "Imported " << cnt << " statements." << endl;
            return 0;
        }
        else {
            QTextStream s( stderr );
            s << "Could not find parser plugin for serialization " << serialization << endl;
            return 1;
        }
    }


    int exportFile( Soprano::StatementIterator data, const QString& fileName, const QString& serialization )
    {
        QFile file( fileName );
        if ( !file.open( QIODevice::WriteOnly ) ) {
            QTextStream s( stderr );
            s << "Could not open file for writing: " << fileName << endl;
            return 1;
        }
        else {
            QTextStream stream( &file );
            return serializeData( data, stream, serialization );
        }
    }


    Soprano::Model* createMemoryModel( const QString& backendName = QString() )
    {
        const Soprano::Backend* backend = 0;
        if ( !backendName.isEmpty() ) {
            backend = PluginManager::instance()->discoverBackendByName( backendName );
        }
        else {
            backend = PluginManager::instance()->discoverBackendByFeatures( BackendFeatureStorageMemory );
        }
        if ( backend )
            return backend->createModel( QList<BackendSetting>() << BackendSetting( BackendOptionStorageMemory ) );
        else
            return 0;
    }


    int printVersion()
    {
        QTextStream s( stdout );
        s << "sopranocmd " << Soprano::versionString() << endl;
        s << "   Copyright (C) 2007-2010 Sebastian Trueg <trueg@kde.org>" << endl;
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

    int printUsage( const QString& error = QString() )
    {
        printVersion();

        QTextStream s( stdout );
        s << endl;
        s << "Usage:" << endl
          << "   sopranocmd --backend <backendname> [--dir <storagedir>] [--port <port>] [--host <host>] "
          << "[--username <username>] [--password <password>] [--settings <settings>] [--serialization <s>] <command> [<parameters>]" << endl
          << "   sopranocmd --port <port> [--host <host>] --model <name> [--serialization <s>] <command> [<parameters>]" << endl
          << "   sopranocmd --socket <socketpath>  --model <name> [--serialization <s>] <command> [<parameters>]" << endl
#ifdef BUILD_DBUS_SUPPORT
          << "   sopranocmd --dbus <dbusservice> --model <name> [--serialization <s>] <command> [<parameters>]" << endl
#endif
          << "   sopranocmd --sparql <sparql end point> [--port <port>] [--username <username>] [--password <password>] [--serialization <s>] <command> [<parameters>]" << endl
          << "   sopranocmd --file <rdf-file> [--serialization <s>] <command> [<parameters>]" << endl
          << endl
          << "   --version           Print version information." << endl
          << endl
          << "   --help              Print this help." << endl
          << endl
          << "   --nrl               Enable NRL (Nepomuk) features. This includes the following features:" << endl
          << "                       - Automatic query prefix expansion based on ontologies stored in the model." << endl
          << "                       - Automatic context creation for imported statements without a predefined context/named" << endl
          << "                         graph. The newly created context will be of type nrl:KnowledgeBase and already have" << endl
          << "                         its creation date set in its very own nrl:GraphMetadata." << endl
          << "                       - Automatic removal of metadata graphs if using the remove command with the only defined" << endl
          << "                         node being the context." << endl
          << endl
          << "   --foo               Enables scriptable output which can be used as input for other commands." << endl
          << endl
          << "   --model <name>      The name of the Soprano model to perform the command on." << endl
          << "                       (only applicable when querying against the Soprano server.)" << endl
          << endl
          << "   --backend           The backend to use when accessing a storage directly and not via the Soprano server." << endl
          << "                       Possible backends are:" << endl
          << "                       " << backendNames().join( ", " ) << endl
          << endl
          << "   --settings <s>      A list of additional settings to be passed to the backend. Only applicable in combination with" << endl
          << "                       --backend. Settings are key=value pairs separated by semicolon." << endl
          << endl
          << "   --dir <dir>         The storage directory. This only applies when specifying the backend. Defaults" << endl
          << "                       to current directory." << endl
          << endl
#ifdef BUILD_DBUS_SUPPORT
          << "   --dbus <service>    Contact the soprano server through D-Bus running on the specified service." << endl
          << endl
#endif
          << "   --port <port>       Specify the port the Soprano server is running on." << endl
          << "                       (only applicable when querying against the Soprano server or a sparql endpoint.)" << endl
          << endl
          << "   --host <host>       Specify the host the Soprano server is running on (defaults to localhost)." << endl
          << "                       (only applicable when querying against the Soprano server.)" << endl
          << endl
          << "   --username <name>   Specify the username for the Soprano backend." << endl
          << "                       (only applicable when using a virtuoso backend or a sparql endpoint.)" << endl
          << endl
          << "   --password <word>   Specify the password for the Soprano backend." << endl
          << "                       (only applicable when using a virtuoso backend or a sparql endpoint.)" << endl
          << endl
          << "   --socket <path>     Specify the path to the local socket the Soprano server is running on." << endl
          << "                       (only applicable when querying against the Soprano server.)" << endl
          << endl
          << "   --sparql <endpoint> Specify the remote Http sparql endpoint to use." << endl
          << endl
          << "   --file <rdf-file>   Use an rdf file as input." << endl
          << endl
#ifdef BUILD_CLUCENE_INDEX
          << "   --index <path>      Use the CLucene index stored at <path> via an IndexFilterModel." << endl
          << endl
#endif
          << "   --serialization <s> The serialization used for commands 'export' and 'import'. Defaults to 'application/x-nquads'." << endl
          << "                       (can also be used to change the output format of construct and describe queries.)" << endl
          << "                       (be aware that Soprano can understand simple string identifiers such as 'trig' or 'n-triples'." << endl
          << "                       There is no need to know the exact mimetype.)" << endl
          << endl
          << "   --querylang <lang>  The query language used for query commands. Defaults to 'SPARQL'" << endl
          << "                       Hint: sopranocmd automatically adds prefix definitions for standard namespaces such as RDF, " << endl
          << "                             RDFS, NRL, etc. if used in a SPARQL query with the --nrl parameter." << endl
          << endl
          << "   --graphselect <variablelist>" << endl
          << "                       This parameter allows to emulate construct queries using a select query. This has the advantage that one" << endl
          << "                       can construct statements including the context/named graph which is not supported by SPARQL construct." << endl
          << "                       <variablelist> contains the four variables bould to subject, predicate, object, and context in that order." << endl
          << "                       It is a list separated by semicolon. Example: --graphselect 's;p;o;g'" << endl
          << endl
          << "   <command>           The command to perform." << endl
          << "                       - 'add':     Add a new statement. The parameters are a list of 3 or 4 nodes as defined below." << endl
          << "                       - 'remove':  Remove one or more statements. The parameters are a list of up to 4 nodes which define" << endl
          << "                         'rm'       the statement pattern to apply. Each statement matching the pattern will be removed." << endl
          << "                                    When used in combination with the --nrl parameter metadata graphs are removed automatically" << endl
          << "                                    if only a context is provided as pattern." << endl
          << "                       - 'rmgraph': Remove a complete graph/context. One can specify a list of graphs/contexts to remove." << endl
          << "                                    When used in combination with the --nrl parameter metadata graphs are removed automatically." << endl
          << "                       - 'list':    List statements. As with 'remove' the parameters are a list of up to 4 nodes." << endl
          << "                       - 'monitor': Monitor a remote model. sopranocmd will continue listing added and removed statements" << endl
          << "                                    matching the statement pattern specified via the parameters until it is stopped. Be aware" << endl
          << "                                    that some backends do not report every removed statement but only the remove pattern." << endl
          << "                       - 'query':   Execute a query on the model and print the results. The paramter is the actual query to perform." << endl
          << "                       - 'import':  Import a set of statements into the model. The parameter is a local filename to read the" << endl
          << "                                    statements to import from. If the option --nrl is given imported statements are added to a new" << endl
          << "                                    context/named graph if they do not have one already." << endl
          << "                       - 'export':  Export a set of statements to a file. The parameters are an optional SPARQL construct query" << endl
          << "                                    which selects the statements to export (if not specified all statements are exported) and a local" << endl
          << "                                    filename to write the exported statements to." << endl << endl
          << "   <parameters>        The parameters to the command as specified above." << endl << endl;

        s << "   Nodes are defined in an N-Triples-like notation:" << endl
          << "   - Resource nodes are defined in angle brackets." << endl
          << "     Example: <http://www.test.org#A>" << endl
          << "   - Blank nodes are defined as \"_:\" followed by their identifier." << endl
          << "     Example: _:a" << endl
          << "   - Literal nodes are defined as a combination of their string value and their datatype URI" << endl
          << "     or as a simple literal string:" << endl
          << "     Examples: \"Hello World\"^^<http://www.w3.org/2001/XMLSchema#string>" << endl
          << "               42         (evaluates to a literal of type integer)" << endl
          << "               0.7        (evaluates to a literal of type double)" << endl
          << "               Hello      (evaluates to a literal of type string)" << endl
          << "               \"Hello\"    (evaluates to a plain literal)" << endl
          << "               \"Hallo\"@de (evaluates to a plain literal)" << endl
          << "   - An empty string evaluates to an empy node (\"\" does the trick)" << endl;

        if ( !error.isEmpty() ) {
            s << endl << error << endl;
            return 0;
        }
        else {
            return 1;
        }
    }
}


#ifndef Q_OS_WIN
void signalHandler( int signal )
{
    switch( signal ) {
    case SIGHUP:
    case SIGQUIT:
    case SIGINT:
        // shut the monitor down gracefully so we can cleanup
        // afterwards
        if ( s_monitor ) {
            s_monitor->stopMonitoring();
        }
        else {
            exit( 2 );
        }
    }
}
#endif


int main( int argc, char *argv[] )
{
    QCoreApplication app( argc, argv );

#ifndef Q_OS_WIN
    struct sigaction sa;
    ::memset( &sa, 0, sizeof( sa ) );
    sa.sa_handler = signalHandler;
    sigaction( SIGHUP, &sa, 0 );
    sigaction( SIGINT, &sa, 0 );
    sigaction( SIGQUIT, &sa, 0 );
#endif

    QHash<QString, bool> allowedCmdLineArgs;
    allowedCmdLineArgs.insert( "model", true );
    allowedCmdLineArgs.insert( "version", false );
    allowedCmdLineArgs.insert( "help", false );
    allowedCmdLineArgs.insert( "backend", true );
    allowedCmdLineArgs.insert( "settings", true );
    allowedCmdLineArgs.insert( "dir", true );
    allowedCmdLineArgs.insert( "port", true );
    allowedCmdLineArgs.insert( "host", true );
    allowedCmdLineArgs.insert( "username", true );
    allowedCmdLineArgs.insert( "password", true );
    allowedCmdLineArgs.insert( "socket", true );
#ifdef BUILD_DBUS_SUPPORT
    allowedCmdLineArgs.insert( "dbus", true );
#endif
    allowedCmdLineArgs.insert( "sparql", true );
    allowedCmdLineArgs.insert( "serialization", true );
    allowedCmdLineArgs.insert( "querylang", true );
    allowedCmdLineArgs.insert( "file", true );
#ifdef BUILD_CLUCENE_INDEX
    allowedCmdLineArgs.insert( "index", true );
#endif
    allowedCmdLineArgs.insert( "nrl", false );
    allowedCmdLineArgs.insert( "foo", false );
    allowedCmdLineArgs.insert( "graphselect", true );

    if ( !CmdLineArgs::parseCmdLine( args, app.arguments(), allowedCmdLineArgs ) ) {
        return 1;
    }

    if ( args.optionSet( "version" ) ) {
        return printVersion();
    }
    if ( args.optionSet( "help" ) ) {
        return printUsage();
    }

    if ( args.hasSetting( "backend" ) ) {
        if ( args.hasSetting( "dbus" ) ||
             args.hasSetting( "socket" ) ||
             args.hasSetting( "sparql" ) ||
             args.hasSetting( "model" ) ) {
            return printUsage( "Cannot mix server parameters with --backend." );
        }
    }

    if ( args.hasSetting( "dir" ) &&
         !args.hasSetting( "backend" ) ) {
        return printUsage( "Parameter --dir only makes sense in combination with --backend." );
    }

    if ( args.hasSetting( "settings" ) &&
         !args.hasSetting( "backend" ) ) {
        return printUsage( "Parameter --settings only makes sense in combination with --backend." );
    }

    if ( args.hasSetting( "file" ) &&
         ( args.hasSetting( "dbus" ) ||
           args.hasSetting( "socket" ) ||
           args.hasSetting( "sparql" ) ||
           args.hasSetting( "model" ) ||
           args.hasSetting( "username" ) ||
           args.hasSetting( "password" ) ||
           args.hasSetting( "host" ) ||
           args.hasSetting( "port" ) ||
           args.hasSetting( "dir" ) ) ) {
        return printUsage( "Invalid parameters for --file mode." );
    }

    if ( args.optionSet( "foo" ) ) {
        s_interactive = false;
    }

    QString backendName = args.getSetting( "backend" );
    QString backendSettings = args.getSetting( "settings" );
    QString dir = args.getSetting( "dir" );
    QString command;
    QString modelName = args.getSetting( "model" );
    QString serialization = args.getSetting( "serialization", "application/x-nquads" );
    QString file = args.getSetting( "file" );
    QString queryLang = args.getSetting( "querylang", "SPARQL" );

    if ( modelName.isEmpty() &&
         backendName.isEmpty() &&
         file.isEmpty() &&
         !args.hasSetting( "sparql" ) ) {
        return printUsage( "No model name specified." );
    }

    QTextStream errStream(stderr);

    //
    // ====================================================================
    // Create the Model
    // ====================================================================
    //

    bool isRemoteModel = false;

    if ( args.hasSetting( "sparql" ) ) {
        QUrl sparqlEndPoint = args.getSetting( "sparql" );
        QString userName = args.getSetting( "username", sparqlEndPoint.userName() );
        QString password = args.getSetting( "password", sparqlEndPoint.password() );

        quint16 port = sparqlEndPoint.port( 80 );
        if ( args.hasSetting( "port" ) )
            port = args.getSetting( "port" ).toInt();

        Soprano::Client::SparqlModel* sparqlModel = new Soprano::Client::SparqlModel( sparqlEndPoint.host(), port,
                                                                                      userName, password );
        if ( !sparqlEndPoint.path().isEmpty() )
            sparqlModel->setPath( sparqlEndPoint.path() );

        s_model = sparqlModel;
        isRemoteModel = true;
    }
    else if ( args.hasSetting( "port" ) &&
              !args.hasSetting( "backend" ) ) {
        QHostAddress host = QHostAddress::LocalHost;
        quint16 port = Soprano::Client::TcpClient::DEFAULT_PORT;
        port = args.getSetting( "port" ).toInt();
        if ( args.hasSetting( "host" ) ) {
            host = args.getSetting( "host" );
        }

        Soprano::Client::TcpClient* tcpClient = new Soprano::Client::TcpClient();

        if ( !tcpClient->connect( host, port ) ) {
            errStream << "Failed to connect to server on port " << port << endl;
            delete tcpClient;
            return 3;
        }
        if ( !( s_model = tcpClient->createModel( modelName ) ) ) {
            errStream << "Failed to create Model: " << tcpClient->lastError() << endl;
            delete tcpClient;
            return 2;
        }
        isRemoteModel = true;
    }
    else if ( args.hasSetting( "socket" ) ) {
        QString socketPath = args.getSetting( "socket" );
        Soprano::Client::LocalSocketClient* localSocketClient = new Soprano::Client::LocalSocketClient();
        if ( !localSocketClient->connect( socketPath ) ) {
            errStream << "Failed to contact Soprano server through socket at " << socketPath << endl;
            delete localSocketClient;
            return 3;
        }
        if ( !( s_model = localSocketClient->createModel( modelName ) ) ) {
            errStream << "Failed to create Model: " << localSocketClient->lastError() << endl;
            delete localSocketClient;
            return 2;
        }
        isRemoteModel = true;
    }
#ifdef BUILD_DBUS_SUPPORT
    else if ( args.hasSetting( "dbus" ) ) {
        QString dbusService = args.getSetting( "dbus" );
        Soprano::Client::DBusClient* dbusClient = new Soprano::Client::DBusClient( dbusService );
        if ( !dbusClient->isValid() ) {
            errStream << "Failed to contact Soprano dbus service at " << dbusService << endl;
            delete dbusClient;
            return 3;
        }
        if ( !( s_model = dbusClient->createModel( modelName ) ) ) {
            errStream << "Failed to create Model: " << dbusClient->lastError() << endl;
            delete dbusClient;
            return 2;
        }
        isRemoteModel = true;
    }
#endif
    else if ( !file.isEmpty() ) {
        if ( !( s_model = createMemoryModel( backendName ) ) ) {
            errStream << "Failed to create temporary model." << endl;
            return 2;
        }
        if ( int r = importFile( s_model, file, serialization ) ) {
            errStream << "Failed to parse " << file << endl;
            return r;
        }
    }
    else if ( !backendName.isEmpty() ) {
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
        if ( args.hasSetting( "port" ) ) {
            settings.append( BackendSetting( BackendOptionPort, args.getSetting( "port" ).toInt() ) );
        }
        if ( args.hasSetting( "host" ) ) {
            settings.append( BackendSetting( BackendOptionHost, args.getSetting( "host" ) ) );
        }
        if ( args.hasSetting( "username" ) ) {
            settings.append( BackendSetting( BackendOptionUsername, args.getSetting( "username" ) ) );
        }
        if ( args.hasSetting( "password" ) ) {
            settings.append( BackendSetting( BackendOptionPassword, args.getSetting( "password" ) ) );
        }

        QStringList userSettings = backendSettings.split( ';', QString::SkipEmptyParts );
        foreach( const QString& setting, userSettings ) {
            QStringList keyValue = setting.split( '=' );
            if ( keyValue.count() != 2 ) {
                errStream << "Invalid backend setting: " << setting;
                return 2;
            }
            settings << BackendSetting( keyValue[0], keyValue[1] );
        }

        if ( !( s_model = backend->createModel( settings ) ) ) {
            errStream << "Failed to create Model: " << backend->lastError() << endl;
            return 2;
        }
    }
    else {
        errStream << "Please specify a backend to be used or details on how to contact" << endl
                  << "a soprano server." << endl;
        return 4;
    }

    int firstArg = 0;
    if ( args.count() >= 1 ) {
        command = args[0];
        ++firstArg;
    }
    else {
        return printUsage();
    }


    //
    // ====================================================================
    // Create an optional NRLModel
    // ====================================================================
    //

    if ( args.optionSet( "nrl" ) ) {
        NRLModel* nrlModel = new NRLModel( s_model );
        nrlModel->setEnableQueryPrefixExpansion( true );
        s_model->setParent( nrlModel ); // mem management
        s_model = nrlModel;
    }


    //
    // ====================================================================
    // Create an optional index
    // ====================================================================
    //

#ifdef BUILD_CLUCENE_INDEX
    if ( args.hasSetting( "index" ) ) {
        Index::IndexFilterModel* filterModel = new Index::IndexFilterModel( args.getSetting( "index" ), s_model );
        s_model->setParent( filterModel ); // mem management
        s_model = filterModel;
    }
#endif


    //
    // ====================================================================
    // Execute the actual command
    // ====================================================================
    //

    // Memory management the convenient way: will delete the model once it goes out of scope
    QSharedPointer<Soprano::Model> modelCleanupPointer( s_model );

    int queryTime = 0;

    if ( command == "import" ) {
        if ( firstArg != args.count()-1 ) {
            return printUsage();
        }

        QString fileName = args[firstArg];

        int r = importFile( s_model, fileName, serialization );
        return r;
    }
    else if ( command == "export" ) {
        QString fileName;
        QString query;

        if ( firstArg == args.count()-1 ) {
            fileName = args[firstArg];
        }
        else if ( firstArg == args.count()-2 ) {
            fileName = args[firstArg+1];
            query = args[firstArg];
        }
        else {
            return printUsage();
        }

        bool success = true;
        if ( query.isEmpty() ) {
            // export all statements
            success = !exportFile( s_model->listStatements(), fileName, serialization );
        }
        else {
            QueryResultIterator it = s_model->executeQuery( query,
                                                            Soprano::Query::queryLanguageFromString( queryLang ), queryLang );
            if ( it.isGraph() ) {
                success = !exportFile( it.iterateStatements(),
                                       fileName,
                                       serialization );
            }
            else {
                errStream << "Can only export graph queries";
                success = false;
            }
        }

        return success ? 0 : 1;
    }
    else {
        if ( command == "query" ) {
            if ( firstArg >= args.count() ) {
                return printUsage();
            }

            QString query = args[firstArg];

            QTime time;
            time.start();

            Soprano::QueryResultIterator it = s_model->executeQuery( query,
                                                                     Soprano::Query::queryLanguageFromString( queryLang ), queryLang );
            queryTime = time.elapsed();

            if ( !args.hasSetting( "file" ) &&
                 args.hasSetting( "serialization" ) &&
                 !( it.isGraph() || args.hasSetting( "graphselect" ) ) ) {
                errStream << "Can only serialize graph queries" << endl;
                return 1;
            }

            printQueryResult( it );
        }


        else if( command == "rmgraph" ) {
            QTime time;
            time.start();

            for( int i = 1; i < args.count(); ++i ) {
                Node n = parseNode( args.arg( i ) );
                if( n.isValid() ) {
                    s_model->removeContext( n );
                }
                else {
                    errStream << "Need to define a list of valid resource nodes with command 'rmgraph'." << endl;
                    return 1;
                }
            }

            queryTime = time.elapsed();
        }

        else {
            //
            // For all commands below (add, remove, list, monitor) we need a list of nodes as parameter
            //

            if ( !args.hasSetting( "file" ) &&
                 args.hasSetting( "serialization" ) ) {
                return printUsage( "Parameter --serialization does only make sense for commands 'import', 'export', and 'query'" );
            }

            if ( args.hasSetting( "querylang" ) ) {
                return printUsage( "--querylang does only make sense with command 'query'" );
            }

            // parse node commands (max 4)
            Soprano::Node n1, n2, n3, n4;
            if ( args.count() > 5 ) {
                return printUsage();
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

            if ( n1.isLiteral() ) {
                errStream << "Subject needs to be a resource or blank node." << endl;
                return 1;
            }
            if ( !n2.isResource() && !n2.isEmpty() ) {
                errStream << "Predicate needs to be a resource node." << endl;
                return 1;
            }
            if ( n4.isLiteral() ) {
                errStream << "Context needs to be a resource or blank node." << endl;
                return 1;
            }
            Statement statement( n1, n2, n3, n4 );

            QTime time;
            time.start();

            if ( command == "list" ) {
                Soprano::StatementIterator it = s_model->listStatements( statement );
                queryTime = time.elapsed();
                printStatementList( it );
            }
            else if ( command == "add" ) {
                if ( !statement.isValid() ) {
                    errStream << "At least subject, predicate, and object have to be defined." << endl;
                    return 1;
                }
                s_model->addStatement( statement );
                queryTime = time.elapsed();
            }
            else if ( command == "remove" || command == "rm" ) {
                s_model->removeAllStatements( statement );
                queryTime = time.elapsed();
            }
            else if ( command == "monitor" ) {
                if ( !isRemoteModel ) {
                    errStream << "Monitoring a non-local model does not make sense since no statements could be added or removed." << endl;
                    return 1;
                }

                ModelMonitor monitor( s_model );
                s_monitor = &monitor;
                monitor.monitor( statement );

                return 0;
            }
            else {
                errStream << "Unsupported command: " << command << endl;
                return 1;
            }
        }
    }

    if ( !s_model->lastError() ) {
        errStream << "Execution time: " << QTime().addMSecs( queryTime ).toString( "hh:mm:ss.z" ) << endl;
        return 0;
    }
    else {
        errStream << "Execution failed: " << s_model->lastError() << endl;
        return 2;
    }
}
