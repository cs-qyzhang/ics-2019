import os
import ycm_core

flags = [
  '-Wall',
  '-Wextra',
  '-Werror',
  '-Wno-long-long',
  '-Wno-variadic-macros',
  '-fexceptions',
  '-ferror-limit=10000',
  '-DNDEBUG',
  '-std=c99',
  '-xc',
  '-isystem/usr/include/',
  '-I./nemu/include',
  '-I./nemu/src/isa/x86/include',
  '-I./nexus-am/am',
  '-I./nexus-am/am/include',
  '-I./nexus-am/libs/klib/include',
  '-I./nanos-lite/include',
  ]

SOURCE_EXTENSIONS = [ '.cpp', '.cxx', '.cc', '.c', '.h', ]

def FlagsForFile( filename, **kwargs ):
  return {
  'flags': flags,
  'do_cache': True
  }
