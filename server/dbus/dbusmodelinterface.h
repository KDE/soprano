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

#ifndef _SOPRANO_SERVER_DBUS_MODEL_INTERFACE_H_
#define _SOPRANO_SERVER_DBUS_MODEL_INTERFACE_H_

#include <QtCore/QObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtDBus/QtDBus>

#include <soprano/statement.h>

Q_DECLARE_METATYPE(Soprano::Statement)
Q_DECLARE_METATYPE(Soprano::Node)


namespace Soprano {
    namespace Client {
	class DBusModelInterface : public QDBusAbstractInterface
	{
	    Q_OBJECT

	public:
	    DBusModelInterface( const QString& service, const QString& path, const QDBusConnection& connection, QObject* parent = 0 );
	    ~DBusModelInterface();

	public Q_SLOTS:
	    inline QDBusReply<int> addStatement( const Soprano::Statement& statement, QDBus::CallMode mode = QDBus::Block )
	    {
		QList<QVariant> argumentList;
		argumentList << qVariantFromValue(statement);
		return callWithArgumentListAndBigTimeout(mode, QLatin1String("addStatement"), argumentList);
	    }

	    inline QDBusReply<bool> containsAnyStatement( const Soprano::Statement& statement, QDBus::CallMode mode = QDBus::Block )
	    {
		QList<QVariant> argumentList;
		argumentList << qVariantFromValue(statement);
		return callWithArgumentListAndBigTimeout(mode, QLatin1String("containsAnyStatement"), argumentList);
	    }

	    inline QDBusReply<bool> containsStatement( const Soprano::Statement& statement, QDBus::CallMode mode = QDBus::Block )
	    {
		QList<QVariant> argumentList;
		argumentList << qVariantFromValue(statement);
		return callWithArgumentListAndBigTimeout(mode, QLatin1String("containsStatement"), argumentList);
	    }

	    inline QDBusReply<Soprano::Node> createBlankNode( QDBus::CallMode mode = QDBus::Block )
	    {
		QList<QVariant> argumentList;
		return callWithArgumentList(mode, QLatin1String("createBlankNode"), argumentList);
	    }

	    inline QDBusReply<QString> executeQuery( const QString &query, int queryLang, const QString& userQueryLanguage, QDBus::CallMode mode = QDBus::Block )
	    {
		QList<QVariant> argumentList;
		argumentList << qVariantFromValue(query) << qVariantFromValue(queryLang) << qVariantFromValue(userQueryLanguage);
		return callWithArgumentListAndBigTimeout(mode, QLatin1String("executeQuery"), argumentList);
	    }

	    inline QDBusReply<bool> isEmpty( QDBus::CallMode mode = QDBus::Block )
	    {
		QList<QVariant> argumentList;
		return callWithArgumentListAndBigTimeout(mode, QLatin1String("isEmpty"), argumentList);
	    }

	    inline QDBusReply<QString> listContexts( QDBus::CallMode mode = QDBus::Block )
	    {
		QList<QVariant> argumentList;
		return callWithArgumentListAndBigTimeout(mode, QLatin1String("listContexts"), argumentList);
	    }

	    inline QDBusReply<QString> listStatements( const Soprano::Statement& statement, QDBus::CallMode mode = QDBus::Block )
	    {
		QList<QVariant> argumentList;
		argumentList << qVariantFromValue(statement);
		return callWithArgumentListAndBigTimeout(mode, QLatin1String("listStatements"), argumentList);
	    }

	    inline QDBusReply<int> removeAllStatements( const Soprano::Statement& statement, QDBus::CallMode mode = QDBus::Block )
	    {
		QList<QVariant> argumentList;
		argumentList << qVariantFromValue(statement);
		return callWithArgumentListAndBigTimeout(mode, QLatin1String("removeAllStatements"), argumentList);
	    }

	    inline QDBusReply<int> removeStatement( const Soprano::Statement& statement, QDBus::CallMode mode = QDBus::Block )
	    {
		QList<QVariant> argumentList;
		argumentList << qVariantFromValue(statement);
		return callWithArgumentListAndBigTimeout(mode, QLatin1String("removeStatement"), argumentList);
	    }

	    inline QDBusReply<int> statementCount( QDBus::CallMode mode = QDBus::Block )
	    {
		QList<QVariant> argumentList;
		return callWithArgumentListAndBigTimeout(mode, QLatin1String("statementCount"), argumentList);
	    }

	Q_SIGNALS:
	    void statementsAdded();
	    void statementsRemoved();

	private:
	    QDBusMessage callWithArgumentListAndBigTimeout( QDBus::CallMode mode,
							    const QString& method,
							    const QList<QVariant>& args );
	};
    }
}

#endif
