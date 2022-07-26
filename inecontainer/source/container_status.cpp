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
* This file implements various classes derived from \ref Container::Status.  Classes are used to report status
* conditions associated with the \ref Container::Container and \ref Container::VirtualFile classes.
***********************************************************************************************************************/

#include <string>
#include <cstdint>
#include <sstream>
#include <memory>
#include <cerrno>
#include <cstring>

#include "container_status_base.h"
#include "container_status.h"

/***********************************************************************************************************************
 * Container::NoStatus
 */

namespace Container {
    NoStatus::NoStatus() {}


    NoStatus::~NoStatus() {}
}

/***********************************************************************************************************************
 * Container::InternalError::PimplBase
 */

namespace Container {
    InternalError::PimplBase::PimplBase() {}


    InternalError::PimplBase::~PimplBase() {}


    Status::Class InternalError::PimplBase::statusClass() const {
        return Status::Class::CONTAINER_INTERNAL_ERROR;
    }
}

/***********************************************************************************************************************
 * Container::InternalError
 */

namespace Container {
    InternalError::~InternalError() {}


    InternalError::InternalError(InternalError::PimplBase* pimpl):Status(pimpl) {}


    InternalError::InternalError(const Status& other):Status(other) {}
}

/***********************************************************************************************************************
 * Container::MetadataMismatchError::PimplBase
 */

namespace Container {
    MetadataMismatchError::PimplBase::PimplBase(unsigned long long filePosition) {
        currentFilePosition = filePosition;
    }


    MetadataMismatchError::PimplBase::~PimplBase() {}


    unsigned long long MetadataMismatchError::PimplBase::filePosition() const {
        return currentFilePosition;
    }
}

/***********************************************************************************************************************
 * Container::MetadataMismatchError
 */

namespace Container {
    MetadataMismatchError::~MetadataMismatchError() {}

    unsigned long long MetadataMismatchError::filePosition() const {
        return std::dynamic_pointer_cast<MetadataMismatchError::PimplBase>(pimpl())->filePosition();
    }


    MetadataMismatchError::MetadataMismatchError(MetadataMismatchError::PimplBase* pimpl):InternalError(pimpl) {}


    MetadataMismatchError::MetadataMismatchError(const Status& other):InternalError(other) {}
}

/***********************************************************************************************************************
 * Container::FormatError::PimplBase
 */

namespace Container {
    FormatError::PimplBase::PimplBase() {}


    FormatError::PimplBase::~PimplBase() {}


    Status::Class FormatError::PimplBase::statusClass() const {
        return Status::Class::CONTAINER_FORMAT_ERROR;
    }
}

/***********************************************************************************************************************
 * Container::FormatError
 */

namespace Container {
    FormatError::~FormatError() {}


    FormatError::FormatError(FormatError::PimplBase* pimpl):Status(pimpl) {}


    FormatError::FormatError(const Status& other):Status(other) {}
}

/***********************************************************************************************************************
 * Container::HeaderError::PimplBase
 */

namespace Container {
    HeaderError::PimplBase::PimplBase() {}


    HeaderError::PimplBase::~PimplBase() {}


    Status::Class HeaderError::PimplBase::statusClass() const {
        return Status::Class::CONTAINER_HEADER_ERROR;
    }
}

/***********************************************************************************************************************
 * Container::HeaderError
 */

namespace Container {
    HeaderError::~HeaderError() {}


    HeaderError::HeaderError(HeaderError::PimplBase* pimpl):Status(pimpl) {}


    HeaderError::HeaderError(const Status& other):Status(other) {}
}

/***********************************************************************************************************************
 * Container::FilesystemStatus::PimplBase
 */

namespace Container {
    FilesystemStatus::PimplBase::PimplBase() {}


    FilesystemStatus::PimplBase::~PimplBase() {}


    Status::Class FilesystemStatus::PimplBase::statusClass() const {
        return Status::Class::FILESYSTEM_STATUS;
    }
}

/***********************************************************************************************************************
 * Container::FilesystemStatus
 */

namespace Container {
    FilesystemStatus::~FilesystemStatus() {}


    FilesystemStatus::FilesystemStatus(FilesystemStatus::PimplBase* pimpl):Status(pimpl) {}


    FilesystemStatus::FilesystemStatus(const Status& other):Status(other) {}
}

/***********************************************************************************************************************
 * Container::FilesystemError::PimplBase
 */

namespace Container {
    FilesystemError::PimplBase::PimplBase() {}


    FilesystemError::PimplBase::~PimplBase() {}


    Status::Class FilesystemError::PimplBase::statusClass() const {
        return Status::Class::FILESYSTEM_ERROR;
    }
}

/***********************************************************************************************************************
 * Container::FilesystemError
 */

namespace Container {
    FilesystemError::~FilesystemError() {}


    FilesystemError::FilesystemError(FilesystemError::PimplBase* pimpl):Status(pimpl) {}


    FilesystemError::FilesystemError(const Status& other):Status(other) {}
}



/***********************************************************************************************************************
 * Container::StreamingReadError::PimplBase
 */

namespace Container {
    StreamingReadError::PimplBase::PimplBase() {}


    StreamingReadError::PimplBase::~PimplBase() {}


    Status::Class StreamingReadError::PimplBase::statusClass() const {
        return Status::Class::STREAMING_READ_ERROR;
    }


    int StreamingReadError::PimplBase::errorCode() const {
        return StreamingReadError::reportedErrorCode;
    }


    std::string StreamingReadError::PimplBase::description() const {
        return "streaming read error";
    }
}

/***********************************************************************************************************************
 * Container::StreamingReadError
 */

namespace Container {
    StreamingReadError::StreamingReadError():Status(new StreamingReadError::PimplBase) {};


    StreamingReadError::StreamingReadError(const Status& other):Status(other) {}


    StreamingReadError::~StreamingReadError() {}


    StreamingReadError::StreamingReadError(StreamingReadError::PimplBase* pimpl):Status(pimpl) {}
}

/***********************************************************************************************************************
 * Container::ContainerUnavailable::Pimpl
 */

namespace Container {
    class ContainerUnavailable::Pimpl:public InternalError::PimplBase {
        public:
            Pimpl();

            ~Pimpl() override;

            int errorCode() const final;

            std::string description() const final;
    };


    ContainerUnavailable::Pimpl::Pimpl() {}


    ContainerUnavailable::Pimpl::~Pimpl() {}


