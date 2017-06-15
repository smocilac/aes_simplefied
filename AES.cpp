 #include "AES.h"

void AES::shiftrow(std::vector <uint32_t> & data){
    std::vector <uint32_t> temp;
    for(uint8_t x = 0; x < 4; x++){
        temp.push_back(0);
        for(uint8_t y = 0; y < 4; y++){
            temp[x] += ((data[y] >> ((3 - x) << 3)) & 255) << ((3 - y) << 3);
        }
    }
    data = temp;

    for(uint8_t x = 0; x < 4; x++){
        data[x] = ROL(data[x], x << 3, 32);
    }

    temp.clear();
    for(uint8_t x = 0; x < 4; x++){
        temp.push_back(0);
        for(uint8_t y = 0; y < 4; y++){
            temp[x] += ((data[y] >> ((3 - x) << 3)) & 255) << ((3 - y) << 3);
        }
    }
    data = temp;
}

void AES::invshiftrow(std::vector <uint32_t> & data){
    std::vector <uint32_t> temp;
    for(uint8_t x = 0; x < 4; x++){
        temp.push_back(0);
        for(uint8_t y = 0; y < 4; y++){
            temp[x] += ((data[y] >> ((3 - x) << 3)) & 255) << ((3 - y) << 3);
        }
    }
    data = temp;

    for(uint8_t x = 0; x < 4; x++){
        data[x] = ROR(data[x], x << 3, 32);
    }

    temp.clear();
    for(uint8_t x = 0; x < 4; x++){
        temp.push_back(0);
        for(uint8_t y = 0; y < 4; y++){
            temp[x] += ((data[y] >> ((3 - x) << 3)) & 255) << ((3 - y) << 3);
        }
    }
    data = temp;
}


uint8_t AES::GF(uint8_t a, uint8_t b){
    uint8_t prim = 0x1b;
    uint8_t p = 0, i = 0;
    while ((i < 8) && (a != 0) and (b != 0)){
        if (b & 1){
            p ^= a;
        }
        uint8_t hi = a & 0x80;
        a = (a << 1) & 255;
        if (hi){
            a ^= prim;
        }
        b >>= 1;
        i += 1;
    }
    return p;
}

void AES::mixcolumns(std::vector <uint32_t> & data){
    std::vector <uint32_t> temp;
    for(uint8_t i = 0; i < 4; i++){
        temp.push_back( ((GF(2, (data[i] >> 24) & 255) ^ GF(3, (data[i] >> 16) & 255) ^ ((data[i] >> 8) & 255) ^ (data[i] & 255)) << 24) +
                        ((GF(2, (data[i] >> 16) & 255) ^ GF(3, (data[i] >> 8) & 255) ^ (data[i] & 255) ^ ((data[i] >> 24) & 255)) << 16) +
                        ((GF(2, (data[i] >> 8) & 255) ^ GF(3, data[i] & 255) ^ ((data[i] >> 24) & 255) ^ ((data[i] >> 16) & 255)) << 8 ) +
                        ((GF(2, data[i] & 255) ^ GF(3, (data[i] >> 24) & 255) ^ ((data[i] >> 16) & 255) ^ ((data[i] >> 8) & 255))));
    }
    data = temp;
}

void AES::invmixcolumns(std::vector <uint32_t> & data){
    std::vector <uint32_t> temp;
    for(uint8_t i = 0; i < 4; i++){
        temp.push_back( ((GF(14, (data[i] >> 24) & 255) ^ GF(9, (data[i] & 255)) ^ GF(13, (data[i] >> 8) & 255) ^ GF(11, (data[i] >> 16) & 255)) << 24) +
                        ((GF(14, (data[i] >> 16) & 255) ^ GF(9, (data[i] >> 24) & 255) ^ GF(13, (data[i] & 255)) ^ GF(11, (data[i] >> 8) & 255)) << 16) +
                        ((GF(14, (data[i] >> 8) & 255) ^ GF(9, (data[i] >> 16) & 255) ^ GF(13, (data[i] >> 24) & 255) ^ GF(11, (data[i] & 255))) << 8 ) +
                        ((GF(14, (data[i] & 255)) ^ GF(9, (data[i] >> 8) & 255) ^ GF(13, (data[i] >> 16) & 255) ^ GF(11, (data[i] >> 24) & 255))));
    }
    data = temp;
}

