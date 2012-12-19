#pragma once
#include "../../ext/buffer.hpp"
#include "Arya.h" //for glm
#include <vector>
#include <string>
#include <vector>

using std::vector;
using std::vector;

using std::vector;
using std::string;

#define PACKETMAGICINT (('A' << 0) | ('r' << 8) | ('P' << 16) | ('k' << 24))

//The first int is always the magic int
//The second int is the size
//The thirs int is the event ID

class Packet
{
    private:
        friend class Connection;
        friend class Network;
        friend class Server;
        friend class ServerClientHandler;

        //For creating packets (for sending)
        Packet(int id) : data(12, 32), readPos(12), markedForSend(false), refCount(0)  //allocate 32 bytes by default
        {
            *(int*)&data[0] = PACKETMAGICINT;
            *(int*)&data[4] = 12;
            *(int*)&data[8] = id;
        }

        //For receiving packets
        Packet(char* databuf, int size) : data(databuf, size), readPos(12), markedForSend(false), refCount(0) {};

        ~Packet(){};

        //When getting the data, for writing it to the network, we write the size in the buffer
        char* getData() { *(int*)&data[4] = getSize(); return data.data(); }

    public:
        int getId() { return *(int*)&data[8]; }

        int getSize() const { return data.size(); }

        void send() { markedForSend = true; }

        //READ functions

        inline Packet& operator>>(int& val)
        {
            if(readPos + sizeof(int) <= data.size())
            {
                val = *(int*)&data[readPos];
                readPos += sizeof(int);
            }
            return *this;
        }

        inline Packet& operator>>(float& val)
        {
            if(readPos + sizeof(float) <= data.size())
            {
                val = *(float*)&data[readPos];
                readPos += sizeof(float);
            }
            return *this;
        }

        inline Packet& operator>>(unsigned int& val)
        {
            if(readPos + sizeof(unsigned int) <= data.size())
            {
                val = *(unsigned int*)&data[readPos];
                readPos += sizeof(unsigned int);
            }
            return *this;
        }

        inline Packet& operator>>(std::string& str)
        {
            str.clear();
            while(readPos <= data.size())
            {
                if(data[readPos] == 0) break;
                str.push_back(data[readPos]);
                ++readPos;
            }
            ++readPos;
            return *this;
        }

        inline Packet& operator>>(std::vector<int>& val)
        {
            val.clear();
            if(readPos + sizeof(unsigned int) <= data.size())
            {
                unsigned int size;
                *this >> size;
                val.resize(size);
                for(unsigned int i = 0; i < size; ++i)
                {
                    if(readPos+sizeof(int)>data.size()) break;
                    *this >> val[i];
                }
            }
            return *this;
        }

        inline Packet& operator>>(vec3& val)
        {
            *this >> val.x >> val.y >> val.z;
            return *this;
        }

        //WRITE functions

        inline Packet& operator<<(unsigned int val)
        {
            //data.append(&val, sizeof(unsigned int));
            return *this;
        }

        inline Packet& operator<<(int val)
        {
            //data.append(&val, sizeof(int));
            return *this;
        }

        inline Packet& operator<<(char val)
        {
            data.append(&val, sizeof(char));
            return *this;
        }

        inline Packet& operator<<(float val)
        {
            data.append(&val, sizeof(val));
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

        inline Packet& operator<<(const vec3& val)
        {
            *this << (float)val.x << (float)val.y << (float)val.z;
            return *this;
        }

    private:
        buffer data;
        unsigned int readPos;
        bool markedForSend;

        //The server must often send a packet to all clients
        //In this case they all add a refcount, and only when
        //all clients received the packet and the refcount is zero
        //then this packet may be deleted
        int refCount;
};

