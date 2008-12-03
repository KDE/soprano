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

#include "sopranodcore.h"
#include "soprano-server-config.h"

#include "../index/indexfiltermodel.h"
#include "../index/cluceneindex.h"
#include "../soprano/backend.h"

#include <QtCore/QList>
#include <QtCore/QDir>


SopranodCore::SopranodCore( bool withIndex, QObject* parent )
    : ServerCore( parent ),
      m_withIndex( withIndex )
{
}


SopranodCore::~SopranodCore()
{
}


Soprano::Model* SopranodCore::createModel( const Soprano::BackendSettings& settings )
{
#ifdef SOPRANO_BUILD_INDEX_LIB
    if ( m_withIndex ) {
        if ( Soprano::Model* m = ServerCore::createModel( settings ) ) {
            QString dir = valueInSettings( settings, Soprano::BackendOptionStorageDir ).toString() + "/index";
            Soprano::Index::CLuceneIndex* index = new Soprano::Index::CLuceneIndex();
            QDir().mkpath( dir );
            if ( !index->open( dir, true ) ) {
                delete m;
                delete index;
                return 0;
            }
            Soprano::Index::IndexFilterModel* indexModel = new Soprano::Index::IndexFilterModel( index, m );
            indexModel->setParent( m );
            // FIXME: memory leak: index is never deleted
            return indexModel;
        }
    }
#endif

    // fallback
    return ServerCore::createModel( settings );
}

#include "sopranodcore.moc"
