============
inecontainer
============
The inecontainer library provides a virtual file system contained within a
actual file or memory buffer on your system.  You can perform most operations
on virtual files that you would on actual files, including:

* Open, close, delete, and rename files.

* Read, write, and overwrite contents.

* Seek to specific byte offsets within a virtual file, and

* Truncate and append to virtual files.

In addition, the API allows you to stream read the physical file contents.  You
can define callbacks to be triggered to open and handle files and each file
contents can be presented in byte order.  The stream read operation will
guarantee that both the container and virtual files will read and present data
in proper byte order using only a single pass through the container file.

This library is used to read and write `Aion <https://inesonic.com>` document
files.


Why inecontainer
================
During the development of **Aion**, we considered a number of different file
formats.  We wanted a file format that allowed us to:

* Track multiple payloads within the same physical file.  We wanted to make
  certain that an entire **Aion** document would be contained in a single
  physical file on the file system.

* Efficiently handle both textual and binary data.  We did not want to impose
  base 64, base 85 or similar on binary data.

* No imposition on the type of data we could store.  We wanted a file format
  that would be as future proof as possible.

* The ability to optionally compress content, but only where appropriate.

* Modify the file contents, in place during operation. We didn't want to
  require that the entire file contents be rebuilt on every save.  This implied
  that we would not know the size of any virtual file apriori and virtual file
  sizes could change over time.

* Support revisioning and deltas if/when we implement it by maintaining
  virtual files containing reverse deltas that we could unwind.

* Allow streaming reads when loading a file to minimize loading time.

We considered a number of existing formats.

tar
---
The tar format is relatively simple, storing data in blocks, typically 512
bytes in length.  Each file is preceded by a header that provides the name
of the file and other information related to the file.  The size of the file
is included in the header.  A tar archive is typically terminated by two blocks
containing all zeros.

The tar format has several key drawbacks:

* You must know the size of each file before writing the header as the header
  contains the file size.

* You can only stream information from a single file into a tar archive at a
  time as each file must exist contiguously in the archive.

* Compression typically occurs after data is inserted into the container so
  the entire container must be streamed.

* While it's possible to add to an existing tar archive, modifying the contents
  of a file in an archive is problematic.

zip
---
The zip file format is a much more complex format.  Files are inserted,
compressed in a contiguous block.  Each file is preceded by a header containing
the filename, compression method, CRC, version information, etc.  The header is
then followed by the filename and then by the compressed file.  The zip archive
is terminated with a directory that points backwards to each file up-to that
directory.

New files are added by appending after the directory.  After each batch of new
files is added, a new directory is appended that point backwards to the newly
added files.

The zip format fails for many of the same reasons as the tar format:

* The format does not support multiple concurrently active virtual files.

* In-place replacement or modification is not possible.

While a standard exists for the format, there are many proprietary extensions
that limit cross-tool support.


7z
--
This is the most attractive standard container format in that it is open source
with all code released into the public domain.  The format also offers good
compression.

At the time, we could not find documentation on the file format except for the
7z source code.  Source is complex enough that simply leveraging the existing
source would take significant effort.

After spending time reviewing, the decision was made that the effort to
thoroughly review and critique this format for our application was not
worthwhile.


Building inecontainer
=====================
The build environment currently supports both qmake and cmake build tools.  The
build environment includes the following subprojects:

+--------------+--------------------------------------------------------------+
| Project      | Purpose                                                      |
+==============+==============================================================+
| inecontainer | The inecontainer static library.                             |
+--------------+--------------------------------------------------------------+
| ic           | The Inesonic container command line tool you can use to      |
|              | manipulate container contents.                               |
+--------------+--------------------------------------------------------------+
| test         | An optional QtTest based test framework to validate the      |
|              | inecontainer library functionality.  The test framework will |
|              | be built automatically by the qmake build environment.  The  |
|              | The cmake build environment will build the test framework    |
|              | only if the QtCore and QtTest libraries can be found.        |
+--------------+--------------------------------------------------------------+

Note that the directions below will work for Linux and MacOS.  For Windows,
either use nmake or jom, or with cmake, select a different generator such as
the ninja generator.


