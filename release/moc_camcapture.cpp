/****************************************************************************
** Meta object code from reading C++ file 'camcapture.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.8.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../CamGui/camcapture.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QList>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'camcapture.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.8.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_CamCapture_t {
    QByteArrayData data[15];
    char stringdata0[133];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CamCapture_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CamCapture_t qt_meta_stringdata_CamCapture = {
    {
QT_MOC_LITERAL(0, 0, 10), // "CamCapture"
QT_MOC_LITERAL(1, 11, 11), // "sendDevList"
QT_MOC_LITERAL(2, 23, 0), // ""
QT_MOC_LITERAL(3, 24, 14), // "QList<QString>"
QT_MOC_LITERAL(4, 39, 8), // "captured"
QT_MOC_LITERAL(5, 48, 5), // "char*"
QT_MOC_LITERAL(6, 54, 16), // "connectionResult"
QT_MOC_LITERAL(7, 71, 10), // "reqDevList"
QT_MOC_LITERAL(8, 82, 4), // "setN"
QT_MOC_LITERAL(9, 87, 1), // "N"
QT_MOC_LITERAL(10, 89, 7), // "capture"
QT_MOC_LITERAL(11, 97, 8), // "filename"
QT_MOC_LITERAL(12, 106, 7), // "connect"
QT_MOC_LITERAL(13, 114, 10), // "disconnect"
QT_MOC_LITERAL(14, 125, 7) // "setPath"

    },
    "CamCapture\0sendDevList\0\0QList<QString>\0"
    "captured\0char*\0connectionResult\0"
    "reqDevList\0setN\0N\0capture\0filename\0"
    "connect\0disconnect\0setPath"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CamCapture[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   64,    2, 0x06 /* Public */,
       4,    5,   67,    2, 0x06 /* Public */,
       6,    1,   78,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       7,    0,   81,    2, 0x0a /* Public */,
       8,    1,   82,    2, 0x0a /* Public */,
      10,    1,   85,    2, 0x0a /* Public */,
      10,    0,   88,    2, 0x2a /* Public | MethodCloned */,
      12,    0,   89,    2, 0x0a /* Public */,
      13,    0,   90,    2, 0x0a /* Public */,
      14,    1,   91,    2, 0x0a /* Public */,

 // signals: parameters
    QMetaType::Void, 0x80000000 | 3,    2,
    QMetaType::Void, QMetaType::Int, QMetaType::Int, 0x80000000 | 5, QMetaType::Int, QMetaType::Int,    2,    2,    2,    2,    2,
    QMetaType::Void, QMetaType::Bool,    2,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,    9,
    QMetaType::Void, QMetaType::QString,   11,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,    2,

       0        // eod
};

void CamCapture::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        CamCapture *_t = static_cast<CamCapture *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->sendDevList((*reinterpret_cast< QList<QString>(*)>(_a[1]))); break;
        case 1: _t->captured((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< char*(*)>(_a[3])),(*reinterpret_cast< int(*)>(_a[4])),(*reinterpret_cast< int(*)>(_a[5]))); break;
        case 2: _t->connectionResult((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: _t->reqDevList(); break;
        case 4: _t->setN((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->capture((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 6: _t->capture(); break;
        case 7: _t->connect(); break;
        case 8: _t->disconnect(); break;
        case 9: _t->setPath((*reinterpret_cast< QString(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 0:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QList<QString> >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (CamCapture::*_t)(QList<QString> );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&CamCapture::sendDevList)) {
                *result = 0;
                return;
            }
        }
        {
            typedef void (CamCapture::*_t)(int , int , char * , int , int );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&CamCapture::captured)) {
                *result = 1;
                return;
            }
        }
        {
            typedef void (CamCapture::*_t)(bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&CamCapture::connectionResult)) {
                *result = 2;
                return;
            }
        }
    }
}

const QMetaObject CamCapture::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_CamCapture.data,
      qt_meta_data_CamCapture,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *CamCapture::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CamCapture::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_CamCapture.stringdata0))
        return static_cast<void*>(const_cast< CamCapture*>(this));
    return QObject::qt_metacast(_clname);
}

int CamCapture::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    }
    return _id;
}

// SIGNAL 0
void CamCapture::sendDevList(QList<QString> _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void CamCapture::captured(int _t1, int _t2, char * _t3, int _t4, int _t5)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)), const_cast<void*>(reinterpret_cast<const void*>(&_t5)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void CamCapture::connectionResult(bool _t1)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