    int ContainerUnavailable::Pimpl::errorCode() const {
        return ContainerUnavailable::reportedErrorCode;
    }


    std::string ContainerUnavailable::Pimpl::description() const {
        return "Container object unavailable";
    }
}

/***********************************************************************************************************************
 * Container::ContainerUnavailable
 */

namespace Container {
    ContainerUnavailable::ContainerUnavailable():InternalError(new ContainerUnavailable::Pimpl()) {}


    ContainerUnavailable::ContainerUnavailable(const Status &other):InternalError(other) {}


    ContainerUnavailable::~ContainerUnavailable() {}
}

/***********************************************************************************************************************
 * Container::StreamIdentifierMismatch::Pimpl
 */

namespace Container {
    class StreamIdentifierMismatch::Pimpl:public MetadataMismatchError::PimplBase {
        public:
            Pimpl(
                unsigned long      detectedStreamIdentifier,
                unsigned long      expectedStreamIdentifier,
                unsigned long long filePosition
            );

            ~Pimpl() override;

            unsigned long detectedStreamIdentifier() const;

            unsigned long expectedStreamIdentifier() const;

            int errorCode() const final;

            std::string description() const final;

        private:
            unsigned long currentDetectedStreamIdentifier;
            unsigned long currentExpectedStreamIdentifier;
    };


    StreamIdentifierMismatch::Pimpl::Pimpl(
            unsigned long      detectedStreamIdentifier,
            unsigned long      expectedStreamIdentifier,
            unsigned long long filePosition
        ):MetadataMismatchError::PimplBase(
            filePosition
        ) {
        currentDetectedStreamIdentifier = detectedStreamIdentifier;
        currentExpectedStreamIdentifier = expectedStreamIdentifier;
    }


    StreamIdentifierMismatch::Pimpl::~Pimpl() {}


    unsigned long StreamIdentifierMismatch::Pimpl::detectedStreamIdentifier() const {
        return currentDetectedStreamIdentifier;
    }


    unsigned long StreamIdentifierMismatch::Pimpl::expectedStreamIdentifier() const {
        return currentExpectedStreamIdentifier;
    }


    int StreamIdentifierMismatch::Pimpl::errorCode() const {
        return StreamIdentifierMismatch::reportedErrorCode;
    }


    std::string StreamIdentifierMismatch::Pimpl::description() const {
        std::stringstream stream;

        stream << "Stream identifier mismatch near " << filePosition()
               << ": expected " << expectedStreamIdentifier() << " detected " << detectedStreamIdentifier();

        return stream.str();
    }
}

/***********************************************************************************************************************
 * Container::StreamIdentifierMismatch
 */

namespace Container {
    StreamIdentifierMismatch::StreamIdentifierMismatch(
            unsigned long      detectedStreamIdentifier,
            unsigned long      expectedStreamIdentifier,
            unsigned long long filePosition
        ):MetadataMismatchError(
            new StreamIdentifierMismatch::Pimpl(
                detectedStreamIdentifier,
                expectedStreamIdentifier,
                filePosition
            )
        ) {}


    StreamIdentifierMismatch::StreamIdentifierMismatch(
            const Status &other
        ):MetadataMismatchError(
            other
        ) {}


    StreamIdentifierMismatch::~StreamIdentifierMismatch() {}


    unsigned long StreamIdentifierMismatch::detectedStreamIdentifier() const {
        return std::dynamic_pointer_cast<StreamIdentifierMismatch::Pimpl>(pimpl())->detectedStreamIdentifier();
    }


    unsigned long StreamIdentifierMismatch::expectedStreamIdentifier() const {
        return std::dynamic_pointer_cast<StreamIdentifierMismatch::Pimpl>(pimpl())->expectedStreamIdentifier();
    }
}

/***********************************************************************************************************************
 * Container::OffsetMismatch::Pimpl
 */

namespace Container {
    class OffsetMismatch::Pimpl:public MetadataMismatchError::PimplBase {
        public:
            Pimpl(
                unsigned long long detectedBaseOffset,
                unsigned long long expectedBaseOffset,
                unsigned long long filePosition
            );

            ~Pimpl() override;

            unsigned long long detectedBaseOffset() const;

            unsigned long long expectedBaseOffset() const;

            int errorCode() const final;

            std::string description() const final;

        private:
            unsigned long long currentDetectedBaseOffset;
            unsigned long long currentExpectedBaseOffset;
    };


    OffsetMismatch::Pimpl::Pimpl(
            unsigned long long detectedBaseOffset,
            unsigned long long expectedBaseOffset,
            unsigned long long filePosition
        ):MetadataMismatchError::PimplBase(
            filePosition
        ) {
        currentDetectedBaseOffset = detectedBaseOffset;
        currentExpectedBaseOffset = expectedBaseOffset;
    }


    OffsetMismatch::Pimpl::~Pimpl() {}


    unsigned long long OffsetMismatch::Pimpl::detectedBaseOffset() const {
        return currentDetectedBaseOffset;
    }


    unsigned long long OffsetMismatch::Pimpl::expectedBaseOffset() const {
        return currentExpectedBaseOffset;
    }


    int OffsetMismatch::Pimpl::errorCode() const {
        return OffsetMismatch::reportedErrorCode;
    }


    std::string OffsetMismatch::Pimpl::description() const {
        std::stringstream stream;

        stream << "Base offset mismatch near " << filePosition()
               << ": expected " << expectedBaseOffset() << " detected " << detectedBaseOffset();

        return stream.str();
    }
}

/***********************************************************************************************************************
 * Container::OffsetMismatch
 */

namespace Container {
    OffsetMismatch::OffsetMismatch(
            unsigned long long detectedBaseOffset,
            unsigned long long expectedBaseOffset,
            unsigned long long filePosition
        ):MetadataMismatchError(
            new OffsetMismatch::Pimpl(
                detectedBaseOffset,
                expectedBaseOffset,
                filePosition
            )
        ) {}


    OffsetMismatch::OffsetMismatch(
            const Status &other
        ):MetadataMismatchError(
            other
        ) {}


    OffsetMismatch::~OffsetMismatch() {}


    unsigned long long OffsetMismatch::detectedBaseOffset() const {
        return std::dynamic_pointer_cast<OffsetMismatch::Pimpl>(pimpl())->detectedBaseOffset();
    }


