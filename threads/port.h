#ifndef PORT_H
#define PORT_H

#include "synch.h"
#include "synchlist.h"

class Port {

  struct PortElement {
	int data;
	Semaphore *sem;
  };

  public:
    Port(const char* debugName);	

    ~Port();	
    const char* getName() { return (name); }

    void Send(int msg);
    void Receive(int *msg);

  private:
	const char* name;
	SynchList<PortElement> portList;
	
};

#endif
