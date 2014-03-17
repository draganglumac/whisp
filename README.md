whisp
=====
P2P decentralised chat program in early pre-Alpha


Usage
=====

Limited commands are

list  //Show guids of other chat clients nearby
convo //Initiate secure connection to a guid


Note
=====

- There are memory leaks where RSA keys are not cleaned
- Multiple dangling pointers and other leaks where cleaning hasn't been done
- Both clients MUST have each other on their peerstore lists before communicating



Structure
=========

	Discovery---UDP MULTICAST--->
	Passive listener<---TCP CONNECTION		
	[ Generate session keys ]
	TCP transmit new session---> Replicate session store public key
	<--- TCP transmit public key
	Generate shared secret---> Store shared secret
	[ Switch to secure socket ]
	<-DES ENCRYPTED TRAFFIC->	


