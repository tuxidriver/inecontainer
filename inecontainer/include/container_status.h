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
* This header defines \ref Container::Status and various classes derived from \ref Container::Status.  Classes are used
* to report status conditions associated with the \ref Container::Container and \ref Container::VirtualFile classes.
***********************************************************************************************************************/

/* .. sphinx-project inecontainer */

#ifndef CONTAINER_STATUS_H
#define CONTAINER_STATUS_H

#include <string>
#include <cstdint>

#include "container_status_base.h"
#include "container_file_container.h"

namespace Container {
    /**
     * Trivial class that extends the \ref Container::Status class.  Provided for readability.
     */
    class NoStatus:public Status {
        public:
            NoStatus();

            ~NoStatus();
    };

    /**
     * Base class for container internal errors.
     */
    class InternalError:public Status {
        public:
            ~InternalError();

        protected:
            /**
             * Private implementation.
             */
            class PimplBase:public Status::ErrorPimplBase {
                public:
                    PimplBase();

                    ~PimplBase() override;

                    /**
                     * Method that returns the status code for this type of error.
                     *
                     * \return Returns the status class.
                     */
                    Class statusClass() const final;
            };

            /**
             * Constructor used to create derived error instances.
             *
             * \param[in] pimpl The pimpl class used to create functional error instances.
             */
            explicit InternalError(PimplBase* pimpl);

            /**
             * Copy constructor.
             *
             * \param[in] other The instance to be copied.
             */
            InternalError(const Status& other);
    };

    /**
     * Base class for errors due to metadata mismatches.
     */
    class MetadataMismatchError:public InternalError {
        public:
            ~MetadataMismatchError();

            /**
             * Method that returns the file position where the mismatch was detected.
             *
             * \return Returns the approximate file position of the mismatch.
             */
            unsigned long long filePosition() const;

        protected:
            /**
             * Private implementation.
             */
            class PimplBase:public InternalError::PimplBase {
                public:
                    /**
                     * Constructor
                     *
                     * \param[in] filePosition The approximate file position where the mismatch was detected.
                     */
                    PimplBase(unsigned long long filePosition);

                    ~PimplBase() override;

                    /**
                     * Method that returns the file position where the mismatch was detected.
                     *
                     * \return Returns the approximate file position of the mismatch.
                     */
                    unsigned long long filePosition() const;

                private:
                    unsigned long long currentFilePosition;
            };

            /**
             * Constructor used to create derived error instances.
             *
             * \param[in] pimpl The pimpl class used to create functional error instances.
             */
            explicit MetadataMismatchError(PimplBase* pimpl);

            /**
             * Copy constructor
             *
             * \param[in] other The instance to be copied.
             */
            MetadataMismatchError(const Status& other);
    };

    /**
     * Base class for container format errors.
     */
    class FormatError:public Status {
        public:
            ~FormatError();

        protected:
            /**
             * Private implementation.
             */
            class PimplBase:public Status::ErrorPimplBase {
                public:
                    PimplBase();

                    ~PimplBase() override;

                    /**
                     * Method that returns the status code for this type of error.
                     *
                     * \return Returns the status class.
                     */
                    Class statusClass() const final;
            };

            /**
             * Constructor used to create derived error instances.
             *
             * \param[in] pimpl The pimpl class used to create functional error instances.
             */
            explicit FormatError(PimplBase* pimpl);

            /**
             * Copy constructor.
             *
             * \param[in] other The instance to be copied.
             */
            FormatError(const Status& other);
    };

    /**
     * Base class for container header errors.
     */
    class HeaderError:public Status {
        public:
            ~HeaderError();

        protected:
            /**
             * Private implementation.
             */
            class PimplBase:public Status::ErrorPimplBase {
                public:
                    PimplBase();

                    ~PimplBase() override;

                    /**
                     * Method that returns the status code for this type of error.
                     *
                     * \return Returns the status class.
                     */
                    Class statusClass() const final;
            };

