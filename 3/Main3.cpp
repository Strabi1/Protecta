
#include <cstdint>
#include <cmath>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <list>

#include <iostream>

const uint32_t MAX_CONN = 100;

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
	Client(uint32_t id);
	~Client();

	unsigned long lastMsgTime_ms = 0;
	uint32_t waitSeqNumber = 1;
	uint32_t ClientId;
	CommType WaitCommState = START;
};

Client::Client(uint32_t id): ClientId{id} {}
Client::~Client() {}


class Communication
{
private:
	bool ReservedIds[MAX_CONN];
	uint32_t ClientNumber = 0; 

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

	std::list<Client> clients;

	Msg_st *GetMessage(uint32_t clientId, uint32_t timeout) const;
	void SendMessage(uint32_t clientId, CommType type) const;
	void AddNewClient(void);
};

Communication::Communication() {}
Communication::~Communication() {}

void Communication::AddNewClient(void)
{
	if(ClientNumber >= MAX_CONN)
	{
		// Dedicated time, to notify the client that there are no more free connections
		SendMessage(MAX_CONN + 1, BUSY);
	}
	else
	{
		uint32_t i = 0;
		for (; i < MAX_CONN; i++)
		{
			if(!ReservedIds[i])
			{
				ReservedIds[i] = true;
				clients.push_back(Client(i));
				++ClientNumber;
				break;
			}
		}

		// Error
		if(i >= MAX_CONN)
			SendMessage(MAX_CONN + 1, BUSY);
	}
}

int main(int argc, char *argv[])
{
	
	return EXIT_SUCCESS;
}

void MessageHandlingLoop(void)
{
	Communication COMM;
	
	Communication::Msg_st *readMsg = nullptr;
	bool comError = false;

	while(true)
	{
		for (auto &&i : COMM.clients)
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
						if(readMsg->type != START)
							comError = true;
						else
							i.WaitCommState = TEXT;
					} break;

					case KEEP:
					case TEXT:
					{
						if(msElapsed() - i.lastMsgTime_ms > IDLE_TIMEOUT)					// overflow save, due to unsigned arithmetic
							comError = true;
						
						else if(!(readMsg->type == TEXT || readMsg->type == KEEP
							|| readMsg->type == STOP && readMsg->seqNumber >= 3))
						{
							// The stop message must be at least the 3rd message
							comError = true;
						}

						else if(readMsg->type == TEXT)
						{
							printf("%s", readMsg->msg);
						}
						else if(readMsg->type == STOP)
						{
							i.WaitCommState = START;
							i.waitSeqNumber = 1;
						}
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
				COMM.clients.remove(i);
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
