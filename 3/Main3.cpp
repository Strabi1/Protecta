
#include <cstdint>
#include <cmath>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <list>

#include <iostream>

using std::string;
using std::vector;
using std::multimap;
using std::set;
using std::list;


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

class Client
{
private:
	
public:
	Client();
	~Client();

	unsigned long lastMsgTime_ms = 0;
	uint32_t ClientId;
	CommType WaitCommState = START;

};

Client::Client()
{
}

Client::~Client()
{
}


class Communication
{
private:
	//static uint32_t nexId = 1;
	

public:
	Communication();
	~Communication();

	typedef struct msg_st
	{
		uint32_t msgId;
		CommType type;
		uint8_t len;
		char *msg;
	} Msg_st;

	Msg_st *GetMessage(uint32_t clientId, uint32_t timeout) const;
	void SendMessage(uint32_t clientId, CommType type) const;
};

Communication::Communication()
{
}

Communication::~Communication()
{
}

unsigned long msElapsed(void);

void MessageHandlingLoop(void);
void MessageHandlingLoop(void)
{
	Communication COMM;
	list<Client> clients;
	Communication::Msg_st *readMsg = nullptr;
	const uint32_t IDLE_TIMEOUT = 30000;

	while(true)
	{
		for (auto &&i : clients)
		{
			readMsg = COMM.GetMessage(i.ClientId, IDLE_TIMEOUT);

			if(readMsg == nullptr)
				continue;

			switch(i.WaitCommState)
			{
				case START:
				{
					if(readMsg->type != TEXT && readMsg->type != KEEP)
					{
						i.WaitCommState = START;
						COMM.SendMessage(i.ClientId, ABORT);
						free(readMsg);
					}
					else
						i.WaitCommState = readMsg->type;
				} break;

				case KEEP:
				case TEXT:
				{
					if(msElapsed() - i.lastMsgTime_ms > IDLE_TIMEOUT)					// overflow save, due to unsigned arithmetic
					{
						i.WaitCommState = START;
						COMM.SendMessage(i.ClientId, ABORT);
						free(readMsg);
					}
					else if(readMsg->type == STOP)
						i.WaitCommState = STOP;
				}

				
				default:
					break;
			}
			
		}
			
	}
	
}


int main(int argc, char *argv[])
{
	// Stored points
	
}