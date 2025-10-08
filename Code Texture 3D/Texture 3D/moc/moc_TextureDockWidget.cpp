/****************************************************************************
** Meta object code from reading C++ file 'TextureDockWidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.13)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../TextureDockWidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'TextureDockWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.13. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_TextureDockWidget_t {
    QByteArrayData data[26];
    char stringdata0[297];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_TextureDockWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_TextureDockWidget_t qt_meta_stringdata_TextureDockWidget = {
    {
QT_MOC_LITERAL(0, 0, 17), // "TextureDockWidget"
QT_MOC_LITERAL(1, 18, 13), // "xValueChanged"
QT_MOC_LITERAL(2, 32, 0), // ""
QT_MOC_LITERAL(3, 33, 1), // "i"
QT_MOC_LITERAL(4, 35, 13), // "yValueChanged"
QT_MOC_LITERAL(5, 49, 13), // "zValueChanged"
QT_MOC_LITERAL(6, 63, 7), // "xInvert"
QT_MOC_LITERAL(7, 71, 7), // "yInvert"
QT_MOC_LITERAL(8, 79, 7), // "zInvert"
QT_MOC_LITERAL(9, 87, 8), // "xDisplay"
QT_MOC_LITERAL(10, 96, 1), // "v"
QT_MOC_LITERAL(11, 98, 8), // "yDisplay"
QT_MOC_LITERAL(12, 107, 8), // "zDisplay"
QT_MOC_LITERAL(13, 116, 18), // "xSliderChangedSlot"
QT_MOC_LITERAL(14, 135, 18), // "ySliderChangedSlot"
QT_MOC_LITERAL(15, 154, 18), // "zSliderChangedSlot"
QT_MOC_LITERAL(16, 173, 16), // "xInvertPlaneSlot"
QT_MOC_LITERAL(17, 190, 16), // "yInvertPlaneSlot"
QT_MOC_LITERAL(18, 207, 16), // "zInvertPlaneSlot"
QT_MOC_LITERAL(19, 224, 12), // "xDisplaySlot"
QT_MOC_LITERAL(20, 237, 12), // "yDisplaySlot"
QT_MOC_LITERAL(21, 250, 12), // "zDisplaySlot"
QT_MOC_LITERAL(22, 263, 15), // "setMaxCutPlanes"
QT_MOC_LITERAL(23, 279, 5), // "_xMax"
QT_MOC_LITERAL(24, 285, 5), // "_yMax"
QT_MOC_LITERAL(25, 291, 5) // "_zMax"

    },
    "TextureDockWidget\0xValueChanged\0\0i\0"
    "yValueChanged\0zValueChanged\0xInvert\0"
    "yInvert\0zInvert\0xDisplay\0v\0yDisplay\0"
    "zDisplay\0xSliderChangedSlot\0"
    "ySliderChangedSlot\0zSliderChangedSlot\0"
    "xInvertPlaneSlot\0yInvertPlaneSlot\0"
    "zInvertPlaneSlot\0xDisplaySlot\0"
    "yDisplaySlot\0zDisplaySlot\0setMaxCutPlanes\0"
    "_xMax\0_yMax\0_zMax"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_TextureDockWidget[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      19,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       9,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,  109,    2, 0x06 /* Public */,
       4,    1,  112,    2, 0x06 /* Public */,
       5,    1,  115,    2, 0x06 /* Public */,
       6,    0,  118,    2, 0x06 /* Public */,
       7,    0,  119,    2, 0x06 /* Public */,
       8,    0,  120,    2, 0x06 /* Public */,
       9,    1,  121,    2, 0x06 /* Public */,
      11,    1,  124,    2, 0x06 /* Public */,
      12,    1,  127,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      13,    1,  130,    2, 0x08 /* Private */,
      14,    1,  133,    2, 0x08 /* Private */,
      15,    1,  136,    2, 0x08 /* Private */,
      16,    0,  139,    2, 0x08 /* Private */,
      17,    0,  140,    2, 0x08 /* Private */,
      18,    0,  141,    2, 0x08 /* Private */,
      19,    1,  142,    2, 0x08 /* Private */,
      20,    1,  145,    2, 0x08 /* Private */,
      21,    1,  148,    2, 0x08 /* Private */,
      22,    3,  151,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, QMetaType::Float,    3,
    QMetaType::Void, QMetaType::Float,    3,
    QMetaType::Void, QMetaType::Float,    3,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   10,
    QMetaType::Void, QMetaType::Bool,   10,
    QMetaType::Void, QMetaType::Bool,   10,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void, QMetaType::Int,    3,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   10,
    QMetaType::Void, QMetaType::Bool,   10,
    QMetaType::Void, QMetaType::Bool,   10,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, QMetaType::Int,   23,   24,   25,

       0        // eod
};

void TextureDockWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<TextureDockWidget *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->xValueChanged((*reinterpret_cast< float(*)>(_a[1]))); break;
        case 1: _t->yValueChanged((*reinterpret_cast< float(*)>(_a[1]))); break;
        case 2: _t->zValueChanged((*reinterpret_cast< float(*)>(_a[1]))); break;
        case 3: _t->xInvert(); break;
        case 4: _t->yInvert(); break;
        case 5: _t->zInvert(); break;
        case 6: _t->xDisplay((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 7: _t->yDisplay((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 8: _t->zDisplay((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 9: _t->xSliderChangedSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 10: _t->ySliderChangedSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 11: _t->zSliderChangedSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 12: _t->xInvertPlaneSlot(); break;
        case 13: _t->yInvertPlaneSlot(); break;
        case 14: _t->zInvertPlaneSlot(); break;
        case 15: _t->xDisplaySlot((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 16: _t->yDisplaySlot((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 17: _t->zDisplaySlot((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 18: _t->setMaxCutPlanes((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (TextureDockWidget::*)(float );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TextureDockWidget::xValueChanged)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (TextureDockWidget::*)(float );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TextureDockWidget::yValueChanged)) {
                *result = 1;
                return;
            }
        }
        {
            using _t = void (TextureDockWidget::*)(float );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TextureDockWidget::zValueChanged)) {
                *result = 2;
                return;
            }
        }
        {
            using _t = void (TextureDockWidget::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TextureDockWidget::xInvert)) {
                *result = 3;
                return;
            }
        }
        {
            using _t = void (TextureDockWidget::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TextureDockWidget::yInvert)) {
                *result = 4;
                return;
            }
        }
        {
            using _t = void (TextureDockWidget::*)();
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TextureDockWidget::zInvert)) {
                *result = 5;
                return;
            }
        }
        {
            using _t = void (TextureDockWidget::*)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TextureDockWidget::xDisplay)) {
                *result = 6;
                return;
            }
        }
        {
            using _t = void (TextureDockWidget::*)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TextureDockWidget::yDisplay)) {
                *result = 7;
                return;
            }
        }
        {
            using _t = void (TextureDockWidget::*)(bool );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&TextureDockWidget::zDisplay)) {
                *result = 8;
                return;
            }
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject TextureDockWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<QDockWidget::staticMetaObject>(),
    qt_meta_stringdata_TextureDockWidget.data,
    qt_meta_data_TextureDockWidget,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *TextureDockWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TextureDockWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_TextureDockWidget.stringdata0))
        return static_cast<void*>(this);
    return QDockWidget::qt_metacast(_clname);
}

int TextureDockWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDockWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 19)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 19;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 19)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 19;
    }
    return _id;
}

// SIGNAL 0
void TextureDockWidget::xValueChanged(float _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void TextureDockWidget::yValueChanged(float _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void TextureDockWidget::zValueChanged(float _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void TextureDockWidget::xInvert()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void TextureDockWidget::yInvert()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void TextureDockWidget::zInvert()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void TextureDockWidget::xDisplay(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 6, _a);
}

// SIGNAL 7
void TextureDockWidget::yDisplay(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}

// SIGNAL 8
void TextureDockWidget::zDisplay(bool _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 8, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