            /**
             * Constructor used to create derived error instances.
             *
             * \param[in] pimpl The pimpl class used to create functional error instances.
             */
            explicit HeaderError(PimplBase* pimpl);

            /**
             * Copy constructor.
             *
             * \param[in] other The instance to be copied.
             */
            HeaderError(const Status& other);
    };

    /**
     * Base class for filesystem status information.
     */
    class FilesystemStatus:public Status {
        public:
            ~FilesystemStatus();

        protected:
            /**
             * Private implementation.
             */
            class PimplBase:public Status::StatusPimplBase {
                public:
                    PimplBase();

                    ~PimplBase() override;

                    /**
                     * Method that returns the status code for this type of error.
                     *
                     * \return Returns the status class.
                     */
                    Class statusClass() const final;
            };

            /**
             * Constructor used to create derived status instances.
             *
             * \param[in] pimpl The pimpl class used to create functional error instances.
             */
            explicit FilesystemStatus(PimplBase* pimpl);

            /**
             * Copy constructor.
             *
             * \param[in] other The instance to be copied.
             */
            FilesystemStatus(const Status& other);
    };

    /**
     * Base class for filesystem errors..
     */
    class FilesystemError:public Status {
        public:
            ~FilesystemError();

        protected:
            /**
             * Private implementation.
             */
            class PimplBase:public Status::ErrorPimplBase {
                public:
                    PimplBase();

                    ~PimplBase() override;

                    /**
                     * Method that returns the status code for this type of error.
                     *
                     * \return Returns the status class.
                     */
                    Class statusClass() const final;
            };

            /**
             * Constructor used to create derived status instances.
             *
             * \param[in] pimpl The pimpl class used to create functional error instances.
             */
            explicit FilesystemError(PimplBase* pimpl);

            /**
             * Copy constructor.
             *
             * \param[in] other The instance to be copied.
             */
            FilesystemError(const Status& other);
    };

    /**
     * Base class for error reported in derived classes during streaming read operations.  The class differs from other
     * status base classes in that it can be used directly.
     */
    class StreamingReadError:public Status {
        public:
            /**
             * The error code used to report container unavailable errors.
             */
            static constexpr int reportedErrorCode = 1;

            StreamingReadError();

            /**
             * Copy constructor
             *
             * \param[in] other The instance to be copied.
             */
            StreamingReadError(const Status& other);

            ~StreamingReadError();

        protected:
            /**
             * Private implementation.
             */
            class PimplBase:public Status::ErrorPimplBase {
                public:
                    PimplBase();

                    ~PimplBase() override;

                    /**
                     * Method that returns the status code for this type of error.
                     *
                     * \return Returns the status class.
                     */
                    Class statusClass() const final;

                    /**
                     * Virtual method you should overload to report an arbitrary error code based on the status class
                     * and type of status being reported.  An example, for filesystem errors, this value might be the
                     * ANSI-C errno value.  For other status classes, the code may represent something else.
                     *
                     * \return Returns the error code tied to the specific status class.
                     */
                    int errorCode() const override;

                    /**
                     * Pure virtual method you should overload to report a textual description of the status condition.
                     *
                     * \return Returns a textual description of the error condition.
                     */
                    std::string description() const override;
            };

            /**
             * Constructor used to create derived error instances.
             *
             * \param[in] pimpl The pimpl class used to create functional error instances.
             */
            explicit StreamingReadError(PimplBase* pimpl);
    };

    /**
     * Class that reports the container object was prematurely deallocated.
     */
    class ContainerUnavailable:public InternalError {
        public:
            /**
             * The error code used to report container unavailable errors.
             */
            static constexpr int reportedErrorCode = 1;

            ContainerUnavailable();

            /**
             * Copy constructor
             *
             * \param[in] other The instance to be copied.
             */
            ContainerUnavailable(const Status& other);

            ~ContainerUnavailable();

        private:
            class Pimpl;
    };

