#!/usr/bin/env python

import subprocess
import os

class MakeException(Exception):
    pass

def swapExt(path, current, replacement):
    path, ext = os.path.splitext(path)
    if ext == current:
        path += replacement
        return path
    else:
        raise MakeException(
            "swapExt: expected file name ending in %s, got file name ending in %s" % \
                (current, replacement))

headerFiles = [
    'benc.h',
    'bencode.h',
]

codeFiles = [
    'benc_int.c',
    'benc_bstr.c',
    'benc_list.c',
    'benc_dict.c',
    'bencode.c',
    'bcopy.c',
]

cflags = ['-g']

programFile = 'bcopy'

def gcc(*packedArgs):
    args = []
    for arg in packedArgs:
        if isinstance(arg, list):
            args += arg
        elif isinstance(arg, tuple):
            args += list(arg)
        else:
            args.append(arg)
    subprocess.check_call(['gcc'] + args)

def compile(codeFile, cflags=[]):
    objectFile = swapExt(codeFile, '.c', '.o')
    gcc(cflags, '-c', ('-o', objectFile), codeFile)
    return objectFile

def link(programFile, objectFiles, cflags=[]):
    gcc(cflags, ('-o', programFile), objectFiles)

if __name__ == '__main__':
    objectFiles = [compile(codeFile, cflags) for codeFile in codeFiles]
    link(programFile, objectFiles, cflags)