    unsigned long long OffsetMismatch::expectedBaseOffset() const {
        return std::dynamic_pointer_cast<OffsetMismatch::Pimpl>(pimpl())->expectedBaseOffset();
    }
}

/***********************************************************************************************************************
 * Container::PayloadSizeMismatch::Pimpl
 */

namespace Container {
    class PayloadSizeMismatch::Pimpl:public MetadataMismatchError::PimplBase {
        public:
            Pimpl(
                unsigned           detectedPayloadSize,
                unsigned           expectedPayloadSize,
                unsigned long long filePosition
            );

            ~Pimpl() override;

            unsigned detectedPayloadSize() const;

            unsigned expectedPayloadSize() const;

            int errorCode() const final;

            std::string description() const final;

        private:
            unsigned currentDetectedPayloadSize;
            unsigned currentExpectedPayloadSize;
    };


    PayloadSizeMismatch::Pimpl::Pimpl(
            unsigned           detectedPayloadSize,
            unsigned           expectedPayloadSize,
            unsigned long long filePosition
        ):MetadataMismatchError::PimplBase(
            filePosition
        ) {
        currentDetectedPayloadSize = detectedPayloadSize;
        currentExpectedPayloadSize = expectedPayloadSize;
    }


    PayloadSizeMismatch::Pimpl::~Pimpl() {}


    unsigned PayloadSizeMismatch::Pimpl::detectedPayloadSize() const {
        return currentDetectedPayloadSize;
    }


    unsigned PayloadSizeMismatch::Pimpl::expectedPayloadSize() const {
        return currentExpectedPayloadSize;
    }


    int PayloadSizeMismatch::Pimpl::errorCode() const {
        return PayloadSizeMismatch::reportedErrorCode;
    }


    std::string PayloadSizeMismatch::Pimpl::description() const {
        std::stringstream stream;

        stream << "Payload size mismatch near " << filePosition()
               << ": expected " << expectedPayloadSize() << " detected " << detectedPayloadSize();

        return stream.str();
    }
}

/***********************************************************************************************************************
 * Container::PayloadSizeMismatch
 */

namespace Container {
    PayloadSizeMismatch::PayloadSizeMismatch(
            unsigned           detectedPayloadSize,
            unsigned           expectedPayloadSize,
            unsigned long long filePosition
        ):MetadataMismatchError(
            new PayloadSizeMismatch::Pimpl(
                detectedPayloadSize,
                expectedPayloadSize,
                filePosition
            )
        ) {}


    PayloadSizeMismatch::PayloadSizeMismatch(
            const Status &other
        ):MetadataMismatchError(
            other
        ) {}


    PayloadSizeMismatch::~PayloadSizeMismatch() {}


    unsigned PayloadSizeMismatch::detectedPayloadSize() const {
        return std::dynamic_pointer_cast<PayloadSizeMismatch::Pimpl>(pimpl())->detectedPayloadSize();
    }


    unsigned PayloadSizeMismatch::expectedPayloadSize() const {
        return std::dynamic_pointer_cast<PayloadSizeMismatch::Pimpl>(pimpl())->expectedPayloadSize();
    }
}

/***********************************************************************************************************************
 * Container::FilenameMismatch::Pimpl
 */

namespace Container {
    class FilenameMismatch::Pimpl:public MetadataMismatchError::PimplBase {
        public:
            Pimpl(
                const std::string& detectedFilename,
                const std::string& expectedFilename,
                unsigned long long filePosition
            );

            ~Pimpl() override;

            std::string detectedFilename() const;

            std::string expectedFilename() const;

            int errorCode() const final;

            std::string description() const final;

        private:
            std::string currentDetectedFilename;
            std::string currentExpectedFilename;
    };


    FilenameMismatch::Pimpl::Pimpl(
            const std::string& detectedFilename,
            const std::string& expectedFilename,
            unsigned long long filePosition
        ):MetadataMismatchError::PimplBase(
            filePosition
        ) {
        currentDetectedFilename = detectedFilename;
        currentExpectedFilename = expectedFilename;
    }


    FilenameMismatch::Pimpl::~Pimpl() {}


    std::string FilenameMismatch::Pimpl::detectedFilename() const {
        return currentDetectedFilename;
    }


    std::string FilenameMismatch::Pimpl::expectedFilename() const {
        return currentExpectedFilename;
    }


    int FilenameMismatch::Pimpl::errorCode() const {
        return FilenameMismatch::reportedErrorCode;
    }


    std::string FilenameMismatch::Pimpl::description() const {
        std::stringstream stream;

        stream << "Filename mismatch near " << filePosition()
               << ": expected " << expectedFilename() << " detected " << detectedFilename();

        return stream.str();
    }
}

/***********************************************************************************************************************
 * Container::FilenameMismatch
 */

namespace Container {
    FilenameMismatch::FilenameMismatch(
            const std::string& detectedFilename,
            const std::string& expectedFilename,
            unsigned long long filePosition
        ):MetadataMismatchError(
            new FilenameMismatch::Pimpl(
                detectedFilename,
                expectedFilename,
                filePosition
            )
        ) {}


    FilenameMismatch::FilenameMismatch(
            const Status &other
        ):MetadataMismatchError(
            other
        ) {}


    FilenameMismatch::~FilenameMismatch() {}


    std::string FilenameMismatch::detectedFilename() const {
        return std::dynamic_pointer_cast<FilenameMismatch::Pimpl>(pimpl())->detectedFilename();
    }


    std::string FilenameMismatch::expectedFilename() const {
        return std::dynamic_pointer_cast<FilenameMismatch::Pimpl>(pimpl())->expectedFilename();
    }
}

/***********************************************************************************************************************
 * Container::FileCreationError::Pimpl
 */

namespace Container {
    class FileCreationError::Pimpl:public MetadataMismatchError::PimplBase {
        public:
            Pimpl(const std::string& detectedFilename, unsigned long long filePosition);

            ~Pimpl() override;

            std::string detectedFilename() const;

            int errorCode() const final;

            std::string description() const final;

        private:
            std::string currentDetectedFilename;
    };


    FileCreationError::Pimpl::Pimpl(
            const std::string& detectedFilename,
            unsigned long long filePosition
        ):MetadataMismatchError::PimplBase(
            filePosition
        ) {
        currentDetectedFilename = detectedFilename;
    }


    FileCreationError::Pimpl::~Pimpl() {}


    std::string FileCreationError::Pimpl::detectedFilename() const {
        return currentDetectedFilename;
    }


