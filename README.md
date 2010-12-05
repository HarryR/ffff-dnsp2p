# dnsp2p
Here is a timeline of events which got the project started

>> Hello all #isp of the world. We're going to add a new competing root-server since we're
>> tired of #ICANN. Please contact me to help.

>Peter Sunde via [Twitter @brokep on November 27th, 23:08](http://twitter.com/brokep/status/8779363872935936)

----

>> Alternative DNS now has a temporary blog.
>> [http://p2pdns.baywords.com/2010/11/30/hello-world/](http://p2pdns.baywords.com/2010/11/30/hello-world/)

> Peter Sunde via [Twitter @brokep on 30 November 2010 08:00](http://twitter.com/brokep/status/9517070882447360)

### Build instructions

For now you need the following libraries installed:

- [tokyocabinet](http://fallabs.com/tokyocabinet/), lightweight and fast embedded database
- [libevent](http://monkey.org/~provos/libevent/) >= 2.0, event based network, http & dns library
- [libgcrypt](http://www.gnupg.org/) >= 1.4.1, general purpose crypto library based on the code used in GnuPG

You can also use cmake to build the project:

    make cmake.build

Here are some of the variables you might want to tweak:

 * ``CMAKE_BUILD_TYPE`` - Debug or Release
 * ``CMAKE_C_FLAGS`` - Additional compiler flags
 * ``LIBEVENT2_PREFIX``
 * ``TOKYOCABINET_PREFIX``
 * ``GCRYPT_PREFIX`` - Installation prefix of dependencies, if non-standard

This can be passed to CMake as follows:

    cmake .. -DCMAKE_BUILD_TYPE=Debug -DLIBEVENT2_PREFIX=/opt/libevent2/

### Give me more
More information at [the wiki](http://dot-p2p.org/index.php?title=Main_Page)
