/****************************************************************************
** Meta object code from reading C++ file 'configurator.h'
**
** Created: Tue Apr 12 18:45:37 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../configurator.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'configurator.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Configurator[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
      15,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       8,       // signalCount

 // signals: signature, parameters, type, tag, flags
      20,   14,   13,   13, 0x05,
      46,   13,   13,   13, 0x05,
      62,   13,   13,   13, 0x05,
      93,   88,   13,   13, 0x05,
     126,  121,   13,   13, 0x05,
     150,   13,   13,   13, 0x05,
     170,   13,   13,   13, 0x05,
     194,   88,   13,   13, 0x05,

 // slots: signature, parameters, type, tag, flags
     216,   13,   13,   13, 0x08,
     228,   88,   13,   13, 0x08,
     246,   13,   13,   13, 0x08,
     258,   13,   13,   13, 0x08,
     272,   88,   13,   13, 0x08,
     293,   13,   13,   13, 0x08,
     316,   88,   13,   13, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_Configurator[] = {
    "Configurator\0\0index\0startingPointChanged(int)\0"
    "clickedSelect()\0clickedDefineProperties()\0"
    "name\0clickedRemoveTween(QString)\0mode\0"
    "setMode(Settings::Mode)\0clickedApplyTween()\0"
    "clickedResetInterface()\0getTweenData(QString)\0"
    "applyItem()\0addTween(QString)\0editTween()\0"
    "removeTween()\0removeTween(QString)\0"
    "closeTweenProperties()\0updateTweenData(QString)\0"
};

const QMetaObject Configurator::staticMetaObject = {
    { &QFrame::staticMetaObject, qt_meta_stringdata_Configurator,
      qt_meta_data_Configurator, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Configurator::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Configurator::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Configurator::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Configurator))
        return static_cast<void*>(const_cast< Configurator*>(this));
    return QFrame::qt_metacast(_clname);
}

int Configurator::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QFrame::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: startingPointChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: clickedSelect(); break;
        case 2: clickedDefineProperties(); break;
        case 3: clickedRemoveTween((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: setMode((*reinterpret_cast< Settings::Mode(*)>(_a[1]))); break;
        case 5: clickedApplyTween(); break;
        case 6: clickedResetInterface(); break;
        case 7: getTweenData((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 8: applyItem(); break;
        case 9: addTween((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 10: editTween(); break;
        case 11: removeTween(); break;
        case 12: removeTween((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 13: closeTweenProperties(); break;
        case 14: updateTweenData((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 15;
    }
    return _id;
}

// SIGNAL 0
void Configurator::startingPointChanged(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void Configurator::clickedSelect()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void Configurator::clickedDefineProperties()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}

// SIGNAL 3
void Configurator::clickedRemoveTween(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void Configurator::setMode(Settings::Mode _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void Configurator::clickedApplyTween()
{
    QMetaObject::activate(this, &staticMetaObject, 5, 0);
}

// SIGNAL 6
void Configurator::clickedResetInterface()
{
    QMetaObject::activate(this, &staticMetaObject, 6, 0);
}

// SIGNAL 7
void Configurator::getTweenData(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}
QT_END_MOC_NAMESPACE