    int FileCreationError::Pimpl::errorCode() const {
        return FileCreationError::reportedErrorCode;
    }


    std::string FileCreationError::Pimpl::description() const {
        std::stringstream stream;

        stream << "File creation error, near " << filePosition() << ", filename " << detectedFilename();

        return stream.str();
    }
}

/***********************************************************************************************************************
 * Container::FileCreationError
 */

namespace Container {
    FileCreationError::FileCreationError(
            const std::string& detectedFilename,
            unsigned long long filePosition
        ):MetadataMismatchError(
            new FileCreationError::Pimpl(
                detectedFilename,
                filePosition
            )
        ) {}


    FileCreationError::FileCreationError(
            const Status &other
        ):MetadataMismatchError(
            other
        ) {}


    FileCreationError::~FileCreationError() {}


    std::string FileCreationError::detectedFilename() const {
        return std::dynamic_pointer_cast<FileCreationError::Pimpl>(pimpl())->detectedFilename();
    }
}

/***********************************************************************************************************************
 * Container::ContainerDataError::Pimpl
 */

namespace Container {
    class ContainerDataError::Pimpl:public MetadataMismatchError::PimplBase {
        public:
            Pimpl(unsigned long long filePosition);

            ~Pimpl() override;

            int errorCode() const final;

            std::string description() const final;
    };


    ContainerDataError::Pimpl::Pimpl(unsigned long long filePosition):MetadataMismatchError::PimplBase(filePosition) {}


    ContainerDataError::Pimpl::~Pimpl() {}


    int ContainerDataError::Pimpl::errorCode() const {
        return ContainerDataError::reportedErrorCode;
    }


    std::string ContainerDataError::Pimpl::description() const {
        std::stringstream stream;

        stream << "Container data error near " << filePosition();

        return stream.str();
    }
}

/***********************************************************************************************************************
 * Container::ContainerDataError
 */

namespace Container {
    ContainerDataError::ContainerDataError(
            unsigned long long filePosition
        ):MetadataMismatchError(
            new ContainerDataError::Pimpl(
                filePosition
            )
        ) {}


    ContainerDataError::ContainerDataError(
            const Status &other
        ):MetadataMismatchError(
            other
        ) {}


    ContainerDataError::~ContainerDataError() {}
}

/***********************************************************************************************************************
 * Container::VersionDownlevelStatus::Pimpl
 */

namespace Container {
    class VersionDownlevelStatus::Pimpl:public Status::ErrorPimplBase {
        public:
            Pimpl(std::uint8_t discoveredMajorVersion, std::uint8_t supportedMajorVersion);

            ~Pimpl() override;

            std::uint8_t discoveredMajorVersion() const;

            std::uint8_t supportedMajorVersion() const;

            Status::Class statusClass() const final;

            int errorCode() const final;

            std::string description() const final;

        private:
            std::uint8_t currentDiscoveredVersion;
            std::uint8_t currentSupportedVersion;
    };


    VersionDownlevelStatus::Pimpl::Pimpl(std::uint8_t discoveredMajorVersion, std::uint8_t supportedMajorVersion) {
        currentDiscoveredVersion = discoveredMajorVersion;
        currentSupportedVersion  = supportedMajorVersion;
    }


    VersionDownlevelStatus::Pimpl::~Pimpl() {}


    std::uint8_t VersionDownlevelStatus::Pimpl::discoveredMajorVersion() const {
        return currentDiscoveredVersion;
    }


    std::uint8_t VersionDownlevelStatus::Pimpl::supportedMajorVersion() const {
        return currentSupportedVersion;
    }


    Status::Class VersionDownlevelStatus::Pimpl::statusClass() const {
        return Status::Class::CONTAINER_VERSION_DOWNLEVEL;
    }


    int VersionDownlevelStatus::Pimpl::errorCode() const {
        return VersionDownlevelStatus::reportedErrorCode;
    }


    std::string VersionDownlevelStatus::Pimpl::description() const {
        std::stringstream stream;

        stream << "Container version downlevel: found version "
               << currentDiscoveredVersion
               << " but supports version "
               << currentSupportedVersion;

        return stream.str();
    };
}

/***********************************************************************************************************************
 * Container::VersionDownlevelStatus
 */

namespace Container {
    VersionDownlevelStatus::VersionDownlevelStatus(
            std::uint8_t detectedMajorVersion,
            std::uint8_t supportedMajorVersion
        ):Status(
            new VersionDownlevelStatus::Pimpl(detectedMajorVersion, supportedMajorVersion)
        ) {}


    VersionDownlevelStatus::VersionDownlevelStatus(const Status &other):Status(other) {}


    VersionDownlevelStatus::~VersionDownlevelStatus() {}


    std::uint8_t VersionDownlevelStatus::discoveredMajorVersion() const {
        return std::dynamic_pointer_cast<VersionDownlevelStatus::Pimpl>(pimpl())->discoveredMajorVersion();
    }


    std::uint8_t VersionDownlevelStatus::supportedMajorVersion() const {
        return std::dynamic_pointer_cast<VersionDownlevelStatus::Pimpl>(pimpl())->supportedMajorVersion();
    }
}

/***********************************************************************************************************************
 * Container::HeaderIdentifierInvalid::Pimpl
 */

namespace Container {
    class HeaderIdentifierInvalid::Pimpl:public HeaderError::PimplBase {
        public:
            Pimpl();

            ~Pimpl() override;

            int errorCode() const final;

            std::string description() const final;
    };


    HeaderIdentifierInvalid::Pimpl::Pimpl() {}


    HeaderIdentifierInvalid::Pimpl::~Pimpl() {}


    int HeaderIdentifierInvalid::Pimpl::errorCode() const {
        return HeaderIdentifierInvalid::reportedErrorCode;
    }


    std::string HeaderIdentifierInvalid::Pimpl::description() const {
        return "invalid header identifier";
    }
}

/***********************************************************************************************************************
 * Container::HeaderIdentifierInvalid
 */

namespace Container {
    HeaderIdentifierInvalid::HeaderIdentifierInvalid():HeaderError(new HeaderIdentifierInvalid::Pimpl()) {}


    HeaderIdentifierInvalid::HeaderIdentifierInvalid(const Status &other):HeaderError(other) {}


    HeaderIdentifierInvalid::~HeaderIdentifierInvalid() {}
}

/***********************************************************************************************************************
 * Container::HeaderVersionInvalid::Pimpl
 */

