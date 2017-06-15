#include <algorithm>
#include <vector>
#include <iostream>
#include <memory>
#include <stdexcept>

#ifndef __AES__
#define __AES__

#include "rotate.h"
#include "includes.h"
#include "AES_Const.h"

class AES {
    public:
        enum Mode {Decrypt, Encrypt};
        AES(const std::string & KEY, Mode mod);
        std::string convert(const std::string& data);
        unsigned int blocksize() const;
    private:
        void setkey(const std::string & KEY);
        std::string Padding(std::string& str);
        std::string encrypt(const std::string & DATA);
        std::string decrypt(const std::string & DATA);
        uint8_t b, rounds, columns;
        std::vector <std::vector <uint32_t> > keys;
        void shiftrow(std::vector <uint32_t> & data);
        void invshiftrow(std::vector <uint32_t> & data);
        uint8_t GF(uint8_t a, uint8_t b);
        void mixcolumns(std::vector <uint32_t> & data);
        void invmixcolumns(std::vector <uint32_t> & data);
        std::string OUT(std::vector <uint32_t> & data);
        Mode mode;
};
#endif
