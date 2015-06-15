#ifndef WEBSOCKET_H
#define WEBSOCKET_H

class WebSocket
{
private:
	int mIP;
	int mPort;
public:
    WebSocket();
	WebSocket(int mIP, int pPort);
    int sendValue();
};

#endif // WEBSOCKET_H
