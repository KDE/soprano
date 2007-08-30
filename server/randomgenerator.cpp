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

#include "randomgenerator.h"

#include <QtCore/QMutex>

#include <stdlib.h>
#include <time.h>


static QMutex s_creationMutex;
static Soprano::RandomGenerator* s_instance = 0;

Soprano::RandomGenerator::RandomGenerator()
{
    s_instance = this;
    srand( time(0) );
}


Soprano::RandomGenerator::~RandomGenerator()
{
    s_instance = 0;
}


int Soprano::RandomGenerator::randomInt()
{
    m_mutex.lock();
    int num = rand();
    m_mutex.unlock();

    return num;
}


Soprano::RandomGenerator* Soprano::RandomGenerator::instance()
{
    s_creationMutex.lock();
    if ( !s_instance ) {
        s_instance = new RandomGenerator();
    }
    s_creationMutex.unlock();

    return s_instance;
}