qmake
-----
To build inecontainer using qmake:

.. code-block:: bash

   cd inecontainer
   mkdir build
   cd build
   qmake ../inecontainer.pro
   make

If you wish to create a debug build, change the qmake line to:

.. code-block:: bash

   qmake ../inecontainer.pro CONFIG+=debug
   make

Note that the qmake build environment currently does not have an install target
defined.


cmake
-----
To build inecontainer using cmake:

.. code-block:: bash

   cd inecontainer
   mkdir build
   cmake -B. -H.. -DCMAKE_INSTALL_PREFIX=/usr/local/
   make

If you wish to build the test framework, change the cmake line to:

.. code-block:: bash

   cmake -B. -H.. -DCMAKE_INSTALL_PREFIX=/usr/local/ -DQt5_DIR=<path to Qt5Config.cmake>

To install, simply run

.. code-block:: bash

   make install


Using inecontainer
==================
The inecontainer library is relatively simple to use.


Examples
--------
Currently the best examples are the ``ic`` executable located in the ``ic``
directory and the test framework located in the ``test`` directory.


Containers
----------
First create a container instance, either ``Container::MemoryContainer`` or
``Container::FileContainer`` that will track the virtual files to be managed.
You will need to call ``Container::*Container::open`` to open the file or
memory container.

.. code-block:: c++

   #include <iostream>
   #include <string>

   #include <container_container.h>
   #include <container_file_container.h>
   #include <container_status.h>
   . . .

   // Identifier string placed at the top of the container.
   static const std::string fileIdentifier = "Inesonic, LLC.\nAleph";

   . . .

   int exitStatus;
   
   Container::FileContainer container(fileIdentifier);

   std::string filename = "container.aion";
   Container::Status status = container.open(
       filename,
       Container::FileContainer::OpenMode::READ_WRITE
   );

   if (!status) { // status casts to true on error.
       // Manipulate container contents.
   } else {
       std::cerr << "*** " << status.description() << std::endl;
       exitStatus = 1;    
   }

   return exitStatus;

Once the container has been opened, you can work with virtual files within
the container.

To obtain a list of virtual files, you can obtain a directory map:

.. code-block:: c++

   #include <container
   Container::FileContainer::DirectoryMap directory = container.directory();
   if (!container.lastStatus()) {
       // Manipulate the directory map.
   }

The ``DirectoryMap`` is simply a ``std::map``

.. code-block:: c++

   typedef std::map<std::string, std::shared_ptr<Container::VirtualFile>> DirectoryMap;

so you can use all the goodness of the C++ STL to search and process the map
contents.  Note that reading the directory map does require scanning the
container contents; however, once scanned, the directory will be cached locally
to reduce I/O requirements.

   
Virtual Files
-------------
You can access a virtual file by name using the
``Container::*Container::newVirtualFile`` method.

.. code-block:: c++

   std::string filename = "my_virtual_image.png";
   std::shared_ptr<Container::VirtualFile> vf = container.newVirtualFile(filename);
   // Manipulate the virtual file here.

Once your virtual file has been instantiated, you can use any of the methods
listed below to manipulate the virtual file.