    /**
     * Class that reports that a stream ID is different from what was expected.
     */
    class StreamIdentifierMismatch:public MetadataMismatchError {
        public:
            /**
             * The error code used to report stream identifier mismatch errors.
             */
            static constexpr int reportedErrorCode = 2;

            /**
             * Constructor
             *
             * \param[in] detectedStreamIdentifier The stream identifier we read from the file.
             *
             * \param[in] expectedStreamIdentifier The stream identifier we expected to find.
             *
             * \param[in] filePosition             The approximate file position where the mismatch was detected.
             */
            StreamIdentifierMismatch(
                unsigned long      detectedStreamIdentifier,
                unsigned long      expectedStreamIdentifier,
                unsigned long long filePosition
            );

            /**
             * Copy constructor
             *
             * \param[in] other The instance to be copied.
             */
            StreamIdentifierMismatch(const Status& other);

            ~StreamIdentifierMismatch();

            /**
             * Method that returns the detected or read stream identifier.
             *
             * \return Returns the detected or read stream identifier.
             */
            unsigned long detectedStreamIdentifier() const;

            /**
             * Method that returns the expected stream identifier.
             *
             * \return Returns the expected stream identifier.
             */
            unsigned long expectedStreamIdentifier() const;

        private:
            class Pimpl;
    };

    /**
     * Class that reports that an offset is different from what is expected.
     */
    class OffsetMismatch:public MetadataMismatchError {
        public:
            /**
             * The error code used to report an offset is different from an expected value.
             */
            static constexpr int reportedErrorCode = 3;

            /**
             * Constructor
             *
             * \param[in] detectedBaseOffset The base offset we read from the file.
             *
             * \param[in] expectedBaseOffset The base offset we expected to see.
             *
             * \param[in] filePosition       The approximate file position where the mismatch was detected.
             */
            OffsetMismatch(
                unsigned long long detectedBaseOffset,
                unsigned long long expectedBaseOffset,
                unsigned long long filePosition
            );

            /**
             * Copy constructor
             *
             * \param[in] other The instance to be copied.
             */
            OffsetMismatch(const Status& other);

            ~OffsetMismatch();

            /**
             * Method that returns the detected or read base offset.
             *
             * \return Returns the detected or read base offset.
             */
            unsigned long long detectedBaseOffset() const;

            /**
             * Method that returns the expected base offset.
             *
             * \return Returns the expected base offset.
             */
            unsigned long long expectedBaseOffset() const;

        private:
            class Pimpl;
    };

    /**
     * Class that reports that a payload size is different from what is expected.
     */
    class PayloadSizeMismatch:public MetadataMismatchError {
        public:
            /**
             * The error code used to report payload size mismatch errors.
             */
            static constexpr int reportedErrorCode = 4;

            /**
             * Constructor
             *
             * \param[in] detectedPayloadSize The payload size found in the file.
             *
             * \param[in] expectedPayloadSize The payload size we expected to find.
             *
             * \param[in] filePosition        The approximate file position where the mismatch was detected.
             */
            PayloadSizeMismatch(
                unsigned           detectedPayloadSize,
                unsigned           expectedPayloadSize,
                unsigned long long filePosition
            );

            /**
             * Copy constructor
             *
             * \param[in] other The instance to be copied.
             */
            PayloadSizeMismatch(const Status& other);

            ~PayloadSizeMismatch();

            /**
             * Method that returns the detected payload size.
             *
             * \return Returns the detected payload size.
             */
            unsigned detectedPayloadSize() const;

            /**
             * Method that returns the expected payload size.
             *
             * \return Returns the expected payload size.
             */
            unsigned expectedPayloadSize() const;

        private:
            class Pimpl;
    };

    /**
     * Class that reports that a filename is different from what is expected.
     */
    class FilenameMismatch:public MetadataMismatchError {
        public:
            /**
             * The error code used to report filename mismatch errors.
             */
            static constexpr int reportedErrorCode = 5;

