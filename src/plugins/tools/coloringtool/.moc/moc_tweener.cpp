/****************************************************************************
** Meta object code from reading C++ file 'tweener.h'
**
** Created: Tue Apr 12 18:45:35 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../tweener.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'tweener.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Tweener[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      14,    9,    8,    8, 0x08,
      39,    8,    8,    8, 0x08,
      51,    8,    8,    8, 0x08,
      76,   71,    8,    8, 0x08,
     109,  103,    8,    8, 0x08,
     131,    8,    8,    8, 0x08,
     144,    8,    8,    8, 0x08,
     157,    9,    8,    8, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_Tweener[] = {
    "Tweener\0\0name\0setCurrentTween(QString)\0"
    "setSelect()\0setPropertiesMode()\0mode\0"
    "updateMode(Settings::Mode)\0index\0"
    "updateStartPoint(int)\0applyReset()\0"
    "applyTween()\0removeTween(QString)\0"
};

const QMetaObject Tweener::staticMetaObject = {
    { &KTToolPlugin::staticMetaObject, qt_meta_stringdata_Tweener,
      qt_meta_data_Tweener, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Tweener::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Tweener::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Tweener::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Tweener))
        return static_cast<void*>(const_cast< Tweener*>(this));
    return KTToolPlugin::qt_metacast(_clname);
}

int Tweener::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = KTToolPlugin::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: setCurrentTween((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: setSelect(); break;
        case 2: setPropertiesMode(); break;
        case 3: updateMode((*reinterpret_cast< Settings::Mode(*)>(_a[1]))); break;
        case 4: updateStartPoint((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: applyReset(); break;
        case 6: applyTween(); break;
        case 7: removeTween((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 8;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
