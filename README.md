Benjamin Jacob Tarman - DUG

** COMPILE **
typing make will give binary called dug


dug has many flag options but typing "./dug -h" will bring up a help menu

in general, you type "./dug <FLAGS> <hostname> <nameserver>"

all the examples on the handout can be typed verbatim, but I have extra
flags for "cacheing" and for "diglike" output.

**supported queries**
MX, A, CNAME, and NS work for sure. I think SOA also works.

***Extra Credits***
I made daemon and implemented caching.

**DAEMON**
The daemon code is mainly in the "matt_daemon.c" file. When ran, it will
accept the next "dig" command you give it then close (I was messing
around with sending data back to dig but didn't seem to work).

To run the daemon
1. start is in one window (./dug -f 138.67.1.2)
2. in another terminal run a dig command with the port assigned
and with the localhost address
"dig -p X imagine.mines.edu @127.0.0.1"

The daemon runs on the localhost 127.0.0.1.


**Cache**
The Cache is implemented with a simple text file is kind of like a key/value
table. The key is the (hostname, nameserver) pair that is passed to the program
and the value is simply the output. The cache will work on both standard
and daemon modes.

To enable cache, use the "-c" flag.

typing "make clean" will "erase" the cache as well.


**Diglike output**
By default the program outputs similarish to the OG dig. To disable this
for maybe easier grading type "-s" flag (for shorten).