            /**
             * Constructor
             *
             * \param[in] detectedFilename The filename found in the container.
             *
             * \param[in] expectedFilename The filename we expected to find.
             *
             * \param[in] filePosition     The approximate file position where the mismatch was detected.
             */
            FilenameMismatch(
                const std::string& detectedFilename,
                const std::string& expectedFilename,
                unsigned long long filePosition
            );

            /**
             * Copy constructor
             *
             * \param[in] other The instance to be copied.
             */
            FilenameMismatch(const Status& other);

            ~FilenameMismatch();

            /**
             * Method that returns the detected filename.
             *
             * \return Returns the detected filename.
             */
            std::string detectedFilename() const;

            /**
             * Method that returns the expected filename.
             *
             * \return Returns the expected filename.
             */
            std::string expectedFilename() const;

        private:
            class Pimpl;
    };

    /**
     * Class that reports an error when creating a new virtual file.
     */
    class FileCreationError:public MetadataMismatchError {
        public:
            /**
             * The error code used to report filename mismatch errors.
             */
            static constexpr int reportedErrorCode = 6;

            /**
             * Constructor
             *
             * \param[in] detectedFilename The filename found in the container.
             *
             * \param[in] filePosition     The approximate file position where the mismatch was detected.
             */
            FileCreationError(const std::string& detectedFilename, unsigned long long filePosition);

            /**
             * Copy constructor
             *
             * \param[in] other The instance to be copied.
             */
            FileCreationError(const Status& other);

            ~FileCreationError();

            /**
             * Method that returns the detected filename.
             *
             * \return Returns the detected filename.
             */
            std::string detectedFilename() const;

        private:
            class Pimpl;
    };

    /**
     * Class that reports unexpected data in the container.
     */
    class ContainerDataError:public MetadataMismatchError {
        public:
            /**
             * The error code used to report filename mismatch errors.
             */
            static constexpr int reportedErrorCode = 7;

            /**
             * Constructor
             *
             * \param[in] filePosition The approximate file position where the mismatch was detected.
             */
            ContainerDataError(unsigned long long filePosition);

            /**
             * Copy constructor
             *
             * \param[in] other The instance to be copied.
             */
            ContainerDataError(const Status& other);

            ~ContainerDataError();

        private:
            class Pimpl;
    };

    /**
     * Class that reports a downlevel container version.
     */
    class VersionDownlevelStatus:public Status {
        public:
            /**
             * The error code used to report version downlevel status errors.
             */
            static constexpr int reportedErrorCode = 8;

            /**
             * Constructor.
             *
             * \param[in] detectedMajorVersion  The major version reported by the container.
             *
             * \param[in] supportedMajorVersion The major version that we support with this container.
             */
            VersionDownlevelStatus(std::uint8_t detectedMajorVersion, std::uint8_t supportedMajorVersion);

            /**
             * Copy constructor
             *
             * \param[in] other The instance to be copied.
             */
            VersionDownlevelStatus(const Status& other);

            ~VersionDownlevelStatus();

            /**
             * Method you can use to obtain the container major version code.
             *
             * \return Returns the discovered or detected container version.
             */
            std::uint8_t discoveredMajorVersion() const;

            /**
             * Method you can use to obtain the supported container major version code.
             *
             * \return Returns the supported container version.
             */
            std::uint8_t supportedMajorVersion() const;

        private:
            class Pimpl;
    };

    /**
     * Class that reports an invalid container identifier string.
     */
    class HeaderIdentifierInvalid:public HeaderError {
        public:
            /**
             * The error code used to report header identifier invalid errors.
             */
            static constexpr int reportedErrorCode = 9;

            HeaderIdentifierInvalid();

            /**
             * Copy constructor
             *
             * \param[in] other The instance to be copied.
             */
            HeaderIdentifierInvalid(const Status& other);

            ~HeaderIdentifierInvalid();

        private:
            class Pimpl;
    };

