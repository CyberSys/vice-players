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
//-----------------------------------------------------
//
// VC:Players Multiplayer Modification For GTA:VC
// Copyright 2010 GTA:Online team
//
// File Author(s): jenksta
//
//-----------------------------------------------------

#include "../../raknet/WindowsIncludes.h"
#include "../main.h"

extern CNetGame *pNetGame;

#include <stdio.h>
#include "scripts.h"

#include "sq_functions.h"

void logprintf(char * format, ...);
void fatal_exit(char * szError);

CScripts::CScripts()
{
	// reset all script slots
	for(int i = 0; i < MAX_SCRIPTS; i++) {
		m_pScripts[i] = NULL;
	}
	m_pTimerPool = new CTimerPool();
}

CScripts::~CScripts()
{
	for(int i = 0; i < MAX_SCRIPTS; i++) {
		if(m_pScripts[i] != NULL) delete m_pScripts[i];
		m_pScripts[i] = NULL;
	}
}

int CScripts::LoadScript(const char * szScriptName)
{
	// make sure a script with the same name isn't already loaded
	for(int i = 0; i < MAX_SCRIPTS; i++) {
		if(!m_pScripts[i]) continue;
		if(!strcmp(m_pScripts[i]->GetScriptName(), szScriptName)) {
			// a script with the same name already exists
			return false;
		}
	}

	// find a free script slot
	int iSlot = -1;

	for(int i = 0; i < MAX_SCRIPTS; i++) {
		if(!m_pScripts[i]) {
			// found a free slot
			iSlot = i;
			break;
		}
	}

	if(iSlot == -1) {
		// no free script slot found
		return false;
	}

	if(m_pScripts[iSlot] == NULL)
	{
		m_pScripts[iSlot] = new CScript(szScriptName);
	}
	// script loaded successfully
	return iSlot;
}

bool CScripts::UnloadScript(const char * szScriptName)
{
	// find the script slot
	for(int i = 0; i < MAX_SCRIPTS; i++) {
		if(!strcmp(m_pScripts[i]->GetScriptName(), szScriptName)) {
			// found the script slot, unload the script

			delete m_pScripts[i];

			m_pScripts[i] = NULL;
			// script unloaded successfully
			return true;
		}
	}

	// script not found
	return false;
}

void CScripts::onInit(int iScript)
{
		if(m_pScripts[iScript]) {
			// get the script vm pointer
			SQVM * pVM = m_pScripts[iScript]->GetVM();

			// Get the stack top
			int iTop = sq_gettop(pVM);

			// Push the root table onto the stack
			sq_pushroottable(pVM);

			// Push the function name onto the stack
			sq_pushstring(pVM, "onInit", -1);

			// Get the closure for the function
			if(SQ_SUCCEEDED(sq_get(pVM, -2))) {
				// Push the root table onto the stack
				sq_pushroottable(pVM);

				// Call the function
				sq_call(pVM, 1, true, true);
			}

			// Restore the stack top
			sq_settop(pVM, iTop);
		}
}

void CScripts::onExit()
{
	for(int i = 0; i < MAX_SCRIPTS; i++) {
		if(m_pScripts[i]) {
			// get the script vm pointer
			SQVM * pVM = m_pScripts[i]->GetVM();

			// Get the stack top
			int iTop = sq_gettop(pVM);

			// Push the root table onto the stack
			sq_pushroottable(pVM);

			// Push the function name onto the stack
			sq_pushstring(pVM, "onExit", -1);

			// Get the closure for the function
			if(SQ_SUCCEEDED(sq_get(pVM, -2))) {
				// Push the root table onto the stack
				sq_pushroottable(pVM);

				// Call the function
				sq_call(pVM, 1, true, true);
			}

			// Restore the stack top
			sq_settop(pVM, iTop);
		}
	}
}

