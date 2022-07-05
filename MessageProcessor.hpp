#ifndef NP2PS_MESSAGE_PROCESSOR
#define NP2PS_MESSAGE_PROCESSOR

#include "MPCreate.hpp"
#include "MPProcess.hpp"

template <typename T1, typename T2>
struct MessageProcessor {
    static void process(MPProcess<T1, T2> mp) {
        throw invalid_stun_message_format_error("Invalid template arguments in MessageProcessor.");
    }

    static void create(MPCreate<T1, T2> mp) {
        throw invalid_stun_message_format_error("Invalid template arguments in MessageProcessor.");
    }
};

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

template <>
struct MessageProcessor<CResponseSuccessTag, MAllocateTag> {
    static void process(MPProcess<CResponseSuccessTag, MAllocateTag>& mpp) {
        XorMappedAddressAttribute* xma;
        PublicIdentifierAttribute* pia;
        for (auto&& attr : mpp.message_orig->attributes) {
            if (attr->attribute_type == StunAttributeEnum::xor_mapped_address) {
                xma = (XorMappedAddressAttribute*)attr.get();
            }
            if (attr->attribute_type == StunAttributeEnum::public_identifier) {
                pia = (PublicIdentifierAttribute*)attr.get();
            }
        }
        mpp.ip_map.update_ip(pia->public_identifier, QHostAddress(xma->get_address()), xma->get_port());
    }

    static void create(MPCreate<CResponseSuccessTag, MAllocateTag>& mpc) {
        mpc.message_new->stun_class = StunClassEnum::response_success;
        mpc.message_new->stun_method = StunMethodEnum::allocate;
        
        mpc.message_new->copy_tid(mpc.message_orig);

        std::shared_ptr<XorMappedAddressAttribute> xma = std::make_shared<XorMappedAddressAttribute>();
        xma->initialize(mpc.message_new.get(), STUN_IPV4, mpc.socket);

        std::shared_ptr<LifetimeAttribute> la = std::make_shared<LifetimeAttribute>();
        la->initialize(mpc.lifetime, mpc.message_new.get());

        mpc.message_new->attributes.push_back(xma);
        mpc.message_new->attributes.push_back(la);
    }
};

template <>
struct MessageProcessor<CResponseErrorTag, MAllocateTag> {
    static void process(MPProcess<CResponseErrorTag, MAllocateTag>& mpp) {
        
    }

    static void create(MPCreate<CResponseErrorTag, MAllocateTag>& mpc) {
        
    }
};

template <>
struct MessageProcessor<CRequestTag, MAllocateTag> {
    static void process(MPProcess<CRequestTag, MAllocateTag> mpp) {
        pk_t public_identifier;
        bool request_transport_found = false;
        std::uint32_t protocol;
        std::uint32_t lifetime = 600;

        for (auto&& attr : mpp.message_orig->attributes) {
            if (attr->attribute_type == StunAttributeEnum::requested_transport) {
                request_transport_found = true;

                protocol = ((RequestedTransportAttribute*)attr.get())->get_protocol();
            }
            if (attr->attribute_type == StunAttributeEnum::lifetime) {
                std::uint32_t temp;

                temp = ((LifetimeAttribute*)attr.get())->time;
                if (temp >= lifetime && temp <= MAX_TIME) {
                    lifetime = temp;
                }
                else if (temp > MAX_TIME) {
                    lifetime = MAX_TIME;
                }
            }
        }

        if (!request_transport_found) {
            //TODO: error 400 (bad request)
        }

        if (protocol != IANA_TCP && protocol != IANA_UDP) {
            //TODO: error 442 (unsupported transport protocol)
        }

        auto it = mpp.allocations.find(public_identifier);
        if (it == mpp.allocations.end()) {
            FiveTuple ft;

            ft.client_ipv4 = mpp.socket->peerAddress();
            ft.client_port = mpp.socket->peerPort();
            ft.server_ipv4 = mpp.server->serverAddress();
            ft.server_port = mpp.server->serverPort();
            ft.protocol = protocol;

            mpp.allocations.emplace(public_identifier, TurnAllocation(ft, lifetime));
        }
        else {
            throw public_identifier_already_allocated("Sadly, this identifier is already allocated.");
            //TODO: send back error 437 (Allocation Mismatch)
        }

        auto mpc = MPCreate<CResponseSuccessTag, MAllocateTag>(mpp.message_orig, mpp.message_new, mpp.socket, lifetime);
        MessageProcessor<CResponseSuccessTag, MAllocateTag>::create(mpc);
    }

