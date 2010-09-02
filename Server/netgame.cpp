//----------------------------------------------------
//
// GPL code license:
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
//----------------------------------------------------
//
// VC:MP Multiplayer Modification For GTA:VC
// Copyright 2004-2005 SA:MP team
//
// File Author(s): kyeman
//
//----------------------------------------------------

#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "netgame.h"
#include "../RakNet/RPC4Plugin.h"
#include "scripts.h"

using namespace RakNet;

extern CConfig *pServerConfig;
RPC4		   *CNetGame::m_pRPC4;
extern CScripts *pScripts;

//----------------------------------------------------

CNetGame::CNetGame(int iMaxPlayers, int iPort, int iGameType, 
				   char * szPassword, char * szGameFile,
				   BYTE byteFriendlyFire, BYTE byteShowOnRadar)
{
	// Setup raknet
	m_pRakPeer = RakPeerInterface::GetInstance();
	m_pRPC4 = RPC4::GetInstance();

	//m_pRakPeer->InitializeSecurity(0,0,0,0);
	m_pRakPeer->Startup(iMaxPlayers,&SocketDescriptor(iPort, 0),1);
	m_pRakPeer->SetMaximumIncomingConnections(iMaxPlayers);
	m_pRakPeer->AttachPlugin(m_pRPC4);

	m_iMaxPlayers = iMaxPlayers;

	LoadBanList();
	
	if(szPassword != NULL) {
		m_pRakPeer->SetIncomingPassword(szPassword, strlen(szPassword));
	}

	// Register our RPC handlers
	RegisterRPCs();

	// Setup player pool
	m_pPlayerPool = new CPlayerPool();

	// Setup vehicle pool
	m_pVehiclePool = new CVehiclePool();
	//m_pVehiclePool->InitialiseFromConfig(pServerConfig);

	// Setup object pool
	m_pObjectPool = new CObjectPool();

	// Setup timer pool
	m_pTimerPool = new CTimerPool();

	// Setup the init positions.
	SetupInitPositions();

	// Init the game logic.
	m_pGameLogic = new CSpawnSelection();
		
	// Flag we're in a running state.
	m_iGameState = GAMESTATE_RUNNING;

	// Set the ff option
	m_byteFriendlyFire = byteFriendlyFire;

	// Set the show player on radar option
	m_byteShowOnRadar = byteShowOnRadar;

	srand((unsigned int)time(NULL));
}

//----------------------------------------------------

CNetGame::~CNetGame()
{
	logprintf("--- Server Shutting Down.");
	m_pRakPeer->DetachPlugin(m_pRPC4);
	UnRegisterRPCs();
	RPC4::DestroyInstance(m_pRPC4);
	RakPeerInterface::DestroyInstance(m_pRakPeer);
	delete m_pPlayerPool;
}

//----------------------------------------------------

void CNetGame::Process()
{
	UpdateNetwork();

	// Process the Players.
	m_pPlayerPool->Process();

	// Process the Vehicles.
	m_pVehiclePool->Process();

	// Process the timers
	m_pTimerPool->Process();
}

//----------------------------------------------------

void CNetGame::UpdateNetwork()
{
	Packet* p=NULL;

	while(p=m_pRakPeer->Receive())
	{
		switch(p->data[0]) {

		case ID_NEW_INCOMING_CONNECTION:
			logprintf("Incoming connection from %s", p->systemAddress.ToString(true));
			break;
		case ID_DISCONNECTION_NOTIFICATION:
			m_pPlayerPool->Delete((BYTE)p->systemAddress.systemIndex,1);
			break;
		case ID_CONNECTION_LOST:
			m_pPlayerPool->Delete((BYTE)p->systemAddress.systemIndex,0);
			break;
		case ID_PLAYER_SYNC:
			PlayerSync(p);
			break;
		case ID_VEHICLE_SYNC:
			VehicleSync(p);
			break;
		case ID_PASSENGER_SYNC:
			PassengerSync(p);
			break;
		}

		m_pRakPeer->DeallocatePacket(p);		
	}
}

