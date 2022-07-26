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
* This header defines \ref Container::Status class.  You will generally always want to include \ref container_status.h
* rather than this file.
***********************************************************************************************************************/

/* .. sphinx-project inecontainer */

#ifndef CONTAINER_STATUS_BASE_H
#define CONTAINER_STATUS_BASE_H

#include <memory>
#include <string>
#include <cstdint>

namespace Container {
    /**
     * Class for all reported status conditions, including a non-error (success) condition.  The code snippet below
     * shows how you can use this class:
     *
     * \code{.cpp}
       bool        success = true;
       MyContainer container();

       Status status = container.open();
       if (status) {
           if (status.recoverable()) {
               cout << "Status reported the recoverable information: " << status.description() << endl;
           } else {
               cout << "Unrecoverable error " << status.description() << " (code " << status.errorCode() << ")" << endl;
               success = false;
           }
       } \endcode
     *
     * If you decide to develop your own container, you'll need to develop a \ref Container::Status class for each type
     * of status you might want to report.  You can do so by deriving from \ref Container::Status and creating your own
     * implementation class, derived from \ref Container::Status::PimplBase.  Note that several derived classes are
     * already defined that you can use to reduce the effort involved.
     *
     * The code snippet below shows how you might define a status class used to report filesystem errors.
     *
     * \code{.cpp}
       class FilesystemError:public Container::Status {
           public:
               FilesystemError(int errno);

               ~FilesystemError();

           private:
               class FilesystemErrorImpl:public Container::Status::ErrorPimplBase {
                   public:
                       FilesystemErrorImpl(int errno);

                       ~FilesystemErrorImpl() override;

                       Container::Status::Class statusClass() const final;

                       int errorCode() const final;

                       std::string description() const final;

                   private:
                       int currentErrorCode;
               };
       };


       FilesystemError::FilesystemError(int errno):Container::Status(new FilesystemError::FilesystemErrorImpl(errno) {}


       FilesystemError::~FilesystemError() {}


       FilesystemError::FilesystemErrorImpl::FilesystemErrorImpl(int errno) {
           currentErrorCode = errno;
       }


       FilesystemError::FilesystemErrorImpl::~FilesystemErrorImpl() {}


       Container::Status::Class FilesystemError::FilesystemErrorImpl::statusClass() const {
           return Container::Status::Class::FILESYSTEM_ERROR;
       }


       int FilesystemError::FilesystemErrorImpl::errorCode() const {
           return currentErrorCode;
       }


       std::string FilesystemError::FilesystemErrorImpl::description() const {
           return std::strerror(currentErrorCode);
       } \endcode
     *
     */
    class Status {
        public:
            /**
             * Enumeration of supported status classes.
             */
            enum class Class : std::uint8_t {
                /**
                 * Indicates no error and no status.
                 */
                NO_STATUS,

                /**
                 * Indicates an internal error associated with the container.
                 */
                CONTAINER_INTERNAL_ERROR,

                /**
                 * Indicates a file format error.  A portion of the container, other than the header was incorrectly
                 * formed.  This class includes such things as CRC errors or payload length errors, or virtual files
                 * that are not marked with an end of file marker.
                 */
                CONTAINER_FORMAT_ERROR,

                /**
                 * Indicates a file header error.  The header for the container was incorrectly formed.  The error can
                 * be returned when the header identifier string mismatches or the container version codes are invalid.
                 */
                CONTAINER_HEADER_ERROR,

                /**
                 * Indicates a downlevel file format version.  This error type is returned when the container major
                 * version code is incompatible with this version and represents a lower version number.
                 */
                CONTAINER_VERSION_DOWNLEVEL,

                /**
                 * Indicates success status from a filesystem operation with additional information.
                 */
                FILESYSTEM_STATUS,

                /**
                 * Indicates a filesystem error reported by a class derived from \ref Container::Container.
                 */
                FILESYSTEM_ERROR,

                /**
                 * Indicates an error reported by a derived class during a streaming read operation.
                 */
                STREAMING_READ_ERROR,

                /**
                 * Indicates an error outside of the container library.  You can use this error type to define user
                 * defined error status conditions.
                 */
                APPLICATION_ERROR
            };

            /**
             * Default constructor.  Creates an error instance that will report no error.
             */
            Status();

            /**
             * Copy constructor.
             *
             * \param[in] other The instance to be copied.
             */
            Status(const Status& other);

            ~Status();

            /**
             * Assignment operator.
             *
             * \param[in] other The instance to be copied.
             */
            Status& operator=(const Status& other);

            /**
             * Method that returns the class of status being reported.
             *
             * \return Returns the status class.
             */
            Class statusClass() const;

