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

#ifndef _SOPRANO_SERVER_DBUS_BINDINGSETITERATOR_INTERFACE_H_
#define _SOPRANO_SERVER_DBUS_BINDINGSETITERATOR_INTERFACE_H_

#include <QtCore/QVariant>
#include <QtDBus/QtDBus>

#include <soprano/bindingset.h>
#include <soprano/node.h>
#include <soprano/statement.h>

Q_DECLARE_METATYPE(Soprano::BindingSet);
Q_DECLARE_METATYPE(Soprano::Statement);
Q_DECLARE_METATYPE(Soprano::Node);

namespace Soprano {

    class BindingSet;

    namespace Server {
	class DBusQueryResultIteratorInterface: public QDBusAbstractInterface
	{
	    Q_OBJECT

	public:
	    DBusQueryResultIteratorInterface( const QString& service, const QString& path, const QDBusConnection& connection, QObject *parent = 0 );
	    ~DBusQueryResultIteratorInterface();

	public Q_SLOTS:
	    inline QDBusReply<Soprano::Node> bindingByIndex(int index)
	    {
		QList<QVariant> argumentList;
		argumentList << qVariantFromValue(index);
		return callWithArgumentList(QDBus::Block, QLatin1String("bindingByIndex"), argumentList);
	    }

	    inline QDBusReply<Soprano::Node> bindingByName(const QString &name)
	    {
		QList<QVariant> argumentList;
		argumentList << qVariantFromValue(name);
		return callWithArgumentList(QDBus::Block, QLatin1String("bindingByName"), argumentList);
	    }

	    inline QDBusReply<int> bindingCount()
	    {
		QList<QVariant> argumentList;
		return callWithArgumentList(QDBus::Block, QLatin1String("bindingCount"), argumentList);
	    }

	    inline QDBusReply<QStringList> bindingNames()
	    {
		QList<QVariant> argumentList;
		return callWithArgumentList(QDBus::Block, QLatin1String("bindingNames"), argumentList);
	    }

	    inline QDBusReply<bool> boolValue()
	    {
		QList<QVariant> argumentList;
		return callWithArgumentList(QDBus::Block, QLatin1String("boolValue"), argumentList);
	    }

	    inline QDBusReply<Soprano::BindingSet> current()
	    {
		QList<QVariant> argumentList;
		return callWithArgumentList(QDBus::Block, QLatin1String("current"), argumentList);
	    }

	    inline QDBusReply<Soprano::Statement> currentStatement()
	    {
		QList<QVariant> argumentList;
		return callWithArgumentList(QDBus::Block, QLatin1String("currentStatement"), argumentList);
	    }

	    inline QDBusReply<bool> isBinding()
	    {
		QList<QVariant> argumentList;
		return callWithArgumentList(QDBus::Block, QLatin1String("isBinding"), argumentList);
	    }

	    inline QDBusReply<bool> isBool()
	    {
		QList<QVariant> argumentList;
		return callWithArgumentList(QDBus::Block, QLatin1String("isBool"), argumentList);
	    }

	    inline QDBusReply<bool> isGraph()
	    {
		QList<QVariant> argumentList;
		return callWithArgumentList(QDBus::Block, QLatin1String("isGraph"), argumentList);
	    }

	    inline QDBusReply<bool> next()
	    {
		QList<QVariant> argumentList;
		return callWithArgumentList(QDBus::Block, QLatin1String("next"), argumentList);
	    }

	    inline QDBusReply<void> close()
	    {
		QList<QVariant> argumentList;
		return callWithArgumentList(QDBus::Block, QLatin1String("close"), argumentList);
	    }
	};
    }
}

#endif