//----------------------------------------------------

void CNetGame::BroadcastData( BitStream *bitStream,
							  PacketPriority priority,
							  PacketReliability reliability,
							  char orderingStream,
							  BYTE byteExcludedPlayer )
{
	BYTE x=0;
	float fDistance;
	BOOL bShouldSend;
	int r=0;

	while(x!=MAX_PLAYERS) {
		if( (m_pPlayerPool->GetSlotState(x) == TRUE) && 
			(x != byteExcludedPlayer) ) {

			 bShouldSend = FALSE;
			 fDistance = m_pPlayerPool->GetDistanceFromPlayerToPlayer(byteExcludedPlayer,x);
			 
			 if(fDistance < 250.0f) {
				bShouldSend = TRUE;
			 } else {
				 // If not within said distance, broadcast
				 // randomly once in ten.
				 r = (int)(rand() % 10);
				 if(!r) bShouldSend = TRUE;
			 }
			
			 if(bShouldSend) {
			 	 m_pRakPeer->Send(bitStream,priority,reliability,
					orderingStream,m_pRakPeer->GetSystemAddressFromIndex(x),FALSE);
			 }
		}
		x++;
	}			
}

//----------------------------------------------------

void CNetGame::PlayerSync(Packet *p)
{
	// get the player pointer
	CPlayer * pPlayer = GetPlayerPool()->GetAt((BYTE)p->systemAddress.systemIndex);

	// make sure player is in player pool
	if(!pPlayer)
	{
		// player is not in player pool
		return;
	}

	// construct the player sync bit stream
	BitStream bsPlayerSync(p->data, p->length, FALSE);

	PLAYER_SYNC_DATA playerSyncData;
	S_CAMERA_AIM caAiming;

	// ignore the packet id
	bsPlayerSync.IgnoreBytes(sizeof(MessageID));

	// read the player sync data struct
	bsPlayerSync.Read((char *)&playerSyncData, sizeof(PLAYER_SYNC_DATA));

	// read weather the bit stream has aim sync data or not
	bool bHasAimSync = bsPlayerSync.ReadBit();

	// read aim sync data if the bit stream has it
	if(bHasAimSync)
	{
		// read the aim sync data
		bsPlayerSync.Read((char *)&caAiming, sizeof(S_CAMERA_AIM));
	}

	// store the player sync data
	pPlayer->StoreOnFootFullSyncData(&playerSyncData);

	// store the aim sync data if the bit stream has it
	if(bHasAimSync) {
		pPlayer->StoreAimSyncData(&caAiming);
	}
}

//----------------------------------------------------

void CNetGame::VehicleSync(Packet *p)
{
	// get the player pointer
	CPlayer * pPlayer = GetPlayerPool()->GetAt((BYTE)p->systemAddress.systemIndex);

	// make sure player is in player pool
	if(!pPlayer)
	{
		// player is not in player pool
		return;
	}

	// construct the vehicle sync bit stream
	BitStream bsVehicleSync(p->data, p->length, FALSE);

	VEHICLE_SYNC_DATA vehicleSyncData;

	// ignore the packet id
	bsVehicleSync.IgnoreBytes(sizeof(MessageID));

	// read the vehicle sync data struct
	bsVehicleSync.Read((char *)&vehicleSyncData, sizeof(VEHICLE_SYNC_DATA));

	// store the vehicle sync data
	pPlayer->StoreInCarFullSyncData(&vehicleSyncData);
}

//----------------------------------------------------