            /**
             * Method that returns an arbitrary error code based on the status class.  For filesystem errors, this
             * value might be the ANSI-C errno value.  For other status classes, the code may represent something
             * else.
             *
             * \return Returns the error code tied to the specific status class.
             */
            int errorCode() const;

            /**
             * Returns a text description of the error condition.
             *
             * \return Returns a textual description of the error condition.
             */
            std::string description() const;

            /**
             * Method that indicates if this status condition has additional details.
             *
             * \return Returns true if this status condition has additional details to report.  Returns false if the
             *         status is successful with no information.
             */
            bool informationAvailable() const;

            /**
             * Method that determines if this status represents a recoverable condtion, i.e. a non-error condition.
             *
             * \ref Container::Status::success
             *
             * \return Returns true if the condition is recoverable, returns false if this status represents an
             *         unrecoverable error condition.
             */
            bool recoverable() const;

            /**
             * Method that determines if this status represents no error and no status information, i.e. success.  The
             * method returns true if \ref Container::Status::informationAvailable returns false or
             * \ref Container::Status::recoverable returns true.
             *
             * \return Returns true on success, returns false if this status has information to report.
             */
            bool success() const;

            /**
             * Convenience method that determines if this status condition is a failure.  True if recoverable returns
             * false, false if recoverable returns true.
             *
             * \return Returns true if an unrecoverable error condition is detected.  Returns false if code accessing
             *         the container can continue.
             */
            bool failure() const;

            /**
             * Convenience cast that allows a status code to be used in a test.  Cast returns true if information is
             * available, returns false if no information is available.
             */
            operator bool() const;

        protected:
            /**
             * Pure virtual base class used for all underlying status implementations.
             */
            class PimplBase {
                public:
                    PimplBase();

                    virtual ~PimplBase();

                    /**
                     * Pure virtual method you should overload to report the status class for this type of status
                     * condition.
                     *
                     * \return Returns the status class.
                     */
                    virtual Class statusClass() const = 0;

                    /**
                     * Pure virtual method you should overload to report an arbitrary error code based on the status
                     * class and type of status being reported.  An example, for filesystem errors, this value might be
                     * the ANSI-C errno value.  For other status classes, the code may represent something else.
                     *
                     * \return Returns the error code tied to the specific status class.
                     */
                    virtual int errorCode() const = 0;

                    /**
                     * Pure virtual method you should overload to report a textual description of the status condition.
                     *
                     * \return Returns a textual description of the error condition.
                     */
                    virtual std::string description() const = 0;

                    /**
                     * Method you should overload to indicates if this status condition has additional details.
                     *
                     * \return Returns true if this status condition has additional details to report.  Returns false if
                     *         the status is successful with no information.  The default implementation returns true.
                     */
                    virtual bool informationAvailable() const;

                    /**
                     * Method that determines if this status represents a recoverable condtion, i.e. a non-error condition.
                     *
                     * \ref Container::Status::success
                     *
                     * \return Returns true if the condition is recoverable, returns false if this status represents an
                     *         unrecoverable error condition.
                     */
                    virtual bool recoverable() const = 0;
            };

            /**
             * Pure virtual base class used to develop non-error status conditions.
             */
            class StatusPimplBase:public PimplBase {
                public:
                    StatusPimplBase();

                    ~StatusPimplBase() override;

                    /**
                     * Method that indicates that the status condition is recoverable.
                     *
                     * \return Returns true if the condition is recoverable, returns false if this status represents an
                     *         unrecoverable error condition.  This version always returns true.
                     */
                    bool recoverable() const final;
            };

            /**
             * Pure virtual base class used to develop error, non-recoverable, status conditions.
             */
            class ErrorPimplBase:public PimplBase {
                public:
                    ErrorPimplBase();

                    ~ErrorPimplBase() override;

                    /**
                     * Method that indicates that the status condition is non-recoverable.
                     *
                     * \return Returns true if the condition is recoverable, returns false if this status represents an
                     *         unrecoverable error condition.  This version always returns false.
                     */
                    bool recoverable() const final;
            };

            /**
             * Constructor used to create derived error instances.
             *
             * \param[in] pimpl The pimpl class used to create functional error instances.
             */
            Status(PimplBase* pimpl);

            /**
             * Method you can use to gain access to the underlying pimpl.
             *
             * \return Returns a shared pointer to the underlying pimpl class.
             */
            std::shared_ptr<PimplBase> pimpl();

            /**
             * Method you can use to gain access to the underlying pimpl.
             *
             * \return Returns a shared pointer to the underlying pimpl class.
             */
            std::shared_ptr<PimplBase> pimpl() const;

        private:
            /**
             * Pimpl.
             */
            std::shared_ptr<PimplBase> impl;
    };
};

#endif
