#ifndef NP2PS_MP_CREATE
#define NP2PS_MP_CREATE

#include "XorMappedAddressAttribute.hpp"
#include "ErrorCodeAttribute.hpp"
#include "UnknownAttributesAttribute.hpp"
#include "TurnAllocation.hpp"
#include "RequestedTransportAttribute.hpp"
#include "LifetimeAttribute.hpp"
#include "XorRelayedAddress.hpp"
#include "IpMap.h"
#include "PublicIdentifierAttribute.hpp"
#include "StunTags.hpp"

template<typename CTag, typename MTag>
struct MPCreate {};

/*
 $$$$$$\  $$\ $$\                               $$\               
$$  __$$\ $$ |$$ |                              $$ |              
$$ /  $$ |$$ |$$ | $$$$$$\   $$$$$$$\ $$$$$$\ $$$$$$\    $$$$$$\  
$$$$$$$$ |$$ |$$ |$$  __$$\ $$  _____|\____$$\\_$$  _|  $$  __$$\ 
$$  __$$ |$$ |$$ |$$ /  $$ |$$ /      $$$$$$$ | $$ |    $$$$$$$$ |
$$ |  $$ |$$ |$$ |$$ |  $$ |$$ |     $$  __$$ | $$ |$$\ $$   ____|
$$ |  $$ |$$ |$$ |\$$$$$$  |\$$$$$$$\\$$$$$$$ | \$$$$  |\$$$$$$$\ 
\__|  \__|\__|\__| \______/  \_______|\_______|  \____/  \_______|
*/

template<>
struct MPCreate<CRequestTag, MAllocateTag> {
    MPCreate() = default;
    MPCreate(CryptoPP::AutoSeededRandomPool& r, stun_header_ptr mn) : rng(r), message_new(mn), lifetime(600) {}
    MPCreate(CryptoPP::AutoSeededRandomPool& r, stun_header_ptr mn, std::uint32_t l) : rng(r), message_new(mn), lifetime(l) {}
    CryptoPP::AutoSeededRandomPool& rng;
    stun_header_ptr message_new;
    std::uint32_t lifetime;
};

template<>
struct MPCreate<CResponseSuccessTag, MAllocateTag> {
    MPCreate() = default;
    MPCreate(stun_header_ptr mo, stun_header_ptr mn, QTcpSocket* socket_, std::uint32_t lifetime_) : message_orig(mo), message_new(mn), socket(socket_), lifetime(lifetime_) {}
    stun_header_ptr message_orig, message_new;
    QTcpSocket* socket;
    std::uint32_t lifetime;
};

template<>
struct MPCreate<CResponseErrorTag, MAllocateTag> {
    MPCreate() = default;
    MPCreate(stun_header_ptr mo, stun_header_ptr mn) : message_orig(mo), message_new(mn) {}
    stun_header_ptr message_orig, message_new;
};

/*
$$$$$$$\  $$\                 $$\ $$\                     
$$  __$$\ \__|                $$ |\__|                    
$$ |  $$ |$$\ $$$$$$$\   $$$$$$$ |$$\ $$$$$$$\   $$$$$$\  
$$$$$$$\ |$$ |$$  __$$\ $$  __$$ |$$ |$$  __$$\ $$  __$$\ 
$$  __$$\ $$ |$$ |  $$ |$$ /  $$ |$$ |$$ |  $$ |$$ /  $$ |
$$ |  $$ |$$ |$$ |  $$ |$$ |  $$ |$$ |$$ |  $$ |$$ |  $$ |
$$$$$$$  |$$ |$$ |  $$ |\$$$$$$$ |$$ |$$ |  $$ |\$$$$$$$ |
\_______/ \__|\__|  \__| \_______|\__|\__|  \__| \____$$ |
                                                $$\   $$ |
                                                \$$$$$$  |
                                                 \______/ 
*/

