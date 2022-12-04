/****************************************************************************
** Meta object code from reading C++ file 'Networking.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../Networking.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'Networking.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_PeerReceiver_t {
    QByteArrayData data[16];
    char stringdata0[230];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_PeerReceiver_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_PeerReceiver_t qt_meta_stringdata_PeerReceiver = {
    {
QT_MOC_LITERAL(0, 0, 12), // "PeerReceiver"
QT_MOC_LITERAL(1, 13, 12), // "start_server"
QT_MOC_LITERAL(2, 26, 0), // ""
QT_MOC_LITERAL(3, 27, 12), // "QHostAddress"
QT_MOC_LITERAL(4, 40, 7), // "address"
QT_MOC_LITERAL(5, 48, 15), // "message_receive"
QT_MOC_LITERAL(6, 64, 30), // "process_received_np2ps_message"
QT_MOC_LITERAL(7, 95, 12), // "QDataStream&"
QT_MOC_LITERAL(8, 108, 3), // "msg"
QT_MOC_LITERAL(9, 112, 11), // "QTcpSocket*"
QT_MOC_LITERAL(10, 124, 6), // "socket"
QT_MOC_LITERAL(11, 131, 27), // "prepare_for_message_receive"
QT_MOC_LITERAL(12, 159, 25), // "message_receive_connected"
QT_MOC_LITERAL(13, 185, 13), // "display_error"
QT_MOC_LITERAL(14, 199, 28), // "QAbstractSocket::SocketError"
QT_MOC_LITERAL(15, 228, 1) // "e"

    },
    "PeerReceiver\0start_server\0\0QHostAddress\0"
    "address\0message_receive\0"
    "process_received_np2ps_message\0"
    "QDataStream&\0msg\0QTcpSocket*\0socket\0"
    "prepare_for_message_receive\0"
    "message_receive_connected\0display_error\0"
    "QAbstractSocket::SocketError\0e"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_PeerReceiver[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   44,    2, 0x0a /* Public */,
       5,    0,   47,    2, 0x0a /* Public */,
       6,    2,   48,    2, 0x0a /* Public */,
      11,    0,   53,    2, 0x0a /* Public */,
      12,    0,   54,    2, 0x0a /* Public */,
      13,    1,   55,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 7, 0x80000000 | 9,    8,   10,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 14,   15,

       0        // eod
};

