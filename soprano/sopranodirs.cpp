/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2007-2009 Sebastian Trueg <trueg@kde.org>
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

#include "sopranodirs.h"
#include "soprano-config.h"

#include <QtCore/QLibrary>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QCoreApplication>


#if defined _WIN32 || defined _WIN64
#define PATH_SEPARATOR ';'
#else
#define PATH_SEPARATOR ':'
#endif



namespace {
    QStringList makeLibNames( const QString& libname )
    {
        int pos = libname.lastIndexOf('/');
        if (pos < 0) {
            pos = 0;
        }

        QStringList libNames;
        if (libname.indexOf('.', pos) < 0) {
            const char* const extList[] = { ".so", ".dylib", ".bundle", ".sl", ".dll", 0 };
            for ( int i = 0; extList[i]; ++i ) {
                if ( QLibrary::isLibrary( libname + extList[i] ) ) {
                    libNames.append( libname + extList[i] );
                }
#ifndef Q_OS_WIN
                if ( QLibrary::isLibrary( QLatin1String( "lib" ) + libname + extList[i] ) ) {
                    libNames.append( QLatin1String( "lib" ) + libname + extList[i] );
                }
#endif
            }
        }
        else {
            libNames.append(libname);
        }
        return libNames;
    }
}


QString Soprano::findLibraryPath( const QString& libName, const QStringList& extraDirs, const QStringList& subDirs_ )
{
    // paths to search for libs
    QStringList dirs = libDirs() + extraDirs;

    // suffixes to search
    QStringList suffixes;
    suffixes << QLatin1String( "/"SOPRANO_LIB_SUFFIX"/" )
             << QString( '/' )
             << QLatin1String( "64/" );

    // we add the empty string to be able to handle all in one loop below
    QStringList subDirs( subDirs_ );
    subDirs << QString();

    QStringList libs = makeLibNames( libName );
    Q_FOREACH( const QString& lib, libs ) {
        if ( lib.startsWith( '/' ) ) {
            QFileInfo fi( lib );
            if ( fi.isFile() )
                return fi.absoluteFilePath();
        }
        else {
            foreach( const QString& dir, dirs ) {
                foreach( const QString& subDir, subDirs ) {
                    foreach( const QString& suffix, suffixes ) {
                        QFileInfo fi( dir + suffix + subDir + '/' + lib );
                        if ( fi.isFile() ) {
                            return fi.absoluteFilePath();
                        }
                    }
                }
            }
        }
    }
    return QString();
}


QStringList Soprano::envDirList( const char* var )
{
    QStringList dirs;
    QByteArray varData = qgetenv( var );
    if ( !varData.isEmpty() ) {
        QStringList d = QString::fromLocal8Bit( varData ).split( PATH_SEPARATOR );
        Q_FOREACH( const QString &dir, d ) {
            dirs += QDir::fromNativeSeparators( dir );
        }
    }
    return dirs;
}


QStringList Soprano::libDirs()
{
    QStringList paths = QCoreApplication::libraryPaths();
#ifndef Q_OS_WIN
    paths << QLatin1String( SOPRANO_PREFIX"/lib" );
    paths << QLatin1String( "/usr/lib" );
    paths << QLatin1String( "/usr/local/lib" );
    paths += Soprano::envDirList( "LD_LIBRARY_PATH" );
#endif
    return paths;
}


QStringList Soprano::dataDirs()
{
    QStringList paths;
    paths << QLatin1String( SOPRANO_PREFIX"/share" )
          << Soprano::envDirList( "SOPRANO_DIRS" )
          << Soprano::envDirList( "XDG_DATA_DIRS" );
    return paths;
}


QStringList Soprano::exeDirs()
{
    QStringList paths;
    paths << QLatin1String( SOPRANO_PREFIX"/bin" )
          << envDirList( "PATH" );
    return paths;
}