    static void create(MPCreate<CRequestTag, MAllocateTag> mpc) {
        mpc.message_new->stun_class = StunClassEnum::request;
        mpc.message_new->stun_method = StunMethodEnum::allocate;

        std::shared_ptr<RequestedTransportAttribute> rta = std::make_shared<RequestedTransportAttribute>();
        rta->initialize(IANA_TCP, mpc.message_new.get());
        mpc.message_new->append_attribute(rta);

        if (mpc.lifetime != 600) {
            std::shared_ptr<LifetimeAttribute> la = std::make_shared<LifetimeAttribute>();
            la->initialize(mpc.lifetime, mpc.message_new.get());
            mpc.message_new->append_attribute(la);
        }

        mpc.message_new->generate_tid(mpc.rng);
    }
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

template <>
struct MessageProcessor<CResponseErrorTag, MBindingTag> {
    static void process(MPProcess<CResponseErrorTag, MBindingTag>& mpp) {
        throw unknown_comprehension_required_attribute_error("In StunClient...");
    }

    static void create(MPCreate<CResponseErrorTag, MBindingTag>& mpc) {
        mpc.message_new->stun_class = StunClassEnum::response_error;
        mpc.message_new->stun_method = StunMethodEnum::binding;

        mpc.message_new->copy_tid(mpc.message_orig);
    }
};

template <>
struct MessageProcessor<CResponseSuccessTag, MBindingTag> {
    static void process(MPProcess<CResponseSuccessTag, MBindingTag>& mpp) {
        std::cout << "MessageProcessor<CResponseSuccessTag, MBindingTag> attributes size: " << mpp.message_orig->attributes.size() << std::endl;
        for (auto&& attr : mpp.message_orig->attributes) {
            if (attr->attribute_type == StunAttributeEnum::xor_mapped_address) {
                mpp.response = QHostAddress(((XorMappedAddressAttribute*)attr.get())->get_address()).toString();
                mpp.port = ((XorMappedAddressAttribute*)attr.get())->get_port();
            }
        }
    }

    static void create(MPCreate<CResponseSuccessTag, MBindingTag>& mpc) {
        mpc.message_new->stun_class = StunClassEnum::response_success;
        mpc.message_new->stun_method = StunMethodEnum::binding;
        
        mpc.message_new->copy_tid(mpc.message_orig);
    }
};

template <>
struct MessageProcessor<CRequestTag, MBindingTag> {
    static void process(MPProcess<CRequestTag, MBindingTag>& mp) {
        bool impostor_found = false;
        for (auto&& attr : mp.message_orig->attributes) {
            if (attr->stun_attr_type < 0x8000 && attr->stun_attr_type != 0x0020) {
                mp.unknown_cr_attr.push_back(attr->stun_attr_type);
                impostor_found = true;
            }
        }

        if (impostor_found) {
            throw unknown_comprehension_required_attribute_error("Unknown comprehension-required attribute.");
        }

        auto mpc = MPCreate<CResponseSuccessTag, MBindingTag>(mp.message_orig, mp.message_new);
        MessageProcessor<CResponseSuccessTag, MBindingTag>::create(mpc);

        std::shared_ptr<XorMappedAddressAttribute> xor_m_a = std::make_shared<XorMappedAddressAttribute>();
        xor_m_a->initialize(mp.message_new.get(), 0x01, mp.socket);
        mp.message_new->append_attribute(xor_m_a);
    }

    static void create(MPCreate<CRequestTag, MBindingTag>& mp) {
        mp.message_new->stun_class = StunClassEnum::request;
        mp.message_new->stun_method = StunMethodEnum::binding;

        for (std::size_t i = 0; i < 3; i++) { //96-bits in tid
            mp.message_new->stun_transaction_id[i] = 0;
            for (std::size_t j = 0; j < 4; j++) { //32-bits in one array entry
                mp.message_new->stun_transaction_id[i] <<= 8;
                mp.message_new->stun_transaction_id[i] |= mp.rng.GenerateByte();
            }
        }
    }
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
struct MessageProcessor<CResponseSuccessTag, MIdentifyTag> {
    static void process(MPProcess<CResponseSuccessTag, MIdentifyTag>& mp) {
        PublicIdentifierAttribute* pia;
        XorRelayedAddressAttribute* xraa;
        for (auto&& attr : mp.message_orig->attributes) {
            if (attr->attribute_type == StunAttributeEnum::xor_relayed_address) {
                xraa = (XorRelayedAddressAttribute*)attr.get();
            }
            if (attr->attribute_type == StunAttributeEnum::public_identifier) {
                pia = (PublicIdentifierAttribute*)attr.get();
            }
        }
        mp.public_identifier = pia->get_public_identifier();
        mp.address = QHostAddress(xraa->get_address());
        mp.port = xraa->get_port();
    }