namespace Container {
    class HeaderVersionInvalid::Pimpl:public HeaderError::PimplBase {
        public:
            Pimpl(std::uint8_t discoveredMajorVersion, std::uint8_t supportedMajorVersion);

            ~Pimpl() override;

            std::uint8_t discoveredMajorVersion() const;

            std::uint8_t supportedMajorVersion() const;

            int errorCode() const final;

            std::string description() const final;

        private:
            std::uint8_t currentDiscoveredVersion;
            std::uint8_t currentSupportedVersion;
    };


    HeaderVersionInvalid::Pimpl::Pimpl(std::uint8_t discoveredMajorVersion, std::uint8_t supportedMajorVersion) {
        currentDiscoveredVersion = discoveredMajorVersion;
        currentSupportedVersion  = supportedMajorVersion;
    }


    HeaderVersionInvalid::Pimpl::~Pimpl() {}


    std::uint8_t HeaderVersionInvalid::Pimpl::discoveredMajorVersion() const {
        return currentDiscoveredVersion;
    }


    std::uint8_t HeaderVersionInvalid::Pimpl::supportedMajorVersion() const {
        return currentSupportedVersion;
    }


    int HeaderVersionInvalid::Pimpl::errorCode() const {
        return HeaderVersionInvalid::reportedErrorCode;
    }


    std::string HeaderVersionInvalid::Pimpl::description() const {
        std::stringstream stream;

        stream << "Container version invalid: found version "
               << currentDiscoveredVersion
               << " but supports version "
               << currentSupportedVersion;

        return stream.str();
    };
}

/***********************************************************************************************************************
 * Container::HeaderVersionInvalid
 */

namespace Container {
    HeaderVersionInvalid::HeaderVersionInvalid(
            std::uint8_t detectedMajorVersion,
            std::uint8_t supportedMajorVersion
        ):HeaderError(
            new HeaderVersionInvalid::Pimpl(detectedMajorVersion, supportedMajorVersion)
        ) {}


    HeaderVersionInvalid::HeaderVersionInvalid(const Status &other):HeaderError(other) {}


    HeaderVersionInvalid::~HeaderVersionInvalid() {}


    std::uint8_t HeaderVersionInvalid::discoveredMajorVersion() const {
        return std::dynamic_pointer_cast<HeaderVersionInvalid::Pimpl>(pimpl())->discoveredMajorVersion();
    }


    std::uint8_t HeaderVersionInvalid::supportedMajorVersion() const {
        return std::dynamic_pointer_cast<HeaderVersionInvalid::Pimpl>(pimpl())->supportedMajorVersion();
    }
}

/***********************************************************************************************************************
 * Container::HeaderCrcError::Pimpl
 */

namespace Container {
    class HeaderCrcError::Pimpl:public HeaderError::PimplBase {
        public:
            Pimpl();

            ~Pimpl() override;

            int errorCode() const final;

            std::string description() const final;
    };


    HeaderCrcError::Pimpl::Pimpl() {}


    HeaderCrcError::Pimpl::~Pimpl() {}


    int HeaderCrcError::Pimpl::errorCode() const {
        return HeaderCrcError::reportedErrorCode;
    }


    std::string HeaderCrcError::Pimpl::description() const {
        return "invalid header CRC";
    }
}

/***********************************************************************************************************************
 * Container::HeaderCrcError
 */

namespace Container {
    HeaderCrcError::HeaderCrcError():HeaderError(new HeaderCrcError::Pimpl()) {}


    HeaderCrcError::HeaderCrcError(const Status &other):HeaderError(other) {}


    HeaderCrcError::~HeaderCrcError() {}
}

/***********************************************************************************************************************
 * Container::ReadSuccessful::Pimpl
 */

namespace Container {
    class ReadSuccessful::Pimpl:public FilesystemStatus::PimplBase {
        public:
            Pimpl(unsigned bytesRead);

            ~Pimpl() override;

            unsigned bytesRead() const;

            int errorCode() const final;

            std::string description() const final;

        private:
            unsigned currentBytesRead;
    };


    ReadSuccessful::Pimpl::Pimpl(unsigned bytesRead) {
        currentBytesRead = bytesRead;
    }


    ReadSuccessful::Pimpl::~Pimpl() {}


    unsigned ReadSuccessful::Pimpl::bytesRead() const {
        return currentBytesRead;
    }


    int ReadSuccessful::Pimpl::errorCode() const {
        return ReadSuccessful::reportedErrorCode;
    }


    std::string ReadSuccessful::Pimpl::description() const {
        std::stringstream stream;

        stream << "Read successful, " << currentBytesRead << " transferred";
        return stream.str();
    };
}

/***********************************************************************************************************************
 * Container::ReadSuccessful
 */

namespace Container {
    ReadSuccessful::ReadSuccessful(unsigned bytesRead):FilesystemStatus(new ReadSuccessful::Pimpl(bytesRead)) {}


    ReadSuccessful::ReadSuccessful(const Status& other):FilesystemStatus(other) {}


    ReadSuccessful::~ReadSuccessful() {}


    unsigned ReadSuccessful::bytesRead() const {
        return std::dynamic_pointer_cast<ReadSuccessful::Pimpl>(pimpl())->bytesRead();
    }
}

/***********************************************************************************************************************
 * Container::WriteSuccessful::Pimpl
 */

namespace Container {
    class WriteSuccessful::Pimpl:public FilesystemStatus::PimplBase {
        public:
            Pimpl(unsigned bytesWritten);

            ~Pimpl() override;

            unsigned bytesWritten() const;

            int errorCode() const final;

            std::string description() const final;

        private:
            unsigned currentBytesWritten;
    };


    WriteSuccessful::Pimpl::Pimpl(unsigned bytesWritten) {
        currentBytesWritten = bytesWritten;
    }


    WriteSuccessful::Pimpl::~Pimpl() {}


    unsigned WriteSuccessful::Pimpl::bytesWritten() const {
        return currentBytesWritten;
    }


    int WriteSuccessful::Pimpl::errorCode() const {
        return WriteSuccessful::reportedErrorCode;
    }


    std::string WriteSuccessful::Pimpl::description() const {
        std::stringstream stream;

        stream << "Write successful, " << currentBytesWritten << " transferred";
        return stream.str();
    };
}

/***********************************************************************************************************************
 * Container::WriteSuccessful
 */

