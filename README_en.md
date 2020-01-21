# znk_project
===================================

## Overview
-----------------------------------

"znk_project" is a repository of free and open source applications and libraries by based on libZnk
developed for Windows, Linux, Cygwin, and Android. 

This repository have following :  
* libZnk : Basically C/C++ library.  
* Moai: Yet another Portable Web-Server and  Local-Proxy Engine.  
  Current version is 2.2.  
* Easter : Easter is a client-viewer system on your virtual browser environment of Moai filters.
  You can also manage your image collections gotten from Image BBS,
* CustomBoy : CustomBoy is the next-generation of USERS(User-Agent Screen-size and Everything Randomizer System)
  based on Moai and build up your virtual browser information instantly.
  You can also create a custom detailed information about each element of browser,
  such as Fingerprint and result values of Javascipt snippet code on Moai CGI user-interface.
...etc  

Japanese Release Note is [here][1]

These applications and librarys source codes are written and developed for VC(Ver6.0-Ver14.0),
MinGW, gcc(on linux), android(ndk) and so on.  

Support site: 
https://mr-moai-2016.github.io

Latest source code:
https://github.com/mr-moai-2016/znk_project/releases/latest


## License
-----------------------------------

zlib 1.2.3 :   
  Copyright (C) 1995-2005 Jean-loup Gailly and Mark Adler  

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.

  [NOTE]  
  libZnk use zlib as statically sub-object.
  zlib in here, its source code itself is not at all modified from its original version,
  but accessory of example and document files are deleted except for the license terms
  (README) to avoid redundancy.

libressl 2.7.3 :   
  LibReSSL files are retained under the copyright of the authors.
  New additions are ISC licensed as per OpenBSD normal licensing policy, or are placed in the public domain. 
  The OpenSSL code is distributed under the terms of the original OpenSSL licenses.

  [NOTE]  
  libRano use libressl as dynamically loadable-module.
  This source code itself is not at all modified from its original version,
  but accessory of example and document files are deleted except for the license terms
  (README) to avoid redundancy.

Otherwise all the code :  
  Copyright (c) Zen-nippon Network Kenkyujo(ZNK)
  Licensed under the NYSL( see http://www.kmonos.net/nysl/index.en.html for detail ).


This software is provided 'as-is', without any express or implied warranty.
In no event will the authors be held liable for any damages arising
from the use of this software.


[1]: https://github.com/mr-moai-2016/znk_project/blob/master/src/ReleaseNote.md
