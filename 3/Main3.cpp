
#include <cstdint>
#include <cmath>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <list>

#include <iostream>

const uint32_t MAX_CONN = 100;
const uint32_t IDLE_TIMEOUT = 30000;

unsigned long msElapsed(void);
void MessageHandlingLoop(void);

typedef enum commType
{
	START,
	KEEP,
	TEXT,
	STOP,
} CommType;

typedef enum errorType
{
	ABORT,
	BUSY
} ErrorType;

class Client
{
private:
	
public:
	Client(uint32_t id) : ClientId{id} {}
	~Client() {}

	unsigned long lastMsgTime_ms = 0;
	uint32_t waitSeqNumber = 1;
	uint32_t ClientId;
	CommType WaitCommState = START;
};

class Communication
{
private:
	bool ReservedIds[MAX_CONN];
	uint32_t ClientNumber = 0; 

public:
	Communication() {}
	~Communication() {}

	typedef struct msg_st
	{
		uint32_t seqNumber;
		CommType type;
		uint8_t len;
		char *msg;
	} Msg_st;

	std::list<Client> clients;

	Msg_st *GetMessage(uint32_t clientId, uint32_t timeout) const;
	void SendMessage(uint32_t clientId, ErrorType type) const;
	void AddNewClient(void);
	void ReleaseId(uint32_t id);
};

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

void Communication::ReleaseId(uint32_t id)
{
	if(id < MAX_CONN)
		ReservedIds[id] = false;
}

int main(int argc, char *argv[])
{
	std::cout << "Server start" << std::endl; 
	while(true)
	{
		MessageHandlingLoop();
	}
	
	return EXIT_SUCCESS;
}

void MessageHandlingLoop(void)
{
	Communication COMM;
	
	Communication::Msg_st *readMsg = nullptr;
	bool comError = false;

	while(true)
	{
		//for (auto i : COMM.clients)
		for (auto it = COMM.clients.begin(); it != COMM.clients.end(); it++)	
		{
			readMsg = COMM.GetMessage(it->ClientId, IDLE_TIMEOUT);

			if(readMsg == nullptr)
				continue;

			if(readMsg->seqNumber != it->waitSeqNumber)
			{
				comError = true;
			}
			else
			{
				switch(it->WaitCommState)
				{
					case START:
					{
						if(readMsg->type != START)
							comError = true;
						else
							it->WaitCommState = TEXT;
					} break;

					case KEEP:
					case TEXT:
					{
						// overflow save, due to unsigned arithmetic
						if(msElapsed() - it->lastMsgTime_ms > IDLE_TIMEOUT)
							comError = true;
						
						else if(!(readMsg->type == TEXT || readMsg->type == KEEP
							|| (readMsg->type == STOP && readMsg->seqNumber >= 3)))
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
							it->WaitCommState = START;
							it->waitSeqNumber = 1;
						}
					} break;
					
					default:
						break;
				}
			}
			if(comError)
			{
				it->WaitCommState = START;
				COMM.SendMessage(it->ClientId, ABORT);
				// Free message
				free(readMsg);
				// Release client id
				COMM.ReleaseId(it->ClientId);
				// Remove a client from the list
				COMM.clients.erase(it);
			}
		}
	}	
}

// It just needs to compile
void Communication::SendMessage(uint32_t clientId, ErrorType type) const
{
}

Communication::Msg_st* Communication::GetMessage(uint32_t clientId, uint32_t timeout) const
{
	return nullptr;
}

unsigned long msElapsed(void)
{
	// Mock
	return 1;
}
