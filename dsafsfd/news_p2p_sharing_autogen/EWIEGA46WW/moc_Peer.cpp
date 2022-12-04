/****************************************************************************
** Meta object code from reading C++ file 'Peer.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../Peer.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'Peer.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Peer_t {
    QByteArrayData data[31];
    char stringdata0[455];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Peer_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Peer_t qt_meta_stringdata_Peer = {
    {
QT_MOC_LITERAL(0, 0, 4), // "Peer"
QT_MOC_LITERAL(1, 5, 26), // "got_newspaper_confirmation"
QT_MOC_LITERAL(2, 32, 0), // ""
QT_MOC_LITERAL(3, 33, 4), // "pk_t"
QT_MOC_LITERAL(4, 38, 7), // "news_id"
QT_MOC_LITERAL(5, 46, 16), // "new_article_list"
QT_MOC_LITERAL(6, 63, 12), // "newspaper_id"
QT_MOC_LITERAL(7, 76, 22), // "ip_credentials_arrived"
QT_MOC_LITERAL(8, 99, 21), // "message_originally_to"
QT_MOC_LITERAL(9, 121, 17), // "new_symmetric_key"
QT_MOC_LITERAL(10, 139, 10), // "key_sender"
QT_MOC_LITERAL(11, 150, 27), // "user_is_member_verification"
QT_MOC_LITERAL(12, 178, 5), // "seq_t"
QT_MOC_LITERAL(13, 184, 18), // "message_seq_number"
QT_MOC_LITERAL(14, 203, 9), // "is_member"
QT_MOC_LITERAL(15, 213, 23), // "symmetric_key_exchanged"
QT_MOC_LITERAL(16, 237, 10), // "other_peer"
QT_MOC_LITERAL(17, 248, 19), // "check_selected_item"
QT_MOC_LITERAL(18, 268, 14), // "handle_message"
QT_MOC_LITERAL(19, 283, 18), // "unique_ptr_message"
QT_MOC_LITERAL(20, 302, 7), // "message"
QT_MOC_LITERAL(21, 310, 23), // "allocate_on_stun_server"
QT_MOC_LITERAL(22, 334, 6), // "target"
QT_MOC_LITERAL(23, 341, 17), // "newspaper_confirm"
QT_MOC_LITERAL(24, 359, 3), // "pid"
QT_MOC_LITERAL(25, 363, 20), // "newspaper_identified"
QT_MOC_LITERAL(26, 384, 13), // "newspaper_key"
QT_MOC_LITERAL(27, 398, 9), // "my_string"
QT_MOC_LITERAL(28, 408, 14), // "newspaper_name"
QT_MOC_LITERAL(29, 423, 11), // "std::string"
QT_MOC_LITERAL(30, 435, 19) // "newspaper_ip_domain"

    },
    "Peer\0got_newspaper_confirmation\0\0pk_t\0"
    "news_id\0new_article_list\0newspaper_id\0"
    "ip_credentials_arrived\0message_originally_to\0"
    "new_symmetric_key\0key_sender\0"
    "user_is_member_verification\0seq_t\0"
    "message_seq_number\0is_member\0"
    "symmetric_key_exchanged\0other_peer\0"
    "check_selected_item\0handle_message\0"
    "unique_ptr_message\0message\0"
    "allocate_on_stun_server\0target\0"
    "newspaper_confirm\0pid\0newspaper_identified\0"
    "newspaper_key\0my_string\0newspaper_name\0"
    "std::string\0newspaper_ip_domain"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Peer[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       7,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   69,    2, 0x06 /* Public */,
       5,    1,   72,    2, 0x06 /* Public */,
       7,    1,   75,    2, 0x06 /* Public */,
       9,    1,   78,    2, 0x06 /* Public */,
      11,    2,   81,    2, 0x06 /* Public */,
      15,    1,   86,    2, 0x06 /* Public */,
      17,    0,   89,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      18,    1,   90,    2, 0x0a /* Public */,
      21,    1,   93,    2, 0x0a /* Public */,
      23,    1,   96,    2, 0x0a /* Public */,
      25,    3,   99,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void, 0x80000000 | 3,    6,
    QMetaType::Void, 0x80000000 | 3,    8,
    QMetaType::Void, 0x80000000 | 3,   10,
    QMetaType::Void, 0x80000000 | 12, QMetaType::Bool,   13,   14,
    QMetaType::Void, 0x80000000 | 3,   16,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 19,   20,
    QMetaType::Void, 0x80000000 | 3,   22,
    QMetaType::Void, 0x80000000 | 3,   24,
    QMetaType::Void, 0x80000000 | 3, 0x80000000 | 27, 0x80000000 | 29,   26,   28,   30,

       0        // eod
};

void Peer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Peer *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->got_newspaper_confirmation((*reinterpret_cast< pk_t(*)>(_a[1]))); break;
        case 1: _t->new_article_list((*reinterpret_cast< pk_t(*)>(_a[1]))); break;
        case 2: _t->ip_credentials_arrived((*reinterpret_cast< pk_t(*)>(_a[1]))); break;
        case 3: _t->new_symmetric_key((*reinterpret_cast< pk_t(*)>(_a[1]))); break;
        case 4: _t->user_is_member_verification((*reinterpret_cast< seq_t(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2]))); break;
        case 5: _t->symmetric_key_exchanged((*reinterpret_cast< pk_t(*)>(_a[1]))); break;
        case 6: _t->check_selected_item(); break;
        case 7: _t->handle_message((*reinterpret_cast< unique_ptr_message(*)>(_a[1]))); break;
        case 8: _t->allocate_on_stun_server((*reinterpret_cast< pk_t(*)>(_a[1]))); break;
        case 9: _t->newspaper_confirm((*reinterpret_cast< pk_t(*)>(_a[1]))); break;
        case 10: _t->newspaper_identified((*reinterpret_cast< pk_t(*)>(_a[1])),(*reinterpret_cast< my_string(*)>(_a[2])),(*reinterpret_cast< std::string(*)>(_a[3]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (Peer::*)(pk_t );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Peer::got_newspaper_confirmation)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (Peer::*)(pk_t );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Peer::new_article_list)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (Peer::*)(pk_t );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Peer::ip_credentials_arrived)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (Peer::*)(pk_t );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Peer::new_symmetric_key)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (Peer::*)(seq_t , bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Peer::user_is_member_verification)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (Peer::*)(pk_t );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Peer::symmetric_key_exchanged)) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (Peer::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&Peer::check_selected_item)) {
                *result = 6;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Peer::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_Peer.data,
    qt_meta_data_Peer,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Peer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Peer::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Peer.stringdata0))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int Peer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 11)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 11;
    }
    return _id;
}

// SIGNAL 0
void Peer::got_newspaper_confirmation(pk_t _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Peer::new_article_list(pk_t _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void Peer::ip_credentials_arrived(pk_t _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void Peer::new_symmetric_key(pk_t _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void Peer::user_is_member_verification(seq_t _t1, bool _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void Peer::symmetric_key_exchanged(pk_t _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void Peer::check_selected_item()
{
    QMetaObject::activate(this, &staticMetaObject, 6, nullptr);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