std::string AES::OUT(std::vector <uint32_t> & data){
    std::string out = "";
    for(uint8_t x = 0; x < 4; x++){
        out += makehex(data[x], 8);
    }
    return unhexlify(out);
}

AES::AES(const std::string & KEY, Mode mod) : b(0), rounds(0), columns(0), keys()
{
    this->mode = mod;
    setkey(KEY);
}

void AES::setkey(const std::string & KEY){
    std::string d = "";
    d += KEY;
    while (d.size() != 32) d += "0";

    uint8_t n = d.size();

    rounds = n / 4 + 6;
    columns = rounds - 6;
    b = (rounds + 1) << 4;
    n >>= 2;

    std::vector <uint32_t> key;
    for(uint8_t x = 0; x < columns; x++)
        key.push_back(static_cast <uint32_t> (toint(d.substr(x << 2, 4), 256)));

    uint8_t i = 1;
    while ((key.size() << 2) < b){
        uint32_t t = ROL(key[key.size() - 1], 8, 32);
        uint32_t s = 0;

        for(uint8_t j = 0; j < 4; j++){
            s += AES_Subbytes[static_cast <uint8_t> (t >> (j << 3))] << (j << 3);
        }

        t = s ^ key[key.size() - n];
        t ^= ((1 << (i++ - 1)) % 229) << 24;
        key.push_back(t);

        for(uint8_t j = 0; j < 3; j++){
            key.push_back(key[key.size() - 1]^ key[key.size() - n]);
        }

        if (n == 8){
            s = 0;
            for(uint8_t j = 0; j < 4; j++){
                s += AES_Subbytes[static_cast <uint8_t> (key[key.size() - 1] >> (j << 3))] << (j << 3);
            }
            key.push_back(s ^ key[key.size() - n]);
        }

        for(uint8_t j = 0; j < 0 * (n == 4) + 2 * (n == 6) + 3 * (n == 8); j++){
            key.push_back(key[key.size() - 1] ^ key[key.size() - n]);
        }
    }

    for(uint8_t j = 0; j < (b >> 4); j++){
        std::vector <uint32_t> temp;
        for(uint8_t k = 0; k < 4; k++){
            temp.push_back(key[(j << 2) + k]);
        }
        keys.push_back(temp);
    }
}

std::string AES::Padding(std::string& str)
{
    int val = 16 - str.size();
    std::string add = NumberToString(val);
    while (str.size() < 16) str += add;
    if (str.size() != 16) str.erase(16, 16);
    return str;
}

std::string AES::convert(const std::string& data)
{
    std::string out = "";
    std::string str;
    if (this->mode == Encrypt)
    {
        for (int counter = 0; counter != (int) (data.size() / 16); counter++)
        {
            str.clear();
            str.append(data.begin() + (counter * 16) , data.begin()+(counter * 16) + 16);
            out += encrypt(str);
        }
        if ((data.size() % 16))
        {
            str.clear();
            str.append((data.begin() + ((int)(data.size() / 16)) * 16) , data.end());
            out += encrypt(str);
        }
        str.clear();
    }
    else
    {
        if ((data.size() % 16)) throw std::runtime_error("Error: Data must be 128 bits long.");
        for (int counter = 0; counter != data.size() / 16; counter++)
        {
            str.clear();
            str.append(data.begin() + (counter * 16) , data.begin()+(counter * 16) + 16);
            out += decrypt(str);
        }
    }
    return out;
    //return (this->mode == Encrypt ? encrypt(data) : decrypt(data));
}