template<>
struct MPCreate<CRequestTag, MBindingTag> {
    MPCreate() = default;
    MPCreate(CryptoPP::AutoSeededRandomPool& r, stun_header_ptr mn) : rng(r), message_new(mn) {}
    CryptoPP::AutoSeededRandomPool& rng;
    stun_header_ptr message_new;
};

template<>
struct MPCreate<CIndicationTag, MBindingTag> {
    MPCreate() = default;
    MPCreate(CryptoPP::AutoSeededRandomPool& r, stun_header_ptr mn) : rng(r), message_new(mn) {}
    CryptoPP::AutoSeededRandomPool& rng;
    stun_header_ptr message_new;
};

template<>
struct MPCreate<CResponseSuccessTag, MBindingTag> {
    MPCreate() = default;
    MPCreate(stun_header_ptr mo, stun_header_ptr mn) : message_orig(mo), message_new(mn) {}
    stun_header_ptr message_orig, message_new;
};

template<>
struct MPCreate<CResponseErrorTag, MBindingTag> {
    MPCreate() = default;
    MPCreate(stun_header_ptr mo, stun_header_ptr mn) : message_orig(mo), message_new(mn) {}
    stun_header_ptr message_orig, message_new;
};

/*
$$$$$$\      $$\                      $$\     $$\  $$$$$$\            
\_$$  _|     $$ |                     $$ |    \__|$$  __$$\           
  $$ |  $$$$$$$ | $$$$$$\  $$$$$$$\ $$$$$$\   $$\ $$ /  \__|$$\   $$\ 
  $$ | $$  __$$ |$$  __$$\ $$  __$$\\_$$  _|  $$ |$$$$\     $$ |  $$ |
  $$ | $$ /  $$ |$$$$$$$$ |$$ |  $$ | $$ |    $$ |$$  _|    $$ |  $$ |
  $$ | $$ |  $$ |$$   ____|$$ |  $$ | $$ |$$\ $$ |$$ |      $$ |  $$ |
$$$$$$\\$$$$$$$ |\$$$$$$$\ $$ |  $$ | \$$$$  |$$ |$$ |      \$$$$$$$ |
\______|\_______| \_______|\__|  \__|  \____/ \__|\__|       \____$$ |
                                                            $$\   $$ |
                                                            \$$$$$$  |
                                                             \______/ 
*/

template<>
struct MPCreate<CRequestTag, MIdentifyTag> {
    MPCreate() = default;
    MPCreate(stun_header_ptr mn, pk_t public_identifier_, CryptoPP::AutoSeededRandomPool& r) : message_new(mn), public_identifier(public_identifier_), rng(r) {}
    CryptoPP::AutoSeededRandomPool& rng;
    pk_t public_identifier;
    stun_header_ptr message_new;
};

template<>
struct MPCreate<CResponseSuccessTag, MIdentifyTag> {
    MPCreate() = default;
    MPCreate(stun_header_ptr mn, stun_header_ptr mo, pk_t public_identifier_, QHostAddress address_, std::uint16_t port_) : message_new(mn), message_orig(mo), public_identifier(public_identifier_), address(address_), port(port_) {}
    pk_t public_identifier;
    stun_header_ptr message_new, message_orig;
    QHostAddress address;
    std::uint16_t port;
};

/*
███████ ███████ ███    ██ ██████  
██      ██      ████   ██ ██   ██ 
███████ █████   ██ ██  ██ ██   ██ 
     ██ ██      ██  ██ ██ ██   ██ 
███████ ███████ ██   ████ ██████  
*/

template<>
struct MPCreate<CRequestTag, MSendTag> {
    MPCreate() = default;
    MPCreate(stun_header_ptr message_to_, std::string np2ps_message_, QTcpSocket* socket_, CryptoPP::AutoSeededRandomPool& rng_, IpMap& ip_map__) : 
        message_to(message_to_), np2ps_message(np2ps_message_), rng(rng_), ip_map_(ip_map__), socket(socket_) {}
    stun_header_ptr message_to;
    std::string np2ps_message;
    CryptoPP::AutoSeededRandomPool& rng;
    IpMap& ip_map_;
    QTcpSocket* socket;
};

#endif