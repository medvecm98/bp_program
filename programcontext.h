#ifndef PROGRAMCONTEXT_H
#define PROGRAMCONTEXT_H

#include "Peer.h"

struct ProgramContext
{
	ProgramContext() : p(0) {}
	ProgramContext(pk_t pid) : p(pid) {}
	ProgramContext(np2ps::Peer& serialized_peer) : p(serialized_peer) {}

	Peer p;
};

#endif // PROGRAMCONTEXT_H
