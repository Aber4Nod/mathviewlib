/****************************************************************************
** Meta object code from reading C++ file 'Qt_RenderArea.hh'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.11.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../viewer/Qt_RenderArea.hh"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'Qt_RenderArea.hh' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.11.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Qt_RenderArea_t {
    QByteArrayData data[10];
    char stringdata0[82];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Qt_RenderArea_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Qt_RenderArea_t qt_meta_stringdata_Qt_RenderArea = {
    {
QT_MOC_LITERAL(0, 0, 13), // "Qt_RenderArea"
QT_MOC_LITERAL(1, 14, 7), // "slotNew"
QT_MOC_LITERAL(2, 22, 0), // ""
QT_MOC_LITERAL(3, 23, 8), // "slotLoad"
QT_MOC_LITERAL(4, 32, 8), // "slotSave"
QT_MOC_LITERAL(5, 41, 10), // "slotSaveAs"
QT_MOC_LITERAL(6, 52, 5), // "close"
QT_MOC_LITERAL(7, 58, 6), // "insert"
QT_MOC_LITERAL(8, 65, 11), // "std::string"
QT_MOC_LITERAL(9, 77, 4) // "name"

    },
    "Qt_RenderArea\0slotNew\0\0slotLoad\0"
    "slotSave\0slotSaveAs\0close\0insert\0"
    "std::string\0name"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Qt_RenderArea[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   44,    2, 0x0a /* Public */,
       3,    0,   45,    2, 0x0a /* Public */,
       4,    0,   46,    2, 0x0a /* Public */,
       5,    0,   47,    2, 0x0a /* Public */,
       6,    0,   48,    2, 0x0a /* Public */,
       7,    1,   49,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 8,    9,

       0        // eod
};

void Qt_RenderArea::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Qt_RenderArea *_t = static_cast<Qt_RenderArea *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->slotNew(); break;
        case 1: _t->slotLoad(); break;
        case 2: _t->slotSave(); break;
        case 3: _t->slotSaveAs(); break;
        case 4: _t->close(); break;
        case 5: _t->insert((*reinterpret_cast< std::string(*)>(_a[1]))); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject Qt_RenderArea::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_Qt_RenderArea.data,
      qt_meta_data_Qt_RenderArea,  qt_static_metacall, nullptr, nullptr}
};


const QMetaObject *Qt_RenderArea::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Qt_RenderArea::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Qt_RenderArea.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int Qt_RenderArea::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 6;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
