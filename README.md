# Dot-P2P
### Timeline/Origin
Here is a timeline of events as they are unfolding at the moment:

    Hello all #isp of the world. We're going to add a new competing root-server since we're 
    tired of #ICANN. Please contact me to help. 

Peter Sunde via Twitter @brokep on November 27th, 23:08 PST.

    Alternative DNS now has a temporary blog.
    http://p2pdns.baywords.com/2010/11/30/hello-world/

Peter Sunde via Twitter @brokep on 30 November 2010 08:00 AM

## Building
Old build system is make, supported by original developers..
    make

New hotness is scons, supported by me.  Makes things easier.
    scons -j4

### Usage

    Usage: dnsp2p <options ...>
    
    Options:
     -D <addr:port>    Listen address & port for DNS resolver (UDP)
     -A <addr:port>    Listen address & port for admin HTTP port (TCP)
     -P <addr:port>    Listen address & port for P2P connectivity (TCP+UDP)
     -s <file>         DB file for node state storage
     -p <file>         DB file for persistent publish storage
     -n <file>         Properties file for peer list
     -h                Show this help


### Build instructions

For now you need the following libraries installed:

- tokyocabinet (http://fallabs.com/tokyocabinet/), a library of routines for managing a database

- libevent >= 2.0 (http://monkey.org/~provos/libevent/), a library to execute a function when a specific event occurs on a file descriptor

- libgcrypt (http://www.gnupg.org/), general purpose crypto library based on the code used in GnuPG

You also will require CMake build system. Quick and dirty build instructions:

    mkdir build; cd build; cmake ..; make

Here are some of the variables you might want to tweak:

| CMAKE_BUILD_TYPE     | Debug or Release                                     |
| CMAKE_C_FLAGS        | the flags passed to gcc                              |
| LIBEVENT2_PREFIX,    |                                                      |
| TOKYOCABINET_PREFIX, |                                                      |
| GCRYPT_PREFIX        | installation prefix of dependencies, if non-standard |

This can be passed to CMake as follows:

    cmake .. -DCMAKE_BUILD_TYPE=Debug -DLIBEVENT2_PREFIX=/opt/libevent2/

### Give me more
More information at [the wiki](http://dot-p2p.org/index.php?title=Main_Page)
