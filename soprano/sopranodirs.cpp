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
#include <QtCore/QFile>


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
            }
        }
        else {
            libNames.append(libname);
        }
        return libNames;
    }
}


QString Soprano::findLibraryPath( const QString& libName, const QStringList& extraDirs )
{
    QStringList dirs( libDirs() );
    dirs << extraDirs;

    QStringList libs = makeLibNames( libName );
    Q_FOREACH( const QString& lib, libs ) {
        if ( lib.startsWith( '/' ) ) {
            return lib;
        }
        else {
            foreach( const QString& dir, dirs ) {
                if ( QFile::exists( dir + SOPRANO_LIB_SUFFIX"/" + lib ) ) {
                    return dir + SOPRANO_LIB_SUFFIX"/" + lib;
                }
                else if ( QFile::exists( dir + '/' + lib ) ) {
                    return dir + "/" + lib;
                }
                else if ( QFile::exists( dir + "64/" + lib ) ) {
                    return dir + "64/" + lib;
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
    QStringList paths;
    paths << QLatin1String( SOPRANO_PREFIX"/lib" );
    paths << QLatin1String( "/usr/lib" );
    paths << QLatin1String( "/usr/local/lib" );
#ifndef Q_OS_WIN
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
