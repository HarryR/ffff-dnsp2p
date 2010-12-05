#
# Franky - fuck all the discussion
# Fuck everybody who thinks this will someday work with a central authority
# Fuck all the non-coders who haven't got a clue
# Fuck all the geeks who do nothing but quibble
# I present: FFFF-dnsp2p
import platform
import re
sys = platform.system()

env = Environment(
  LIBS = ['gcrypt', 'event', 'tokyocabinet'],
  CCCOMSTR = "[ C] $TARGET",
  ARCOMSTR = "[AR] $TARGET",
  RANLIBCOMSTR = "[RL] $TARGET",
)

env.Append(CPPPATH = Split("""
        ../libevent-root/include
        ../tokyocabinet-root/include 
        ../libgcrypt-root/include
    """),
    CFLAGS = " -Wall -Wextra -std=gnu99",
    LIBPATH = ["/usr/local/lib",
               "../libevent-root/lib",
               "../tokyocabinet-root/lib",
               "../libgcrypt-root/lib"],
)

if sys == "Darwin":
    env.Append(CFLAGS=["-fnested-functions"])
elif re.search("BSD", sys):
    env.Append(CPPDEFINES = {
        "HAVE_MEMMEM" : 1,
    })
    env.Append(CFLAGS=["-ggdb3"])

libffff_objs = Split("""
    ffff.c
    properties.c
    admin.c
    dns.c
    rbtree.c
    ops.c
    op_get.c
""")
seccure_objs = Split("""
    seccure/curves.o
    seccure/serialize.o
    seccure/protocol.o
    seccure/ecc.o
    seccure/aes256ctr.o
    seccure/numtheory.o
""")
dht_objs = "dht/dht.o"

conf = Configure(env)
if not conf.CheckCHeader('tcutil.h'):
        print 'Install tokyocabinet headers!'
        Exit(1)
if not conf.CheckCHeader('event2/dns.h'):
        print 'Install libevent 2 headers!'
        Exit(1)
if not conf.CheckLib('event') or not conf.CheckFunc('evdns_base_new'):
        print 'Install libevent 2 library (do you only have 1.x?)!'
        Exit(1)
conf.Finish()

Export('env')
SConscript(['#seccure/SConscript', '#dht/SConscript'])

env.StaticLibrary('ffff', [libffff_objs, seccure_objs, dht_objs])
env.Program('dnsp2p', ['stub.c', 'libffff.a', '#dht/libdht.a'])
