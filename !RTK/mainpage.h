/** @mainpage The RISC OS Toolkit
 * @section intro Introduction
 * The RISC OS Toolkit (RTK) is a class library for developing RISC OS
 * application programs in C++.  It differs from other such libraries
 * currently available for RISC OS in its support for automatic layout.
 * This is achieved by specifying the relationship between different
 * visual components (for example, the fact that they are arranged in
 * a grid or a column).  The precise coordinates are calculated by the
 * RTK.  This eliminates the need for a template editor, and allows
 * the layout to change at run-time to accommodate varying content.
 *
 * (Support will be provided for RISC OS template files, but that is
 * not intended to be the normal method of use.)
 *
 * Other goals are:
 * - to provide high-level support for common window types, icon types
 *   and protocols;
 * - to provide an extensible, type-safe event delivery mechanism.
 * - to implement style-guide compliant behaviour by default.
 *
 * Please be aware that the RTK is currently at an early stage of
 * development.  Most of the basic infrastructure is complete, but
 * many important classes have not been written.  Some functions are
 * correct but not efficient.
 * @section requirements Requirements
 * Supported operating systems include:
 * - RISC OS 4.02
 * - RISC OS 5.03
 *
 * The RTK may work with earlier versions of RISC OS provided that the
 * nested window manager is present.
 *
 * Supported compilers include:
 * - GCC 2.95.4 (release 3)
 * - GCC 2.95.4 (experimental 32-bit, 05 May 2003)
 *
 * The Acorn C/C++ compiler (up to version 3.19 at least) is not
 * supported because there are important parts of the C++ language
 * that it does not implement.
 * @section installation Installation
 * The !RTK application directory can be located anywhere.
 *
 * Ensure that it has been booted and/or run before attempting to
 * compile or link against it.
 *
 * (If two or more versions are present, running !RTK will change
 * the active version whereas booting it will not.)
 * @section use Use
 * When compiling, append -Irtk: to the GCC command line.  This
 * adds rtk: to the list of paths that are searched for include
 * files.
 *
 * When linking, append -lrtk:a.rtk to the GCC command line.
 * This adds rtk:a.rtk to the list of libraries searched by the
 * linker.
 *
 * Include directives should be of the form:
 * @code
 * #include "rtk/desktop/application.h"
 * @endcode
 * Be aware that all RTK classes are declared within namespaces.
 * You must either specify the namespace expicitly:
 * @code
 * rtk::desktop::application
 * @endcode
 * or place a using directive at the start of each source file:
 * @code
 * using namespace rtk::desktop
 * @endcode
 * The main() function is normally of the form:
 * @code
 * int main(void)
 * {
 *   myapp app;
 *   app.run();
 *   return 0;
 * }
 * @endcode
 * where myapp is a class derived from rtk::desktop::application.
 *
 * Documentation available from the RTK web site currently includes:
 * - the RTK reference manual
 * - a number of example programs
 * @section building Building from Source
 * To build the RTK from souce code the following tools must be present
 * on the Run$Path:
 * - make
 * - gcc
 * - libfile
 * - fixdeps
 * Suitable versions of make, gcc and libfile may be found within
 * supported distributions of GCC.  fixdeps may be obtained from the
 * RTK web site.
 *
 * No configuration is necessary.  Change the current directory to !RTK
 * then issue the command:
 * @code
 * make all
 * @endcode
 * Please note that some difficulties have been encountered when using
 * recursive makefiles with the experimental 32-bit version of make.
 * They can be overcome by executing the makefiles individually from
 * an obey file.
 * @section support Support
 * The RTK home page may be found at:
 *
 * http://www.sagitta.demon.co.uk/rtk/
 *
 * There is also a mailing list.  To subscribe, send an empty message
 * to:
 *
 * mailto:rtk-request@sagitta.demon.co.uk
 *
 * with a subject of "subscribe".
 * @todo an absolute layout class
 * @todo a template layout class
 * @todo virtual row, column and grid layout classes
 * @todo a card layout class
 * @todo a discard/cancel/save dialogue box class
 * @todo a save-as dialogue box class
 */
