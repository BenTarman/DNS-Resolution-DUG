Benjamin Jacob Tarman - DUG

**COMPILE**

typing make will give binary called dug

dug has many flag options but typing "./dug -h" will bring up a help menu

in general, you type "./dug <FLAGS> <hostname> <nameserver>"

all the examples on the handout can be typed verbatim, but I have extra
flags for "cacheing" and for "diglike" output.


**supported queries**

MX, A, CNAME, and NS work for sure. I think SOA also works.

**Extra Credits**

**==============**

I did both EC options. I made a daemon and implemented caching.

**DAEMON**

The daemon code is mainly in the "matt_daemon.c" file. When ran, it will
accept the next "dig" command you give it then close (I was messing
around with sending data back to dig but didn't seem to work).

To run the daemon
1. start is in one window (./dug -f 138.67.1.2)
2. in another terminal run a dig command with the port assigned

and with the localhost address or server address:

3. "dig -p X imagine.mines.edu @127.0.0.1"

or on isengard

3. "dig -p X imagine.mines.edu @138.67.209.20"


**Cache**

The Cache is implemented with a simple text file is kind of like a key/value
table. The key is the (hostname, nameserver) pair that is passed to the program
and the value is simply the output. The cache will work on both standard
and daemon modes.

To enable cache, use the "-c" flag.

When we use the cache, the output is in magenta and no debugging is shown (since basically no code is ran).

typing "make clean" will "erase" the cache as well.


**Diglike output**

By default the program outputs similarish to the OG dig. To disable this
for maybe easier grading type "-s" flag (for shorten).


<h2> Example Runs </h2>

<h1> standard run (defaults to A type) </h1>

$ > ./dug imagine.mines.edu 138.67.1.2

querying imagine.mines.edu....


Query Processed!


================

QUERY: 1, ANSWER: 2, AUTHORITY: 0, ADDITIONAL: 0



;;  ANSWER SECTION:

imagine.mines.edu	CNAME	pvm-jumpbox.mines.edu

pvm-jumpbox.mines.edu	A	138.67.208.118

;;  AUTHORITY SECTION:

;;  ADDITIONAL SECTION:

Authoritative answer: 138.67.208.118



<h1> MX type query ("shortened" answer) </h1>

$ > ./dug -s -t MX imagine.mines.edu 138.67.1.2

querying imagine.mines.edu....
<br><br>



Non-authoritative answer: pvm-jumpbox.mines.edu


<h1> Cache query (CNAME) </h1>


$ > ./dug -s -c -t MX cse.unl.edu 138.67.1.2

querying cse.unl.edu....




Non-authoritative answer: 10 cse-barracuda.unl.edu


<h1> cache hit (with daemon server!) </h1>

$ > ./dug -c -f 138.67.1.2

Using port 3626

(open other terminal)

$ > dig -p 3626 cse.unl.edu @127.0.0.1 MX

Query Cached!


================

QUERY: 1, ANSWER: 1, AUTHORITY: 2, ADDITIONAL: 5

;;  ANSWER SECTION:

cse.unl.edu	MX	10 cse-barracuda.unl.edu

;;  AUTHORITY SECTION:

dns1.unl.edu

unl.edu	NS	dns1.unl.edu

dns2.unl.edu

unl.edu	NS	dns2.unl.edu

;;  ADDITIONAL SECTION:

cse-barracuda.unl.edu	A	129.93.164.185

dns1.unl.edu	A	129.93.168.59

dns2.unl.edu	A	129.93.168.60

record type unrecognized or corrupted

record type unrecognized or corrupted

Non-authoritative answer: 10 cse-barracuda.unl.edu




