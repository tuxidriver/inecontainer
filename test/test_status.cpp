/*-*-c++-*-*************************************************************************************************************
* Copyright 2016 - 2022 Inesonic, LLC.
*
* MIT License:
*   Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
*   documentation files (the "Software"), to deal in the Software without restriction, including without limitation the
*   rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
*   permit persons to whom the Software is furnished to do so, subject to the following conditions:
*   
*   The above copyright notice and this permission notice shall be included in all copies or substantial portions of the
*   Software.
*   
*   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
*   WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS
*   OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
*   OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
********************************************************************************************************************//**
* \file
*
* This file implements tests of the Container::Status class.
***********************************************************************************************************************/

#include <QDebug>
#include <QtTest/QtTest>

#include <string>

#include <container_status.h>

#include "test_status.h"

/***********************************************************************************************************************
 * Status
 */

class Status:public Container::Status {
    public:
        Status(Container::Status::Class statusClass, int errorCode, const std::string& description);

        ~Status();

    private:
        class TestStatusImpl:public Container::Status::StatusPimplBase {
            public:
                TestStatusImpl(Container::Status::Class statusClass, int errorCode, const std::string& description);

                ~TestStatusImpl();

                Container::Status::Class statusClass() const final;

                int errorCode() const final;

                std::string description() const final;

            private:
                Container::Status::Class currentStatusClass;
                int                      currentErrorCode;
                std::string              currentDescription;
        };
};


Status::Status(
        Container::Status::Class statusClass,
        int                      errorCode,
        const std::string&       description
    ):Container::Status(
        new Status::TestStatusImpl(statusClass, errorCode, description)
    ) {}


Status::~Status() {}


Status::TestStatusImpl::TestStatusImpl(
        Container::Status::Class statusClass,
        int                      errorCode,
        const std::string&       description
    ) {
    currentStatusClass = statusClass;
    currentErrorCode   = errorCode;
    currentDescription = description;
}


Status::TestStatusImpl::~TestStatusImpl() {}


Container::Status::Class Status::TestStatusImpl::statusClass() const {
    return currentStatusClass;
}


int Status::TestStatusImpl::errorCode() const {
    return currentErrorCode;
}


std::string Status::TestStatusImpl::description() const {
    return currentDescription;
}

/***********************************************************************************************************************
 * TestStatus
 */

void TestStatus::testConstructorsDestructors() {
    // Default constructor
    Container::Status status1;

    QVERIFY(!status1.informationAvailable());
    QVERIFY( status1.recoverable());
    QVERIFY( status1.success());
    QVERIFY(!status1.failure());
    QVERIFY(!status1);

    QVERIFY(status1.errorCode() == 0);
    QVERIFY(status1.statusClass() == Container::Status::Class::NO_STATUS);
    QVERIFY(status1.description().empty());

    // Protected constructor
    Status status2(Container::Status::Class::FILESYSTEM_ERROR, 1, "Error description");

    QVERIFY( status2.informationAvailable());
    QVERIFY( status2.recoverable());
    QVERIFY( status2.success());
    QVERIFY(!status2.failure());
    QVERIFY( status2);

    QVERIFY(status2.errorCode() == 1);
    QVERIFY(status2.statusClass() == Container::Status::Class::FILESYSTEM_ERROR);
    QVERIFY(status2.description() == "Error description");

    // Copy constructor
    Container::Status status3 = status2;

    QVERIFY( status3.informationAvailable());
    QVERIFY( status3.recoverable());
    QVERIFY( status3.success());
    QVERIFY(!status3.failure());
    QVERIFY( status3);

    QVERIFY(status3.errorCode() == 1);
    QVERIFY(status3.statusClass() == Container::Status::Class::FILESYSTEM_ERROR);
    QVERIFY(status3.description() == "Error description");
}


void TestStatus::testAccessors() {
    Status status(Container::Status::Class::FILESYSTEM_ERROR, 1, "Error description");

    QVERIFY( status.informationAvailable());
    QVERIFY( status.recoverable());
    QVERIFY( status.success());
    QVERIFY(!status.failure());
    QVERIFY( status);

    QVERIFY(status.errorCode() == 1);
    QVERIFY(status.statusClass() == Container::Status::Class::FILESYSTEM_ERROR);
    QVERIFY(status.description() == "Error description");
}


void TestStatus::testAssignmentOperator() {
    Container::Status status1;

    QVERIFY(!status1.informationAvailable());
    QVERIFY( status1.recoverable());
    QVERIFY( status1.success());
    QVERIFY(!status1.failure());
    QVERIFY(!status1);

    QVERIFY(status1.errorCode() == 0);
    QVERIFY(status1.statusClass() == Container::Status::Class::NO_STATUS);
    QVERIFY(status1.description().empty());

    Status status2(Container::Status::Class::FILESYSTEM_ERROR, 1, "Error description");

    QVERIFY( status2.informationAvailable());
    QVERIFY( status2.recoverable());
    QVERIFY( status2.success());
    QVERIFY(!status2.failure());
    QVERIFY( status2);

    QVERIFY(status2.errorCode() == 1);
    QVERIFY(status2.statusClass() == Container::Status::Class::FILESYSTEM_ERROR);
    QVERIFY(status2.description() == "Error description");

    status1 = status2;

    QVERIFY( status1.informationAvailable());
    QVERIFY( status1.recoverable());
    QVERIFY( status1.success());
    QVERIFY(!status1.failure());
    QVERIFY( status1);

    QVERIFY(status1.errorCode() == 1);
    QVERIFY(status1.statusClass() == Container::Status::Class::FILESYSTEM_ERROR);
    QVERIFY(status1.description() == "Error description");
}