    static void create(MPCreate<CResponseSuccessTag, MIdentifyTag> mp) {
        mp.message_new->stun_class = StunClassEnum::response_success;
        mp.message_new->stun_method = StunMethodEnum::identify;

        std::shared_ptr<PublicIdentifierAttribute> pia = std::make_shared<PublicIdentifierAttribute>();
        pia->initialize(mp.public_identifier, mp.message_new.get());

        std::shared_ptr<XorRelayedAddressAttribute> xraa = std::make_shared<XorRelayedAddressAttribute>();
        xraa->initialize(mp.message_new.get(), STUN_IPV4, mp.address, mp.port);

        mp.message_new->append_attribute(pia);
        mp.message_new->append_attribute(xraa);

        mp.message_new->copy_tid(mp.message_orig);
    }
};

template<>
struct MessageProcessor<CRequestTag, MIdentifyTag> {
    static void process(MPProcess<CRequestTag, MIdentifyTag>& mp) {
        pk_t public_identifier;
        PublicIdentifierAttribute* pia;
        for (auto&& attr : mp.message_orig->attributes) {
            if (attr->attribute_type == StunAttributeEnum::public_identifier) {
                pia = (PublicIdentifierAttribute*)attr->attribute_type;
                public_identifier = pia->get_public_identifier();
            }
        }
        auto i = mp.allocations.at(public_identifier).five_tuple.client_ipv4;
        auto p = mp.allocations.at(public_identifier).five_tuple.client_port;

        MPCreate<CResponseSuccessTag, MIdentifyTag> mpc(mp.message_new, mp.message_orig, public_identifier, i, p);
        MessageProcessor<CResponseSuccessTag, MIdentifyTag>::create(mpc);
    }

    static void create(MPCreate<CRequestTag, MIdentifyTag> mpc) {
        mpc.message_new->stun_class = StunClassEnum::request;
        mpc.message_new->stun_method = StunMethodEnum::identify;

        std::shared_ptr<PublicIdentifierAttribute> pia = std::make_shared<PublicIdentifierAttribute>();
        pia->initialize(mpc.public_identifier, mpc.message_new.get());

        mpc.message_new->append_attribute(pia);

        mpc.message_new->generate_tid(mpc.rng);
    }
};

/*
███████ ███████ ███    ██ ██████  
██      ██      ████   ██ ██   ██ 
███████ █████   ██ ██  ██ ██   ██ 
     ██ ██      ██  ██ ██ ██   ██ 
███████ ███████ ██   ████ ██████  
*/

template <>
struct MessageProcessor<CRequestTag, MSendTag> {
    static void process(MPProcess<CRequestTag, MSendTag> mp) {
        for (auto&& attr : mp.message_to_relay->attributes) {
            if (attr->attribute_type == StunAttributeEnum::xor_relayed_address) {
                auto xraa = (XorRelayedAddressAttribute*) attr.get();
                mp.where_address = xraa->get_address();
                mp.where_port = xraa->get_port();
            }
        }

        mp.message_to_relay->stun_class = StunClassEnum::indication;
    }

    static void create(MPCreate<CRequestTag, MSendTag> mp) {
        mp.message_to->stun_class = StunClassEnum::request;
        mp.message_to->stun_method = StunMethodEnum::send;

        pk_t to = mp.ip_map_.my_public_id;

        auto xmaa = std::make_shared<XorMappedAddressAttribute>();
        xmaa->initialize(mp.message_to.get(), 0x0001, mp.socket);
        mp.message_to->append_attribute(xmaa);

        auto xraa = std::make_shared<XorRelayedAddressAttribute>();
        xraa->initialize(mp.message_to.get(), 0x0001, mp.ip_map_.get_ip4(to), mp.ip_map_.get_port(to));
        mp.message_to->append_attribute(xraa);

        auto data = std::make_shared<DataAttribute>();
        data->initialize(mp.np2ps_message, mp.message_to.get());
        mp.message_to->append_attribute(data);

        mp.message_to->generate_tid(mp.rng);
    }
};

template <>
struct MessageProcessor<CIndicationTag, MSendTag> {
    static void process(MPProcess<CIndicationTag, MSendTag> mp) {
        for (auto&& attr : mp.message_received->attributes) {
            if (attr->attribute_type == StunAttributeEnum::xor_mapped_address) {
                auto xmaa = (XorMappedAddressAttribute*) attr.get();
                mp.from_address = xmaa->get_address();
                mp.from_port = xmaa->get_port();
            }
            if (attr->attribute_type == StunAttributeEnum::data) {
                auto data = (DataAttribute*) attr.get();
                mp.np2ps_message = data->get_np2ps_messsage();
            }
        }
    }
};

#endif