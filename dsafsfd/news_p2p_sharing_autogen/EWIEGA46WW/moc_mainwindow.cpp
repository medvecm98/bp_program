/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../mainwindow.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_MainWindow_t {
    QByteArrayData data[46];
    char stringdata0[907];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MainWindow_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MainWindow_t qt_meta_stringdata_MainWindow = {
    {
QT_MOC_LITERAL(0, 0, 10), // "MainWindow"
QT_MOC_LITERAL(1, 11, 15), // "add_new_article"
QT_MOC_LITERAL(2, 27, 0), // ""
QT_MOC_LITERAL(3, 28, 4), // "path"
QT_MOC_LITERAL(4, 33, 12), // "start_server"
QT_MOC_LITERAL(5, 46, 12), // "QHostAddress"
QT_MOC_LITERAL(6, 59, 7), // "address"
QT_MOC_LITERAL(7, 67, 10), // "add_margin"
QT_MOC_LITERAL(8, 78, 11), // "article_ptr"
QT_MOC_LITERAL(9, 90, 7), // "article"
QT_MOC_LITERAL(10, 98, 21), // "newspaper_added_to_db"
QT_MOC_LITERAL(11, 120, 4), // "pk_t"
QT_MOC_LITERAL(12, 125, 7), // "news_id"
QT_MOC_LITERAL(13, 133, 21), // "article_list_received"
QT_MOC_LITERAL(14, 155, 12), // "newspaper_id"
QT_MOC_LITERAL(15, 168, 18), // "enable_add_article"
QT_MOC_LITERAL(16, 187, 15), // "enable_add_news"
QT_MOC_LITERAL(17, 203, 17), // "enable_print_peer"
QT_MOC_LITERAL(18, 221, 16), // "disable_new_peer"
QT_MOC_LITERAL(19, 238, 17), // "newspaper_created"
QT_MOC_LITERAL(20, 256, 22), // "got_network_interfaces"
QT_MOC_LITERAL(21, 279, 15), // "address_vec_ptr"
QT_MOC_LITERAL(22, 295, 24), // "addresses_and_interfaces"
QT_MOC_LITERAL(23, 320, 10), // "new_margin"
QT_MOC_LITERAL(24, 331, 11), // "std::string"
QT_MOC_LITERAL(25, 343, 4), // "type"
QT_MOC_LITERAL(26, 348, 8), // "contents"
QT_MOC_LITERAL(27, 357, 19), // "check_selected_item"
QT_MOC_LITERAL(28, 377, 31), // "on_pushButton_new_peer_released"
QT_MOC_LITERAL(29, 409, 33), // "on_pushButton_print_peer_rele..."
QT_MOC_LITERAL(30, 443, 31), // "on_pushButton_add_news_released"
QT_MOC_LITERAL(31, 475, 34), // "on_pushButton_add_article_rel..."
QT_MOC_LITERAL(32, 510, 35), // "on_pushButton_article_list_re..."
QT_MOC_LITERAL(33, 546, 29), // "on_pushButton_set_ip_released"
QT_MOC_LITERAL(34, 576, 39), // "on_pushButton_external_articl..."
QT_MOC_LITERAL(35, 616, 35), // "on_treeWidget_newspaper_itemC..."
QT_MOC_LITERAL(36, 652, 16), // "QTreeWidgetItem*"
QT_MOC_LITERAL(37, 669, 4), // "item"
QT_MOC_LITERAL(38, 674, 6), // "column"
QT_MOC_LITERAL(39, 681, 36), // "on_pushButton_delete_article_..."
QT_MOC_LITERAL(40, 718, 35), // "on_pushButton_addJournalist_c..."
QT_MOC_LITERAL(41, 754, 21), // "on_pushButton_clicked"
QT_MOC_LITERAL(42, 776, 33), // "on_pushButton_view_margin_cli..."
QT_MOC_LITERAL(43, 810, 32), // "on_pushButton_add_margin_clicked"
QT_MOC_LITERAL(44, 843, 31), // "on_pushButton_testPeer1_clicked"
QT_MOC_LITERAL(45, 875, 31) // "on_pushButton_testPeer2_clicked"

    },
    "MainWindow\0add_new_article\0\0path\0"
    "start_server\0QHostAddress\0address\0"
    "add_margin\0article_ptr\0article\0"
    "newspaper_added_to_db\0pk_t\0news_id\0"
    "article_list_received\0newspaper_id\0"
    "enable_add_article\0enable_add_news\0"
    "enable_print_peer\0disable_new_peer\0"
    "newspaper_created\0got_network_interfaces\0"
    "address_vec_ptr\0addresses_and_interfaces\0"
    "new_margin\0std::string\0type\0contents\0"
    "check_selected_item\0on_pushButton_new_peer_released\0"
    "on_pushButton_print_peer_released\0"
    "on_pushButton_add_news_released\0"
    "on_pushButton_add_article_released\0"
    "on_pushButton_article_list_released\0"
    "on_pushButton_set_ip_released\0"
    "on_pushButton_external_article_released\0"
    "on_treeWidget_newspaper_itemClicked\0"
    "QTreeWidgetItem*\0item\0column\0"
    "on_pushButton_delete_article_clicked\0"
    "on_pushButton_addJournalist_clicked\0"
    "on_pushButton_clicked\0"
    "on_pushButton_view_margin_clicked\0"
    "on_pushButton_add_margin_clicked\0"
    "on_pushButton_testPeer1_clicked\0"
    "on_pushButton_testPeer2_clicked"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MainWindow[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      28,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,  154,    2, 0x06 /* Public */,
       4,    1,  157,    2, 0x06 /* Public */,
       7,    1,  160,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      10,    1,  163,    2, 0x0a /* Public */,
      13,    1,  166,    2, 0x0a /* Public */,
      15,    0,  169,    2, 0x0a /* Public */,
      16,    0,  170,    2, 0x0a /* Public */,
      17,    0,  171,    2, 0x0a /* Public */,
      18,    0,  172,    2, 0x0a /* Public */,
      19,    0,  173,    2, 0x0a /* Public */,
      20,    1,  174,    2, 0x0a /* Public */,
      23,    2,  177,    2, 0x0a /* Public */,
      27,    0,  182,    2, 0x0a /* Public */,
      28,    0,  183,    2, 0x08 /* Private */,
      29,    0,  184,    2, 0x08 /* Private */,
      30,    0,  185,    2, 0x08 /* Private */,
      31,    0,  186,    2, 0x08 /* Private */,
      32,    0,  187,    2, 0x08 /* Private */,
      33,    0,  188,    2, 0x08 /* Private */,
      34,    0,  189,    2, 0x08 /* Private */,
      35,    2,  190,    2, 0x08 /* Private */,
      39,    0,  195,    2, 0x08 /* Private */,
      40,    0,  196,    2, 0x08 /* Private */,
      41,    0,  197,    2, 0x08 /* Private */,
      42,    0,  198,    2, 0x08 /* Private */,
      43,    0,  199,    2, 0x08 /* Private */,
      44,    0,  200,    2, 0x08 /* Private */,
      45,    0,  201,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void, 0x80000000 | 5,    6,
    QMetaType::Void, 0x80000000 | 8,    9,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 11,   12,
    QMetaType::Void, 0x80000000 | 11,   14,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 21,   22,
    QMetaType::Void, 0x80000000 | 24, 0x80000000 | 24,   25,   26,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 36, QMetaType::Int,   37,   38,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MainWindow *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->add_new_article((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: _t->start_server((*reinterpret_cast< QHostAddress(*)>(_a[1]))); break;
        case 2: _t->add_margin((*reinterpret_cast< article_ptr(*)>(_a[1]))); break;
        case 3: _t->newspaper_added_to_db((*reinterpret_cast< pk_t(*)>(_a[1]))); break;
        case 4: _t->article_list_received((*reinterpret_cast< pk_t(*)>(_a[1]))); break;
        case 5: _t->enable_add_article(); break;
        case 6: _t->enable_add_news(); break;
        case 7: _t->enable_print_peer(); break;
        case 8: _t->disable_new_peer(); break;
        case 9: _t->newspaper_created(); break;
        case 10: _t->got_network_interfaces((*reinterpret_cast< address_vec_ptr(*)>(_a[1]))); break;
        case 11: _t->new_margin((*reinterpret_cast< std::string(*)>(_a[1])),(*reinterpret_cast< std::string(*)>(_a[2]))); break;
        case 12: _t->check_selected_item(); break;
        case 13: _t->on_pushButton_new_peer_released(); break;
        case 14: _t->on_pushButton_print_peer_released(); break;
        case 15: _t->on_pushButton_add_news_released(); break;
        case 16: _t->on_pushButton_add_article_released(); break;
        case 17: _t->on_pushButton_article_list_released(); break;
        case 18: _t->on_pushButton_set_ip_released(); break;
        case 19: _t->on_pushButton_external_article_released(); break;
        case 20: _t->on_treeWidget_newspaper_itemClicked((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 21: _t->on_pushButton_delete_article_clicked(); break;
        case 22: _t->on_pushButton_addJournalist_clicked(); break;
        case 23: _t->on_pushButton_clicked(); break;
        case 24: _t->on_pushButton_view_margin_clicked(); break;
        case 25: _t->on_pushButton_add_margin_clicked(); break;
        case 26: _t->on_pushButton_testPeer1_clicked(); break;
        case 27: _t->on_pushButton_testPeer2_clicked(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (MainWindow::*)(QString );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MainWindow::add_new_article)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (MainWindow::*)(QHostAddress );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MainWindow::start_server)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (MainWindow::*)(article_ptr );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&MainWindow::add_margin)) {
                *result = 2;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject MainWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_MainWindow.data,
    qt_meta_data_MainWindow,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 28)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 28;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 28)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 28;
    }
    return _id;
}

// SIGNAL 0
void MainWindow::add_new_article(QString _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void MainWindow::start_server(QHostAddress _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void MainWindow::add_margin(article_ptr _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
