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


## Give me more
More information at [the wiki](http://dot-p2p.org/index.php?title=Main_Page)
