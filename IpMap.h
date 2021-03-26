//
// Created by michal on 26. 3. 2021.
//
#include "GlobalUsing.h"

#ifndef PROGRAM_IPMAP_H
#define PROGRAM_IPMAP_H


class IpMap {
public:
	bool add_to_map(pk_t, IpWrapper&& ip);
	void remove_from_map(pk_t);
	bool update_ip(pk_t, IpWrapper&& ip);
private:
	ip_map _map;
};


#endif //PROGRAM_IPMAP_H
