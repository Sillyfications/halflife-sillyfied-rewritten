/***
*
*	Copyright (c) 1996-2001, Valve LLC. All rights reserved.
*   Modified by Sillyfications! for the Half-Life mod Half-Life: Sillyfied.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/


//The Bernett AMS-12 
//The AMS-12 is a semi-automatic shotgun that has a slow firing speed but high accuracy. Alt fire makes the weapon shoot faster at a lower accuracy.
//Ammo: 12 GAUGE
//Damage: 9x8 (72)
//Firerate: 120 (180 alt fire)
//Magazine: Tube fed, 5/25
//DPS: 0-144 (0-216 alt fire)
//CND: 25 shots before malfunction
//Accuracy: 4 degrees (6 degrees alt fire)
//Recoil: 10 upwards, 5 backwards


#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "player.h"
#include "gamerules.h"
#include "UserMessages.h"

// special deathmatch shotgun spreads
#define VECTOR_CONE_DM_SHOTGUN Vector(0.08716, 0.04362, 0.00)		// 10 degrees by 5 degrees
#define VECTOR_CONE_DM_DOUBLESHOTGUN Vector(0.17365, 0.04362, 0.00) // 20 degrees by 5 degrees

LINK_ENTITY_TO_CLASS(weapon_shotgun, CShotgun);

void CShotgun::Spawn()
{
	Precache();
	m_iId = WEAPON_SHOTGUN;
	SET_MODEL(ENT(pev), "models/w_shotgun.mdl");
	m_iSecondaryAmmoType -= 26; // reset CND
	m_iDefaultAmmo = SHOTGUN_DEFAULT_GIVE;

	FallInit(); // get ready to fall
}


void CShotgun::Precache()
{
	PRECACHE_MODEL("models/v_shotgun.mdl");
	PRECACHE_MODEL("models/w_shotgun.mdl");
	PRECACHE_MODEL("models/p_shotgun.mdl");

	m_iShell = PRECACHE_MODEL("models/shotgunshell.mdl"); // shotgun shell

	PRECACHE_SOUND("items/9mmclip1.wav");

	PRECACHE_SOUND("weapons/dbarrel1.wav"); //shotgun
	PRECACHE_SOUND("weapons/sbarrel1.wav"); //shotgun

	PRECACHE_SOUND("weapons/reload1.wav"); // shotgun reload
	PRECACHE_SOUND("weapons/reload3.wav"); // shotgun reload

	//	PRECACHE_SOUND ("weapons/sshell1.wav");	// shotgun reload - played on client
	//	PRECACHE_SOUND ("weapons/sshell3.wav");	// shotgun reload - played on client

	PRECACHE_SOUND("weapons/357_cock1.wav"); // gun empty sound
	PRECACHE_SOUND("weapons/scock1.wav");	 // cock gun

	m_usSingleFire = PRECACHE_EVENT(1, "events/shotgun1.sc");
	m_usDoubleFire = PRECACHE_EVENT(1, "events/shotgun2.sc");
}

bool CShotgun::GetItemInfo(ItemInfo* p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "buckshot";
	p->iMaxAmmo1 = BUCKSHOT_MAX_CARRY;
	p->pszAmmo2 = "CNDShotgun";
	p->iMaxAmmo2 = 26;
	p->iMaxClip = SHOTGUN_MAX_CLIP;
	p->iSlot = 4;
	p->iPosition = 0;
	p->iFlags = 0;
	p->iId = m_iId = WEAPON_SHOTGUN;
	p->iWeight = SHOTGUN_WEIGHT;

	return true;
}



bool CShotgun::Deploy()
{
	return DefaultDeploy("models/v_shotgun.mdl", "models/p_shotgun.mdl", SHOTGUN_DRAW, "shotgun");
}

void CShotgun::PrimaryAttack()
{
	// don't fire underwater
	if (m_pPlayer->pev->waterlevel == 3)
	{
		PlayEmptySound();
		m_flNextPrimaryAttack = GetNextAttackDelay(0.15);
		return;
	}

	if (m_iClip <= 0)
	{
		Reload();
		if (m_iClip == 0)
			PlayEmptySound();
		return;
	}

	if (m_iSecondaryAmmoType == 26) // no DP... no shoot....
	{
		PlayEmptySound();
		return;
	}

	

	m_pPlayer->m_iWeaponVolume = LOUD_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = NORMAL_GUN_FLASH;

	m_iClip--;

	int flags;
#if defined(CLIENT_WEAPONS)
	flags = FEV_NOTHOST;
#else
	flags = 0;
#endif


	m_pPlayer->pev->effects = (int)(m_pPlayer->pev->effects) | EF_MUZZLEFLASH;

	Vector vecSrc = m_pPlayer->GetGunPosition();
	Vector vecAiming = m_pPlayer->GetAutoaimVector(AUTOAIM_5DEGREES);

	Vector vecDir;

#ifdef CLIENT_DLL
	if (bIsMultiplayer())
#else
	if (g_pGameRules->IsMultiplayer())
#endif
	{
		vecDir = m_pPlayer->FireBulletsPlayer(4, vecSrc, vecAiming, VECTOR_CONE_DM_SHOTGUN, 2048, BULLET_PLAYER_BUCKSHOT, 0, 0, m_pPlayer->pev, m_pPlayer->random_seed);
	}
	else
	{
		// regular old, untouched spread.
		vecDir = m_pPlayer->FireBulletsPlayer(9, vecSrc, vecAiming, VECTOR_CONE_4DEGREES, 8196, BULLET_PLAYER_BUCKSHOT, 0, 0, m_pPlayer->pev, m_pPlayer->random_seed); // 9 times 8 = 72 damage
	}

	PLAYBACK_EVENT_FULL(flags, m_pPlayer->edict(), m_usSingleFire, 0.0, g_vecZero, g_vecZero, vecDir.x, vecDir.y, 0, 0, 0, 0);


	if (0 == m_iClip && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
		// HEV suit - indicate out of ammo condition
		m_pPlayer->SetSuitUpdate("!HEV_AMO0", false, 0);

	//if (m_iClip != 0)
	m_flPumpTime = gpGlobals->time + 0.5;
	m_iSecondaryAmmoType++; // remove 4 CMD because 72 damage

	// start weapon condition message
	if (m_iSecondaryAmmoType == 1)
	{
		ClientPrint(m_pPlayer->pev, HUD_PRINTTALK, "Your Bernett AMS-12 is starting to show some wear.");
	}
	if (m_iSecondaryAmmoType == 6)
	{
		ClientPrint(m_pPlayer->pev, HUD_PRINTTALK, "Your Bernett AMS-12 has now 80 CND.");
	}
	if (m_iSecondaryAmmoType == 11)
	{
		ClientPrint(m_pPlayer->pev, HUD_PRINTTALK, "Your Bernett AMS-12 has now 60 CND.");
	}
	if (m_iSecondaryAmmoType == 16)
	{
		ClientPrint(m_pPlayer->pev, HUD_PRINTTALK, "Your Bernett AMS-12 has now 40 CND, try to find a replacement weapon or repair your current weapon.");
	}
	if (m_iSecondaryAmmoType == 21)
	{
		ClientPrint(m_pPlayer->pev, HUD_PRINTTALK, "Your Bernett AMS-12 has now 20 CND, It will malfunction soon, so try to switch to a different weapon.");
	}
	if (m_iSecondaryAmmoType == 24)
	{
		ClientPrint(m_pPlayer->pev, HUD_PRINTTALK, "Your Bernett AMS-12 has now 8 CND, It will malfunction, Switch to a different weapon.");
	}
	if (m_iSecondaryAmmoType == 26)
	{
		ClientPrint(m_pPlayer->pev, HUD_PRINTTALK, "Your Bernett AMS-12 is now broken. Switch to a different weapon.");
	}
	// end weapon condition messages
	m_flNextPrimaryAttack = GetNextAttackDelay(0.50);
	m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.50;
	if (m_iClip != 0)
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 5.0;
	else
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1;
	m_fInSpecialReload = 0;
}


void CShotgun::SecondaryAttack()
{
	{
		// don't fire underwater
		if (m_pPlayer->pev->waterlevel == 3)
		{
			PlayEmptySound();
			m_flNextPrimaryAttack = GetNextAttackDelay(0.15);
			return;
		}

		if (m_iClip <= 0)
		{
			Reload();
			if (m_iClip == 0)
				PlayEmptySound();
			return;
		}

		if (m_iSecondaryAmmoType == 26) // no DP... no shoot....
		{
			PlayEmptySound();
			ClientPrint(m_pPlayer->pev, HUD_PRINTTALK, "Your Bernett AMS-12 is broken. Switch to a different weapon.");
			return;
		}

		

		m_pPlayer->m_iWeaponVolume = LOUD_GUN_VOLUME;
		m_pPlayer->m_iWeaponFlash = NORMAL_GUN_FLASH;

		m_iClip--;

		int flags;
#if defined(CLIENT_WEAPONS)
		flags = FEV_NOTHOST;
#else
		flags = 0;
#endif


		m_pPlayer->pev->effects = (int)(m_pPlayer->pev->effects) | EF_MUZZLEFLASH;

		Vector vecSrc = m_pPlayer->GetGunPosition();
		Vector vecAiming = m_pPlayer->GetAutoaimVector(AUTOAIM_5DEGREES);

		Vector vecDir;

#ifdef CLIENT_DLL
		if (bIsMultiplayer())
#else
		if (g_pGameRules->IsMultiplayer())
#endif
		{
			vecDir = m_pPlayer->FireBulletsPlayer(4, vecSrc, vecAiming, VECTOR_CONE_DM_SHOTGUN, 2048, BULLET_PLAYER_BUCKSHOT, 0, 0, m_pPlayer->pev, m_pPlayer->random_seed);
		}
		else
		{
			// regular old, untouched spread.
			vecDir = m_pPlayer->FireBulletsPlayer(9, vecSrc, vecAiming, VECTOR_CONE_6DEGREES, 8196, BULLET_PLAYER_BUCKSHOT, 0, 0, m_pPlayer->pev, m_pPlayer->random_seed);
		}

		PLAYBACK_EVENT_FULL(flags, m_pPlayer->edict(), m_usSingleFire, 0.0, g_vecZero, g_vecZero, vecDir.x, vecDir.y, 0, 0, 0, 0);


		if (0 == m_iClip && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
			// HEV suit - indicate out of ammo condition
			m_pPlayer->SetSuitUpdate("!HEV_AMO0", false, 0);

		// if (m_iClip != 0)
		m_flPumpTime = gpGlobals->time + 0.5;
		m_iSecondaryAmmoType++; // remove 4 CMD because 72 damage

		// start weapon condition message
		if (m_iSecondaryAmmoType == 1)
		{
			ClientPrint(m_pPlayer->pev, HUD_PRINTTALK, "Your Bernett AMS-12 is starting to show some wear.");
		}
		if (m_iSecondaryAmmoType == 6)
		{
			ClientPrint(m_pPlayer->pev, HUD_PRINTTALK, "Your Bernett AMS-12 has now 80 CND.");
		}
		if (m_iSecondaryAmmoType == 11)
		{
			ClientPrint(m_pPlayer->pev, HUD_PRINTTALK, "Your Bernett AMS-12 has now 60 CND.");
		}
		if (m_iSecondaryAmmoType == 16)
		{
			ClientPrint(m_pPlayer->pev, HUD_PRINTTALK, "Your Bernett AMS-12 has now 40 CND, try to find a replacement weapon or repair your current weapon.");
		}
		if (m_iSecondaryAmmoType == 21)
		{
			ClientPrint(m_pPlayer->pev, HUD_PRINTTALK, "Your Bernett AMS-12 has now 20 CND, It will malfunction soon, so try to switch to a different weapon.");
		}
		if (m_iSecondaryAmmoType == 24)
		{
			ClientPrint(m_pPlayer->pev, HUD_PRINTTALK, "Your Bernett AMS-12 has now 8 CND, It will malfunction, Switch to a different weapon.");
		}
		if (m_iSecondaryAmmoType == 26)
		{
			ClientPrint(m_pPlayer->pev, HUD_PRINTTALK, "Your Bernett AMS-12 is now broken. Switch to a different weapon.");
		}
		// end weapon condition messages

		m_flNextPrimaryAttack = GetNextAttackDelay(0.30);
		m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.30;
		if (m_iClip != 0)
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 5.0;
		else
			m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1;
		m_fInSpecialReload = 0;
	}
}


void CShotgun::Reload()
{
	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0 || m_iClip == SHOTGUN_MAX_CLIP)
		return;

	// don't reload until recoil is done
	if (m_flNextPrimaryAttack > UTIL_WeaponTimeBase())
		return;

	// check to see if we're ready to reload
	if (m_fInSpecialReload == 0)
	{
		SendWeaponAnim(SHOTGUN_START_RELOAD);
		m_fInSpecialReload = 1;
		m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.8;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.8;
		m_flNextPrimaryAttack = GetNextAttackDelay(1.0);
		m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 1.0;
		return;
	}
	else if (m_fInSpecialReload == 1)
	{
		if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase())
			return;
		// was waiting for gun to move to side
		m_fInSpecialReload = 2;

		if (RANDOM_LONG(0, 1))
			EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/reload1.wav", 1, ATTN_NORM, 0, 85 + RANDOM_LONG(0, 0x1f));
		else
			EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/reload3.wav", 1, ATTN_NORM, 0, 85 + RANDOM_LONG(0, 0x1f));

		SendWeaponAnim(SHOTGUN_RELOAD);

		m_flNextReload = UTIL_WeaponTimeBase() + 0.8;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.8;
	}
	else
	{
		// Add them to the clip
		m_iClip += 1;
		m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] -= 1;
		m_fInSpecialReload = 1;
	}
}


void CShotgun::WeaponIdle()
{
	ResetEmptySound();

	m_pPlayer->GetAutoaimVector(AUTOAIM_5DEGREES);

	//Moved to ItemPostFrame
	/*
	if ( m_flPumpTime && m_flPumpTime < gpGlobals->time )
	{
		// play pumping sound
		EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/scock1.wav", 1, ATTN_NORM, 0, 95 + RANDOM_LONG(0,0x1f));
		m_flPumpTime = 0;
	}
	*/

	if (m_flTimeWeaponIdle < UTIL_WeaponTimeBase())
	{
		if (m_iClip == 0 && m_fInSpecialReload == 0 && 0 != m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType])
		{
			Reload();
		}
		else if (m_fInSpecialReload != 0)
		{
			if (m_iClip != 5 && 0 != m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType])
			{
				Reload();
			}
			else
			{
				// reload debounce has timed out
				SendWeaponAnim(SHOTGUN_PUMP);

				
				m_fInSpecialReload = 0;
				m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.43;
			}
		}
		else
		{
			int iAnim;
			float flRand = UTIL_SharedRandomFloat(m_pPlayer->random_seed, 0, 1);
			if (flRand <= 0.8)
			{
				iAnim = SHOTGUN_IDLE_DEEP;
				m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + (60.0 / 12.0); // * RANDOM_LONG(2, 5);
			}
			else if (flRand <= 0.95)
			{
				iAnim = SHOTGUN_IDLE;
				m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + (20.0 / 9.0);
			}
			else
			{
				iAnim = SHOTGUN_IDLE4;
				m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + (20.0 / 9.0);
			}
			SendWeaponAnim(iAnim);
		}
	}
}

