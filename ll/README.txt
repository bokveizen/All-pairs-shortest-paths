--------------------------------------------------------------------------------------

This is the Leonardo Library (LL), a C cross-platform program development toolkit. 
The LL is developed as part of the Leonardo Computing Environment project 
(see http://www.leonardo-vm.org).

--------------------------------------------------------------------------------------
OVERVIEW: 

The LL provides a well-documented collection of C components with a clean and modular 
interface and lets application developers target different operating systems with a 
single application source code. An LL component is a coherent collection of functions, 
types, and constants targeted to a specific task.

The library covers a large number of tasks including graphic user interface, process, 
thread, I/O and memory management, and contains components with fundamental algorithms 
and data structures, as well as general-purpose utility functions. 

--------------------------------------------------------------------------------------
REPOSITORY STRUCTURE:

docs +- html:         html documentation sheets for each component
include:              C headers of the library
src +- core + common: portable core C code, built on top of other LL components 
    |       + posix:  core C code based on posix
    |       + win32:  core C code based on win32
    +- gui  + qt:     gui C++ code based on qt
            + win32:  gui C code based on win32
examples:             demo applications based on LL

--------------------------------------------------------------------------------------
BUILDING THE LIBRARY:

To compile the library, you will need to perform a few steps, depending on the target 
platform. Currently, the gui part of the library is only tested in Windows and Linux 
platforms. The core part has been tested also in MacOS (Classic/MacOS X), and Solaris.

* Building LL for Windows:
1. Open file include/LConfig.h and set macro name __LSL_PLATFORM__ to __LSL_Win32_x86__:
   #define __LSL_PLATFORM__ __LSL_Win32_x86__
2. Create a new C-gui project and make sure that the following libraries are included:
   - Comdgl32.lib
   - Gdi32.lib
   - Kernel32.lib
   - User32.lib
   - Comctl32.lib
   - Shell32.lib
3. Include in your project:
   - examples/main_gui.c (or any other example file)
   - all files in src and include (include should also be set as an "additional include      directory")

* Building LL for Linux:
1. To compile the library, use "make qt" (object code is built in bin). The Makefile 
   assumes that the qt library is correctly installed in /usr/local/qt
   To compile the test application, use "make testqt" (executable code is built in bin)
2. To compile only the core part, use "make linux"

--------------------------------------------------------------------------------------
DOCUMENTATION:
Technical sheets for stable components are available in the docs/html directory.

--------------------------------------------------------------------------------------
LICENSE:
The LL is distributed under the GNU Lesser General Public License: 
http://www.gnu.org/licenses/lgpl.html

--------------------------------------------------------------------------------------
CONTACT: 
Please send feedback, comments and questions to: 
<feedback@leonardo-vm.org>

--------------------------------------------------------------------------------------
The Leonardo Computing Environment Team,
May 2003

