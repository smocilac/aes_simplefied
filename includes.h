#ifndef __INCLUDES__
#define __INCLUDES__

#include <cstdint>
#include <iostream>
#include <sstream>
#include <stdexcept>

const std::string b = "01";
const std::string d = b + "23456789";
const std::string h_lower = d + "abcdef";
const std::string h_upper = d + "ABCDEF";

// string u uint64_t
uint64_t toint(const std::string & s, const int & base = 10)
{
    uint64_t value = 0;
    switch (base){
        case 2:
            for(const unsigned char & c : s){
                value = (value << 1) + (static_cast <uint8_t> (c) - '\x30');
            }
            break;
        case 8:
            std::stringstream(s) >> std::oct >> value;
            break;
        case 10:
            std::stringstream(s) >> std::dec >> value;
            break;
        case 16:
            std::stringstream(s) >> std::hex >> value;
            break;
        case 256:
            for(const unsigned char & c : s){
                value = (value << 8) + static_cast <uint8_t> (c);
            }
            break;
        default:
            std::stringstream s; s << std::dec << base;
            throw std::runtime_error("Error: toint() undefined for base: " + s.str());
            break;
    };
    return value;
}
// uzeto sa StackOverflowa
template <typename T> std::string NumberToString ( T Number )
{
    std::ostringstream ss;
    ss << Number;
    std::string pom = ss.str();
    return pom;
}


template <typename T> std::string makehex(T value, unsigned int size = 2 * sizeof(T), bool caps = false){
    if (!size){
        std::stringstream out;
        out << std::hex << value;
        return out.str();
    }
    std::string out(size, '0');
    while (value && size){
        if (!caps){
            out[--size] = h_lower[value & 15];
        }
        else{
            out[--size] = h_upper[value & 15];
        }
        value >>= 4;
    }
    return out;
}


std::string unhexlify(const std::string & in){
	if (in.size() & 1){
		throw std::runtime_error("Error: input string of odd length.");
    }
    std::string out(in.size() >> 1, 0);
	for(unsigned int x = 0; x < in.size(); x += 2){
        if (('0' <= in[x]) && (in[x] <= '9')){
            out[x >> 1] = static_cast <uint8_t> ((in[x] - '0') << 4);
        }
        else if(('a' <= in[x]) && (in[x] <= 'f')){
            out[x >> 1] = static_cast <uint8_t> ((in[x] - 'a' + 10) << 4);
        }
        else if(('A' <= in[x]) && (in[x] <= 'F')){
            out[x >> 1] = static_cast <uint8_t> ((in[x] - 'A' + 10) << 4);
        }
        else{
            throw std::runtime_error("Error: Invalid character found: " + std::string(1, in[x]));
        }
        if (('0' <= in[x + 1]) && (in[x + 1] <= '9')){
            out[x >> 1] |= static_cast <uint8_t> (in[x + 1] - '0');
        }
        else if(('a' <= in[x + 1]) && (in[x + 1] <= 'f')){
            out[x >> 1] |= static_cast <uint8_t> (in[x + 1] - 'a' + 10);
        }
        else if(('A' <= in[x + 1]) && (in[x + 1] <= 'F')){
            out[x >> 1] |= static_cast <uint8_t> (in[x + 1] - 'A' + 10);
        }
        else{
            throw std::runtime_error("Error: Invalid character found: " + std::string(1, in[x + 1]));
        }
    }
	return out;
}

#endif
