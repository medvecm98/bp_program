#ifndef PROGRAMCONTEXT_H
#define PROGRAMCONTEXT_H

#include "Peer.h"

struct ProgramContext
{
	Peer p;
	/**
	 * Serialize using boost archive.
	 */
	template <class Archive>
	void serialize(Archive& ar, const unsigned int version) {
		ar & p;
	}
};

using program_ptr = std::shared_ptr<ProgramContext>;

#endif // PROGRAMCONTEXT_H
