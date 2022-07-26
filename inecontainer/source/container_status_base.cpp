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
* This file implements the \ref Container::Status class.
***********************************************************************************************************************/

#include <string>
#include <cstdint>
#include <sstream>
#include <memory>
#include <cerrno>
#include <cstring>

#include "container_status_base.h"

/***********************************************************************************************************************
 * Container::Status
 */

namespace Container {
    Status::Status(PimplBase* pimpl) {
        impl = std::shared_ptr<Status::PimplBase>(pimpl);
    }


    Status::Status() {
        impl = std::shared_ptr<Status::PimplBase>(nullptr);
    }


    Status::Status(const Status& other) {
        impl = other.impl;
    }


    Status::~Status() {}


    Status& Status::operator=(const Status& other) {
        impl = other.impl;
        return *this;
    }


    Status::Class Status::statusClass() const {
        return impl ? impl->statusClass() : Status::Class::NO_STATUS;
    }


    int Status::errorCode() const {
        return impl ? impl->errorCode() : 0;
    }


    std::string Status::description() const {
        return impl ? impl->description() : std::string();
    }


    bool Status::informationAvailable() const {
        return impl ? impl->informationAvailable() : false;
    }


    bool Status::recoverable() const {
        return impl ? impl->recoverable() : true;
    }


    bool Status::success() const {
        return impl ? (informationAvailable() == false || recoverable() == true) : true;
    }


    bool Status::failure() const {
        return impl ? !recoverable() : false;
    }


    Status::operator bool() const {
        return informationAvailable();
    }


    std::shared_ptr<Status::PimplBase> Status::pimpl() {
        return impl;
    }


    std::shared_ptr<Status::PimplBase> Status::pimpl() const {
        return impl;
    }
};

/***********************************************************************************************************************
 * Container::Status::PimplBase
 */

namespace Container {
    Status::PimplBase::PimplBase() {}


    Status::PimplBase::~PimplBase() {}


    bool Status::PimplBase::informationAvailable() const {
        return true;
    }
};

/***********************************************************************************************************************
 * Container::Status::StatusPimplBase
 */

namespace Container {
    Status::StatusPimplBase::StatusPimplBase() {}


    Status::StatusPimplBase::~StatusPimplBase() {}


    bool Status::StatusPimplBase::recoverable() const {
        return true;
    }
};


/***********************************************************************************************************************
 * Container::Status::ErrorPimplBase
 */

namespace Container {
    Status::ErrorPimplBase::ErrorPimplBase() {}


    Status::ErrorPimplBase::~ErrorPimplBase() {}


    bool Status::ErrorPimplBase::recoverable() const {
        return false;
    }
};
