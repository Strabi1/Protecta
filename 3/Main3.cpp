
#include <cstdint>
#include <cmath>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <list>

#include <iostream>
#include <algorithm>

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

	ABORT,
	BUSY
} CommType;

// typedef enum errorType
// {
// 	ABORT,
// 	BUSY
// } ErrorType;

class Client
{
private:
	
public:
	Client(uint32_t id) : ClientId{id} {}
	~Client() {}

	unsigned long LastMsgTime_ms = 0;
	uint32_t WaitSeqNumber = 1;
	uint32_t ClientId;
	CommType WaitCommState = START;
};

// Singleton
class Communication
{
protected:
	Communication() {}
	~Communication() {}

	static Communication *Comm;

public:
	// Prevents copying
	Communication(Communication const &comm) = delete;
	void operator=(Communication const &comm) = delete;

	static Communication *getInstance();

	typedef struct msg_st
	{
		uint32_t clienId;
		uint32_t seqNumber;
		CommType type;
		uint8_t len;
		char *msg;
	} Msg_st;

	virtual Msg_st *GetMessage(uint32_t timeout) const = 0;
	virtual void SendMessage(const Msg_st &msg) const = 0;
};

class ClientIdEqaul
{
private:
	uint32_t than;
public:
	ClientIdEqaul(uint32_t than): than{than} {}
	~ClientIdEqaul() {}
	
	bool operator()(Client &client) const { return than == client.ClientId; }
};


class MyComm : public Communication
{
protected:
	MyComm() {}
	~MyComm() {}

public:
	static Communication *getInstance()
	{
		if(!Comm)
			Comm = new MyComm;

		return Comm;
	}

	Msg_st *GetMessage(uint32_t timeout) const { return nullptr; }
	void SendMessage(const Msg_st &msg) const {}
};

Communication *Communication::Comm = nullptr;

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
	MyComm *COMM = COMM->getInstance();
	std::list<Client> clients;
	Communication::Msg_st *readMsg = nullptr;
	bool comError = false;

	while(true)
	{
		readMsg = COMM->GetMessage(IDLE_TIMEOUT);

		if(!readMsg)
			continue;					// Timeout

		auto actClient = std::find_if(clients.begin(), clients.end(), ClientIdEqaul(readMsg->clienId));

		if(actClient == clients.end())
			continue;					// Error, unknown client


		if(readMsg->seqNumber != actClient->WaitSeqNumber)
			comError = true;
			
		else
		{
			switch(actClient->WaitCommState)
			{
				case START:
				{
					if(readMsg->type != START)
						comError = true;
					else
						actClient->WaitCommState = TEXT;
				} break;

				case KEEP:
				case TEXT:
				{
					// overflow save, due to unsigned arithmetic
					if(msElapsed() - actClient->LastMsgTime_ms > IDLE_TIMEOUT)
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
						actClient->WaitCommState = START;
						actClient->WaitSeqNumber = 0;
					}
				} break;
				
				default:
					break;
			}

			actClient->WaitSeqNumber++;
		}
		if(comError)
		{
			Communication::Msg_st respMsg {.clienId = readMsg->clienId, .type = ABORT};

			actClient->WaitCommState = START;
			COMM->SendMessage(respMsg);

			// Free message
			free(readMsg);
			clients.erase(actClient);
		}
	}	
}

unsigned long msElapsed(void)
{
	// Mock
	return 1;
}
