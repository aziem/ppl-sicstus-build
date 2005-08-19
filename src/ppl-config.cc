/* A program to enquire about the configuration of the PPL
   and of the applications using it.
   Copyright (C) 2001-2005 Roberto Bagnara <bagnara@cs.unipr.it>

This file is part of the Parma Polyhedra Library (PPL).

The PPL is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 2 of the License, or (at your
option) any later version.

The PPL is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
USA.

For the most up-to-date information see the Parma Polyhedra Library
site: http://www.cs.unipr.it/ppl/ . */

#include "ppl_install.hh"

namespace PPL = Parma_Polyhedra_Library;

#if PPL_VERSION_MAJOR == 0 && PPL_VERSION_MINOR < 8
#error "PPL version 0.8 or following is required"
#endif

#include <cstdarg>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <stdexcept>

#ifdef HAVE_GETOPT_H
#include <getopt.h>
#endif

#ifdef HAVE_UNISTD_H
// Include this for `getopt()': especially important if we do not have
// <getopt.h>.
# include <unistd.h>
#endif

namespace {

enum Format {
  PLAIN,
  MAKEFILE,
  SH,
  CSH
};

enum Interface {
  CXX,
  C,
  CIAO_PROLOG,
  GNU_PROLOG,
  SICSTUS_PROLOG,
  SWI_PROLOG,
  XSB_PROLOG,
  YAP_PROLOG,
  OCAML,
  JAVA
};

Format required_format = PLAIN;

Interface required_interface = CXX;

const char* variable_prefix = "PPL_";

#define PPL_LICENSE "GNU GENERAL PUBLIC LICENSE, Version 2"

const char* prefix = PREFIX;
const char* exec_prefix = EXEC_PREFIX;

bool required_application = false;
bool required_library = false;
bool required_prefix = false;
bool required_exec_prefix = false;
bool required_configure_options = false;
bool required_version = false;
bool required_version_major = false;
bool required_version_minor = false;
bool required_version_revision = false;
bool required_version_beta = false;
bool required_banner = false;
bool required_libs = false;
bool required_includes = false;
bool required_cppflags = false;
bool required_cflags = false;
bool required_cxxflags = false;
bool required_ldflags = false;
bool required_license = false;
bool required_copying = false;
bool required_bugs = false;
bool required_credits = false;
bool required_all = false;

unsigned num_required_items = 0;

#ifdef HAVE_GETOPT_H
struct option long_options[] = {
  {"format",            required_argument, 0, 'f'},
  {"interface",         required_argument, 0, 'i'},
  {"application",       no_argument,       0, 'a'},
  {"library",           no_argument,       0, 'l'},
  {"prefix",            no_argument,       0, 'p'},
  {"exec-prefix",       no_argument,       0, 'e'},
  {"configure-options", no_argument,       0, 'O'},
  {"version",           no_argument,       0, 'V'},
  {"version-major",     no_argument,       0, 'M'},
  {"version-minor",     no_argument,       0, 'N'},
  {"version-revision",  no_argument,       0, 'R'},
  {"version-beta",      no_argument,       0, 'B'},
  {"banner",            no_argument,       0, 'E'},
  {"libs",              no_argument,       0, 'L'},
  {"includes",          no_argument,       0, 'I'},
  {"cppflags",          no_argument,       0, 'P'},
  {"cflags",            no_argument,       0, 'C'},
  {"cxxflags",          no_argument,       0, 'X'},
  {"ldflags",           no_argument,       0, 'D'},
  {"license",           no_argument,       0, 'n'},
  {"copying",           no_argument,       0, 'c'},
  {"bugs",              no_argument,       0, 'b'},
  {"credits",           no_argument,       0, 'r'},
  {"all",               no_argument,       0, 'A'},
  {0, 0, 0, 0}
};
#endif

static const char* usage_string
= "Usage: %s [OPTION]... [FILE]...\n\n"
"  -CSECS, --max-cpu=SECS  limits CPU usage to SECS seconds\n"
"  -VMB, --max-memory=MB   limits memory usage to MB megabytes\n"
"  -h, --help              prints this help text to stderr\n"
"  -oPATH, --output=PATH   appends output to PATH\n"
"  -t, --timings           prints timings to stderr\n"
"  -v, --verbose           produces lots of output\n"
#if defined(USE_PPL)
"  -cPATH, --check=PATH    checks if the result is equal to what is in PATH\n"
#endif
#ifndef HAVE_GETOPT_H
"\n"
"NOTE: this version does not support long options.\n"
#endif
;

#define OPTION_LETTERS "laf:peOVMNRBELIPCXDncbrA"

const char* program_name = 0;

void
fatal(const char* format, ...) {
  va_list ap;
  va_start(ap, format);
  fprintf(stderr, "%s: ", program_name);
  vfprintf(stderr, format, ap);
  fprintf(stderr, "\n");
  va_end(ap);
  exit(1);
}

void
warning(const char* format, ...) {
  va_list ap;
  va_start(ap, format);
  fprintf(stderr, "%s: Warning: ", program_name);
  vfprintf(stderr, format, ap);
  fprintf(stderr, "\n");
  va_end(ap);
}

void
process_options(int argc, char* argv[]) {
  while (true) {
#ifdef HAVE_GETOPT_H
    int option_index = 0;
    int c = getopt_long(argc, argv, OPTION_LETTERS, long_options,
			&option_index);
#else
    int c = getopt(argc, argv, OPTION_LETTERS);
#endif

    if (c == EOF)
      break;

    switch (c) {
    case 0:
      break;

    case '?':
    case 'h':
      fprintf(stderr, usage_string, argv[0]);
      exit(0);
      break;

    case 'l':
      required_library = true;
      break;

    case 'a':
      required_application = true;
      break;

    case 'f':
      if (strcmp(optarg, "plain") == 0)
	required_format = PLAIN;
      else if (strcmp(optarg, "makefile") == 0)
	required_format = MAKEFILE;
      else if (strcmp(optarg, "sh") == 0)
	required_format = SH;
      else if (strcmp(optarg, "csh") == 0)
	required_format = CSH;
      else
	fatal("invalid argument `%s' to --format: "
	      "must be `plain', `makefile', `sh' or `csh'",
	      optarg);
      break;

    case 'i':
      // FIXME
      required_interface = CXX;
      break;

    case 'p':
      required_prefix = true;
      ++num_required_items;
      break;

    case 'e':
      required_exec_prefix = true;
      ++num_required_items;
      break;

    case 'V':
      required_version = true;
      ++num_required_items;
      break;

    case 'O':
      required_configure_options = true;
      ++num_required_items;
      break;

    case 'M':
      required_version_major = true;
      ++num_required_items;
      break;

    case 'N':
      required_version_minor = true;
      ++num_required_items;
      break;

    case 'R':
      required_version_revision = true;
      ++num_required_items;
      break;

    case 'B':
      required_version_beta = true;
      ++num_required_items;
      break;

    case 'E':
      required_banner = true;
      ++num_required_items;
      break;

    case 'L':
      required_libs = true;
      ++num_required_items;
      break;

    case 'I':
      required_includes = true;
      ++num_required_items;
      break;

    case 'P':
      required_cppflags = true;
      ++num_required_items;
      break;

    case 'C':
      required_cflags = true;
      ++num_required_items;
      break;

    case 'X':
      required_cxxflags = true;
      ++num_required_items;
      break;

    case 'D':
      required_ldflags = true;
      ++num_required_items;
      break;

    case 'n':
      required_license = true;
      ++num_required_items;
      break;

    case 'c':
      required_copying = true;
      ++num_required_items;
      break;

    case 'b':
      required_bugs = true;
      ++num_required_items;
      break;

    case 'r':
      required_credits = true;
      ++num_required_items;
      break;

    case 'A':
      required_all = true;
      break;

    default:
      abort();
    }
  }

  if (argc != optind)
    // We have a spurious argument.
    fatal("no arguments besides options are accepted");

  if (required_application && required_library)
    fatal("the --application and --library are mutually exclusive");
}

void
portray_name(const char* name) {
  switch (required_format) {
  case PLAIN:
    if (num_required_items > 1)
      std::cout << variable_prefix << name << ": ";
    break;
  case MAKEFILE:
    std::cout << variable_prefix << name << '=';
    break;
  case SH:
    std::cout << "export " << variable_prefix << name << '=';
    break;
  case CSH:
    std::cout << "setenv " << variable_prefix << name << ' ';
    break;
  }
}

void
portray(const char* const array[]) {
  for (unsigned i = 0; array[i] != 0; ++i)
    std::cout << array[i] << std::endl;
}

void
portray(const char* string) {
  std::cout << string;
}

void
portray(long n) {
  std::cout << n;
}

void
portray(const char* name, const char* const array[]) {
  portray_name(name);
  portray(array);
  std::cout << std::endl;
}

void
portray(const char* name, const char* string) {
  portray_name(name);
  portray(string);
  std::cout << std::endl;
}

void
portray(const char* name, long n) {
  portray_name(name);
  portray(n);
  std::cout << std::endl;
}

} // namespace

