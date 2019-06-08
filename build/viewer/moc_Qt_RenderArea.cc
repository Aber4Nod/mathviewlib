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
    QByteArrayData data[18];
    char stringdata0[257];
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
QT_MOC_LITERAL(9, 77, 4), // "name"
QT_MOC_LITERAL(10, 82, 22), // "deleteSelectedElements"
QT_MOC_LITERAL(11, 105, 11), // "copyElement"
QT_MOC_LITERAL(12, 117, 19), // "insertCopiedElement"
QT_MOC_LITERAL(13, 137, 19), // "unselectAllElements"
QT_MOC_LITERAL(14, 157, 16), // "insertGlyphAfter"
QT_MOC_LITERAL(15, 174, 23), // "std::basic_string<char>"
QT_MOC_LITERAL(16, 198, 24), // "insertElementAfterCursor"
QT_MOC_LITERAL(17, 223, 33) // "std::map<std::string,std::str..."

    },
    "Qt_RenderArea\0slotNew\0\0slotLoad\0"
    "slotSave\0slotSaveAs\0close\0insert\0"
    "std::string\0name\0deleteSelectedElements\0"
    "copyElement\0insertCopiedElement\0"
    "unselectAllElements\0insertGlyphAfter\0"
    "std::basic_string<char>\0"
    "insertElementAfterCursor\0"
    "std::map<std::string,std::string>"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Qt_RenderArea[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   74,    2, 0x0a /* Public */,
       3,    0,   75,    2, 0x0a /* Public */,
       4,    0,   76,    2, 0x0a /* Public */,
       5,    0,   77,    2, 0x0a /* Public */,
       6,    0,   78,    2, 0x0a /* Public */,
       7,    1,   79,    2, 0x0a /* Public */,
      10,    0,   82,    2, 0x0a /* Public */,
      11,    0,   83,    2, 0x0a /* Public */,
      12,    0,   84,    2, 0x0a /* Public */,
      13,    0,   85,    2, 0x0a /* Public */,
      14,    1,   86,    2, 0x0a /* Public */,
      16,    2,   89,    2, 0x0a /* Public */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 8,    9,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 15,    2,
    QMetaType::Void, 0x80000000 | 8, 0x80000000 | 17,    2,    2,

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
        case 6: _t->deleteSelectedElements(); break;
        case 7: _t->copyElement(); break;
        case 8: _t->insertCopiedElement(); break;
        case 9: _t->unselectAllElements(); break;
        case 10: _t->insertGlyphAfter((*reinterpret_cast< std::basic_string<char>(*)>(_a[1]))); break;
        case 11: _t->insertElementAfterCursor((*reinterpret_cast< std::string(*)>(_a[1])),(*reinterpret_cast< std::map<std::string,std::string>(*)>(_a[2]))); break;
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
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 12)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 12;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
