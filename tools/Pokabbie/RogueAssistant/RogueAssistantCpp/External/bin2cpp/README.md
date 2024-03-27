![bin2cpp logo](docs/bin2cpp-splashscreen.png)

# bin2cpp

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Github Releases](https://img.shields.io/github/release/end2endzone/bin2cpp.svg)](https://github.com/end2endzone/bin2cpp/releases)

bin2cpp is a command line tool for embedding small files (like images, icons or raw data files) into a C++ executable.

When executed, bin2cpp takes binary file as input and outputs c++ code (a function) that when called allows a c++ program to retrieve the content of the input binary file.



## Status

Build:

| Service/Platform    | Build                                                                                                                                                                                   | Tests                                                                                                                                                                                                                                                  |
| ------------------- | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ |
| AppVeyor            | [![Build status](https://img.shields.io/appveyor/ci/end2endzone/bin2cpp/master.svg?logo=AppVeyor&logoColor=white)](https://ci.appveyor.com/project/end2endzone/bin2cpp)                 | [![Tests status](https://img.shields.io/appveyor/tests/end2endzone/bin2cpp/master.svg?logo=AppVeyor&logoColor=white)](https://ci.appveyor.com/project/end2endzone/bin2cpp/branch/master/tests)                                                         |
| Travis CI           | [![Build Status](https://img.shields.io/travis/end2endzone/bin2cpp/master.svg?logo=Travis-CI&style=flat&logoColor=white)](https://travis-ci.org/end2endzone/bin2cpp)                    |                                                                                                                                                                                                                                                        |
| Windows Server 2019 | [![Build on Windows](https://github.com/end2endzone/bin2cpp/actions/workflows/build_windows.yml/badge.svg)](https://github.com/end2endzone/bin2cpp/actions/workflows/build_windows.yml) | [![Tests on Windows](https://img.shields.io/endpoint?url=https://gist.githubusercontent.com/end2endzone/58cf6c72c08e706335337d5ef9ca48e8/raw/bin2cpp.master.Windows.json)](https://github.com/end2endzone/bin2cpp/actions/workflows/build_windows.yml) |
| Ubuntu 20.04        | [![Build on Linux](https://github.com/end2endzone/bin2cpp/actions/workflows/build_linux.yml/badge.svg)](https://github.com/end2endzone/bin2cpp/actions/workflows/build_linux.yml)       | [![Tests on Linux](https://img.shields.io/endpoint?url=https://gist.githubusercontent.com/end2endzone/58cf6c72c08e706335337d5ef9ca48e8/raw/bin2cpp.master.Linux.json)](https://github.com/end2endzone/bin2cpp/actions/workflows/build_linux.yml)       |
| macOS 10.15         | [![Build on macOS](https://github.com/end2endzone/bin2cpp/actions/workflows/build_macos.yml/badge.svg)](https://github.com/end2endzone/bin2cpp/actions/workflows/build_macos.yml)       | [![Tests on macOS](https://img.shields.io/endpoint?url=https://gist.githubusercontent.com/end2endzone/58cf6c72c08e706335337d5ef9ca48e8/raw/bin2cpp.master.macOS.json)](https://github.com/end2endzone/bin2cpp/actions/workflows/build_macos.yml)       |

Statistics:

| AppVeyor                                                                                                                                       | Travic CI                                                                                                              | GitHub                                                                                                                     |
| ---------------------------------------------------------------------------------------------------------------------------------------------- | ---------------------------------------------------------------------------------------------------------------------- | -------------------------------------------------------------------------------------------------------------------------- |
| [![Statistics](https://buildstats.info/appveyor/chart/end2endzone/bin2cpp)](https://ci.appveyor.com/project/end2endzone/bin2cpp/branch/master) | [![Statistics](https://buildstats.info/travisci/chart/end2endzone/bin2cpp)](https://travis-ci.org/end2endzone/bin2cpp) | [![Statistics](https://buildstats.info/github/chart/end2endzone/bin2cpp)](https://github.com/end2endzone/bin2cpp/actions/) |




# Purpose

bin2cpp is not implemented using [executable resources](http://en.wikipedia.org/wiki/Resource_(Windows)). Instead, bin2cpp creates a single function call for retrieving the content and the properties of a file which makes it harder to steal the executable's content. It also makes it even harder to replace an existing resource of the executable.

It is designed to be easy to use by developers and to provide easy call functions to get access to the data of the embedded files.

The generated functions that reads and extracts the embedded content does not rely on external libraries so you don't need to setup your projects to use any third party library to start using bin2cpp. All your embedded data can be accessed right away.




# Features

The main features of the project are:

* Easily converts small files as C++ source code for embedding into a C++ executable.
* Access content with a unique function call for each embedded file.
* Supports multiple embedded files at once.
* Keep the directory structure when embedding directories.
* Supports encoding and extracting files with a custom directory structure.
* Makes it harder for resource hacker to modify or steal the embedded files.
* No third party libraries required for retrieving the data of the embedded files.
* Supports different types of code generator: string, segment, array, win32 resources.
* File's originals `size`, `filename` and `directory` properties available from generated source code.
* Control generated source code: choose your custom _File_ interface and namespace.
* Print a file encoded content to stdout. Useful for scripts and integration with third party application.
* Generated code is C++98 standard-compliant.



## Use cases

The following list show situations where bin2cpp is useful:

* Embedding default configuration files if none are provided.
* Embedding GLSL shaders into the executable.
* Embedding text or XML databases (gui layout, names, dictionaries or localization strings)
* A portable alternative to C++11 raw string literals.
* Prevent stealing copyrighted resources.
* Embedding images/icons/sounds for a GUI application. The application executable can be shipped/copied as a single file. 
* Embedding a long PL/SQL code string.
* Allowing an executable to be downloaded from an intranet server as a single file.
* Distributing an application without an installer package. All configurations files and resources can be embedded and extracted at first launch of the application.




# Usage

The following section shows how to use bin2cpp with code examples:



## Command Line Usage

```
bin2cpp --file=<path> --output=<path> [--headerfile=<name>] [--identifier=<name>]
        [--generator=<name>] [--encoding=<name>] [--chunksize=<value>] [--namespace=<value>]
        [--baseclass=<name>] [--managerfile=<name>] [--registerfile] 
        [--reportedfilepath=<value>] [--override] [--noheader] [--quiet]
bin2cpp --dir=<path> --output=<path> [--keepdirs]
        [--generator=<name>] [--encoding=<name>] [--chunksize=<value>] [--namespace=<value>]
        [--baseclass=<name>] [--managerfile=<name>] [--registerfile] 
        [--override] [--noheader] [--quiet]
bin2cpp --help
bin2cpp --version
```

| Argument                        | Description                                                                                                                                                                                                                                                                         |
|---------------------------------|-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| --help                          | Display this help message.                                                                                                                                                                                                                                                          |
| --version                       | Display this application version.                                                                                                                                                                                                                                                   |
| --file=&lt;path&gt;             | Path of the input file used for embedding as C++ source code.                                                                                                                                                                                                                       |
| --dir=&lt;path&gt;              | Path of the input directory used for embedding all files of the directory as C++ source code.                                                                                                                                                                                       |
| --output=&lt;path&gt;           | Path of the output directory where to create generated code.<br>ie: ./generated_files                                                                                                                                                                                               |
| --headerfile=&lt;name&gt;       | File name or relative path of the generated C++ header file. If a relative path from the output directory is specified, the #include statement in the generated cpp file will match the relative path. <br>ie: SplashScreen.h<br>Default value: input file name (without extension) |
| --identifier=&lt;name&gt;       | Identifier of the function name that is used to get an instance of the file.<br>ie: SplashScreen<br>Default value is based on input file with format 'NameExt'.                                                                                                                     |
| --generator=&lt;name&gt;        | Name of the generator to use. Possible values are 'segment', 'string', 'array' and 'win32'.<br>[default: segment]                                                                                                                                                                   |
| --encoding=&lt;name&gt;         | Name of the binary to string literal encoding to use. Possible values are 'oct' and 'hex'.<br>[default: oct]                                                                                                                                                                        |
| --chunksize=&lt;value&gt;       | Size in bytes of each string segments (bytes per LoC).<br>[default: 200]                                                                                                                                                                                                            |
| --baseclass=&lt;name&gt;        | The name of the interface for embedded files.<br>[default: File]                                                                                                                                                                                                                    |
| --namespace=&lt;name&gt;        | The namespace of the generated source code.<br>[default: bin2cpp]                                                                                                                                                                                                                   |
| --reportedfilepath=&lt;path&gt; | The relative reported path of the File. Path returned when calling method getFilePath() of the File class. Automatically calculated when --dir mode is used.<br>ie: images/DCIM/IMG_0001.jpg                                                                                        |
| --managerfile=&lt;path&gt;      | File name or relative path of the generated C++ header file for the FileManager class.<br>ie: FileManager.h.                                                                                                                                                                        |
| --registerfile                  | Register the generated file to the FileManager class. This flags is automatically set when parameter 'managerfile' is specified.                                                                                                                                                    |
| --keepdirs                      | Keep the directory structure. Forces the output files to have the same directory structure as the input files. Valid only when --dir is used.                                                                                                                                       |
| --plainoutput                   | Print the encoded string in plain format to stdout. Useful for scripts and integration with third party application.                                                                                                                                                                |
| --override                      | Tells bin2cpp to overwrite the destination files.                                                                                                                                                                                                                                   |
| --noheader                      | Do not print program header to standard output.                                                                                                                                                                                                                                     |
| --quiet                         | Do not log any message to standard output.                                                                                                                                                                                                                                          |



## Example 1 - single file

This example shows how to use bin2cpp to convert a single html file to c++ source code.


### Input file: helloworld.html

```html
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1, user-scalable=yes">
  <title>Hello World!</title>
</head>
<body>
Hello World!
</body>
</html>
```


### Command:

```
bin2cpp.exe --file=helloworld.html --output=.\outdir
```


### Console output

```
bin2cpp v3.0.0 - Convert binary files into C++ source code.
Copyright (C) 2013-2021 end2endzone.com. All rights reserved.
bin2cpp is open source software, see http://github.com/end2endzone/bin2cpp
Embedding "helloworld.html"...
Writing file ".\outdir\helloworld.h"...
Writing file ".\outdir\helloworld.cpp"...
```


### Output file: helloworld.h

```cpp
/**
 * This file was generated by bin2cpp v3.0.0
 * Copyright (C) 2013-2021 end2endzone.com. All rights reserved.
 * bin2cpp is open source software, see http://github.com/end2endzone/bin2cpp
 * Source code for file 'helloworld.html', last modified 1548537787.
 * Do not modify this file.
 */
#ifndef HELLOWORLD_H
#define HELLOWORLD_H

#include <stddef.h>

namespace bin2cpp
{
  #ifndef BIN2CPP_EMBEDDEDFILE_CLASS
  #define BIN2CPP_EMBEDDEDFILE_CLASS
  class File
  {
  public:
    virtual size_t getSize() const = 0;
    virtual const char * getFileName() const = 0;
    virtual const char * getFilePath() const = 0;
    virtual const char * getBuffer() const = 0;
    virtual bool save(const char * filename) const = 0;
  };
  #endif //BIN2CPP_EMBEDDEDFILE_CLASS
  const File & getHelloworldHtmlFile();
}; //bin2cpp

#endif //HELLOWORLD_H
```


### Output file: helloworld.cpp

```cpp
/**
 * This file was generated by bin2cpp v3.0.0
 * Copyright (C) 2013-2021 end2endzone.com. All rights reserved.
 * bin2cpp is open source software, see http://github.com/end2endzone/bin2cpp
 * Source code for file 'helloworld.html', last modified 1548537787.
 * Do not modify this file.
 */
#if defined(_WIN32) && !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#endif
#include "helloworld.h"
#include <string> //for std::string
#include <iostream>
#include <fstream>  //for ofstream
namespace bin2cpp
{
  class HelloworldHtmlFile : public virtual bin2cpp::File
  {
  public:
    HelloworldHtmlFile() { build(); }
    virtual ~HelloworldHtmlFile() {}
    virtual size_t getSize() const { return 238; }
    virtual const char * getFileName() const { return "helloworld.html"; }
    virtual const char * getFilePath() const { return getFileName(); }
    virtual const char * getBuffer() const { return mBuffer.c_str(); }
    void build()
    {
      mBuffer.clear();
      mBuffer.reserve(getSize()); //allocate all required memory at once to prevent reallocations
      mBuffer.append("<!DOCTYPE html>\r\n<html lang=\"en\">\r\n<head>\r\n  <meta charset=\"utf-8\">\r\n  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=yes\">\r\n  <title>Hello World!</title>\r\n</head>\r\n", 200);
      mBuffer.append("<body>\r\nHello World!\r\n</body>\r\n</html>", 38);
    }
    virtual bool save(const char * filename) const
    {
      std::ofstream f(filename, std::ios::out | std::ios::binary | std::ios::trunc);
      if (f.fail()) return false;
      size_t fileSize = getSize();
      const char * buffer = getBuffer();
      f.write(buffer, fileSize);
      f.close();
      return true;
    }
  private:
    std::string mBuffer;
  };
  const File & getHelloworldHtmlFile() { static HelloworldHtmlFile _instance; return _instance; }
}; //bin2cpp
```


### Code sample (querying the generated code)

At runtime, show file properties and save/export data back to a file.

```cpp
#include <stdio.h>
#include <string>
#include <iostream>

#include "helloworld.h" //a single include file is all you need

int main(int argc, char* argv[])
{
  //get a reference to the embedded file
  const bin2cpp::File & resource = bin2cpp::getHelloworldhtmlFile();

  //print information about the file.
  std::cout << "Embedded file '" << resource.getFileName() << "' is " << resource.getSize() << " bytes long.\n";

  //Saving content back to a file.
  std::cout << " Saving embedded file to 'helloworld_copy.html'...\n";
  bool saved = resource.save("helloworld_copy.html");
  if (saved)
    std::cout << "saved\n";
  else
    std::cout << "failed\n";

  //Get the internal buffer and do something with the binary data
  const char * buffer = resource.getBuffer();
  size_t bufferSize = resource.getSize();
  //...
  
  return 0;
}
```



## Example 2 - directory

This example shows how to use bin2cpp to convert multiple files of the same directory to c++ source code.


### Input directory: [samples/demo_icons/flat-color-icons](samples/demo_icons/flat-color-icons).

The [samples/demo_icons/flat-color-icons](samples/demo_icons/flat-color-icons) directory contains the following 56 icons :

![Flat Color Icons by Icons8](docs/Flat%20Color%20Icons%20by%20Icons8.png)

These icons are from the *Very Basic* set of [Icons8 Flat Color Icons](https://github.com/icons8/flat-color-icons) and are licensed under the [Good Boy License](https://icons8.com/good-boy-license).


### Command:

```
bin2cpp.exe --dir=flat-color-icons --managerfile=IconsFileManager.h --output=.\outdir --chunksize=50
```


### Console output

```
bin2cpp v3.0.0 - Convert binary files into C++ source code.
Copyright (C) 2013-2021 end2endzone.com. All rights reserved.
bin2cpp is open source software, see http://github.com/end2endzone/bin2cpp
Embedding "flat-color-icons\about.png" using chunks of 50 bytes...
Writing file ".\outdir\about.h"...
Writing file ".\outdir\about.cpp"...
Embedding "flat-color-icons\address_book.png" using chunks of 50 bytes...
Writing file ".\outdir\address_book.h"...
Writing file ".\outdir\address_book.cpp"...
...
Embedding "flat-color-icons\video_file.png" using chunks of 50 bytes...
Writing file ".\outdir\video_file.h"...
Writing file ".\outdir\video_file.cpp"...
Generating "IconsFileManager.h"...
Writing file ".\outdir\IconsFileManager.h"...
Writing file ".\outdir\IconsFileManager.cpp"...
```

Notice that additional files `IconsFileManager.h` and `IconsFileManager.cpp` were also generated and will allow retreiving all files at once.


### Code sample (querying the generated code)

At runtime, show a file listing and save/export the icons files in users temporary directory.

```cpp
#include <stdio.h>  // printf
#include <stdlib.h> // getenv
#include <iostream>

#include "IconsFileManager.h"

int main(int argc, char* argv[])
{
  bin2cpp::FileManager & mgr = bin2cpp::FileManager::getInstance();

  //Print information about all files generated with "--managerfile" or --registerfile flags.
  size_t num_files = mgr.getFileCount();
  std::cout << "Found " << num_files << " embedded icons...\n";

  //Listing files.
  for(size_t i=0; i<num_files; i++)
  {
    const bin2cpp::File * file = mgr.getFile(i);
    std::cout << "  File '" << file->getFileName() << "', " << file->getSize() << " bytes\n";
  }

  //Saving content back to files.
  const char * temp_dir = getenv("TEMP");
  std::cout << "Saving embedded icons to directory '" << temp_dir << "'...\n";
  bool saved = mgr.saveFiles(temp_dir);
  if (saved)
    std::cout << "saved\n";
  else
    std::cout << "failed\n";

  return 0;
}
```


### Console output

```
Found 56 embedded icons...
  File 'about.png', 520 bytes
  File 'address_book.png', 1412 bytes
  File 'alarm_clock.png', 2334 bytes
  File 'audio_file.png', 523 bytes
  File 'binoculars.png', 1347 bytes
  File 'bookmark.png', 478 bytes
  File 'broken_link.png', 1522 bytes
  File 'calculator.png', 574 bytes
  File 'calendar.png', 574 bytes
  File 'cancel.png', 1159 bytes
  File 'checkmark.png', 666 bytes
  File 'clock.png', 1534 bytes
  File 'contacts.png', 626 bytes
  ...
  File 'support.png', 1292 bytes
  File 'synchronize.png', 1269 bytes
  File 'unlock.png', 718 bytes
  File 'upload.png', 467 bytes
  File 'video_file.png', 472 bytes
Saving embedded icons to directory 'C:\Users\foobar\AppData\Local\Temp'...
saved
```



## Example 3 - relative header file (project's *include* directory)

This example shows how to use bin2cpp to generate files in a relative sub directory based on the project's root directory.

Many projects has an *include* directory located at the project's root directory. These projects are usually configured to add *$ROOT_DIR/include* to the list of include directories. bin2cpp can be configured to take advantage of this situation.

If you output files in the *include* directory (with the command  `--output=$ROOT_DIR/include --headerfile=foo.h`), the generated `#include "foo.h"` statement will have no problem finding the generated header. However if you want the generated files to be in a sub directory (for example with the command  `--output=$ROOT_DIR/include/bin2cpp`), this won't work since you would need the sub directory path in the include statement.

The solution is to specify a *relative path* for the header file which also changes the generated `#include` statement. For example, the command  `--output=$ROOT_DIR/include --headerfile=bin2cpp/foo.h`), generates `#include "bin2cpp/foo.h"` which is perfect since `$ROOT_DIR/include` is already an include directory.

This example shows how to specify a relative path for the header file.



### Command:

```
bin2cpp.exe --file=res/icon.ico --output=include --headerfile=bin2cpp/file_icon.h --identifier=icon  
```

Note the *bin2cpp* sub directory in `--headerfile=bin2cpp/file_icon.h`. The output sub directory must already exists.



### Console output

```
bin2cpp v3.0.0 - Convert binary files into C++ source code.
Copyright (C) 2013-2021 end2endzone.com. All rights reserved.
bin2cpp is open source software, see http://github.com/end2endzone/bin2cpp
Embedding "res/icon.ico"...
Writing file "include/bin2cpp/file_icon.h"...
Writing file "include/bin2cpp/file_icon.cpp"...
```



### Output file: file_icon.cpp

Here are the first few generated lines of file `file_icon.cpp`.

```cpp
/**
 * This file was generated by bin2cpp v3.0.0
 * Copyright (C) 2013-2021 end2endzone.com. All rights reserved.
 * bin2cpp is open source software, see http://github.com/end2endzone/bin2cpp
 * Source code for file 'icon.ico', last modified 1548537787.
 * Do not modify this file.
 */
#if defined(_WIN32) && !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#endif
#include "bin2cpp/file_icon.h"
#include <string> //for std::string
#include <iostream>
#include <fstream>  //for ofstream
...
```

Note the `#include "bin2cpp/file_icon.h"` statement which is relative to the project's *include* directory.



## Example 4 - web site (embedding directory structure)

This example shows how to use bin2cpp to convert files from multiple directories to c++ source code. The input directory structure and file location are preserved.


### Input directory: [samples/demo_website/www](samples/demo_website/www).

The [samples/demo_website/www](samples/demo_website/www) directory contains web pages in the following directory structure :

```
www
├── blog
│   ├── how-to-create-a-web-site
│   │   └── index.html
│   ├── index.html
│   └── using-bin2cpp
│       └── index.html
├── contact
│   └── index.html
├── home
│   └── index.html
└── index.html
```

The directories above contains multiple files named `index.html`.

bin2cpp can create unique identifiers for each files. In case of duplicate identifiers, bin2cpp appends a counter that increases by 1 on every duplicate. The pattern `_<counter>` is added to the end where `<counter>` is the next counter value. For example, the files above create the following identifiers:
 * Indexhtml
 * Indexhtml_1
 * Indexhtml_2
 * Indexhtml_3
 * Indexhtml_4
 * Indexhtml_5

The same strategy is implemented for duplicate file names.


### Command:

```
bin2cpp.exe --dir=www --managerfile=PagesFileManager.h --namespace=www --output=.\outdir --chunksize=50 --keepdirs
```

Note the `--keepdirs` command line option which keep the input directory structure and allows the output files to have the same directory structure as the input files. This prevents duplicate file names.


### Console output

```
bin2cpp v3.0.0 - Convert binary files into C++ source code.
Copyright (C) 2013-2021 end2endzone.com. All rights reserved.
bin2cpp is open source software, see http://github.com/end2endzone/bin2cpp
Embedding "www\blog\how-to-create-a-web-site\index.html" using chunks of 50 bytes...
Creating directory ".\outdir\blog\how-to-create-a-web-site"...
Writing file ".\outdir\blog\how-to-create-a-web-site\index.h"...
Writing file ".\outdir\blog\how-to-create-a-web-site\index.cpp"...
Embedding "www\blog\index.html" using chunks of 50 bytes...
Writing file ".\outdir\blog\index.h"...
Writing file ".\outdir\blog\index.cpp"...
Embedding "www\blog\using-bin2cpp\index.html" using chunks of 50 bytes...
Creating directory ".\outdir\blog\using-bin2cpp"...
Writing file ".\outdir\blog\using-bin2cpp\index.h"...
Writing file ".\outdir\blog\using-bin2cpp\index.cpp"...
Embedding "www\contact\index.html" using chunks of 50 bytes...
Creating directory ".\outdir\contact"...
Writing file ".\outdir\contact\index.h"...
Writing file ".\outdir\contact\index.cpp"...
Embedding "www\home\index.html" using chunks of 50 bytes...
Creating directory ".\outdir\home"...
Writing file ".\outdir\home\index.h"...
Writing file ".\outdir\home\index.cpp"...
Embedding "www\index.html" using chunks of 50 bytes...
Writing file ".\outdir\index.h"...
Writing file ".\outdir\index.cpp"...
Generating "PagesFileManager.h"...
Writing file ".\outdir\PagesFileManager.h"...
Writing file ".\outdir\PagesFileManager.cpp"...
```

Notice that files `PagesFileManager.h` and `PagesFileManager.cpp` were also generated. They provide support for extracting the content of the embedded ***www*** directory.


### Code sample (querying the generated code)

At runtime, show a file listing and save/export all `index.html` files in the right directories.

```cpp
#include <stdio.h>  // printf
#include <stdlib.h> // getenv
#include <iostream>
#include <string>

#include "PagesFileManager.h"

int main(int argc, char* argv[])
{
  www::FileManager & mgr = www::FileManager::getInstance();

  //Print information about all files generated with "--managerfile" or --registerfile flags.
  size_t num_files = mgr.getFileCount();
  std::cout << "Found " << num_files << " embedded web pages...\n";

  //Listing files.
  for(size_t i=0; i<num_files; i++)
  {
    const www::File * file = mgr.getFile(i);
    std::cout << "  File '" << file->getFilePath() << "', " << file->getSize() << " bytes\n";
  }

  //Saving content back to files preserving the original directory structure.
  std::string temp_dir = getenv("TEMP");
#ifdef _Win32
  temp_dir += "\\";
#else
  temp_dir += "/";
#endif
  temp_dir += "www";
  std::cout << "Saving embedded web pages to directory '" << temp_dir << "'...\n";
  bool saved = mgr.saveFiles(temp_dir.c_str());
  if (saved)
    std::cout << "saved\n";
  else
    std::cout << "failed\n";

  return 0;
}
```


### Console output

```
Found 6 embedded web pages...
  File 'index.html', 241 bytes
  File 'blog\index.html', 543 bytes
  File 'blog\using-bin2cpp\index.html', 4332 bytes
  File 'blog\how-to-create-a-web-site\index.html', 3645 bytes
  File 'contact\index.html', 2375 bytes
  File 'home\index.html', 1422 bytes
Saving embedded web pages to directory 'C:\Users\foobar\AppData\Local\Temp\www'...
saved
```

The executed code above has extracted the files above with the following directory structure:

```
C:\Users\username\AppData\Local\Temp\www
├── blog
│   ├── how-to-create-a-web-site
│   │   └── index.html
│   ├── index.html
│   └── using-bin2cpp
│       └── index.html
├── contact
│   └── index.html
├── home
│   └── index.html
└── index.html
```



## Screenshots

[![bin2cpp v2.4.0 Sample](docs/bin2cpp-v2.4.0-sample.png)](docs/bin2cpp-v2.4.0-sample.png)

bin2cpp v2.4.0 Sample

[![Demo extraction sample](docs/demo-extraction-sample.png)](docs/demo-extraction-sample.png)

Demo extraction sample




# Build

Please refer to file [INSTALL.md](INSTALL.md) for details on how installing/building the application.




# Platform

bin2cpp has been tested with the following platform:

* Linux, 32 and 64 bit
* Windows, 32 and 64 bit
* macOS, 32 and 64 bit




# Versioning

We use [Semantic Versioning 2.0.0](http://semver.org/) for versioning. For the versions available, see the [tags on this repository](https://github.com/end2endzone/bin2cpp/tags).




# Authors

* **Antoine Beauchamp** - *Initial work* - [end2endzone](https://github.com/end2endzone)

See also the list of [contributors](https://github.com/end2endzone/bin2cpp/blob/master/AUTHORS) who participated in this project.




# License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details
