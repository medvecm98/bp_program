#ifndef NP2PS_MESSAGE_PROCESSOR
#define NP2PS_MESSAGE_PROCESSOR

#include "MPCreate.hpp"
#include "MPProcess.hpp"

/**
 * @brief This was supposed to be a universal message processor.
 * 
 * It turned out that tags doesn't work very well like that.
 * 
 * @tparam T1 Message class tag
 * @tparam T2 Message method tag
 */
template <typename T1, typename T2>
struct MessageProcessor {
    static void process(MPProcess<T1, T2> mp) {
        throw invalid_stun_message_format_error("Invalid template arguments in MessageProcessor.");
    }

    static void create(MPCreate<T1, T2> mp) {
        throw invalid_stun_message_format_error("Invalid template arguments in MessageProcessor.");
    }
};


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

#endif