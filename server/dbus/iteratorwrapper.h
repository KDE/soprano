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

#ifndef _SOPRANO_SERVER_DBUS_ITERATOR_WRAPPER_H_
#define _SOPRANO_SERVER_DBUS_ITERATOR_WRAPPER_H_

#include <QtCore/QObject>

#include <soprano/statementiterator.h>
#include <soprano/nodeiterator.h>
#include <soprano/queryresultiterator.h>

namespace Soprano {
    namespace Server {
	/**
	 * The only purpose of the IteratorWrapper is to provide
	 * a QObject parent for the DBus interfaces of the Soprano
	 * iterators.
	 */
	class IteratorWrapper : public QObject
	{
	    Q_OBJECT
	    
	public:
	    IteratorWrapper( StatementIterator it, QObject* parent );
	    IteratorWrapper( NodeIterator it, QObject* parent );
	    IteratorWrapper( QueryResultIterator it, QObject* parent );
	    ~IteratorWrapper();

	    inline StatementIterator& statementIterator() { return m_statementIterator; }
	    inline NodeIterator& nodeIterator() { return m_nodeIterator; }
	    inline QueryResultIterator& queryResultIterator() { return m_queryResultIterator; }

	private:
	    StatementIterator m_statementIterator;
	    NodeIterator m_nodeIterator;
	    QueryResultIterator m_queryResultIterator;
	};
    }
}

#endif