void CScripts::onPulse()
{
	for(int i = 0; i < MAX_SCRIPTS; i++) {
		if(m_pScripts[i]) {
			// get the script vm pointer
			SQVM * pVM = m_pScripts[i]->GetVM();

			// Get the stack top
			int iTop = sq_gettop(pVM);

			// Push the root table onto the stack
			sq_pushroottable(pVM);

			// Push the function name onto the stack
			sq_pushstring(pVM, "onPulse", -1);

			// Get the closure for the function
			if(SQ_SUCCEEDED(sq_get(pVM, -2))) {
				// Push the root table onto the stack
				sq_pushroottable(pVM);

				// Call the function
				sq_call(pVM, 1, true, true);
			}

			// Restore the stack top
			sq_settop(pVM, iTop);
		}
	}
}
/*
void CScripts::onPlayerConnect(int playerId)
{
	for(int i = 0; i < MAX_SCRIPTS; i++) {
		if(m_pScripts[i]) {
			// get the script vm pointer
			SQVM * pVM = m_pScripts[i]->GetVM();

			// Get the stack top
			int iTop = sq_gettop(pVM);

			// Push the root table onto the stack
			sq_pushroottable(pVM);

			// Push the function name onto the stack
			sq_pushstring(pVM, "onPlayerConnect", -1);

			// Get the closure for the function
			if(SQ_SUCCEEDED(sq_get(pVM, -2))) {
				// Push the root table onto the stack
				sq_pushroottable(pVM);

				// Push the player id onto the stack
				sq_pushinteger(pVM, playerId);

				// Call the function
				sq_call(pVM, 2, true, true);
			}

			// Restore the stack top
			sq_settop(pVM, iTop);
		}
	}
}

void CScripts::onPlayerDisconnect(int playerId, int reason)
{
	for(int i = 0; i < MAX_SCRIPTS; i++) {
		if(m_pScripts[i]) {
			// get the script vm pointer
			SQVM * pVM = m_pScripts[i]->GetVM();

			// Get the stack top
			int iTop = sq_gettop(pVM);

			// Push the root table onto the stack
			sq_pushroottable(pVM);

			// Push the function name onto the stack
			sq_pushstring(pVM, "onPlayerDisconnect", -1);

			// Get the closure for the function
			if(SQ_SUCCEEDED(sq_get(pVM, -2))) {
				// Push the root table onto the stack
				sq_pushroottable(pVM);

				// Push the player id onto the stack
				sq_pushinteger(pVM, playerId);

				// Push the reason onto the stack
				sq_pushinteger(pVM, reason);

				// Call the function
				sq_call(pVM, 3, true, true);
			}

			// Restore the stack top
			sq_settop(pVM, iTop);
		}
	}
}

bool CScripts::onPlayerText(int playerId, const char *text)
{
	bool ret = true;
	for(int i = 0; i < MAX_SCRIPTS; i++) {
		if(m_pScripts[i]) {
			// get the script vm pointer
			SQVM * pVM = m_pScripts[i]->GetVM();

			// Get the stack top
			int iTop = sq_gettop(pVM);

			// Push the root table onto the stack
			sq_pushroottable(pVM);

			// Push the function name onto the stack
			sq_pushstring(pVM, "onPlayerText", -1);

			// Get the closure for the function
			if(SQ_SUCCEEDED(sq_get(pVM, -2))) {
				// Push the root table onto the stack
				sq_pushroottable(pVM);

				// Push the player id onto the stack
				sq_pushinteger(pVM, playerId);

				// Push the text onto the stack
				sq_pushstring(pVM, text, -1);

				// Call the function
				// Call the function
				if (!SQ_FAILED(sq_call(pVM, 3, true, true)))
				{
					SQBool result;
					sq_getbool(pVM, sq_gettop(pVM), &result);
					if(result == false) ret = false;
				}
			}

			// Restore the stack top
			sq_settop(pVM, iTop);
		}
	}
	return ret;
}

void CScripts::onPlayerCommand(int playerId, const char *command)
{
	for(int i = 0; i < MAX_SCRIPTS; i++) {
		if(m_pScripts[i]) {
			// get the script vm pointer
			SQVM * pVM = m_pScripts[i]->GetVM();

			// Get the stack top
			int iTop = sq_gettop(pVM);

			// Push the root table onto the stack
			sq_pushroottable(pVM);

			// Push the function name onto the stack
			sq_pushstring(pVM, "onPlayerCommand", -1);

			// Get the closure for the function
			if(SQ_SUCCEEDED(sq_get(pVM, -2))) {
				// Push the root table onto the stack
				sq_pushroottable(pVM);

				// Push the player id onto the stack
				sq_pushinteger(pVM, playerId);

				// Push the text onto the stack
				sq_pushstring(pVM, command, -1);

				// Call the function
				sq_call(pVM, 3, true, true);
			}

			// Restore the stack top
			sq_settop(pVM, iTop);
		}
	}
}

void CScripts::onPlayerSpawn(int playerId)
{
	for(int i = 0; i < MAX_SCRIPTS; i++) {
		if(m_pScripts[i]) {
			// get the script vm pointer
			SQVM * pVM = m_pScripts[i]->GetVM();

			// Get the stack top
			int iTop = sq_gettop(pVM);

			// Push the root table onto the stack
			sq_pushroottable(pVM);

			// Push the function name onto the stack
			sq_pushstring(pVM, "onPlayerSpawn", -1);

			// Get the closure for the function
			if(SQ_SUCCEEDED(sq_get(pVM, -2))) {
				// Push the root table onto the stack
				sq_pushroottable(pVM);

				// Push the player id onto the stack
				sq_pushinteger(pVM, playerId);

				// Call the function
				sq_call(pVM, 2, true, true);
			}

			// Restore the stack top
			sq_settop(pVM, iTop);
		}
	}
}

void CScripts::onPlayerRequestClass(int playerId, int classid)
{
	for(int i = 0; i < MAX_SCRIPTS; i++) {
		if(m_pScripts[i]) {
			// get the script vm pointer
			SQVM * pVM = m_pScripts[i]->GetVM();

			// Get the stack top
			int iTop = sq_gettop(pVM);

			// Push the root table onto the stack
			sq_pushroottable(pVM);

			// Push the function name onto the stack
			sq_pushstring(pVM, "onPlayerRequestClass", -1);

			// Get the closure for the function
			if(SQ_SUCCEEDED(sq_get(pVM, -2))) {
				// Push the root table onto the stack
				sq_pushroottable(pVM);

				// Push the player id onto the stack
				sq_pushinteger(pVM, playerId);

				// Push the class onto the stack
				sq_pushinteger(pVM, classid);

				// Call the function
				sq_call(pVM, 3, true, true);
			}

			// Restore the stack top
			sq_settop(pVM, iTop);
		}
	}
}

void CScripts::onPlayerDeath(int playerId, int killerId, int reason)
{
	for(int i = 0; i < MAX_SCRIPTS; i++) {
		if(m_pScripts[i]) {
			// get the script vm pointer
			SQVM * pVM = m_pScripts[i]->GetVM();

			// Get the stack top
			int iTop = sq_gettop(pVM);

			// Push the root table onto the stack
			sq_pushroottable(pVM);

			// Push the function name onto the stack
			sq_pushstring(pVM, "onPlayerDeath", -1);

			// Get the closure for the function
			if(SQ_SUCCEEDED(sq_get(pVM, -2))) {
				// Push the root table onto the stack
				sq_pushroottable(pVM);

				// Push the player id onto the stack
				sq_pushinteger(pVM, playerId);

				// Push the killer id onto the stack
				sq_pushinteger(pVM, killerId);

				// Push the death reason onto the stack
				sq_pushinteger(pVM, reason);

				// Call the function
				sq_call(pVM, 4, true, true);
			}

			// Restore the stack top
			sq_settop(pVM, iTop);
		}
	}
}

void CScripts::onPlayerEnterVehicle(int playerId, int vehicleid)
{
	for(int i = 0; i < MAX_SCRIPTS; i++) {
		if(m_pScripts[i]) {
			// get the script vm pointer
			SQVM * pVM = m_pScripts[i]->GetVM();

			// Get the stack top
			int iTop = sq_gettop(pVM);

			// Push the root table onto the stack
			sq_pushroottable(pVM);

			// Push the function name onto the stack
			sq_pushstring(pVM, "onPlayerEnterVehicle", -1);

			// Get the closure for the function
			if(SQ_SUCCEEDED(sq_get(pVM, -2))) {
				// Push the root table onto the stack
				sq_pushroottable(pVM);

				// Push the player id onto the stack
				sq_pushinteger(pVM, playerId);

				// Push the vehicle id onto the stack
				sq_pushinteger(pVM, vehicleid);

				// Call the function
				sq_call(pVM, 3, true, true);
			}

			// Restore the stack top
			sq_settop(pVM, iTop);
		}
	}
}

void CScripts::onPlayerExitVehicle(int playerId, int vehicleid)
{
	for(int i = 0; i < MAX_SCRIPTS; i++) {
		if(m_pScripts[i]) {
			// get the script vm pointer
			SQVM * pVM = m_pScripts[i]->GetVM();

			// Get the stack top
			int iTop = sq_gettop(pVM);

			// Push the root table onto the stack
			sq_pushroottable(pVM);

			// Push the function name onto the stack
			sq_pushstring(pVM, "onPlayerExitVehicle", -1);

			// Get the closure for the function
			if(SQ_SUCCEEDED(sq_get(pVM, -2))) {
				// Push the root table onto the stack
				sq_pushroottable(pVM);

				// Push the player id onto the stack
				sq_pushinteger(pVM, playerId);

				// Push the vehicle id onto the stack
				sq_pushinteger(pVM, vehicleid);

				// Call the function
				sq_call(pVM, 3, true, true);
			}

			// Restore the stack top
			sq_settop(pVM, iTop);
		}
	}
}

bool CScripts::onRconCommand(const char *command, const char *arg)
{
	bool found = false;
	for(int i = 0; i < MAX_SCRIPTS; i++) {
		if(m_pScripts[i]) {
			// get the script vm pointer
			SQVM * pVM = m_pScripts[i]->GetVM();

			// Get the stack top
			int iTop = sq_gettop(pVM);

			// Push the root table onto the stack
			sq_pushroottable(pVM);

			// Push the function name onto the stack
			sq_pushstring(pVM, "onRconCommand", -1);

			// Get the closure for the function
			if(SQ_SUCCEEDED(sq_get(pVM, -2))) {
				// Push the root table onto the stack
				sq_pushroottable(pVM);

				// Push the command onto the stack
				sq_pushstring(pVM, command, -1);

				// Push the command arguments onto the stack
				sq_pushstring(pVM, arg, -1);

				// Call the function
				if (!SQ_FAILED(sq_call(pVM, 3, true, true)))
				{
					SQBool result;
					sq_getbool(pVM, sq_gettop(pVM), &result);
					if(result) found = true;
				}

			}

			// Restore the stack top
			sq_settop(pVM, iTop);
		}
	}
	return found;
}

void CScripts::onPlayerSync(int playerId)
{
	for(int i = 0; i < MAX_SCRIPTS; i++) {
		if(m_pScripts[i]) {
			// get the script vm pointer
			SQVM * pVM = m_pScripts[i]->GetVM();

			// Get the stack top
			int iTop = sq_gettop(pVM);

			// Push the root table onto the stack
			sq_pushroottable(pVM);

			// Push the function name onto the stack
			sq_pushstring(pVM, "onPlayerSync", -1);

			// Get the closure for the function
			if(SQ_SUCCEEDED(sq_get(pVM, -2))) {
				// Push the root table onto the stack
				sq_pushroottable(pVM);

				// Push the player id onto the stack
				sq_pushinteger(pVM, playerId);

				// Call the function
				sq_call(pVM, 2, true, true);
			}

			// Restore the stack top
			sq_settop(pVM, iTop);
		}
	}
}

void CScripts::onPlayerDamage(int playerId, int oldhp, int newhp)
{
	for(int i = 0; i < MAX_SCRIPTS; i++) {
		if(m_pScripts[i]) {
			// get the script vm pointer
			SQVM * pVM = m_pScripts[i]->GetVM();

			// Get the stack top
			int iTop = sq_gettop(pVM);

			// Push the root table onto the stack
			sq_pushroottable(pVM);

			// Push the function name onto the stack
			sq_pushstring(pVM, "onPlayerDamage", -1);

			// Get the closure for the function
			if(SQ_SUCCEEDED(sq_get(pVM, -2))) {
				// Push the root table onto the stack
				sq_pushroottable(pVM);

				// Push the player id onto the stack
				sq_pushinteger(pVM, playerId);

				// Push the player old health onto the stack
				sq_pushinteger(pVM, oldhp);

				// Push the player new health onto the stack
				sq_pushinteger(pVM, newhp);

				// Call the function
				sq_call(pVM, 4, true, true);
			}

			// Restore the stack top
			sq_settop(pVM, iTop);
		}
	}
}

void CScripts::onVehicleCreate(int vehicleid)
{
	for(int i = 0; i < MAX_SCRIPTS; i++) {
		if(m_pScripts[i]) {
			// get the script vm pointer
			SQVM * pVM = m_pScripts[i]->GetVM();

			// Get the stack top
			int iTop = sq_gettop(pVM);

			// Push the root table onto the stack
			sq_pushroottable(pVM);

			// Push the function name onto the stack
			sq_pushstring(pVM, "onVehicleCreate", -1);

			// Get the closure for the function
			if(SQ_SUCCEEDED(sq_get(pVM, -2))) {
				// Push the root table onto the stack
				sq_pushroottable(pVM);

				// Push the vehicle id onto the stack
				sq_pushinteger(pVM, vehicleid);

				// Call the function
				sq_call(pVM, 2, true, true);
			}

			// Restore the stack top
			sq_settop(pVM, iTop);
		}
	}
}

void CScripts::onVehicleDestroy(int vehicleid)
{
	for(int i = 0; i < MAX_SCRIPTS; i++) {
		if(m_pScripts[i]) {
			// get the script vm pointer
			SQVM * pVM = m_pScripts[i]->GetVM();

			// Get the stack top
			int iTop = sq_gettop(pVM);

			// Push the root table onto the stack
			sq_pushroottable(pVM);

			// Push the function name onto the stack
			sq_pushstring(pVM, "onVehicleDestroy", -1);

			// Get the closure for the function
			if(SQ_SUCCEEDED(sq_get(pVM, -2))) {
				// Push the root table onto the stack
				sq_pushroottable(pVM);

				// Push the vehicle id onto the stack
				sq_pushinteger(pVM, vehicleid);

				// Call the function
				sq_call(pVM, 2, true, true);
			}

			// Restore the stack top
			sq_settop(pVM, iTop);
		}
	}
}

void CScripts::onVehicleSync(int vehicleid)
{
	for(int i = 0; i < MAX_SCRIPTS; i++) {
		if(m_pScripts[i]) {
			// get the script vm pointer
			SQVM * pVM = m_pScripts[i]->GetVM();

			// Get the stack top
			int iTop = sq_gettop(pVM);

			// Push the root table onto the stack
			sq_pushroottable(pVM);

			// Push the function name onto the stack
			sq_pushstring(pVM, "onVehicleSync", -1);

			// Get the closure for the function
			if(SQ_SUCCEEDED(sq_get(pVM, -2))) {
				// Push the root table onto the stack
				sq_pushroottable(pVM);

				// Push the vehicle id onto the stack
				sq_pushinteger(pVM, vehicleid);

				// Call the function
				sq_call(pVM, 2, true, true);
			}

			// Restore the stack top
			sq_settop(pVM, iTop);
		}
	}
}

void CScripts::onVehicleDamage(int vehicleId, float oldhp, float newhp)
{
	for(int i = 0; i < MAX_SCRIPTS; i++) {
		if(m_pScripts[i]) {
			// get the script vm pointer
			SQVM * pVM = m_pScripts[i]->GetVM();

			// Get the stack top
			int iTop = sq_gettop(pVM);

			// Push the root table onto the stack
			sq_pushroottable(pVM);

			// Push the function name onto the stack
			sq_pushstring(pVM, "onVehicleDamage", -1);

			// Get the closure for the function
			if(SQ_SUCCEEDED(sq_get(pVM, -2))) {
				// Push the root table onto the stack
				sq_pushroottable(pVM);

				// Push the vehicle id onto the stack
				sq_pushinteger(pVM, vehicleId);

				// Push the player old health onto the stack
				sq_pushfloat(pVM, oldhp);

				// Push the player new health onto the stack
				sq_pushfloat(pVM, newhp);

				// Call the function
				sq_call(pVM, 4, true, true);
			}

			// Restore the stack top
			sq_settop(pVM, iTop);
		}
	}
}

void CScripts::onVehicleSpawn(int vehicleid)
{
	for(int i = 0; i < MAX_SCRIPTS; i++) {
		if(m_pScripts[i]) {
			// get the script vm pointer
			SQVM * pVM = m_pScripts[i]->GetVM();

			// Get the stack top
			int iTop = sq_gettop(pVM);

			// Push the root table onto the stack
			sq_pushroottable(pVM);

			// Push the function name onto the stack
			sq_pushstring(pVM, "onVehicleSpawn", -1);

			// Get the closure for the function
			if(SQ_SUCCEEDED(sq_get(pVM, -2))) {
				// Push the root table onto the stack
				sq_pushroottable(pVM);

				// Push the vehicle id onto the stack
				sq_pushinteger(pVM, vehicleid);

				// Call the function
				sq_call(pVM, 2, true, true);
			}

			// Restore the stack top
			sq_settop(pVM, iTop);
		}
	}
}

void CScripts::onVehicleDeath(int vehicleid)
{
	for(int i = 0; i < MAX_SCRIPTS; i++) {
		if(m_pScripts[i]) {
			// get the script vm pointer
			SQVM * pVM = m_pScripts[i]->GetVM();

			// Get the stack top
			int iTop = sq_gettop(pVM);

			// Push the root table onto the stack
			sq_pushroottable(pVM);

			// Push the function name onto the stack
			sq_pushstring(pVM, "onVehicleDeath", -1);

			// Get the closure for the function
			if(SQ_SUCCEEDED(sq_get(pVM, -2))) {
				// Push the root table onto the stack
				sq_pushroottable(pVM);

				// Push the vehicle id onto the stack
				sq_pushinteger(pVM, vehicleid);

				// Call the function
				sq_call(pVM, 2, true, true);
			}

			// Restore the stack top
			sq_settop(pVM, iTop);
		}
	}
}

void CScripts::onBan(const char *szText)
{
	for(int i = 0; i < MAX_SCRIPTS; i++) {
		if(m_pScripts[i]) {
			// get the script vm pointer
			SQVM * pVM = m_pScripts[i]->GetVM();

			// Get the stack top
			int iTop = sq_gettop(pVM);

			// Push the root table onto the stack
			sq_pushroottable(pVM);

			// Push the function name onto the stack
			sq_pushstring(pVM, "onBan", -1);

			// Get the closure for the function
			if(SQ_SUCCEEDED(sq_get(pVM, -2))) {
				// Push the root table onto the stack
				sq_pushroottable(pVM);

				// Push the ban mask onto the stack
				sq_pushstring(pVM, szText, -1);

				// Call the function
				sq_call(pVM, 2, true, true);
			}

			// Restore the stack top
			sq_settop(pVM, iTop);
		}
	}
}

void CScripts::onKick(int playerId)
{
	for(int i = 0; i < MAX_SCRIPTS; i++) {
		if(m_pScripts[i]) {
			// get the script vm pointer
			SQVM * pVM = m_pScripts[i]->GetVM();

			// Get the stack top
			int iTop = sq_gettop(pVM);

			// Push the root table onto the stack
			sq_pushroottable(pVM);

			// Push the function name onto the stack
			sq_pushstring(pVM, "onKick", -1);

			// Get the closure for the function
			if(SQ_SUCCEEDED(sq_get(pVM, -2))) {
				// Push the root table onto the stack
				sq_pushroottable(pVM);

				// Push the player id onto the stack
				sq_pushinteger(pVM, playerId);

				// Call the function
				sq_call(pVM, 2, true, true);
			}

			// Restore the stack top
			sq_settop(pVM, iTop);
		}
	}
}
*/
void CScripts::onTimerCreate(int timerId)
{
	for(int i = 0; i < MAX_SCRIPTS; i++) {
		if(m_pScripts[i]) {
			// get the script vm pointer
			SQVM * pVM = m_pScripts[i]->GetVM();

			// Get the stack top
			int iTop = sq_gettop(pVM);

			// Push the root table onto the stack
			sq_pushroottable(pVM);

			// Push the function name onto the stack
			sq_pushstring(pVM, "onTimerCreate", -1);

			// Get the closure for the function
			if(SQ_SUCCEEDED(sq_get(pVM, -2))) {
				// Push the root table onto the stack
				sq_pushroottable(pVM);

				// Push the timer id onto the stack
				sq_pushinteger(pVM, timerId);

				// Call the function
				sq_call(pVM, 2, true, true);
			}

			// Restore the stack top
			sq_settop(pVM, iTop);
		}
	}
}

void CScripts::onTimerDestroy(int timerId)
{
	for(int i = 0; i < MAX_SCRIPTS; i++) {
		if(m_pScripts[i]) {
			// get the script vm pointer
			SQVM * pVM = m_pScripts[i]->GetVM();

			// Get the stack top
			int iTop = sq_gettop(pVM);

			// Push the root table onto the stack
			sq_pushroottable(pVM);

			// Push the function name onto the stack
			sq_pushstring(pVM, "onTimerDestroy", -1);

			// Get the closure for the function
			if(SQ_SUCCEEDED(sq_get(pVM, -2))) {
				// Push the root table onto the stack
				sq_pushroottable(pVM);

				// Push the timer id onto the stack
				sq_pushinteger(pVM, timerId);

				// Call the function
				sq_call(pVM, 2, true, true);
			}

			// Restore the stack top
			sq_settop(pVM, iTop);
		}
	}
}
