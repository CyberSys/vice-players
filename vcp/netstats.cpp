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
// File Author(s): kyeman (in a rush)
//
//----------------------------------------------------

#include "main.h"
#include "../raknet/GetTime.h"
#include <stdio.h>

using namespace RakNet;

extern CNetGame* pNetGame;
extern GAME_SETTINGS tSettings;
extern IDirect3DDevice8 *pD3DDevice;
extern CChatWindow *pChatWindow;

CNetStats::CNetStats()
{
	m_dwLastUpdateTick = GetTickCount();
	m_dwLastTotalBytesRecv = 0;
	m_dwLastTotalBytesSent = 0;
	m_dwBPSDownload = 0;
	m_dwBPSUpload = 0;
}

void CNetStats::Draw()
{
	char szDispBuf[1024];
	D3DDISPLAYMODE dDisplayMode;
	RakNetStatistics *pRakStats = pNetGame->GetRakPeer()->GetStatistics(UNASSIGNED_SYSTEM_ADDRESS);

	unsigned int uBytesPerSecondReceived = (unsigned int)pRakStats->valueOverLastSecond[ACTUAL_BYTES_RECEIVED];
	unsigned int uBytesPerSecondSent = (unsigned int)pRakStats->valueOverLastSecond[ACTUAL_BYTES_SENT];
	unsigned int uTotalBytesReceived= (unsigned int)pRakStats->runningTotal[ACTUAL_BYTES_RECEIVED];
	unsigned int uTotalBytesSent = (unsigned int)pRakStats->runningTotal[ACTUAL_BYTES_SENT];
	unsigned int uConnectionTime = (unsigned int)((GetTimeUS() - pRakStats->connectionStartTime)/1000000);
	float fCurrentPacketLoss = pRakStats->packetlossLastSecond;
	float fAveragePacketLoss = pRakStats->packetlossTotal;

	sprintf(szDispBuf,"--- Network Stats ---\n" \
					  "\n" \
					  "Bytes Per Second Received: %d\n" \
					  "Bytes Per Second Sent: %d\n" \
					  "Total Bytes Received: %d\n" \
					  "Total Bytes Sent: %d\n" \
					  "Current Packet Loss: %.02f\n" \
					  "Average Packet Loss: %.02f\n" \
					  "Connection Time: %ds\n" \
					  "\n", 
					  uBytesPerSecondReceived, uBytesPerSecondSent, 
					  uTotalBytesReceived, uTotalBytesSent, 
					  fCurrentPacketLoss, fAveragePacketLoss, 
					  uConnectionTime);
	
//	StatisticsToString(pRakStats,szStatBuf,1);

	pD3DDevice->GetDisplayMode(&dDisplayMode);
	pChatWindow->m_pD3DFont->DrawTextBox((float)((dDisplayMode.Width / 2) - 100),50.0f,50.0f,0x50000000,0xFFFFFFFF,szDispBuf,0L);
}
