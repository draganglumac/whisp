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
