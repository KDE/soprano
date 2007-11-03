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

#ifndef _INDEX_MULTITHREAD_TEST_H_
#define _INDEX_MULTITHREAD_TEST_H_

#include "multithreadingtest.h"

namespace Soprano {
    class Model;
    namespace Index {
	class IndexFilterModel;
    }
}

class IndexMultiThreadTest : public MultiThreadingTest
{
    Q_OBJECT

protected:
    virtual Soprano::Model* createModel();
    void deleteModel( Soprano::Model* );

private:
    Soprano::Model* m_model;
    Soprano::Index::IndexFilterModel* m_indexModel;
};

#endif
