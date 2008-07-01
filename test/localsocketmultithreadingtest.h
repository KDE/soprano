/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2008 Sebastian Trueg <trueg@kde.org>
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

#ifndef _LOCALSOCKET_MULTITHREAD_TEST_H_
#define _LOCALSOCKET_MULTITHREAD_TEST_H_

#include "multithreadingtest.h"

namespace Soprano {
    class Model;
    namespace Client {
        class LocalSocketClient;
    }
}

class LocalSocketMultiThreadingTest : public MultiThreadingTest
{
    Q_OBJECT

public:
    LocalSocketMultiThreadingTest();

protected:
    virtual Soprano::Model* createModel();

private:
    Soprano::Client::LocalSocketClient* m_client;
    int m_modelCounter;
};

#endif
