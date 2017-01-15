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
#include <QtCore/QDebug>

#ifdef Q_OS_WIN
static inline QString getWinPrefix()
{
    // returns the parent directory of the application
    return QDir( QCoreApplication::applicationDirPath() ).absoluteFilePath( ".." );
}
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
            const char* const extList[] = { ".so", ".dylib", ".bundle", ".sl", ".dll", "d.dll", 0 };
            for ( int i = 0; extList[i]; ++i ) {
                if ( QLibrary::isLibrary( libname + extList[i] ) ) {
                    libNames.append( libname + extList[i] );
                }
                if ( QLibrary::isLibrary( QLatin1String( "lib" ) + libname + extList[i] ) ) {
                    libNames.append( QLatin1String( "lib" ) + libname + extList[i] );
                }
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

    // subdirs to search
    QStringList subDirs;
    foreach( const QString& subDir, subDirs_ ) {
        QString s( subDir );
        if ( !s.endsWith( '/' ) )
            s += '/';
        subDirs << s;
    }
    // we add the empty string to be able to handle all in one loop below
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
                    QFileInfo fi( dir + '/' + subDir + lib );
                    if ( fi.isFile() ) {
                        return fi.absoluteFilePath();
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
    paths << QLatin1String( SOPRANO_FULL_LIB_DIR );
#ifdef Q_OS_WIN
    paths << QLatin1String( SOPRANO_BIN_DIR );
    paths << getWinPrefix() + QLatin1String( "/bin" );
    paths << getWinPrefix() + QLatin1String( SOPRANO_FULL_LIB_DIR );
#else
    paths << QLatin1String( "/usr/" SOPRANO_LIB_DIR );
    paths << QLatin1String( "/usr/local/" SOPRANO_LIB_DIR );
    paths += Soprano::envDirList( "LD_LIBRARY_PATH" );
#endif
    return paths;
}


QStringList Soprano::dataDirs()
{
    QStringList paths;
    paths << QLatin1String( SOPRANO_DATA_DIR )
#ifdef Q_OS_WIN
          << getWinPrefix() + QLatin1String( "/share" )
#endif
          << Soprano::envDirList( "SOPRANO_DIRS" )
          << Soprano::envDirList( "XDG_DATA_DIRS" );
    return paths;
}


QStringList Soprano::exeDirs()
{
    QStringList paths;
    paths << QLatin1String( SOPRANO_BIN_DIR )
#ifdef Q_OS_WIN
          << getWinPrefix() + QLatin1String( "/bin" )
#endif
          << envDirList( "PATH" );
    return paths;
}
