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

#include "inifile.h"

#include <QtCore/QHash>
#include <QtCore/QFile>
#include <QtCore/QDebug>
#include <QtCore/QRegExp>
#include <QtCore/QTextStream>


class Soprano::IniFile::Private
{
public:
    QString fileName;
    QHash<QString, QHash<QString, QString> > entries;
};


Soprano::IniFile::IniFile()
    : d( new Private() )
{
}


Soprano::IniFile::IniFile( const QString& path )
    : d( new Private() )
{
    open( path );
}


Soprano::IniFile::~IniFile()
{
    close();
    delete d;
}


bool Soprano::IniFile::open( const QString& path )
{
    close();

    QFile f( path );
    if ( f.open( QIODevice::ReadOnly ) ) {
        QRegExp groupEx( "\\[(.*)\\]" );
        QRegExp entryEx( "([^\\s]*)=(.*)" );
        QTextStream s( &f );

        // FIXME: handle internationalization

        QString currentGroup;

        QString line = s.readLine();
        while ( !line.isNull() ) {
            if ( line.startsWith( '#' ) ) {
//                qDebug() << "(IniFile) comment:" << line;
            }
            else if ( !line.isEmpty() ) {
                if ( groupEx.exactMatch( line ) ) {
                    QString group = groupEx.cap( 1 );
//                    qDebug() << "(IniFile) found group:" << group;
                    currentGroup = group;
                }
                else if ( entryEx.exactMatch( line ) ) {
                    QString key = entryEx.cap( 1 );
                    QString value = entryEx.cap( 2 );
//                    qDebug() << "(IniFile) found entry in group" << currentGroup << ":" << key << "->" << value;
                    d->entries[currentGroup][key] = value;
                }
                else {
                    qDebug() << "(IniFile) could not handle line" << line;
                    close();
                    return false;
                }
            }

            line = s.readLine();
        }

        d->fileName = path;
        return true;
    }
    else {
        qDebug() << "(IniFile) Could not open" << path;
        return false;
    }
}


void Soprano::IniFile::close()
{
    d->entries.clear();
    // no-op for now
}


QString Soprano::IniFile::fileName() const
{
    return d->fileName;
}


QStringList Soprano::IniFile::groups() const
{
    return d->entries.keys();
}


QString Soprano::IniFile::readEntry( const QString& group, const QString& entry ) const
{
    return d->entries[group][entry];
}
