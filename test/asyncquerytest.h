/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2010 Sebastian Trueg <trueg@k3b.org>
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

#ifndef _ASYNC_QUERY_TEST_H_
#define _ASYNC_QUERY_TEST_H_

#include <QtTest/QtTest>
#include <QtCore/QObject>

namespace Soprano {
    class Model;
    namespace Util {
        class AsyncQuery;
    }
}
class QEventLoop;

class AsyncQueryHandler : public QObject
{
    Q_OBJECT

public:
    AsyncQueryHandler( Soprano::Util::AsyncQuery* );

    bool waitForNext();
    bool waitForFinished();

private Q_SLOTS:
    void slotNextReady();
    void slotFinished();

private:
    Soprano::Util::AsyncQuery* m_query;
    QEventLoop* m_nextLoop;
    QEventLoop* m_finishedLoop;
    bool m_nextReady;
    bool m_finished;
};


class AsyncQueryTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void testBindingNames();
    void testQueryType();
    void testBindingsQuery();
};

#endif
