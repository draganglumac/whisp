whisp 
=====
P2P decentralised chat program in early pre-Alpha

Example - http://showterm.io/4bb6e10b6e4118e8e2a61#fast

Usage
=====

Set the conf/settings.conf to your prefered interface
make
./whisp

Commands

list  //Show guids of other chat clients nearby
convo //Initiate secure connection to a guid [ Pass guid ID as next input ]

-- Once connection is established

send //Will request to open the session for communication [ Pass session ID as next input ]

-- All further input will be in session mode until Cntrl+C exit

Note
=====


- Both clients MUST have each other on their peerstore lists before communicating
- No implementation of end-session or re-establish yet built


Structure
=========

	Discovery---UDP MULTICAST--->
	Passive listener<---TCP CONNECTION		
	[ Generate session keys ]
	TCP transmit new session---> Replicate session store public key
	<--- TCP transmit public key
	Generate asyemtrical shared secret---> Store shared secret
	[ Switch to secure socket ]
	<-Symmetrically encrypted messages->	


