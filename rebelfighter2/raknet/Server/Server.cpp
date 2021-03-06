/*
 *  Copyright (c) 2014, Oculus VR, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant 
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

// ----------------------------------------------------------------------
// RakNet version 1.0
// Filename ChatExample.cpp
// Very basic chat engine example
// ----------------------------------------------------------------------

#include "MessageIdentifiers.h"

#include "RakPeerInterface.h"
#include "RakNetStatistics.h"
#include "RakNetTypes.h"
#include "BitStream.h"
#include "RakSleep.h"
#include "PacketLogger.h"
#include <assert.h>
#include <cstdio>
#include <cstring>
#include <stdlib.h>
#include "Kbhit.h"
#include <stdio.h>
#include <string.h>
#include "Gets.h"

#include <vector>

#ifdef _DEBUG
#pragma comment(lib, "./../../lib/RebelFighter2_LibStatic_Debug_Win32.lib")
#else
#pragma comment(lib, "./../../lib/RebelFighter2_LibStatic_Release_Win32.lib")
#endif
#pragma comment(lib, "ws2_32.lib")


#if LIBCAT_SECURITY==1
#include "SecureHandshake.h" // Include header for secure handshake
#endif

#if defined(_CONSOLE_2)
#include "Console2SampleIncludes.h"
#endif

// We copy this from Multiplayer.cpp to keep things all in one file for this example
unsigned char GetPacketIdentifier(RakNet::Packet *p);

#ifdef _CONSOLE_2
_CONSOLE_2_SetSystemProcessParams
#endif



class CUserInfoList
{
public:
	RakNet::SystemAddress	internalId;
	unsigned short			usUserNum;

	CUserInfoList() {};
	~CUserInfoList() {};

	CUserInfoList(RakNet::SystemAddress	_internalId, unsigned short	_usUserNum) {
		internalId = _internalId;
		usUserNum = _usUserNum;
	}
};

std::vector<CUserInfoList*> g_UserInfoList;

//int g_iUserCnt;

template <typename T>
unsigned short FindUserNum(T a, RakNet::Packet* p) {

	//g_iUserCnt = 0;

	T::iterator _F = a.begin();
	T::iterator _L = a.end();

	if (a.size() == 0)
		return 0;

	/*
	const char* temp1 = p->systemAddress.ToString(true); p->systemAddress.GetPort();
	const char* temp2;

	for (; _F != _L; _F++)
	{
		temp2 = (*_F)->internalId.ToString(true);
		if (strcmp(temp1,temp2 ) == 0)
			return (*_F)->usUserNum;
	}
	*/
	 
	for (; _F != _L; _F++)
	{
		if (p->systemAddress.GetPort() == (*_F)->internalId.GetPort())
			return (*_F)->usUserNum;
	}
	return 0;
}

template <typename T>
unsigned short DeleteUser(T* a, RakNet::Packet* p) {

	T::iterator _F = a->begin();
	T::iterator _L = a->end();

	if (a->size() == 0)
		return 0;

	unsigned short temp1 = 0;
	unsigned short temp2 = 0;

	temp1 = p->systemAddress.GetPort();

	for (; _F != _L; _F++)
	{
		if (*_F == 0)
			continue;

		temp2 = (*_F)->internalId.GetPort();
		if ( temp1 ==  temp2 )
		{
			if (*_F != 0)
			{
				delete (*_F);
				*_F = 0;
			}
		}

	}

	_F = a->begin();
	while (_F != a->end())
	{
		if (*_F == 0)
			_F = a->erase(_F);
		else
			_F++;
	}
	return 0;
}




