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
* This header provides a base class for tests of the Container::FileContainer class.
***********************************************************************************************************************/

#ifndef TEST_FILE_CONTAINER_H
#define TEST_FILE_CONTAINER_H

#include <QObject>
#include <QtTest/QtTest>

#include <memory>
#include <string>

#include <container_file_container.h>

#include "test_container_base.h"

/**
 * Class that extends \ref TestContainerBase to support tests of the \ref Container::FileContainer class.
 */
class TestFileContainer:public TestContainerBase {
    Q_OBJECT

    protected:
        /**
         * Method that is called by the base class to allocate a memory container.
         *
         * \param[in] fileIdentifier A string placed at a fixed location near the beginning of the file.  The string can
         *                           be used as a magic number to identifier the file type and is used as a check when
         *                           opening a new container.
         *
         * \return Returns pointer to the requested container.
         */
        std::shared_ptr<Container::Container> allocateContainer(const std::string& fileIdentifier) final;

        /**
         * Method that is called by the base class to open a container of the appropriate type.
         *
         * \param[in] container A shared pointer to the container to be opened.
         *
         * \param[in] resetContents If true, the contents of the container should be reset to an empty state.
         */
        Container::Status openContainer(std::shared_ptr<Container::Container> container,bool resetContents) final;

        /**
         * Method that is called by the base class to close a container.
         *
         * \param[in] container A shared pointer to the container to be closed.
         */
        Container::Status closeContainer(std::shared_ptr<Container::Container> container) final;

        /**
         * Method that is called to determine the size of the container file.
         *
         * \return Returns the size of the container file, in bytes.
         */
        unsigned long long containerSize() const final;

    private:
        static const char containerFilename[];
};

#endif
