#ifndef _PACKET_H 
#define _PACKET_H

#define MAX_PACKET 5000

typedef unsigned char byte;

struct packet{//共有 100bytes
	int id;
	short dataShort[5];
	long dataLong[5];
	double dataDouble[5];
	byte dataByte[6];
};

#endif
