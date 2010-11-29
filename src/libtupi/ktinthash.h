/***************************************************************************
 *   Project TUPI: Magia 2D                                                *
 *   Project Contact: info@maefloresta.com                                 *
 *   Project Website: http://www.maefloresta.com                           *
 *   Project Leader: Gustav Gonzalez <info@maefloresta.com>                *
 *                                                                         *
 *   Developers:                                                           *
 *   2010:                                                                 *
 *    Gustavo Gonzalez                                                     *
 *                                                                         *
 *   KTooN's versions:                                                     * 
 *                                                                         *
 *   2006:                                                                 *
 *    David Cuadrado                                                       *
 *    Jorge Cuadrado                                                       *
 *   2003:                                                                 *
 *    Fernado Roldan                                                       *
 *    Simena Dinas                                                         *
 *                                                                         *
 *   Copyright (C) 2010 Gustav Gonzalez - http://www.maefloresta.com       *
 *   License:                                                              *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 ***************************************************************************/

#ifndef KTINTHASH_H
#define KTINTHASH_H

#include <QHash>

/**
 * @author David Cuadrado <krawek@gmail.com>
*/
template<typename T>
class KTIntHash
{
    public:
        KTIntHash();
        KTIntHash(const KTIntHash<T> &other);
        ~KTIntHash();
        
        void removeObject(int pos);
        void remove(int pos);
        void copyObject(int from, int to);
        void exchangeObject(int from, int to);
        
        int objectIndex(T val);
        
        T value(int pos);
        T takeObject(int pos);
        
        void clear(bool alsoDelete = false)
        {
            k->dataHash.clear();

            if (alsoDelete)
                qDeleteAll(k->dataHash);
 
            k->counter = 0;
        }
        
        int count() const
        {
            return k->dataHash.count();
        }
        
        bool contains(int pos);
        
        void insert(int pos, T value);
        void add(T value);
        
        void expandValue(int index, int times);
        
        T operator[](int index) const;
        QList<T> values() const;
        
        QList<int> indexes() const;
        
        bool isEmpty();
        
    public:
        KTIntHash &operator=(const KTIntHash &other);
        
    private:
        struct Private;
        Private *const k;
};

template<typename T>
struct KTIntHash<T>::Private
{
    Private() : counter(0) {}
    QHash<int, T> dataHash;
    int counter;
};

template<typename T>
KTIntHash<T>::KTIntHash() : k(new Private)
{
}

template<typename T>
KTIntHash<T>::KTIntHash(const KTIntHash<T> &other) : k(new Private)
{
    k->dataHash = other.k->dataHash;
    k->counter = other.k->counter;
}

template<typename T>
KTIntHash<T>::~KTIntHash()
{
    delete k;
}

template<typename T>
void KTIntHash<T>::removeObject(int pos)
{
    if (contains(pos))
        k->dataHash.remove(pos);
}

template<typename T>
bool KTIntHash<T>::contains(int pos)
{
    return k->dataHash.contains(pos);
}

template<typename T>
void KTIntHash<T>::copyObject(int from, int to)
{
    if (contains(from) && contains(to)) {
        T fromValue = this->value(from);
        this->insert(to, fromValue);
    } 
}

template<typename T>
void KTIntHash<T>::exchangeObject(int from, int to)
{
    if (contains(from) && contains(to)) {
        T fromValue = this->value(from);
        T toValue = this->value(to);
        this->insert(to, fromValue);
        this->insert(from, toValue);
    }
}

template<typename T>
KTIntHash<T> &KTIntHash<T>::operator=(const KTIntHash<T> &other)
{
    k->dataHash = other.k->dataHash;
    k->counter = other.k->counter;
    
    return *this;
}

template<typename T>
int KTIntHash<T>::objectIndex(T val)
{
    return k->dataHash.key(val);
}

template<typename T>
T KTIntHash<T>::value(int pos)
{
    if (contains(pos)) 
        return k->dataHash.value(pos);
    
    return k->dataHash.value(-1);
}

template<typename T>
T KTIntHash<T>::takeObject(int pos)
{
    if (contains(pos))
        return k->dataHash.take(pos);
    
    return k->dataHash.take(-1);
}

template<typename T>
void KTIntHash<T>::insert(int pos, T value)
{
    if (k->dataHash.contains(pos))
        qDebug("######   OVERRIDING!! %d", pos);
    else
        k->counter++;

    k->dataHash.insert(pos, value);
}

template<typename T>
void KTIntHash<T>::remove(int pos)
{
    if (contains(pos)) {
        if (pos == count()-1) {
            k->dataHash.remove(pos);
            k->counter--;
        } else {
            int total = count() - 1;
            for (int i=pos+1;i<=total;i++) {
                 T value = this->takeObject(i); 
                 int index = i - 1;
                 k->dataHash.insert(index, value);
            }
        }
    } 
}

template<typename T>
void KTIntHash<T>::add(T value)
{
    this->insert(k->counter, value);
}

template<typename T>
void KTIntHash<T>::expandValue(int index, int times)
{
    int limit = index + times;
    T value = this->value(index);

    for (int i=index+1; i<=limit; i++)
         k->dataHash.insert(i, value);
}

template<typename T>
T KTIntHash<T>::operator[](int index) const
{
    return k->dataHash.value(index);
}

template<typename T>
QList<T> KTIntHash<T>::values() const
{
    return k->dataHash.values();
}

template<typename T>
QList<int> KTIntHash<T>::indexes() const
{
    return k->dataHash.keys();
}

template<typename T>
bool KTIntHash<T>::isEmpty()
{
    return k->dataHash.isEmpty();
}

#endif
