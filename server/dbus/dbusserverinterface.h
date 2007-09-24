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

#ifndef _SOPRANO_SERVER_DBUS_SERVER_INTERFACE_H_
#define _SOPRANO_SERVER_DBUS_SERVER_INTERFACE_H_

#include <QtCore/QObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtDBus/QtDBus>

#include <soprano/backend.h>

Q_DECLARE_METATYPE(Soprano::BackendSettings)

namespace Soprano {
    namespace Client {
	class DBusServerInterface: public QDBusAbstractInterface
	{
	    Q_OBJECT

	public:
	    DBusServerInterface( const QString& service, const QString& path, const QDBusConnection& connection, QObject* parent = 0 );
	    ~DBusServerInterface();

	public Q_SLOTS:
	    inline QDBusReply<QStringList> allModels()
	    {
		QList<QVariant> argumentList;
		return callWithArgumentList(QDBus::Block, QLatin1String("allModels"), argumentList);
	    }

	    inline QDBusReply<QString> createModel( const QString &name )
	    {
		QList<QVariant> argumentList;
		argumentList << qVariantFromValue(name);
		return callWithArgumentList(QDBus::Block, QLatin1String("createModel"), argumentList);
	    }
	};
    }
}

#endif