    /**
     * Class that reports that the header version code is invalid for this container version.
     */
    class HeaderVersionInvalid:public HeaderError {
        public:
            /**
             * The error code used to report header identifier invalid errors.
             */
            static constexpr int reportedErrorCode = 10;

            /**
             * Constructor.
             *
             * \param[in] detectedMajorVersion  The major version reported by the container.
             *
             * \param[in] supportedMajorVersion The major version that we support with this container.
             */
            HeaderVersionInvalid(std::uint8_t detectedMajorVersion, std::uint8_t supportedMajorVersion);

            /**
             * Copy constructor
             *
             * \param[in] other The instance to be copied.
             */
            HeaderVersionInvalid(const Status& other);

            ~HeaderVersionInvalid();

            /**
             * Method you can use to obtain the container major version code.
             *
             * \return Returns the discovered or detected container version.
             */
            std::uint8_t discoveredMajorVersion() const;

            /**
             * Method you can use to obtain the supported container major version code.
             *
             * \return Returns the supported container version.
             */
            std::uint8_t supportedMajorVersion() const;

        private:
            class Pimpl;
    };

    /**
     * Class that reports a header CRC error.
     */
    class HeaderCrcError:public HeaderError {
        public:
            /**
             * The error code used to report header identifier invalid errors.
             */
            static constexpr int reportedErrorCode = 11;

            HeaderCrcError();

            /**
             * Copy constructor
             *
             * \param[in] other The instance to be copied.
             */
            HeaderCrcError(const Status& other);

            ~HeaderCrcError();

        private:
            class Pimpl;
    };

    /**
     * Class that reports a successful read operation.
     */
    class ReadSuccessful:public FilesystemStatus {
        public:
            /**
             * The error code used to report header identifier invalid errors.
             */
            static constexpr int reportedErrorCode = 12;

            /**
             * Constructor
             *
             * \param[in] bytesRead The number of bytes read by this read operation.
             */
            explicit ReadSuccessful(unsigned bytesRead);

            /**
             * Copy constructor
             *
             * \param[in] other The instance to be copied.
             */
            ReadSuccessful(const Status& other);

            ~ReadSuccessful();

            /**
             * Method that indicates the number of bytes read.
             *
             * \return Returns the number of bytes read.
             */
            unsigned bytesRead() const;

        private:
            class Pimpl;
    };

    /**
     * Class that reports a successful write operation.
     */
    class WriteSuccessful:public FilesystemStatus {
        public:
            /**
             * The error code used to report header identifier invalid errors.
             */
            static constexpr int reportedErrorCode = 13;

            /**
             * Constructor
             *
             * \param[in] bytesWritten The number of bytes written by this operation.
             */
            explicit WriteSuccessful(unsigned bytesWritten);

            /**
             * Copy constructor
             *
             * \param[in] other The instance to be copied.
             */
            WriteSuccessful(const Status& other);

            ~WriteSuccessful();

            /**
             * Method that indicates the number of bytes read.
             *
             * \return Returns the number of bytes read.
             */
            unsigned bytesWritten() const;

        private:
            class Pimpl;
    };

    /**
     * Class that reports an attempt to seek past the end of the container.
     */
    class SeekError:public FilesystemError {
        public:
            /**
             * The error code used to report header identifier invalid errors.
             */
            static constexpr int reportedErrorCode = 14;

            /**
             * Constructor
             *
             * \param[in] desiredPosition      The desired seek position in the container.
             *
             * \param[in] containerSizeInBytes The container size, in bytes.
             */
            SeekError(unsigned long long desiredPosition, unsigned long long containerSizeInBytes);

            /**
             * Copy constructor
             *
             * \param[in] other The instance to be copied.
             */
            SeekError(const Status& other);

            ~SeekError();

            /**
             * Method that indicates the desired seek position.
             *
             * \return Returns the position that the code attempted to seek to.
             */
            unsigned long long position() const;

            /**
             * Method that indicates the container size.
             *
             * \return Returns the container size, in bytes, when the seek was attempted.
             */
            unsigned long long containerSize() const;

