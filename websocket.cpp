#include "websocket.h"

WebSocket::WebSocket() : mIP(0), mPort(0)
{

}

WebSocket::WebSocket(int pIP, int pPort) : mIP(pIP), mPort(pPort)
{
	
}

// TODO: needs to be implemented, derp.
int
WebSocket::sendValue()
{
	return 0;
}