void CNetGame::PassengerSync(Packet *p)
{
	CPlayer * pPlayer = GetPlayerPool()->GetAt((BYTE)p->systemAddress.systemIndex);
	BitStream bsPassengerSync(p->data, p->length, FALSE);
	BitStream bsPassengerSend;

	BYTE		vehicleID=0;
	UINT		uiPassengerSeat;
	float		x,y,z;

	bsPassengerSync.IgnoreBytes(sizeof(MessageID));
	bsPassengerSync.Read(vehicleID);
	bsPassengerSync.Read(uiPassengerSeat);
	bsPassengerSync.Read(x);
	bsPassengerSync.Read(y);
	bsPassengerSync.Read(z);

	pPlayer->UpdatePosition(x,y,z);

	// Now broadcast it.
	bsPassengerSend.Write((BYTE)p->systemAddress.systemIndex);
	bsPassengerSend.Write(vehicleID);
	bsPassengerSend.Write(uiPassengerSeat);
	m_pRPC4->Call("Passenger", &bsPassengerSend,HIGH_PRIORITY,RELIABLE,0,p->systemAddress,TRUE);

	pScripts->onPlayerSync((BYTE)p->systemAddress.systemIndex);

}

//----------------------------------------------------

void CNetGame::SetupInitPositions()
{	
	char *szParseMe;

	// Init Player Pos
	szParseMe = pServerConfig->GetConfigEntryAsString("PLAYERPOS");
	if(!szParseMe) { 
		logprintf("I can't init a network game because it's missing a PLAYERPOS setting.");
		exit(1);
	}
	sscanf(szParseMe,"%f%f%f",&m_vecInitPlayerPos.X,&m_vecInitPlayerPos.Y,&m_vecInitPlayerPos.Z);

	// Init Camera Pos
	szParseMe = pServerConfig->GetConfigEntryAsString("CAMPOS");
	if(!szParseMe) { 
		logprintf("I can't init a network game because it's missing a CAMPOS setting.");
		exit(1);
	}
	sscanf(szParseMe,"%f%f%f",&m_vecInitCameraPos.X,&m_vecInitCameraPos.Y,&m_vecInitCameraPos.Z);

	// Init Camera Look
	szParseMe = pServerConfig->GetConfigEntryAsString("CAMLOOK");
	if(!szParseMe) { 
		logprintf("I can't init a network game because it's missing a CAMLOOK setting.");
		exit(1);
	}
	sscanf(szParseMe,"%f%f%f",&m_vecInitCameraLook.X,&m_vecInitCameraLook.Y,&m_vecInitCameraLook.Z);

	// Init World Bounds
	szParseMe = pServerConfig->GetConfigEntryAsString("WORLDBOUNDS");
	if(!szParseMe) { 
		logprintf("I can't init a network game because it's missing a WORLDBOUNDS setting.");
		exit(1);
	}
	sscanf(szParseMe,"%f%f%f%f",&m_WorldBounds[0],&m_WorldBounds[1],&m_WorldBounds[2],&m_WorldBounds[3]);
}

//----------------------------------------------------

void CNetGame::KickPlayer(BYTE byteKickPlayer)
{
	if (byteKickPlayer < MAX_PLAYERS)
	{
		if (m_pPlayerPool->GetSlotState(byteKickPlayer))
		{
			m_pPlayerPool->Delete(byteKickPlayer,2);
			pScripts->onKick(byteKickPlayer);
		}
	}
}

//----------------------------------------------------

void CNetGame::AddBan(char * ip_mask)
{
	m_pRakPeer->AddToBanList(ip_mask);
	
	FILE * fileBanList = fopen("vcp-svr.banlist","a");
	if(!fileBanList) return;

	fprintf(fileBanList,"%s\n",ip_mask);

	fclose(fileBanList);

	pScripts->onBan(ip_mask);
}

//----------------------------------------------------

void CNetGame::LoadBanList()
{
	FILE * fileBanList = fopen("vcp-svr.banlist","r");
	if(!fileBanList) return;

	char ban_ip[256];

	while(!feof(fileBanList)) {
		fgets(ban_ip,256,fileBanList);
		ban_ip[strlen(ban_ip) - 1] = '\0';
		m_pRakPeer->AddToBanList(ban_ip);
	}

	fclose(fileBanList);
}

//----------------------------------------------------