        private:
            class Pimpl;
    };

    /**
     * Class that reports an invalid open mode.
     */
    class InvalidOpenMode:public FilesystemError {
        public:
            /**
             * The error code used to report header identifier invalid errors.
             */
            static constexpr int reportedErrorCode = 15;

            /**
             * Constructor
             *
             * \param[in] openMode The attempted open mode.
             */
            InvalidOpenMode(FileContainer::OpenMode openMode);

            /**
             * Copy constructor
             *
             * \param[in] other The instance to be copied.
             */
            InvalidOpenMode(const Status& other);

            ~InvalidOpenMode();

            /**
             * Method that returns the specified open mode.
             *
             * \return Returns the specified open mode.
             */
            FileContainer::OpenMode openMode() const;

        private:
            class Pimpl;
    };

    /**
     * Class that reports that the container is not open.
     */
    class FileContainerNotOpen:public FilesystemError {
        public:
            /**
             * The error code used to report header identifier invalid errors.
             */
            static constexpr int reportedErrorCode = 16;

            FileContainerNotOpen();

            /**
             * Copy constructor
             *
             * \param[in] other The instance to be copied.
             */
            FileContainerNotOpen(const Status& other);

            ~FileContainerNotOpen();

        private:
            class Pimpl;
    };

    /**
     * Class that reports that a file could not be opened.
     */
    class FailedToOpenFile:public FilesystemError {
        public:
            /**
             * The error code used to report header identifier invalid errors.
             */
            static constexpr int reportedErrorCode = 17;

            /**
             * Constructor
             *
             * \param[in] filename    The name of the file that was attempted to be opened.
             *
             * \param[in] openMode    The attempted open mode.
             *
             * \param[in] errorNumber The operating system error number.
             */
            FailedToOpenFile(const std::string& filename, FileContainer::OpenMode openMode, int errorNumber);

            /**
             * Copy constructor
             *
             * \param[in] other The instance to be copied.
             */
            FailedToOpenFile(const Status& other);

            ~FailedToOpenFile();

            /**
             * Method that returns the filename of the file we attempted to open.
             *
             * \return Returns the filename of the file we attempted to open.
             */
            std::string filename() const;

            /**
             * Method that returns the specified open mode.
             *
             * \return Returns the specified open mode.
             */
            FileContainer::OpenMode openMode() const;

            /**
             * Method that returns the operating system error number.
             *
             * \return Returns the OS error number.
             */
            int errorNumber() const;

        private:
            class Pimpl;
    };

    /**
     * Class that reports a filesystem error during a file close operation.
     */
    class FileCloseError:public FilesystemError {
        public:
            /**
             * The error code used to report header identifier invalid errors.
             */
            static constexpr int reportedErrorCode = 18;

            /**
             * Constructor
             *
             * \param[in] filename    The name of the file that was attempted to be opened.
             *
             * \param[in] errorNumber The operating system error number.
             */
            FileCloseError(const std::string& filename, int errorNumber);

            /**
             * Copy constructor
             *
             * \param[in] other The instance to be copied.
             */
            FileCloseError(const Status& other);

            ~FileCloseError();

            /**
             * Method that returns the filename of the file we attempted to open.
             *
             * \return Returns the filename of the file we attempted to open.
             */
            std::string filename() const;

            /**
             * Method that returns the operating system error number.
             *
             * \return Returns the OS error number.
             */
            int errorNumber() const;

        private:
            class Pimpl;
    };

    /**
     * Class that reports a filesystem error during a file read operation.
     */
    class FileReadError:public FilesystemError {
        public:
            /**
             * The error code used to report header identifier invalid errors.
             */
            static constexpr int reportedErrorCode = 19;

            /**
             * Constructor
             *
             * \param[in] filename    The name of the file that was attempted to be opened.
             *
             * \param[in] position    The current file position.
             *
             * \param[in] errorNumber The operating system error number.
             */
            FileReadError(const std::string& filename, unsigned long long position, int errorNumber);

