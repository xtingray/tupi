/****************************************************************************
** Meta object code from reading C++ file 'settings.h'
**
** Created: Tue Apr 12 18:45:38 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../settings.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'settings.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_Settings[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: signature, parameters, type, tag, flags
      10,    9,    9,    9, 0x05,
      26,    9,    9,    9, 0x05,
      52,    9,    9,    9, 0x05,
      72,    9,    9,    9, 0x05,
      98,   92,    9,    9, 0x05,

 // slots: signature, parameters, type, tag, flags
     124,    9,    9,    9, 0x08,
     144,  137,    9,    9, 0x08,
     167,   92,    9,    9, 0x08,
     189,   92,    9,    9, 0x08,
     214,  208,    9,    9, 0x08,
     238,  208,    9,    9, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_Settings[] = {
    "Settings\0\0clickedSelect()\0"
    "clickedDefineProperties()\0clickedApplyTween()\0"
    "clickedResetTween()\0index\0"
    "startingPointChanged(int)\0applyTween()\0"
    "option\0emitOptionChanged(int)\0"
    "checkBottomLimit(int)\0checkTopLimit(int)\0"
    "state\0updateLoopCheckbox(int)\0"
    "updateReverseCheckbox(int)\0"
};

const QMetaObject Settings::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_Settings,
      qt_meta_data_Settings, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &Settings::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *Settings::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *Settings::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_Settings))
        return static_cast<void*>(const_cast< Settings*>(this));
    return QWidget::qt_metacast(_clname);
}

int Settings::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: clickedSelect(); break;
        case 1: clickedDefineProperties(); break;
        case 2: clickedApplyTween(); break;
        case 3: clickedResetTween(); break;
        case 4: startingPointChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: applyTween(); break;
        case 6: emitOptionChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: checkBottomLimit((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: checkTopLimit((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 9: updateLoopCheckbox((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 10: updateReverseCheckbox((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 11;
    }
    return _id;
}

// SIGNAL 0
void Settings::clickedSelect()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void Settings::clickedDefineProperties()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void Settings::clickedApplyTween()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}

// SIGNAL 3
void Settings::clickedResetTween()
{
    QMetaObject::activate(this, &staticMetaObject, 3, 0);
}

// SIGNAL 4
void Settings::startingPointChanged(int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}
QT_END_MOC_NAMESPACE