+-------------------------------------------+---------------------------------+
| Method                                    | Provides                        |
+===========================================+=================================+
| Container::VirtualFile::name              | Returns the name of the virtual |
|                                           | file.                           |
+-------------------------------------------+---------------------------------+
| Container::VirtualFile::size              | Returns the size of the virtual |
|                                           | file, in bytes.  A negative     |
|                                           | value is returned on error.     |
+-------------------------------------------+---------------------------------+
| Container::VirtualFile::setPosition       | Sets the file pointer to a      |
|                                           | specific byte offset.           |
+-------------------------------------------+---------------------------------+
| Container::VirtualFile::setPositionLast   | Sets the file pointer just past |
|                                           | the end of the virtual file.    |
+-------------------------------------------+---------------------------------+
| Container::VirtualFile::position          | Returns the current file        |
|                                           | pointer.                        |
+-------------------------------------------+---------------------------------+
| Container::VirtualFile::read              | Reads content into an array of  |
|                                           | bytes.  The returned status     |
|                                           | returns details on the read     |
|                                           | operation.                      |
+-------------------------------------------+---------------------------------+
| Container::VirtualFile::bytesInWriteCache | Returns the number of cached    |
|                                           | bytes of data, pending write.   |
+-------------------------------------------+---------------------------------+
| Container::VirtualFile::write             | Writes data to virtual file.    |
+-------------------------------------------+---------------------------------+
| Container::VirtualFile::append            | Writes data to the end of the   |
|                                           | virtual file.                   |
+-------------------------------------------+---------------------------------+
| Container::VirtualFile::truncate          | Truncates the virtual file at   |
|                                           | the current file pointer.       |
+-------------------------------------------+---------------------------------+
| Container::VirtualFile::flush             | Flushes any pending write data  |
|                                           | to media.                       |
+-------------------------------------------+---------------------------------+
| Container::VirtualFile::rename            | Renames the virtual file.       |
+-------------------------------------------+---------------------------------+
| Container::VirtualFile::erase             | Erases an existing virtual      |
|                                           | file.  Note that space consumed |
|                                           | by the container may not be     |
|                                           | recovered immediately.          |
+-------------------------------------------+---------------------------------+


Streaming Write API
-------------------
To stream write operations, simply limit your use of the virtual file functions
to ``Container::VirtualFile::write``.  The library will fill any unused space
in the container.  Once all unused space is consumed, the library will append
to the end of the container.


Streaming Read API
------------------
The streaming read API allows you to process an entire container's contents in
a single pass, including overhead required to check the file identifier. locate
unused regions in the container, and to build the directory map for the
container.

To use the streaming read API, you must create classes derived from
``Container::FileContainer`` and ``Container::VirtualFile``.

The class derived from ``Container::VirtualFile`` should overload the method
Overload the protected methods ``Container::VirtualFile::receivedData`` which
will be called when new data is available for the virtual file and
``Container::VirtualFile::endOfFile`` which will be called when the end of the
virtual file has been reached.

The class derived from ``Container::FileContainer`` should overload the
``Container::Container::createFile`` method.  In this method you should
instantiate instances of your overloaded ``Container::VirtualFile`` class.


Container Format
================
The container format is based on the concept of a *chunk*.  A chunk is a base
unit of data.

Each chunk can be a power-of-2 unit of size, ranging from 32 bytes in length
to 4096 bytes in length.  The first 16-bits of each chunk is a header that
is used to determine the type, size, and amount of valid data in the chunk.

The header is followed by a payload whose format is determined by the chunk
header.


Chunk Header
------------
The format of a chunk header is described below.  Note data is in little-endian
format.

.. _table-container-chunk-header:
.. table:: Chunk header format

   +-------------+----------------+--------------------+----------------------+
   | Bit Offset  | Length (Bits)  | Name               | Purpose              |
   +=============+================+====================+======================+
   | 0           | 2              | type               | Indicates the chunk  |
   |             |                |                    | type.                |
   +-------------+----------------+--------------------+----------------------+
   | 2           | 3              | s\ :sub:`p2`       | Indicates the        |
   |             |                |                    | power-of-2 size of   |
   |             |                |                    | the chunk, in bytes, |
   |             |                |                    | including this       |
   |             |                |                    | header.              |
   +-------------+----------------+--------------------+----------------------+
   | 5           | 11             | s\ :sub:`i`        | Indicates the        |
   |             |                |                    | number of invalid    |
   |             |                |                    | bytes of data at the |
   |             |                |                    | end of the chunk.    |
   +-------------+----------------+--------------------+----------------------+
   | 16          | 16             | crc                | CCITT-16 systematic  |
   |             |                |                    | CRC covering all the |
   |             |                |                    | valid data in the    |
   |             |                |                    | chunk, including the |
   |             |                |                    | first 16-bits of the |
   |             |                |                    | header.              |
   +-------------+----------------+--------------------+----------------------+

The two bits for the :math:`type` field allow for four distinct types of chunks
as documented below.