namespace Container {
    WriteSuccessful::WriteSuccessful(
            unsigned bytesWritten
        ):FilesystemStatus(
            new WriteSuccessful::Pimpl(bytesWritten)
        ) {}


    WriteSuccessful::WriteSuccessful(const Status& other):FilesystemStatus(other) {}


    WriteSuccessful::~WriteSuccessful() {}


    unsigned WriteSuccessful::bytesWritten() const {
        return std::dynamic_pointer_cast<WriteSuccessful::Pimpl>(pimpl())->bytesWritten();
    }
}

/***********************************************************************************************************************
 * Container::SeekError::Pimpl
 */

namespace Container {
    class SeekError::Pimpl:public FilesystemError::PimplBase {
        public:
            Pimpl(unsigned long long desiredPosition, unsigned long long containerSizeInBytes);

            ~Pimpl() override;

            unsigned long long position() const;

            unsigned long long containerSize() const;

            int errorCode() const final;

            std::string description() const final;

        private:
            unsigned long long currentPosition;
            unsigned long long currentContainerSize;
    };


    SeekError::Pimpl::Pimpl(unsigned long long desiredPosition, unsigned long long containerSizeInBytes) {
        currentPosition      = desiredPosition;
        currentContainerSize = containerSizeInBytes;
    }


    SeekError::Pimpl::~Pimpl() {}


    unsigned long long SeekError::Pimpl::position() const {
        return currentPosition;
    }


    unsigned long long SeekError::Pimpl::containerSize() const {
        return currentContainerSize;
    }


    int SeekError::Pimpl::errorCode() const {
        return SeekError::reportedErrorCode;
    }


    std::string SeekError::Pimpl::description() const {
        std::stringstream stream;

        stream << "Seek error, "<< currentPosition << " > " << currentContainerSize;
        return stream.str();
    };
}

/***********************************************************************************************************************
 * Container::SeekError
 */

namespace Container {
    SeekError::SeekError(
            unsigned long long desiredPosition,
            unsigned long long containerSizeInBytes
        ):FilesystemError(
            new SeekError::Pimpl(desiredPosition, containerSizeInBytes)
        ) {}


    SeekError::SeekError(const Status &other):FilesystemError(other) {}


    SeekError::~SeekError() {}


    unsigned long long SeekError::position() const {
        return std::dynamic_pointer_cast<SeekError::Pimpl>(pimpl())->position();
    }


    unsigned long long SeekError::containerSize() const {
        return std::dynamic_pointer_cast<SeekError::Pimpl>(pimpl())->containerSize();
    }
}

/***********************************************************************************************************************
 * Container::InvalidOpenMode::Pimpl
 */

namespace Container {
    class InvalidOpenMode::Pimpl:public FilesystemError::PimplBase {
        public:
            Pimpl(FileContainer::OpenMode openMode);

            ~Pimpl() override;

            FileContainer::OpenMode openMode() const;

            int errorCode() const final;

            std::string description() const final;

        private:
            FileContainer::OpenMode currentOpenMode;
    };


    InvalidOpenMode::Pimpl::Pimpl(FileContainer::OpenMode openMode) {
        currentOpenMode = openMode;
    }


    InvalidOpenMode::Pimpl::~Pimpl() {}


    FileContainer::OpenMode InvalidOpenMode::Pimpl::openMode() const {
        return currentOpenMode;
    }


    int InvalidOpenMode::Pimpl::errorCode() const {
        return InvalidOpenMode::reportedErrorCode;
    }


    std::string InvalidOpenMode::Pimpl::description() const {
        std::stringstream stream;

        stream << "Invalid open mode " << static_cast<unsigned>(currentOpenMode);
        return stream.str();
    };
}

/***********************************************************************************************************************
 * Container::InvalidOpenMode
 */

namespace Container {
    InvalidOpenMode::InvalidOpenMode(
            FileContainer::OpenMode openMode
        ):FilesystemError(
            new InvalidOpenMode::Pimpl(openMode)
        ) {}


    InvalidOpenMode::InvalidOpenMode(const Status& other):FilesystemError(other) {}


    InvalidOpenMode::~InvalidOpenMode() {}


    FileContainer::OpenMode InvalidOpenMode::openMode() const {
        return std::dynamic_pointer_cast<InvalidOpenMode::Pimpl>(pimpl())->openMode();
    }
}

/***********************************************************************************************************************
 * Container::FileContainerNotOpen::Pimpl
 */

namespace Container {
    class FileContainerNotOpen::Pimpl:public FilesystemError::PimplBase {
        public:
            Pimpl();

            ~Pimpl() override;

            int errorCode() const final;

            std::string description() const final;
    };


    FileContainerNotOpen::Pimpl::Pimpl() {}


    FileContainerNotOpen::Pimpl::~Pimpl() {}


    int FileContainerNotOpen::Pimpl::errorCode() const {
        return FileContainerNotOpen::reportedErrorCode;
    }


    std::string FileContainerNotOpen::Pimpl::description() const {
        return "Container not open";
    };
}

/***********************************************************************************************************************
 * Container::FileContainerNotOpen
 */

namespace Container {
    FileContainerNotOpen::FileContainerNotOpen():FilesystemError(new FileContainerNotOpen::Pimpl()) {}


    FileContainerNotOpen::FileContainerNotOpen(const Status& other):FilesystemError(other) {}


    FileContainerNotOpen::~FileContainerNotOpen() {}
}

/***********************************************************************************************************************
 * Container::FailedToOpenFile::Pimpl
 */

namespace Container {
    class FailedToOpenFile::Pimpl:public FilesystemError::PimplBase {
        public:
            Pimpl(const std::string& filename, FileContainer::OpenMode openMode, int errorNumber);

            ~Pimpl() override;

            std::string filename() const;

            FileContainer::OpenMode openMode() const;

            int errorNumber() const;

            int errorCode() const final;

            std::string description() const final;

        private:
            std::string             currentFilename;
            FileContainer::OpenMode currentOpenMode;
            int                     currentErrorNumber;
    };


    FailedToOpenFile::Pimpl::Pimpl(const std::string& filename, FileContainer::OpenMode openMode, int errorNumber) {
        currentFilename = filename;
        currentOpenMode = openMode;
        currentErrorNumber = errorNumber;
    }


    FailedToOpenFile::Pimpl::~Pimpl() {}


    std::string FailedToOpenFile::Pimpl::filename() const {
        return currentFilename;
    }


