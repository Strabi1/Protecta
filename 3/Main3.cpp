

#include <iostream>
#include <map>
#include <string>
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


class Client
{
private:
	
public:
	Client(uint32_t id) {}
	~Client() {}

	bool TimedOut(void) { return (( (msElapsed() - LastMsgTime_ms) > IDLE_TIMEOUT) ? true : false); }

	unsigned long LastMsgTime_ms = 0;
	uint32_t WaitSeqNumber = 2;
};



// Singleton
class Communication
{
protected:
	Communication() {}
	~Communication() {}

	static Communication *Comm;

public:
	static Communication *getInstance();

	// Prevents copying
	Communication(Communication const &comm) = delete;
	void operator=(Communication const &comm) = delete;

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

// For testing, the Communication class is an abstract class
class MyComm : public Communication
{
protected:
	MyComm() {}
	~MyComm() {}

public:

	// Prevents copying
	MyComm(MyComm const &comm) = delete;
	void operator=(MyComm const &comm) = delete;

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
	MyComm *COMM = (MyComm*)COMM->getInstance();
	//std::list<Client> clients;
	std::map<uint32_t, Client> clients;

	Communication::Msg_st *readMsg = nullptr;
	bool comError = false;

	while(true)
	{
		// timeout observer
		for (auto it = clients.begin(); it != clients.end(); ++it)
		{
			if(it->second.TimedOut())
				clients.erase(it);
		}
		

		readMsg = COMM->GetMessage(IDLE_TIMEOUT);

		if(!readMsg)
			continue;					// No messages

		if(readMsg->type == START && readMsg->seqNumber == 1)
		{
			// New client
			if(!clients.count(readMsg->clienId))
			{
				if(clients.size() < MAX_CONN)
					clients.insert(std::pair<uint32_t, Client>(readMsg->clienId, Client(readMsg->clienId)));
				else
				{
					Communication::Msg_st respMsg {.clienId = readMsg->clienId, .type = BUSY};
					COMM->SendMessage(respMsg);
					continue;
				}
			}
		}

		auto actClient = &clients[readMsg->clienId];

		if(readMsg->seqNumber != actClient->WaitSeqNumber)
			comError = true;

		else if(actClient->TimedOut())
			comError = true;		//??

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
			//actClient->WaitCommState = START;
			actClient->WaitSeqNumber = 0;
		}
		else if(readMsg->type == TEXT)
		{
			printf("%s", readMsg->msg);
		}

		if(comError || readMsg->type == STOP)
		{
			if (comError)
			{
				Communication::Msg_st respMsg {.clienId = readMsg->clienId, .type = ABORT};
				COMM->SendMessage(respMsg);
			}

			// Free message
			clients.erase(readMsg->clienId);
		}
		else
		{
			actClient->LastMsgTime_ms = msElapsed();
			actClient->WaitSeqNumber++;
		}

		free(readMsg);
	}	
}

void timeoutObserver(void);

unsigned long msElapsed(void)
{
	// Mock
	return 1;
}
