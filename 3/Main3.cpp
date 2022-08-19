
#include <cstdint>
#include <cmath>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <list>

#include <iostream>

unsigned long msElapsed(void);
void MessageHandlingLoop(void);

typedef enum commType
{
	// client side
	START,
	KEEP,
	TEXT,
	STOP,

	// server side
	ABORT = 0x10,
	BUSY
} CommType;

const uint32_t IDLE_TIMEOUT = 30000;

class Client
{
private:
	
public:
	Client();
	~Client();

	unsigned long lastMsgTime_ms = 0;
	uint32_t waitSeqNumber = 1;
	uint32_t ClientId;
	CommType WaitCommState = START;
};

Client::Client() {}
Client::~Client() {}


class Communication
{
private:

public:
	Communication();
	~Communication();

	typedef struct msg_st
	{
		uint32_t seqNumber;
		CommType type;
		uint8_t len;
		char *msg;
	} Msg_st;

	Msg_st *GetMessage(uint32_t clientId, uint32_t timeout) const;
	void SendMessage(uint32_t clientId, CommType type) const;
};

Communication::Communication() {}
Communication::~Communication() {}


int main(int argc, char *argv[])
{
	
	return EXIT_SUCCESS;
}

void MessageHandlingLoop(void)
{
	Communication COMM;
	std::list<Client> clients;
	Communication::Msg_st *readMsg = nullptr;
	bool comError = false;

	while(true)
	{
		for (auto &&i : clients)
		{
			readMsg = COMM.GetMessage(i.ClientId, IDLE_TIMEOUT);

			if(readMsg == nullptr)
				continue;

			if(readMsg->seqNumber != i.waitSeqNumber)
			{
				comError = true;
			}
			else
			{
				switch(i.WaitCommState)
				{
					case START:
					{
						if(readMsg->type != TEXT && readMsg->type != KEEP)
							comError = true;
						else
							i.WaitCommState = readMsg->type;
					} break;

					case KEEP:
					case TEXT:
					{
						if(msElapsed() - i.lastMsgTime_ms > IDLE_TIMEOUT)					// overflow save, due to unsigned arithmetic
							comError = true;
						else if(readMsg->type == STOP)
							i.WaitCommState = STOP;
					} break;

					case STOP:
					{
						if(readMsg->type != TEXT)
							comError = true;
						else
							i.WaitCommState = START;
					} break;
					
					default:
						break;
				}
			}
			if(comError)
			{
				i.WaitCommState = START;
				COMM.SendMessage(i.ClientId, ABORT);
				free(readMsg);
			}
		}		
	}	
}

// It just needs to compile
unsigned long msElapsed(void)
{
	// Mock
	return 1;
}