void PeerReceiver::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<PeerReceiver *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->start_server((*reinterpret_cast< QHostAddress(*)>(_a[1]))); break;
        case 1: _t->message_receive(); break;
        case 2: _t->process_received_np2ps_message((*reinterpret_cast< QDataStream(*)>(_a[1])),(*reinterpret_cast< QTcpSocket*(*)>(_a[2]))); break;
        case 3: _t->prepare_for_message_receive(); break;
        case 4: _t->message_receive_connected(); break;
        case 5: _t->display_error((*reinterpret_cast< QAbstractSocket::SocketError(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 2:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 1:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QTcpSocket* >(); break;
            }
            break;
        case 5:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QAbstractSocket::SocketError >(); break;
            }
            break;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject PeerReceiver::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_PeerReceiver.data,
    qt_meta_data_PeerReceiver,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *PeerReceiver::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PeerReceiver::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_PeerReceiver.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int PeerReceiver::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    return _id;
}
struct qt_meta_stringdata_PeerSender_t {
    QByteArrayData data[7];
    char stringdata0[96];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_PeerSender_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_PeerSender_t qt_meta_stringdata_PeerSender = {
    {
QT_MOC_LITERAL(0, 0, 10), // "PeerSender"
QT_MOC_LITERAL(1, 11, 13), // "display_error"
QT_MOC_LITERAL(2, 25, 0), // ""
QT_MOC_LITERAL(3, 26, 28), // "QAbstractSocket::SocketError"
QT_MOC_LITERAL(4, 55, 1), // "e"
QT_MOC_LITERAL(5, 57, 14), // "host_connected"
QT_MOC_LITERAL(6, 72, 23) // "handle_connection_error"

    },
    "PeerSender\0display_error\0\0"
    "QAbstractSocket::SocketError\0e\0"
    "host_connected\0handle_connection_error"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_PeerSender[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   29,    2, 0x0a /* Public */,
       5,    0,   32,    2, 0x0a /* Public */,
       6,    0,   33,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void PeerSender::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<PeerSender *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->display_error((*reinterpret_cast< QAbstractSocket::SocketError(*)>(_a[1]))); break;
        case 1: _t->host_connected(); break;
        case 2: _t->handle_connection_error(); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 0:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QAbstractSocket::SocketError >(); break;
            }
            break;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject PeerSender::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_PeerSender.data,
    qt_meta_data_PeerSender,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *PeerSender::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PeerSender::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_PeerSender.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int PeerSender::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    return _id;
}
struct qt_meta_stringdata_Networking_t {
    QByteArrayData data[21];
    char stringdata0[347];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Networking_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Networking_t qt_meta_stringdata_Networking = {
    {
QT_MOC_LITERAL(0, 0, 10), // "Networking"
QT_MOC_LITERAL(1, 11, 20), // "new_message_enrolled"
QT_MOC_LITERAL(2, 32, 0), // ""
QT_MOC_LITERAL(3, 33, 18), // "unique_ptr_message"
QT_MOC_LITERAL(4, 52, 20), // "new_message_received"
QT_MOC_LITERAL(5, 73, 20), // "newspaper_identified"
QT_MOC_LITERAL(6, 94, 4), // "pk_t"
QT_MOC_LITERAL(7, 99, 2), // "id"
QT_MOC_LITERAL(8, 102, 9), // "my_string"
QT_MOC_LITERAL(9, 112, 14), // "newspaper_name"
QT_MOC_LITERAL(10, 127, 11), // "std::string"
QT_MOC_LITERAL(11, 139, 19), // "newspaper_ip_domain"
QT_MOC_LITERAL(12, 159, 22), // "got_network_interfaces"
QT_MOC_LITERAL(13, 182, 15), // "address_vec_ptr"
QT_MOC_LITERAL(14, 198, 24), // "addresses_and_interfaces"
QT_MOC_LITERAL(15, 223, 12), // "send_message"
QT_MOC_LITERAL(16, 236, 26), // "decrypt_encrypted_messages"
QT_MOC_LITERAL(17, 263, 20), // "symmetric_key_sender"
QT_MOC_LITERAL(18, 284, 19), // "symmetric_exchanged"
QT_MOC_LITERAL(19, 304, 10), // "other_peer"
QT_MOC_LITERAL(20, 315, 31) // "peer_process_disconnected_users"

    },
    "Networking\0new_message_enrolled\0\0"
    "unique_ptr_message\0new_message_received\0"
    "newspaper_identified\0pk_t\0id\0my_string\0"
    "newspaper_name\0std::string\0"
    "newspaper_ip_domain\0got_network_interfaces\0"
    "address_vec_ptr\0addresses_and_interfaces\0"
    "send_message\0decrypt_encrypted_messages\0"
    "symmetric_key_sender\0symmetric_exchanged\0"
    "other_peer\0peer_process_disconnected_users"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Networking[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   54,    2, 0x06 /* Public */,
       4,    1,   57,    2, 0x06 /* Public */,
       5,    3,   60,    2, 0x06 /* Public */,
      12,    1,   67,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      15,    1,   70,    2, 0x0a /* Public */,
      16,    1,   73,    2, 0x0a /* Public */,
      18,    1,   76,    2, 0x0a /* Public */,
      20,    0,   79,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void, 0x80000000 | 6, 0x80000000 | 8, 0x80000000 | 10,    7,    9,   11,
    QMetaType::Void, 0x80000000 | 13,   14,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void, 0x80000000 | 6,   17,
    QMetaType::Void, 0x80000000 | 6,   19,
    QMetaType::Void,

       0        // eod
};

void Networking::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Networking *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->new_message_enrolled((*reinterpret_cast< unique_ptr_message(*)>(_a[1]))); break;
        case 1: _t->new_message_received((*reinterpret_cast< unique_ptr_message(*)>(_a[1]))); break;
        case 2: _t->newspaper_identified((*reinterpret_cast< pk_t(*)>(_a[1])),(*reinterpret_cast< my_string(*)>(_a[2])),(*reinterpret_cast< std::string(*)>(_a[3]))); break;
        case 3: _t->got_network_interfaces((*reinterpret_cast< address_vec_ptr(*)>(_a[1]))); break;
        case 4: _t->send_message((*reinterpret_cast< unique_ptr_message(*)>(_a[1]))); break;
        case 5: _t->decrypt_encrypted_messages((*reinterpret_cast< pk_t(*)>(_a[1]))); break;
        case 6: _t->symmetric_exchanged((*reinterpret_cast< pk_t(*)>(_a[1]))); break;
        case 7: _t->peer_process_disconnected_users(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (Networking::*)(unique_ptr_message );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Networking::new_message_enrolled)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (Networking::*)(unique_ptr_message );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Networking::new_message_received)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (Networking::*)(pk_t , my_string , std::string );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Networking::newspaper_identified)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (Networking::*)(address_vec_ptr );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Networking::got_network_interfaces)) {
                *result = 3;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Networking::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_Networking.data,
    qt_meta_data_Networking,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Networking::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Networking::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Networking.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "std::enable_shared_from_this<Networking>"))
        return static_cast< std::enable_shared_from_this<Networking>*>(this);
    return QObject::qt_metacast(_clname);
}

int Networking::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void Networking::new_message_enrolled(unique_ptr_message _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Networking::new_message_received(unique_ptr_message _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void Networking::newspaper_identified(pk_t _t1, my_string _t2, std::string _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void Networking::got_network_interfaces(address_vec_ptr _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
