#include <string>
#include <exception>

class invalid_stun_message_format_error : public std::exception {
public:
    invalid_stun_message_format_error(const char* what_arg) throw() : std::exception() {
        message = std::string(what_arg);
    }

    invalid_stun_message_format_error(std::string& what_arg) throw() : std::exception() {
        message = what_arg;
    }

    virtual const char* what() const throw() {
        return message.data();
    }
private:
    std::string message;
};

class unknown_comprehension_required_attribute_error : public std::exception {
public:
    unknown_comprehension_required_attribute_error(const char* what_arg) throw() : std::exception() {
        message = std::string(what_arg);
    }

    unknown_comprehension_required_attribute_error(std::string& what_arg) throw() : std::exception() {
        message = what_arg;
    }

    virtual const char* what() const throw() {
        return message.data();
    }
private:
    std::string message;
};

class invalid_class_or_method : public std::exception {
public:
    invalid_class_or_method(const char* what_arg) throw() : std::exception() {
        message = std::string(what_arg);
    }

    invalid_class_or_method(std::string& what_arg) throw() : std::exception() {
        message = what_arg;
    }

    virtual const char* what() const throw() {
        return message.data();
    }
private:
    std::string message;
};

class invalid_address_family : public std::exception {
public:
    invalid_address_family(const char* what_arg) throw() : std::exception() {
        message = std::string(what_arg);
    }

    invalid_address_family(std::string& what_arg) throw() : std::exception() {
        message = what_arg;
    }

    virtual const char* what() const throw() {
        return message.data();
    }
private:
    std::string message;
};

class public_identifier_already_allocated : public std::exception {
public:
    public_identifier_already_allocated(const char* what_arg) throw() : std::exception() {
        message = std::string(what_arg);
    }

    public_identifier_already_allocated(std::string& what_arg) throw() : std::exception() {
        message = what_arg;
    }

    virtual const char* what() const throw() {
        return message.data();
    }
private:
    std::string message;
};

class user_not_found_in_database : public std::exception {
public:
    user_not_found_in_database(const char* what_arg) throw() : std::exception() {
        message = std::string(what_arg);
    }

    user_not_found_in_database(std::string& what_arg) throw() : std::exception() {
        message = what_arg;
    }

    virtual const char* what() const throw() {
        return message.data();
    }
private:
    std::string message;
};

class unsupported_message_type_in_context : public std::exception {
public:
    unsupported_message_type_in_context(const char* what_arg) throw() : std::exception() {
        message = std::string(what_arg);
    }

    unsupported_message_type_in_context(std::string& what_arg) throw() : std::exception() {
        message = what_arg;
    }

    virtual const char* what() const throw() {
        return message.data();
    }
private:
    std::string message;
};

class unknown_newspaper_error : public std::exception {
public:
    unknown_newspaper_error(const char* what_arg) throw() : std::exception() {
        message = std::string(what_arg);
    }

    unknown_newspaper_error(std::string& what_arg) throw() : std::exception() {
        message = what_arg;
    }

    virtual const char* what() const throw() {
        return message.data();
    }
private:
    std::string message;
};

class article_not_found_database : public std::exception {
public:
    article_not_found_database(const char* what_arg) throw() : std::exception() {
        message = std::string(what_arg);
    }

    article_not_found_database(std::string& what_arg) throw() : std::exception() {
        message = what_arg;
    }

    virtual const char* what() const throw() {
        return message.data();
    }
private:
    std::string message;
};

class malformed_message_context_or_type : public std::exception {
public:
    malformed_message_context_or_type(const char* what_arg) throw() : std::exception() {
        message = std::string(what_arg);
    }

    malformed_message_context_or_type(std::string& what_arg) throw() : std::exception() {
        message = what_arg;
    }

    virtual const char* what() const throw() {
        return message.data();
    }
private:
    std::string message;
};


class other_error : public std::exception {
public:
    other_error(const char* what_arg) throw() : std::exception() {
        message = std::string(what_arg);
    }

    other_error(std::string& what_arg) throw() : std::exception() {
        message = what_arg;
    }

    virtual const char* what() const throw() {
        return message.data();
    }
private:
    std::string message;
};

class no_rsa_key_found : public std::exception {
public:
    no_rsa_key_found(const char* what_arg) throw() : std::exception() {
        message = std::string(what_arg);
    }

    no_rsa_key_found(std::string& what_arg) throw() : std::exception() {
        message = what_arg;
    }

    virtual const char* what() const throw() {
        return message.data();
    }
private:
    std::string message;
};

class no_eax_key_found : public std::exception {
public:
    no_eax_key_found(const char* what_arg) throw() : std::exception() {
        message = std::string(what_arg);
    }

    no_eax_key_found(std::string& what_arg) throw() : std::exception() {
        message = what_arg;
    }

    virtual const char* what() const throw() {
        return message.data();
    }
private:
    std::string message;
};

class unsupported_feature : public std::exception {
public:
    unsupported_feature(const char* what_arg) throw() : std::exception() {
        message = std::string(what_arg);
    }

    unsupported_feature(std::string& what_arg) throw() : std::exception() {
        message = what_arg;
    }

    virtual const char* what() const throw() {
        return message.data();
    }
private:
    std::string message;
};