.. _table-container-chunk-types:
.. table:: Chunk types

   +--------------+-----------------------------------------------------------+
   | :math:`type` | Purpose                                                   |
   +==============+===========================================================+
   | 0            | File header chunk.  One of these will exist as the first  |
   |              | chunk in the file.  Provides a magic number and container |
   |              | revision information.                                     |
   +--------------+-----------------------------------------------------------+
   | 1            | Stream start chunk.  This type of chunk is used to mark   |
   |              | the beginning of a virtual file and tied a virtual file   |
   |              | to a specific stream ID.                                  |
   +--------------+-----------------------------------------------------------+
   | 2            | Stream continuation chunk.  This type of chunk is used to |
   |              | store data associated with virtual file.                  |
   +--------------+-----------------------------------------------------------+
   | 3            | Fill/unused.  This type of chunk contains no useful data. |
   +--------------+-----------------------------------------------------------+

Details on each chunk type will follow in subsequent sections.

The chunk size is calculated based on the :math:`s _ { p2 }` field and will
always be a power of 2 in size.  The total chunk size, in bytes, including the
header can be calculated by:

.. math::

   s _ { chunk } = 2 ^ { s _ { p2 } + 5 }

The table below lists the chunk sizes and number of bytes of usable payload,
s\ :sub`payload,max`.

.. _table-container-chunk-sizes:
.. table:: Chunk sizes

   +--------------------+-----------------------+------------------------------+
   | s\ :sub:`p2`       | s\ :sub:`chunk`       | s\ :sub:`payload,max`        |
   +====================+=======================+==============================+
   | 0                  | 32 bytes              | 30 bytes                     |
   +--------------------+-----------------------+------------------------------+
   | 1                  | 64 bytes              | 62 bytes                     |
   +--------------------+-----------------------+------------------------------+
   | 2                  | 128 bytes             | 126 bytes                    |
   +--------------------+-----------------------+------------------------------+
   | 3                  | 256 bytes             | 254 bytes                    |
   +--------------------+-----------------------+------------------------------+
   | 4                  | 512 bytes             | 510 bytes                    |
   +--------------------+-----------------------+------------------------------+
   | 5                  | 1024 bytes            | 1022 bytes                   |
   +--------------------+-----------------------+------------------------------+
   | 6                  | 2048 bytes            | 2046 bytes                   |
   +--------------------+-----------------------+------------------------------+
   | 7                  | 4096 bytes            | 4094 bytes                   |
   +--------------------+-----------------------+------------------------------+

Using chunks that are strictly a power-of-2 in size allows chunks to be easily
split and aggregated when the container is modified-in-place.  To maximize
storage efficiency, you should always use the largest chunk size possible if
sufficient data exists.

When insufficient data exists to use a 4096 byte long chunk, you should use the
smallest chunk size possible that will fully contain the data.

The number of valid bytes of data in the payload is calculated by knowing the
number of invalid bytes of data in the chunk, s\ :sub:`i`.

The number of valid bytes of data, less the header, s\ :sub:`v` is given
by:

.. math::

   s _ { v } = s _ { chunk } - s _ { i } - 4

   s _ { chunk } - s _ { v } - 4 = s _ { i }


The CRC is calculated across all *valid* bytes of data, including the first
two bytes.

You should use the generator polynomial:

.. math::

   x ^ { 16 } + x ^ { 15 } + x ^ 2 + 1

The CRC generator should use a Galois LFSR structure with the register
initialized to 0 at the start of each block calculation.  Bits are shifted into
the CRC on a byte basis, MSB first, starting with byte 0.


File Header Chunk
-----------------
The file header chunk will be inserted as the first chunk in an container.  The
chunk exists to:

* Provide identifying magic numbers that can be used by the operating system to
  identify the file contents.  Note that the API allows you to set an identifier
  string to facilitate the use of this container format for multiple file types.

* Provide a container version code.

The file header chunk format is listed below.