std::string AES::encrypt(const std::string & IN){
    std::string DATA = "";
    DATA += IN;
    if (DATA.size() != 16){
        DATA = Padding(DATA);
        if (DATA.size() != 16) throw std::runtime_error("Error: Padding.");
    }

    std::vector <uint32_t> data;
    for(uint8_t x = 0; x < 4; x++){
        std::string d = "";
        for(uint8_t y = 0; y < 4; y++){
            d += DATA[(x << 2) + y];
        }
        data.push_back(static_cast <uint32_t> (toint(d, 256)));
    }

    for(uint8_t x = 0; x < 4; x++){
        data[x] ^= keys[0][x];
    }

    for(uint8_t r = 1; r < rounds; r++){
        for(uint8_t x = 0; x < 4; x++){
            data[x] = (AES_Subbytes[data[x] >> 24] << 24) + (AES_Subbytes[(data[x] >> 16) & 255] << 16) + (AES_Subbytes[(data[x] >> 8) & 255] << 8) + AES_Subbytes[data[x] & 255];
        }
        shiftrow(data);
        mixcolumns(data);
        for(uint8_t x = 0; x < 4; x++){
            data[x] ^= keys[r][x];
        }
    }

    for(uint8_t x = 0; x < 4; x++){
        data[x] = (AES_Subbytes[data[x] >> 24] << 24) + (AES_Subbytes[(data[x] >> 16) & 255] << 16) + (AES_Subbytes[(data[x] >> 8) & 255] << 8) + AES_Subbytes[data[x] & 255];
    }

    shiftrow(data);

    for(uint8_t x = 0; x < 4; x++){
        data[x] ^= keys[rounds][x];
    }
    return OUT(data);
}

std::string AES::decrypt(const std::string & DATA){
    if (DATA.size() != 16){
        throw std::runtime_error("Error: Data must be 128 bits long.");
    }

    std::reverse(keys.begin(), keys.end());
    std::vector <uint32_t> data;
    for(uint8_t x = 0; x < 4; x++){
        std::string d = "";
        for(uint8_t y = 0; y < 4; y++){
            d += DATA[(x << 2) + y];
        }
        data.push_back(static_cast <uint32_t> (toint(d, 256)));
    }

    for(uint8_t x = 0; x < 4; x++){
        data[x] ^= keys[0][x];
    }

    for(uint8_t r = 1; r < rounds; r++){
        invshiftrow(data);
        for(uint8_t x = 0; x < 4; x++){
            data[x] = (AES_Inv_Subbytes[data[x] >> 24] << 24) + (AES_Inv_Subbytes[(data[x] >> 16) & 255] << 16) + (AES_Inv_Subbytes[(data[x] >> 8) & 255] << 8) + AES_Inv_Subbytes[data[x] & 255];
        }
        for(uint8_t x = 0; x < 4; x++){
            data[x] ^= keys[r][x];
        }
        invmixcolumns(data);
    }

    invshiftrow(data);

    for(uint8_t x = 0; x < 4; x++){
        data[x] = (AES_Inv_Subbytes[data[x] >> 24] << 24) + (AES_Inv_Subbytes[(data[x] >> 16) & 255] << 16) + (AES_Inv_Subbytes[(data[x] >> 8) & 255] << 8) + AES_Inv_Subbytes[data[x] & 255];
    }

    for(uint8_t x = 0; x < 4; x++){
        data[x] ^= keys[rounds][x];
    }

    std::reverse(keys.begin(), keys.end());
    return OUT(data);
}

unsigned int AES::blocksize() const{
    return 128;
}
//MAIN

std::string bl1 = "Ja sam dijeljiv sa 128 bita !!!!";
std::string bl2 = "jos nesto od 128";
std::string bl3 = "yzxyzxyzxyzxyzxyzxyzxyzxyzxyzxyz"; // zadnji blok ne mora nužno biti dijeljiv sa 128 da bi vrijedila jednakost
std::string r1, r2, r3;
std::string d1, d2, d3;
int main (void)
{
    AES en("Password", AES::Encrypt);
    AES de("Password", AES::Decrypt);
    // vrijedi li za enkripciju ?
    std::cout<<(r1 = en.convert(bl1))<<std::endl;
    std::cout<<(r2 = en.convert(bl2))<<std::endl;
    std::cout<<(r3 = en.convert(bl3))<<std::endl;
    if ((r1 + r2 + r3) == en.convert(bl1 + bl2 + bl3))
        std::cout<<std::endl<<"enkripcija OK!"<<std::endl;
    // sada za dekripciju ?
    std::cout<<(d1 = de.convert(r1))<<std::endl;
    std::cout<<(d2 = de.convert(r2))<<std::endl;
    std::cout<<(d3 = de.convert(r3))<<std::endl;

    if ((d1 + d2 + d3) == de.convert(r1 + r2 + r3))
        std::cout<<std::endl<<"dekripcija OK!"<<std::endl;

    return 0;
}

