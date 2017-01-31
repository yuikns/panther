// Copyright 2015 Yu Jing<yujing5b5d@gmail.com>
#ifndef INCLUDE_RDSEXTR_UTIL_H_
#define INCLUDE_RDSEXTR_UTIL_H_

#include <vector>

#include "leveldb/db.h"
#include "leveldb/cache.h"
#include "leveldb/options.h"
#include "leveldb/write_batch.h"

namespace rdsextr {

template<class T>
class MinHeap {
private:
    int size_limit;
    int size;
    int (*compare)(T,T);
    T * data;

public:
    MinHeap(int size_limit,int (*compare)(T,T)):
        size_limit(size_limit),
        size(0),
        compare(compare){
        data = new T[size_limit+1];
    }
    
    ~MinHeap(){
        delete [] data;
    }

    void adjust() {
        int pos = (size - 2) / 2;
        int csize = size;
        while (pos >= 0) {
            sift_down(pos, csize  - 1);
            pos--;
        }
    }

    void sift_down(int i, int m) {
        T t = data[i]; // T
        for (int j = 2 * i + 1; j <= m; j = 2 * j + 1) {
            if (j < m && compare(data[j], data[j + 1]) > 0) {
                j++;
            }
            if (compare(t, data[j]) <= 0) {
                break;
            } else {
                //data.set(i, data.get(j));
                data[i] = data[j];
                i = j;
            }
        }
        //data.set(i, t);
        data[i] = t;
    }

    void sift_up(int start) {
        T t = data[start]; // T
        int j = start;
        int i = (j - 1) / 2;
        while (j > 0) {
            if (compare(data[i], t) <= 0) {
                break;
            } else {
                data[j] = data[i];
                j = i;
                i = (i - 1) / 2;
            }
        }
        data[j] = t;
    }

    int get_size() {
        return size;
    }

    int get_size_limit() {
        return size_limit;
    }

    void set_size_limit(int my_size_limit) {
        size_limit = my_size_limit <= 0 ? 0 : my_size_limit;
    }

    // a > b return 1;
    // a == b return 0;
    // a < b return -1;
    //virtual int compare(T t1, T t2);
    //virtual int compare(T a, T b) {
        //printf("compare : %f - %f = ? %d\n",(double)a,(double)b,(a > b ? 1 : (a == b ? 0 : -1)));
    //    return a > b ? 1 : (a == b ? 0 : -1);
    //}

    bool pop(T & val) {
        if (size == 0)
            return false;
        T rt = data[0];
        data[0] = data[size - 1];
        size--;
        sift_down(0, size - 1);
        val = rt;
        return true;
    }
    
    bool pop() { T val; return pop(val); }

    bool push(const T & item) {
        if (size >= size_limit) { // to limit
            // pop & push
            //printf("push and pop , size : %d , size_limit : %d\n", size, size_limit);
            if (compare(item, data[0]) > 0) {
                data[size] = item;
                sift_up(size);
                size++;
                pop();
            }
        } else { // just add it
            //printf("push size : %d , size_limit : %d\n", size, size_limit);
            data[size] = item;
            sift_up(size);
            size++;
        }
        return true;
    }

    bool empty() {
        return size == 0;
    }
    
    void clear() {
        delete [] data;
        size = 0;
    }
};

//inline int int_compare(int a, int b){
//    return a > b ? 1 : (a == b ? 0 : -1);
//}

template<typename K, typename V>
inline int pair_compare_by_value(std::pair<K, V> a, std::pair<K, V> b){
    return a.second > b.second ? 1 : 
        //(a.second == b.second ? (int_compare(a.first,b.first)) : -1);
        (a.second == b.second ? 0 : -1);
}


template<typename T>
void vector_reverse(std::vector<T> & v) {
    size_t len = v.size();
    for(size_t i = 0 ; i < len / 2 ; i ++ ) {
        T t = v[i];
        v[i] = v[len - 1 - i];
        v[len - 1 - i] = t;
    }
}


//inline bool ldb_set(leveldb::DB* db, const std::string &key, const std::string &val = "") {
//    return ((db->Put(leveldb::WriteOptions(),key,val)).ok());
//}

inline bool ldb_get(leveldb::DB* db, const std::string &key, std::string *_val) {
    return ((db->Get(leveldb::ReadOptions(), key, _val)).ok());
}

inline bool ldb_batch_add(leveldb::DB* db, const std::map<std::string,std::string> & kvs) {
    leveldb::WriteBatch * _wb = new leveldb::WriteBatch;
    for (std::map<std::string,std::string>::const_iterator it= kvs.begin();
            it!=kvs.end();
            it++ ) {
        //printf("ldb_batch_add %s -> %s\n", it->first.c_str(), it->second.c_str());
        _wb->Put(it->first, it->second);
    }
    bool rst = ((db->Write(leveldb::WriteOptions(), _wb)).ok());
    delete _wb;
    return rst;
}

}

#endif // INCLUDE_RDSEXTR_UTIL_H_
