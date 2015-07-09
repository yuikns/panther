// Copyright 2014 Yu Jing<yujing5b5d@gmail.com>
#include "argcv/util/util.h"

#include <cstdint>  // uint64_t

#include <algorithm>
#include <sstream>
#include <string>
#include <vector>


namespace argcv {
namespace util {

BlzKeygen::BlzKeygen() {
    // crypt table genrate
    uint64_t seed = 0x00100001, idx_a = 0, idx_b = 0, i;

    for ( idx_a = 0; idx_a < 0x100; idx_a++ ) {
        for ( idx_b = idx_a, i = 0; i < 5; i++, idx_b += 0x100 ) {
            uint64_t t1, t2;
            seed = (seed * 125 + 3) % 0x2AAAAB;
            t1 = (seed & 0xFFFF) << 0x10;
            seed = (seed * 125 + 3) % 0x2AAAAB;
            t2 = (seed & 0xFFFF);
            crypt[idx_b] = (t1|t2);
        }
    }
}


BlzKeygen::~BlzKeygen() {
}

uint64_t BlzKeygen::hash(const std::string & k, uint16_t offset) {
    uint64_t seed_a = 0x7FED7FED, seed_b = 0xEEEEEEEE;
    uint64_t ch;
    for (size_t i = 0 ; i < k.length() ; i++) {
        ch = (uint64_t)k[i];
        // ch = toupper(*k++);
        seed_a = crypt[(offset << 8) + ch] ^ (seed_a + seed_b);
        seed_b = ch + seed_a + seed_b + (seed_b << 5) + 3;
    }
    return seed_a;
}

std::vector<std::string> &split(const std::string &s, const std::string &delim,
    std::vector<std::string> *_elems) {
    std::stringstream ss(s);
    std::string item;
    if (delim.empty()) {
        _elems->push_back(s);
        return * _elems;
    }
    std::string::const_iterator substart = s.begin(), subend;
    while (true) {
        subend = search(substart, s.end(), delim.begin(), delim.end());
        std::string temp(substart, subend);
        // if (keep_empty || !temp.empty()) {
        if (!temp.empty()) {
            _elems->push_back(temp);
        }
        if (subend == s.end()) {
            break;
        }
        substart = subend + delim.size();
    }
    return * _elems;
}

std::vector<std::string> split(const std::string &s, const std::string &delim) {
    std::vector<std::string> elems;
    split(s, delim, & elems);
    return elems;
}




}  // namespace util
}  // namespace argcv