int main(void)
{
	// Pointers to the interfaces of our server and client.
	// Note we can easily have both in the same program
	RakNet::RakPeerInterface *server=RakNet::RakPeerInterface::GetInstance();
	RakNet::RakNetStatistics *rss;
	server->SetIncomingPassword("Rumpelstiltskin", (int)strlen("Rumpelstiltskin"));
	server->SetTimeoutTime(30000,RakNet::UNASSIGNED_SYSTEM_ADDRESS);
//	RakNet::PacketLogger packetLogger;
//	server->AttachPlugin(&packetLogger);

#if LIBCAT_SECURITY==1
	cat::EasyHandshake handshake;
	char public_key[cat::EasyHandshake::PUBLIC_KEY_BYTES];
	char private_key[cat::EasyHandshake::PRIVATE_KEY_BYTES];
	handshake.GenerateServerKey(public_key, private_key);
	server->InitializeSecurity(public_key, private_key, false);
	FILE *fp = fopen("publicKey.dat","wb");
	fwrite(public_key,sizeof(public_key),1,fp);
	fclose(fp);
#endif

	// Holds packets
	RakNet::Packet* p;

	// GetPacketIdentifier returns this
	unsigned char packetIdentifier;

	// Record the first client that connects to us so we can pass it to the ping function
	RakNet::SystemAddress clientID=RakNet::UNASSIGNED_SYSTEM_ADDRESS;

	// Holds user data
	char portstring[30];

	//printf("This is a sample implementation of a text based chat server.\n");
	//printf("Connect to the project 'Chat Example Client'.\n");
	//printf("Difficulty: Beginner\n\n");
	

	printf("Rebelfighter2 Server. Powered by RakNet (Multiplayer game network engine). \n\n");

	// A server

	//#포트 설정.
	//puts("Enter the server port to listen on");
	//Gets(portstring,sizeof(portstring));
	//if (portstring[0]==0)
		strcpy(portstring, "1234");
	
	puts("Starting server. \n");
	// Starting the server is very simple.  2 players allowed.
	// 0 means we don't care about a connectionValidationInteger, and false
	// for low priority threads
	// I am creating two socketDesciptors, to create two sockets. One using IPV6 and the other IPV4
	RakNet::SocketDescriptor socketDescriptors[2];
	socketDescriptors[0].port=atoi(portstring);
	socketDescriptors[0].socketFamily=AF_INET; // Test out IPV4
	socketDescriptors[1].port=atoi(portstring);
	socketDescriptors[1].socketFamily=AF_INET6; // Test out IPV6
	bool b = server->Startup(4, socketDescriptors, 2 )==RakNet::RAKNET_STARTED;
	server->SetMaximumIncomingConnections(4);
	if (!b)
	{
		printf("Failed to start dual IPV4 and IPV6 ports. Trying IPV4 only.\n");

		// Try again, but leave out IPV6
		b = server->Startup(4, socketDescriptors, 1 )==RakNet::RAKNET_STARTED;
		if (!b)
		{
			puts("Server failed to start.  Terminating.");
			exit(1);
		}
	}
	server->SetOccasionalPing(true);
	server->SetUnreliableTimeout(1000);

	DataStructures::List< RakNet::RakNetSocket2* > sockets;
	server->GetSockets(sockets);
	printf("Socket addresses used by RakNet:\n");
	for (unsigned int i=0; i < sockets.Size(); i++)
	{
		printf("%i. %s\n", i+1, sockets[i]->GetBoundAddress().ToString(true));
	}

	printf("\nMy IP addresses:\n");
	for (unsigned int i=0; i < server->GetNumberOfAddresses(); i++)
	{
		RakNet::SystemAddress sa = server->GetInternalID(RakNet::UNASSIGNED_SYSTEM_ADDRESS, i);
		printf("%i. %s (LAN=%i)\n", i+1, sa.ToString(false), sa.IsLANAddress());
	}

	printf("\nMy GUID is %s\n", server->GetGuidFromSystemAddress(RakNet::UNASSIGNED_SYSTEM_ADDRESS).ToString());
	puts("'quit' to quit. 'stat' to show stats. 'ping' to ping.\n'pingip' to ping an ip address\n'ban' to ban an IP from connecting.\n'kick to kick the first connected player.\nType to talk.");
	char message[2048];

	// Loop for input
	while (1)
	{

	// This sleep keeps RakNet responsive
	RakSleep(30);

	if (kbhit())
	{
		// Notice what is not here: something to keep our network running.  It's
		// fine to block on gets or anything we want
		// Because the network engine was painstakingly written using threads.
		Gets(message,sizeof(message));

		if (strcmp(message, "quit")==0)
		{
			puts("Quitting.");
			break;
		}

		if (strcmp(message, "stat")==0)
		{
			rss=server->GetStatistics(server->GetSystemAddressFromIndex(0));
			StatisticsToString(rss, message, 2);
			printf("%s", message);
			printf("Ping %i\n", server->GetAveragePing(server->GetSystemAddressFromIndex(0)));
	
			continue;
		}

		if (strcmp(message, "ping")==0)
		{
			server->Ping(clientID);

			continue;
		}

		if (strcmp(message, "pingip")==0)
		{
			printf("Enter IP: ");
			Gets(message,sizeof(message));
			printf("Enter port: ");
			Gets(portstring,sizeof(portstring));
			if (portstring[0]==0)
				strcpy(portstring, "1234");
			server->Ping(message, atoi(portstring), false);

			continue;
		}

		if (strcmp(message, "kick")==0)
		{
			server->CloseConnection(clientID, true, 0);

			continue;
		}

		if (strcmp(message, "getconnectionlist")==0)
		{
			RakNet::SystemAddress systems[10];
			unsigned short numConnections=10;
			server->GetConnectionList((RakNet::SystemAddress*) &systems, &numConnections);
			for (int i=0; i < numConnections; i++)
			{
				printf("%i. %s\n", i+1, systems[i].ToString(true));
			}
			continue;
		}

		if (strcmp(message, "ban")==0)
		{
			printf("Enter IP to ban.  You can use * as a wildcard\n");
			Gets(message,sizeof(message));
			server->AddToBanList(message);
			printf("IP %s added to ban list.\n", message);

			continue;
		}


		// Message now holds what we want to broadcast
		char message2[2048];
		// Append Server: to the message so clients know that it ORIGINATED from the server
		// All messages to all clients come from the server either directly or by being
		// relayed from other clients
		message2[0]=0;
		const static char prefix[] = "Server: ";
		strncpy(message2, prefix, sizeof(message2));
		strncat(message2, message, sizeof(message2) - strlen(prefix) - 1);
	
		// message2 is the data to send
		// strlen(message2)+1 is to send the null terminator
		// HIGH_PRIORITY doesn't actually matter here because we don't use any other priority
		// RELIABLE_ORDERED means make sure the message arrives in the right order
		// We arbitrarily pick 0 for the ordering stream
		// RakNet::UNASSIGNED_SYSTEM_ADDRESS means don't exclude anyone from the broadcast
		// true means broadcast the message to everyone connected
		server->Send(message2, (const int) strlen(message2)+1, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
	}

		// Get a packet from either the server or the client

		for (p=server->Receive(); p; server->DeallocatePacket(p), p=server->Receive())
		{
			// We got a packet, get the identifier with our handy function
			packetIdentifier = GetPacketIdentifier(p);

			// Check if this is a network message packet
			switch (packetIdentifier)
			{
			case ID_DISCONNECTION_NOTIFICATION:
				// Connection lost normally
				printf("ID_DISCONNECTION_NOTIFICATION from %s\n", p->systemAddress.ToString(true));
				printf("Before g_UserInfoList.size(): %d\n", g_UserInfoList.size());
				DeleteUser(&g_UserInfoList, p);
				printf("After  g_UserInfoList.size(): %d\n", g_UserInfoList.size());
				//#ToDo 전체유저에게 한 유가가 접속이 끊겨서 게임 진행이 안됨을 통보.

				break;


			case ID_NEW_INCOMING_CONNECTION:
				// Somebody connected.  We have their IP now
				printf("ID_NEW_INCOMING_CONNECTION from %s with GUID %s\n", p->systemAddress.ToString(true), p->guid.ToString());
				clientID = p->systemAddress; // Record the player ID of the client

				printf("Remote internal IDs:\n");
				for (int index = 0; index < MAXIMUM_NUMBER_OF_INTERNAL_IDS; index++)
				{
					RakNet::SystemAddress internalId = server->GetInternalID(p->systemAddress, index);
					if (internalId != RakNet::UNASSIGNED_SYSTEM_ADDRESS)
					{
						printf("%i. %s\n", index + 1, internalId.ToString(true));

						//#추가함. 유저가 들어오면 벡터 컨테이너에 넣는다.
						//g_iUserCnt++;
						g_UserInfoList.push_back(new CUserInfoList(internalId, g_UserInfoList.size()+1));
					}
				}


				break;

			case ID_INCOMPATIBLE_PROTOCOL_VERSION:
				printf("ID_INCOMPATIBLE_PROTOCOL_VERSION\n");
				break;

			case ID_CONNECTED_PING:
			case ID_UNCONNECTED_PING:
				printf("Ping from %s\n", p->systemAddress.ToString(true));
				break;

			case ID_CONNECTION_LOST:
				// Couldn't deliver a reliable packet - i.e. the other system was abnormally
				// terminated
				printf("ID_CONNECTION_LOST from %s\n", p->systemAddress.ToString(true));
				printf("Before g_UserInfoList.size(): %d\n", g_UserInfoList.size());
				DeleteUser(&g_UserInfoList, p);
				printf("After  g_UserInfoList.size(): %d\n", g_UserInfoList.size());
				//#ToDo 전체유저에게 한 유가가 접속이 끊겨서 게임 진행이 안됨을 통보.

				break;
			case ID_USER_MOVE:
			{
				printf("ID_USER_MOVE\n");
				TID_USER_MOVE packet;
				packet.typeId = ID_USER_MOVE;
				memcpy(&packet.data, &(p->data[1]), sizeof(TID_USER_MOVE_DATA));
				memcpy(message, &packet, sizeof(packet));
				server->Send(message, sizeof(packet), HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
			}
			break;

			case ID_USER_LASER_FIRE:
			{
				printf("ID_USER_LASER_FIRE\n");
				TID_USER_LASER_FIRE packet;
				packet.typeId = ID_USER_LASER_FIRE;
				memcpy(&packet.data, &(p->data[1]), sizeof(TID_USER_LASER_FIRE_DATA));
				memcpy(message, &packet, sizeof(packet));
				server->Send(message, sizeof(packet), HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
			}
			break;

			case ID_USER_ASK_PLAYER_NUM:
			{
				printf("ID_USER_ASK_PLAYER_NUM received. \n");
				if (g_UserInfoList.size()==0) {
					printf("g_UserInfoList.size()==0 \n");
				}
				else {
					TID_USER_ANS_PLAYER_NUM packet;
					packet.typeId = ID_USER_ANS_PLAYER_NUM;
					packet.data.howmany = g_UserInfoList.size();
					packet.data.yournum = FindUserNum(g_UserInfoList, p);
					memcpy(message, &packet, sizeof(packet));
					server->Send(message, sizeof(packet), HIGH_PRIORITY, RELIABLE_ORDERED, 0, p->systemAddress, false);
					printf("ID_USER_ANS_PLAYER_NUM sent. \n");
				}
			}
			break;
			
			case ID_USER_ANS_PLAYER_NUM: {}break;
			case ID_USER_LASER_MOVE: {}break;
			case ID_USER_LASER_ERASE: {}break;
			case ID_USER_OBJECT_CREATE: {}break;
			case ID_USER_OBJECT_MOVE: {}break;
			case ID_USER_OBJECT_ERASE: {}break;
			case ID_USER_GAMESCORE_UPDATE: {}break;
			case ID_USER_KILLCOUNT_UPDATE: {}break;
			case ID_USER_2P_STATUS_UPDATE: {}break;

			default:
				// The server knows the static data of all clients, so we can prefix the message
				// With the name data
				printf("%s\n", p->data);

				// Relay the message.  We prefix the name for other clients.  This demonstrates
				// That messages can be changed on the server before being broadcast
				// Sending is the same as before
				sprintf(message, "%s", p->data);
				server->Send(message, (const int) strlen(message)+1, HIGH_PRIORITY, RELIABLE_ORDERED, 0, p->systemAddress, true);

				break;
			}

		}
	}

	server->Shutdown(300);
	// We're done with the network
	RakNet::RakPeerInterface::DestroyInstance(server);


	std::vector<CUserInfoList*>::iterator _F = g_UserInfoList.begin();
	std::vector<CUserInfoList*>::iterator _L = g_UserInfoList.end();

	for (; _F != _L; ++_F)
	{
		delete (*_F);
	}
	g_UserInfoList.clear();

	return 0;
}

// Copied from Multiplayer.cpp
// If the first byte is ID_TIMESTAMP, then we want the 5th byte
// Otherwise we want the 1st byte
unsigned char GetPacketIdentifier(RakNet::Packet *p)
{
	if (p==0)
		return 255;

	if ((unsigned char)p->data[0] == ID_TIMESTAMP)
	{
		RakAssert(p->length > sizeof(RakNet::MessageID) + sizeof(RakNet::Time));
		return (unsigned char) p->data[sizeof(RakNet::MessageID) + sizeof(RakNet::Time)];
	}
	else
		return (unsigned char) p->data[0];
}
