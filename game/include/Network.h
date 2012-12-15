#pragma once

class Server;

class Network
{
    public:
        Network();
        ~Network();

        void startServer();

    private:
        Server* server;
};

