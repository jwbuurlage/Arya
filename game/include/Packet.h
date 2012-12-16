#pragma once
#include "../../ext/buffer.hpp"
#include <string>

//The first int of the packet is always the size
//The second int of the packet is always the event ID

class Packet
{
    public:
        Packet(int id) : data(8, 32), readPos(8) { *(int*)&buffer[4] = id; } //allocate 32 bytes by default
        virtual ~Packet(){};

        int getSize() const { return buffer.size(); }
        int getId() const { return *(int*)&buffer[4]; }

        //When getting the data, for writing it to the network, we write the size in the buffer
        char* getData() { *(int*)&buffer[0] = getSize(); return buffer.data(); }

        //READ functions

        inline Packet& operator>>(int val)
        {
            if(readPos + sizeof(int) <= buffer.size())
            {
                val = *(int*)&buffer[readPos];
                readPos += sizeof(int);
            }
            return *this;
        }

        //WRITE functions

        inline Packet& operator<<(int val)
        {
            data.append(&val, sizeof(int));
            return *this;
        }

        inline Packet& operator<<(char val)
        {
            data.append(&val, sizeof(char));
            return *this;
        }

        inline Packet& operator<<(const std::string& str)
        {
            data.append(str.c_str(), str.lenght()+1);
            return *this;
        }

        inline Packet& operator<<(const std::vector<int>& val)
        {
            *this << val.size();
            if(!val.empty()) data.append(&val.front(), sizeof(int)*val.size());
        }

    private:
        buffer data;
        int readPos;
};