void CShotgun::ItemPostFrame()
{
	if (0 != m_flPumpTime && m_flPumpTime < gpGlobals->time)
	{
		
		m_flPumpTime = 0;
	}

	CBasePlayerWeapon::ItemPostFrame();
}


class CShotgunAmmo : public CBasePlayerAmmo
{
	void Spawn() override
	{
		Precache();
		SET_MODEL(ENT(pev), "models/w_shotbox.mdl");
		CBasePlayerAmmo::Spawn();
	}
	void Precache() override
	{
		PRECACHE_MODEL("models/w_shotbox.mdl");
		PRECACHE_SOUND("items/9mmclip1.wav");
	}
	bool AddAmmo(CBaseEntity* pOther) override
	{

		if (pOther->GiveAmmo(AMMO_BUCKSHOTBOX_GIVE, "buckshot", BUCKSHOT_MAX_CARRY) != -1)
		{
			EMIT_SOUND(ENT(pev), CHAN_ITEM, "items/9mmclip1.wav", 1, ATTN_NORM);
			ClientPrint(pOther->pev, HUD_PRINTTALK, "Picked up buckshots and slugs.\n");
			return true;
		}
		return false;
	}
};
LINK_ENTITY_TO_CLASS(ammo_buckshot, CShotgunAmmo);
