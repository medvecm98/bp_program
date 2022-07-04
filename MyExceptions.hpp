#include "GlobalUsing.h"

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