Steganos LockNote - Self-modifying encrypted notepad
Copyright (C) 2006 Steganos GmbH

Build Notes
-----------
Written by Leonard Ritter, 2006/03/06
Amended by Robert Foertsch, 2010/02/02

At the time of release, LockNote builds with Microsoft Visual Studio .NET 2003
only, but it is not impossible that it builds with other compilers/IDE's as
well.

There is also initial support for the SCons build system, but you need
to manually modify the SConstruct file to get it to work.

The LockNote source has following dependencies:

* CryptoPP, available at http://www.cryptopp.com/.
  The source must reside in the cryptopp subdirectory, and a compiled libfile
named 'cryptopp.lib' must reside in the root folder of the LockNote source.

* AESPHM, a module using the CryptoPP, available at
  http://www.denisbider.com/aesphm.zip
 
* WTL, available at http://wtl.sourceforge.net/.
  The WTL include files must be available through the global path settings.

The release build will compile with warnings related to exception handling.
Exception handling is not important for stable execution, but a requirement of
the CryptoPP library.

History
-------

* 1.0.4, 2007/04/05:
	- NEW: Added dutch translation
	- FIX: Path fix for cryptopp.lib in project

* 1.0.3, 2006/03/06:
	- NEW: Added spanish translation (Castilian)

* 1.0.2, 2006/02/14:
	- NEW: Added french translation
	- NEW: Added 'Save As...' option

* 1.0.1, 2006/01/16:
	- FIX: Removed 30k character limit
	- NEW: Conversion displays a summary message box
	- FIX: Wrong filename displayed when conversion fails, fixed
	- NEW: Made conversion error message more comprehensible
	- FIX: Made case insensitive search work
	- FIX: Filenames are no longer being converted to lowercase
	- FIX: Document was touched even when closing without changes
	- NEW: Dragging files on program icon converts files as well

* 1.0, 2006/01/06: Initial release.


License
-------

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

