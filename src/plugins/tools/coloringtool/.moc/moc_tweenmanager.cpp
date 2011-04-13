/****************************************************************************
** Meta object code from reading C++ file 'tweenmanager.h'
**
** Created: Wed Apr 13 01:47:46 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../tweenmanager.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'tweenmanager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_TweenManager[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: signature, parameters, type, tag, flags
      19,   14,   13,   13, 0x05,
      40,   14,   13,   13, 0x05,
      66,   14,   13,   13, 0x05,
      94,   14,   13,   13, 0x05,

 // slots: signature, parameters, type, tag, flags
     116,   13,   13,   13, 0x08,
     127,   13,   13,   13, 0x08,
     144,  139,   13,   13, 0x08,
     172,   13,   13,   13, 0x08,
     192,  186,   13,   13, 0x08,
     209,  139,   13,   13, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_TweenManager[] = {
    "TweenManager\0\0name\0addNewTween(QString)\0"
    "editCurrentTween(QString)\0"
    "removeCurrentTween(QString)\0"
    "getTweenData(QString)\0addTween()\0"
    "editTween()\0item\0editTween(QListWidgetItem*)\0"
    "removeTween()\0point\0showMenu(QPoint)\0"
    "updateTweenData(QListWidgetItem*)\0"
};

const QMetaObject TweenManager::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_TweenManager,
      qt_meta_data_TweenManager, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &TweenManager::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *TweenManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *TweenManager::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_TweenManager))
        return static_cast<void*>(const_cast< TweenManager*>(this));
    return QWidget::qt_metacast(_clname);
}

int TweenManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: addNewTween((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: editCurrentTween((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: removeCurrentTween((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: getTweenData((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: addTween(); break;
        case 5: editTween(); break;
        case 6: editTween((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        case 7: removeTween(); break;
        case 8: showMenu((*reinterpret_cast< const QPoint(*)>(_a[1]))); break;
        case 9: updateTweenData((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 10;
    }
    return _id;
}

// SIGNAL 0
void TweenManager::addNewTween(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void TweenManager::editCurrentTween(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void TweenManager::removeCurrentTween(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void TweenManager::getTweenData(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_END_MOC_NAMESPACE
