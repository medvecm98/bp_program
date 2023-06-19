#ifndef PROGRAMCONTEXT_H
#define PROGRAMCONTEXT_H

#include "Peer.h"

struct ProgramContext
{
	ProgramContext() : peer(0) {}
	ProgramContext(pk_t pid) : peer(pid) {}
	ProgramContext(np2ps::Peer& serialized_peer) : peer(serialized_peer) {}

	Peer peer;
};

#endif // PROGRAMCONTEXT_H