            /**
             * Copy constructor
             *
             * \param[in] other The instance to be copied.
             */
            FileReadError(const Status& other);

            ~FileReadError();

            /**
             * Method that returns the filename of the file we attempted to open.
             *
             * \return Returns the filename of the file we attempted to open.
             */
            std::string filename() const;

            /**
             * Method that returns the reported position of the read error.
             *
             * \return Returns the read error position.
             */
            unsigned long long position() const;

            /**
             * Method that returns the operating system error number.
             *
             * \return Returns the OS error number.
             */
            int errorNumber() const;

        private:
            class Pimpl;
    };

    /**
     * Class that reports a filesystem error during a file write operation.
     */
    class FileWriteError:public FilesystemError {
        public:
            /**
             * The error code used to report header identifier invalid errors.
             */
            static constexpr int reportedErrorCode = 20;

            /**
             * Constructor
             *
             * \param[in] filename    The name of the file that was attempted to be opened.
             *
             * \param[in] position    The current file position.
             *
             * \param[in] errorNumber The operating system error number.
             */
            FileWriteError(const std::string& filename, unsigned long long position, int errorNumber);

            /**
             * Copy constructor
             *
             * \param[in] other The instance to be copied.
             */
            FileWriteError(const Status& other);

            ~FileWriteError();

            /**
             * Method that returns the filename of the file we attempted to open.
             *
             * \return Returns the filename of the file we attempted to open.
             */
            std::string filename() const;

            /**
             * Method that returns the reported position of the read error.
             *
             * \return Returns the read error position.
             */
            unsigned long long position() const;

            /**
             * Method that returns the operating system error number.
             *
             * \return Returns the OS error number.
             */
            int errorNumber() const;

        private:
            class Pimpl;
    };

    /**
     * Class that reports a filesystem error during a file truncation operation.
     */
    class FileTruncateError:public FilesystemError {
        public:
            /**
             * The error code used to report header identifier invalid errors.
             */
            static constexpr int reportedErrorCode = 21;

            /**
             * Constructor
             *
             * \param[in] filename    The name of the file that was attempted to be opened.
             *
             * \param[in] position    The current file position.
             *
             * \param[in] errorNumber The operating system error number.
             */
            FileTruncateError(const std::string& filename, unsigned long long position, int errorNumber);

            /**
             * Copy constructor
             *
             * \param[in] other The instance to be copied.
             */
            FileTruncateError(const Status& other);

            ~FileTruncateError();

            /**
             * Method that returns the filename of the file we attempted to open.
             *
             * \return Returns the filename of the file we attempted to open.
             */
            std::string filename() const;

            /**
             * Method that returns the reported position of the read error.
             *
             * \return Returns the read error position.
             */
            unsigned long long position() const;

            /**
             * Method that returns the operating system error number.
             *
             * \return Returns the OS error number.
             */
            int errorNumber() const;

        private:
            class Pimpl;
    };

    /**
     * Class that reports a filesystem error during a flush operation.
     */
    class FileFlushError:public FilesystemError {
        public:
            /**
             * The error code used to report header identifier invalid errors.
             */
            static constexpr int reportedErrorCode = 22;

            /**
             * Constructor
             *
             * \param[in] filename    The name of the file that was attempted to be opened.
             *
             * \param[in] errorNumber The operating system error number.
             */
            FileFlushError(const std::string& filename, int errorNumber);

            /**
             * Copy constructor
             *
             * \param[in] other The instance to be copied.
             */
            FileFlushError(const Status& other);

            ~FileFlushError();

            /**
             * Method that returns the filename of the file we attempted to open.
             *
             * \return Returns the filename of the file we attempted to open.
             */
            std::string filename() const;

            /**
             * Method that returns the operating system error number.
             *
             * \return Returns the OS error number.
             */
            int errorNumber() const;

        private:
            class Pimpl;
    };
};

#endif
