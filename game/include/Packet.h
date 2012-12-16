#pragma once
#include "../../ext/buffer.hpp"
#include <string>

#define PACKETMAGICINT (('A' << 0) | ('r' << 8) | ('P' << 16) | ('k' << 24))

//The first int of the packet is always the size
//The second int of the packet is always the event ID

class Packet
{
    private:
        friend class Connection;
        friend class Network;

        Packet(int id) : data(8, 32), readPos(8), markedForSend(false) //allocate 32 bytes by default
        {
            *(int*)&data[0] = 8;
            *(int*)&data[4] = id;
        }
        ~Packet(){};

        //When getting the data, for writing it to the network, we write the size in the buffer
        char* getData() { *(int*)&data[0] = getSize(); return data.data(); }

    public:
        int getId() { return *(int*)&data[4]; }

        int getSize() const { return data.size(); }

        void send() { markedForSend = true; }

        //READ functions

        inline Packet& operator>>(int val)
        {
            if(readPos + sizeof(int) <= data.size())
            {
                val = *(int*)&data[readPos];
                readPos += sizeof(int);
            }
            return *this;
        }

        //WRITE functions

        inline Packet& operator<<(unsigned int val)
        {
            data.append(&val, sizeof(unsigned int));
            return *this;
        }

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
            data.append(str.c_str(), str.size()+1);
            return *this;
        }

        inline Packet& operator<<(const std::vector<int>& val)
        {
            *this << (unsigned int)val.size();
            if(!val.empty()) data.append(&val.front(), sizeof(int)*val.size());
            return *this;
        }

    private:
        buffer data;
        int readPos;
        bool markedForSend;
};

