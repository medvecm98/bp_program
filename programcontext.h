#ifndef PROGRAMCONTEXT_H
#define PROGRAMCONTEXT_H

#include "Peer.h"

struct ProgramContext
{
	Peer p;
};

using program_ptr = std::shared_ptr<ProgramContext>;

#endif // PROGRAMCONTEXT_H