int
main(int argc, char* argv[]) try {
  program_name = argv[0];

  if (strcmp(PPL_VERSION, PPL::version()) != 0)
    fatal("was compiled with PPL version %s, but linked with version %s",
	  PPL_VERSION, PPL::version());

#if 0
  if (verbose)
    std::cerr << "Parma Polyhedra Library version:\n" << PPL::version()
	      << "\n\nParma Polyhedra Library banner:\n" << PPL::banner()
	      << std::endl;
#endif

  // Process command line options.
  process_options(argc, argv);

  if (required_prefix)
    portray("PREFIX", prefix);

  if (required_exec_prefix)
    portray("EXEC_PREFIX", exec_prefix);

  if (required_configure_options)
    portray("CONFIGURE_OPTIONS", PPL_CONFIGURE_OPTIONS);

  if (required_version)
    portray("VERSION", PPL_VERSION);

  if (required_version_major)
    portray("VERSION_MAJOR", long(PPL_VERSION_MAJOR));

  if (required_version_minor)
    portray("VERSION_MINOR", long(PPL_VERSION_MINOR));

  if (required_version_revision)
    portray("VERSION_REVISION", long(PPL_VERSION_REVISION));

  if (required_version_beta)
    portray("VERSION_BETA", long(PPL_VERSION_BETA));

  if (required_banner)
    portray("BANNER", PPL::banner());

  if (required_libs)
    ;

  if (required_includes)
    ;

  if (required_cppflags)
    ;

  if (required_cflags)
    ;

  if (required_cxxflags)
    ;

  if (required_ldflags)
    ;

  if (required_license)
    portray("LICENSE", PPL_LICENSE);

  if (required_copying) {
    extern const char* const COPYING_array[]; 
    portray("COPYING", COPYING_array);
  }

  if (required_bugs) {
    extern const char* const BUGS_array[]; 
    portray("BUGS", BUGS_array);
  }

  if (required_credits) {
    extern const char* const CREDITS_array[]; 
    portray("CREDITS", CREDITS_array);
  }

  return 0;
}
catch(const std::bad_alloc&) {
  fatal("out of memory");
  exit(1);
}
catch(const std::overflow_error& e) {
  fatal("arithmetic overflow (%s)", e.what());
  exit(1);
}
catch(...) {
  fatal("internal error: please submit a bug report to ppl-devel@cs.unipr.it");
  exit(1);
}