.. _table-container-file-header-chunk-format:
.. table:: File header chunk

   +--------------+----------------+------------------------------------------+
   | Byte Offset  | Length (Bytes) | Purpose                                  |
   +==============+================+==========================================+
   | 0            | 4              | Chunk header, ``type = 0``.              |
   +--------------+----------------+------------------------------------------+
   | 4            | 1              | Container format minor version code.     |
   +--------------+----------------+------------------------------------------+
   | 5            | 1              | Container format major version code.     |
   +--------------+----------------+------------------------------------------+
   | 6            | 2              | Reserved for future use, set to 0x0000.  |
   +--------------+----------------+------------------------------------------+
   | 8            | ???            | Identifier string in UTF-8 format.       |
   +--------------+----------------+------------------------------------------+

Note that the container format version codes will change as the container
format is adjusted and updated.


Stream Chunk
------------
A stream chunk contains data for a given virtual file.  There are two types of
stream chunks, discussed below, that represent the start of a stream of data
representing the first bytes of a virtual file and a continuation of a stream,
representing subsequent bytes in a virtual file.

All stream chunks contain a stream ID which uniquely tie the chunks to a given
virtual file.  Note that the stream ID used for a virtual file must be unique
to each stream representing a virtual file.

All stream chunks also contain a sequence number used to confirm that no data
is missing.  The sequence number 0x0000 indicates that the stream chunk is the
first chunk for a virtual file.    Note that, to support arbitrarily large
virtual files, sequence numbers are allowed to roll over from 0xFFFF to 0x0001,
skipping 0x0000 when rolling over.


Stream Start Chunk
------------------
The stream start chunk indicates the start of a new virtual file in the
container.

The container chunk format is documented below.

.. _table-container-stream-start-chunk-format:
.. table:: Stream start chunk

   +-------------+---------------+--------------------------------------------+
   | Bit Offset  | Length (Bits) | Purpose                                    |
   +=============+===============+============================================+
   | 0           | 32            | Chunk header, ``type = 1``.                |
   +-------------+---------------+--------------------------------------------+
   | 32          | 31            | Stream ID.                                 |
   +-------------+---------------+--------------------------------------------+
   | 63          | 1             | Indicates EOF, always set to 0.            |
   +-------------+---------------+--------------------------------------------+
   | 64          | 960           | UTF-8 encoded string holding the name of   |
   |             | (120 bytes)   | the virtual file as a nul terminated       |
   |             |               | string.                                    |
   +-------------+---------------+--------------------------------------------+


Stream Continuation Chunk
-------------------------
The stream continuation chunk indicates continuation of a virtual file after
the stream start chunk.

The container chunk format is documented below.

.. _table-container-stream-continuation-chunk-format:
.. table:: Stream start chunk

   +-------------+---------------+--------------------------------------------+
   | Bit Offset  | Length (Bits) | Purpose                                    |
   +=============+===============+============================================+
   | 0           | 32            | Chunk header, ``type = 2``.                |
   +-------------+---------------+--------------------------------------------+
   | 32          | 31            | Stream ID.                                 |
   +-------------+---------------+--------------------------------------------+
   | 63          | 1             | Indicates EOF, if set.  Note this bit is   |
   |             |               | currently unused.                          |
   +-------------+---------------+--------------------------------------------+
   | 64          | 48            | Byte offset from the start of the stream   |
   |             | (6 bytes)     | represented by the first byte of data in   |
   |             |               | this chunk.                                |
   +-------------+---------------+--------------------------------------------+
   | 112         | ???           | Stream data.                               |
   +-------------+---------------+--------------------------------------------+


Fill Chunk
----------
A fill or unused chunk is used to indicate a hole in the file created when a
stream is deleted or truncated.  New streams can use these holes with the note
that the *Stream Start Chunk* requires a chunk size of at least 128 bytes in
length.

The fill chunk is simply a chunk header with a type set to 3.  The invalid byte
count, s\ :sub:`i`, is ignored for fill chunks and should be set to 0
as fill chunks, by definition, contain no valid data.

Note that, due the fact that chunks are always multiples of 2 in size, adjacent
fill chunks can be merged into larger chunks, provided that the combined chunks
remain powers-of-2 in size.