    FileContainer::OpenMode FailedToOpenFile::Pimpl::openMode() const {
        return currentOpenMode;
    }


    int FailedToOpenFile::Pimpl::errorNumber() const {
        return currentErrorNumber;
    }


    int FailedToOpenFile::Pimpl::errorCode() const {
        return FailedToOpenFile::reportedErrorCode;
    }


    std::string FailedToOpenFile::Pimpl::description() const {
        std::stringstream stream;

        stream << "Failed to open " << currentFilename
               << ", mode " << static_cast<unsigned>(currentOpenMode)
               << ", (" << std::strerror(currentErrorNumber) << ")";

        return stream.str();
    };
}

/***********************************************************************************************************************
 * Container::FailedToOpenFile
 */

namespace Container {
    FailedToOpenFile::FailedToOpenFile(
            const std::string&      filename,
            FileContainer::OpenMode openMode,
            int                     errorNumber
        ):FilesystemError(
            new FailedToOpenFile::Pimpl(filename, openMode, errorNumber)
        ) {}


    FailedToOpenFile::FailedToOpenFile(const Status& other):FilesystemError(other) {}


    FailedToOpenFile::~FailedToOpenFile() {}


    std::string FailedToOpenFile::filename() const {
        return std::dynamic_pointer_cast<FailedToOpenFile::Pimpl>(pimpl())->filename();
    }


    FileContainer::OpenMode FailedToOpenFile::openMode() const {
        return std::dynamic_pointer_cast<FailedToOpenFile::Pimpl>(pimpl())->openMode();
    }


    int FailedToOpenFile::errorNumber() const {
        return std::dynamic_pointer_cast<FailedToOpenFile::Pimpl>(pimpl())->errorNumber();
    }
}

/***********************************************************************************************************************
 * Container::FileCloseError::Pimpl
 */

namespace Container {
    class FileCloseError::Pimpl:public FilesystemError::PimplBase {
        public:
            Pimpl(const std::string& filename, int errorNumber);

            ~Pimpl() override;

            std::string filename() const;

            int errorNumber() const;

            int errorCode() const final;

            std::string description() const final;

        private:
            std::string currentFilename;
            int         currentErrorNumber;
    };


    FileCloseError::Pimpl::Pimpl(const std::string& filename, int errorNumber) {
        currentFilename = filename;
        currentErrorNumber = errorNumber;
    }


    FileCloseError::Pimpl::~Pimpl() {}


    std::string FileCloseError::Pimpl::filename() const {
        return currentFilename;
    }


    int FileCloseError::Pimpl::errorNumber() const {
        return currentErrorNumber;
    }


    int FileCloseError::Pimpl::errorCode() const {
        return FileCloseError::reportedErrorCode;
    }


    std::string FileCloseError::Pimpl::description() const {
        std::stringstream stream;

        stream << "Failed to close " << currentFilename
               << ", (" << std::strerror(currentErrorNumber) << ")";

        return stream.str();
    };
}

/***********************************************************************************************************************
 * Container::FileCloseError
 */

namespace Container {
    FileCloseError::FileCloseError(
            const std::string& filename,
            int                errorNumber
        ):FilesystemError(
            new FileCloseError::Pimpl(filename, errorNumber)
        ) {}


    FileCloseError::FileCloseError(const Status& other):FilesystemError(other) {}


    FileCloseError::~FileCloseError() {}


    std::string FileCloseError::filename() const {
        return std::dynamic_pointer_cast<FileCloseError::Pimpl>(pimpl())->filename();
    }


    int FileCloseError::errorNumber() const {
        return std::dynamic_pointer_cast<FileCloseError::Pimpl>(pimpl())->errorNumber();
    }
}

/***********************************************************************************************************************
 * Container::FileReadError::Pimpl
 */

namespace Container {
    class FileReadError::Pimpl:public FilesystemError::PimplBase {
        public:
            Pimpl(const std::string& filename, unsigned long long position, int errorNumber);

            ~Pimpl() override;

            std::string filename() const;

            unsigned long long position() const;

            int errorNumber() const;

            int errorCode() const final;

            std::string description() const final;

        private:
            std::string        currentFilename;
            unsigned long long currentPosition;
            int                currentErrorNumber;
    };


    FileReadError::Pimpl::Pimpl(const std::string& filename, unsigned long long position, int errorNumber) {
        currentFilename = filename;
        currentPosition = position;
        currentErrorNumber = errorNumber;
    }


    FileReadError::Pimpl::~Pimpl() {}


    std::string FileReadError::Pimpl::filename() const {
        return currentFilename;
    }


    unsigned long long FileReadError::Pimpl::position() const {
        return currentPosition;
    }


    int FileReadError::Pimpl::errorNumber() const {
        return currentErrorNumber;
    }


    int FileReadError::Pimpl::errorCode() const {
        return FileReadError::reportedErrorCode;
    }


    std::string FileReadError::Pimpl::description() const {
        std::stringstream stream;

        stream << "Read error in file \"" << currentFilename << "\""
               << " at or near " << currentPosition
               << ", (" << std::strerror(currentErrorNumber) << ")";

        return stream.str();
    };
}

/***********************************************************************************************************************
 * Container::FileReadError
 */

namespace Container {
    FileReadError::FileReadError(
            const std::string& filename,
            unsigned long long position,
            int                errorNumber
        ):FilesystemError(
            new FileReadError::Pimpl(filename, position, errorNumber)
        ) {}


    FileReadError::FileReadError(const Status& other):FilesystemError(other) {}


    FileReadError::~FileReadError() {}


    std::string FileReadError::filename() const {
        return std::dynamic_pointer_cast<FileReadError::Pimpl>(pimpl())->filename();
    }


    unsigned long long FileReadError::position() const {
        return std::dynamic_pointer_cast<FileReadError::Pimpl>(pimpl())->position();
    }


    int FileReadError::errorNumber() const {
        return std::dynamic_pointer_cast<FileReadError::Pimpl>(pimpl())->errorNumber();
    }
}

/***********************************************************************************************************************
 * Container::FileWriteError::Pimpl
 */

namespace Container {
    class FileWriteError::Pimpl:public FilesystemError::PimplBase {
        public:
            Pimpl(const std::string& filename, unsigned long long position, int errorNumber);

            ~Pimpl() override;

            std::string filename() const;

            unsigned long long position() const;

            int errorNumber() const;

            int errorCode() const final;

