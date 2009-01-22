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

#ifndef _SOPRANO_SERVER_OPERATOR_TEST_H_
#define _SOPRANO_SERVER_OPERATOR_TEST_H_

#include <QObject>

class ServerOperatorTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void testInt32_data();
    void testInt32();
    void testUnsignedInt8_data();
    void testUnsignedInt8();
    void testUnsignedInt16_data();
    void testUnsignedInt16();
    void testUnsignedInt32_data();
    void testUnsignedInt32();
    void testBool_data();
    void testBool();
    void testString_data();
    void testString();
    void testUrl_data();
    void testUrl();
    void testLocator_data();
    void testLocator();
    void testError_data();
    void testError();
    void testErrorCode_data();
    void testErrorCode();
/*     void testBackendSetting_data(); */
/*     void testBackendSetting(); */
    void testLiteralValue_data();
    void testLiteralValue();
    void testNode_data();
    void testNode();
    void testStatement_data();
    void testStatement();
    void testBinding_data();
    void testBinding();
};

#endif
