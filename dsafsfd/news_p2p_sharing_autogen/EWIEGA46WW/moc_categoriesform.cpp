/****************************************************************************
** Meta object code from reading C++ file 'categoriesform.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../categoriesform.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'categoriesform.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_CategoriesForm_t {
    QByteArrayData data[14];
    char stringdata0[266];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CategoriesForm_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CategoriesForm_t qt_meta_stringdata_CategoriesForm = {
    {
QT_MOC_LITERAL(0, 0, 14), // "CategoriesForm"
QT_MOC_LITERAL(1, 15, 15), // "add_new_article"
QT_MOC_LITERAL(2, 31, 0), // ""
QT_MOC_LITERAL(3, 32, 4), // "path"
QT_MOC_LITERAL(4, 37, 34), // "on_pushButton_add_category_cl..."
QT_MOC_LITERAL(5, 72, 37), // "on_pushButton_remove_category..."
QT_MOC_LITERAL(6, 110, 28), // "on_pushButton_accept_clicked"
QT_MOC_LITERAL(7, 139, 28), // "on_pushButton_cancel_clicked"
QT_MOC_LITERAL(8, 168, 10), // "closeEvent"
QT_MOC_LITERAL(9, 179, 12), // "QCloseEvent*"
QT_MOC_LITERAL(10, 192, 5), // "event"
QT_MOC_LITERAL(11, 198, 9), // "showEvent"
QT_MOC_LITERAL(12, 208, 11), // "QShowEvent*"
QT_MOC_LITERAL(13, 220, 45) // "on_listWidget_categories_item..."

    },
    "CategoriesForm\0add_new_article\0\0path\0"
    "on_pushButton_add_category_clicked\0"
    "on_pushButton_remove_category_clicked\0"
    "on_pushButton_accept_clicked\0"
    "on_pushButton_cancel_clicked\0closeEvent\0"
    "QCloseEvent*\0event\0showEvent\0QShowEvent*\0"
    "on_listWidget_categories_itemSelectionChanged"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CategoriesForm[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   54,    2, 0x0a /* Public */,
       4,    0,   57,    2, 0x08 /* Private */,
       5,    0,   58,    2, 0x08 /* Private */,
       6,    0,   59,    2, 0x08 /* Private */,
       7,    0,   60,    2, 0x08 /* Private */,
       8,    1,   61,    2, 0x08 /* Private */,
      11,    1,   64,    2, 0x08 /* Private */,
      13,    0,   67,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    3,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 9,   10,
    QMetaType::Void, 0x80000000 | 12,   10,
    QMetaType::Void,

       0        // eod
};

void CategoriesForm::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CategoriesForm *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->add_new_article((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: _t->on_pushButton_add_category_clicked(); break;
        case 2: _t->on_pushButton_remove_category_clicked(); break;
        case 3: _t->on_pushButton_accept_clicked(); break;
        case 4: _t->on_pushButton_cancel_clicked(); break;
        case 5: _t->closeEvent((*reinterpret_cast< QCloseEvent*(*)>(_a[1]))); break;
        case 6: _t->showEvent((*reinterpret_cast< QShowEvent*(*)>(_a[1]))); break;
        case 7: _t->on_listWidget_categories_itemSelectionChanged(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject CategoriesForm::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_CategoriesForm.data,
    qt_meta_data_CategoriesForm,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *CategoriesForm::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CategoriesForm::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CategoriesForm.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int CategoriesForm::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
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
QT_WARNING_POP
QT_END_MOC_NAMESPACE
