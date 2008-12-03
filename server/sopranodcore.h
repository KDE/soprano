/*
 * This file is part of Soprano Project.
 *
 * Copyright (C) 2007 Sebastian Trueg <trueg@kde.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * See the file "COPYING" for the exact licensing terms.
 */

#ifndef _SOPRANOD_CORE_H_
#define _SOPRANOD_CORE_H_

#include "servercore.h"
#include "../soprano/backend.h"

#include <QtCore/QHash>

class SopranodCore : public Soprano::Server::ServerCore
{
    Q_OBJECT

public:
    SopranodCore( bool withIndex, QObject* parent = 0 );
    ~SopranodCore();

private:
    Soprano::Model* createModel( const Soprano::BackendSettings& settings );

    bool m_withIndex;
    QHash<QString, Soprano::Model*> m_models;
};

#endif