            std::string description() const final;

        private:
            std::string        currentFilename;
            unsigned long long currentPosition;
            int                currentErrorNumber;
    };


    FileWriteError::Pimpl::Pimpl(const std::string& filename, unsigned long long position, int errorNumber) {
        currentFilename = filename;
        currentPosition = position;
        currentErrorNumber = errorNumber;
    }


    FileWriteError::Pimpl::~Pimpl() {}


    std::string FileWriteError::Pimpl::filename() const {
        return currentFilename;
    }


    unsigned long long FileWriteError::Pimpl::position() const {
        return currentPosition;
    }


    int FileWriteError::Pimpl::errorNumber() const {
        return currentErrorNumber;
    }


    int FileWriteError::Pimpl::errorCode() const {
        return FileWriteError::reportedErrorCode;
    }


    std::string FileWriteError::Pimpl::description() const {
        std::stringstream stream;

        stream << "Write error in file \"" << currentFilename << "\""
               << " at or near " << currentPosition
               << ", (" << std::strerror(currentErrorNumber) << ")";

        return stream.str();
    };
}

/***********************************************************************************************************************
 * Container::FileWriteError
 */

namespace Container {
    FileWriteError::FileWriteError(
            const std::string& filename,
            unsigned long long position,
            int                errorNumber
        ):FilesystemError(
            new FileWriteError::Pimpl(filename, position, errorNumber)
        ) {}


    FileWriteError::FileWriteError(const Status& other):FilesystemError(other) {}


    FileWriteError::~FileWriteError() {}


    std::string FileWriteError::filename() const {
        return std::dynamic_pointer_cast<FileWriteError::Pimpl>(pimpl())->filename();
    }


    unsigned long long FileWriteError::position() const {
        return std::dynamic_pointer_cast<FileWriteError::Pimpl>(pimpl())->position();
    }


    int FileWriteError::errorNumber() const {
        return std::dynamic_pointer_cast<FileWriteError::Pimpl>(pimpl())->errorNumber();
    }
}

/***********************************************************************************************************************
 * Container::FileTruncateError::Pimpl
 */

namespace Container {
    class FileTruncateError::Pimpl:public FilesystemError::PimplBase {
        public:
            Pimpl(const std::string& filename, unsigned long long position, int errorNumber);

            ~Pimpl() override;

            std::string filename() const;

            unsigned long long position() const;

            int errorNumber() const;

            int errorCode() const final;

            std::string description() const final;

        private:
            std::string        currentFilename;
            unsigned long long currentPosition;
            int                currentErrorNumber;
    };


    FileTruncateError::Pimpl::Pimpl(const std::string& filename, unsigned long long position, int errorNumber) {
        currentFilename = filename;
        currentPosition = position;
        currentErrorNumber = errorNumber;
    }


    FileTruncateError::Pimpl::~Pimpl() {}


    std::string FileTruncateError::Pimpl::filename() const {
        return currentFilename;
    }


    unsigned long long FileTruncateError::Pimpl::position() const {
        return currentPosition;
    }


    int FileTruncateError::Pimpl::errorNumber() const {
        return currentErrorNumber;
    }


    int FileTruncateError::Pimpl::errorCode() const {
        return FileTruncateError::reportedErrorCode;
    }


    std::string FileTruncateError::Pimpl::description() const {
        std::stringstream stream;

        stream << "Write error in file \"" << currentFilename << "\""
               << " at or near " << currentPosition
               << ", (" << std::strerror(currentErrorNumber) << ")";

        return stream.str();
    };
}

/***********************************************************************************************************************
 * Container::FileTruncateError
 */

namespace Container {
    FileTruncateError::FileTruncateError(
            const std::string& filename,
            unsigned long long position,
            int                errorNumber
        ):FilesystemError(
            new FileTruncateError::Pimpl(filename, position, errorNumber)
        ) {}


    FileTruncateError::FileTruncateError(const Status& other):FilesystemError(other) {}


    FileTruncateError::~FileTruncateError() {}


    std::string FileTruncateError::filename() const {
        return std::dynamic_pointer_cast<FileTruncateError::Pimpl>(pimpl())->filename();
    }


    unsigned long long FileTruncateError::position() const {
        return std::dynamic_pointer_cast<FileTruncateError::Pimpl>(pimpl())->position();
    }


    int FileTruncateError::errorNumber() const {
        return std::dynamic_pointer_cast<FileTruncateError::Pimpl>(pimpl())->errorNumber();
    }
}

/***********************************************************************************************************************
 * Container::FileFlushError::Pimpl
 */

namespace Container {
    class FileFlushError::Pimpl:public FilesystemError::PimplBase {
        public:
            Pimpl(const std::string& filename, int errorNumber);

            ~Pimpl() override;

            std::string filename() const;

            int errorNumber() const;

            int errorCode() const final;

            std::string description() const final;

        private:
            std::string currentFilename;
            int         currentErrorNumber;
    };


    FileFlushError::Pimpl::Pimpl(const std::string& filename, int errorNumber) {
        currentFilename = filename;
        currentErrorNumber = errorNumber;
    }


    FileFlushError::Pimpl::~Pimpl() {}


    std::string FileFlushError::Pimpl::filename() const {
        return currentFilename;
    }


    int FileFlushError::Pimpl::errorNumber() const {
        return currentErrorNumber;
    }


    int FileFlushError::Pimpl::errorCode() const {
        return FileFlushError::reportedErrorCode;
    }


    std::string FileFlushError::Pimpl::description() const {
        std::stringstream stream;

        stream << "Flush error in file \"" << currentFilename << "\""
               << " (" << std::strerror(currentErrorNumber) << ")";

        return stream.str();
    };
}

/***********************************************************************************************************************
 * Container::FileFlushError
 */

namespace Container {
    FileFlushError::FileFlushError(
            const std::string& filename,
            int                errorNumber
        ):FilesystemError(
            new FileFlushError::Pimpl(filename, errorNumber)
        ) {}


    FileFlushError::FileFlushError(const Status& other):FilesystemError(other) {}


    FileFlushError::~FileFlushError() {}


    std::string FileFlushError::filename() const {
        return std::dynamic_pointer_cast<FileFlushError::Pimpl>(pimpl())->filename();
    }


    int FileFlushError::errorNumber() const {
        return std::dynamic_pointer_cast<FileFlushError::Pimpl>(pimpl())->errorNumber();
    }
}
