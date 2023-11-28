// Half-Life: Sillyfied weapon file

// dualelites.cpp
// Function: New custom akimbo weapon with the newest mechanics
// Weapon Name: Akimbo Kriss Vectors

//legal mumbo jumbo below

/***
 *
 *	Copyright (c) 1996-2001, Valve LLC. All rights reserved.
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
 *	The usage and modification of this source code is allowed in other GoldSrc mods that
 *  is not Half-Life: Sillyfied. You cannot use this code to make commerical products 
	with it or sell it.

	Code modified by: Sillyfications 
	version 2.0
	Code is NOT generated with Half-Life: Sillyfied Weapon Manager
 *
 ****/
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "player.h"
#include "soundent.h"
#include "gamerules.h"
#include "UserMessages.h"

LINK_ENTITY_TO_CLASS(weapon_elite, CElite);

void CElite::Spawn()
{
	Precache();
	SET_MODEL(ENT(pev), "models/w_elite.mdl");
	m_iId = WEAPON_ELITE;
	m_iDefaultAmmo = 34; // The weapon has 2 mags of 17 rounds capacity
	m_iSecondaryAmmoType -= 101; // restart CND
	FallInit();			 // get ready to fall down.
	
}

void CElite::Precache()
{
	PRECACHE_MODEL("models/v_elite.mdl");
	PRECACHE_MODEL("models/w_elite.mdl");
	PRECACHE_MODEL("models/p_elite.mdl");

	m_iShell = PRECACHE_MODEL("models/shell.mdl"); // brass shell

	PRECACHE_SOUND("weapons/elite.wav");
	PRECACHE_SOUND("weapons/crit.wav");
}

bool CElite::GetItemInfo(ItemInfo* p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "22"; // Which ammo type this weapon use 
	p->iMaxAmmo1 = 170;	 // What's the max ammo quantity for that kind of ammo
	p->pszAmmo2 = "VectorCND";
	p->iMaxAmmo2 = 101;
	p->iMaxClip = 34; // How many ammo this weapon's clip or magazine has
	p->iSlot = 2;	  // Which "slot" (column) in the HUD this weapon is located (2 = same slot as OICW and P90)
	p->iPosition = 2; // Which "position" (row) in the HUD this weapon is located (2 = after P90)
	p->iFlags = 0;	  // Special flags this weapon has
	p->iId = m_iId = WEAPON_ELITE;
	p->iWeight = MP5_WEIGHT; // How much "priority" this weapon has when auto-switch is triggered

	return true;
}

bool CElite::Deploy()
{
	//  The last parameter is the animation set for the player model in thirdperson to use
	return DefaultDeploy("models/v_elite.mdl", "models/p_elite.mdl", ELITE_DRAW, "glock");
}

void CElite::PrimaryAttack()
{
	// don't fire underwater
	if (m_pPlayer->pev->waterlevel == 3)
	{
		PlayEmptySound();
		m_flNextPrimaryAttack = 0.15;
		return;
	}

	// Don't fire if empty
	if (m_iClip <= 0)
	{
		PlayEmptySound();
		m_flNextPrimaryAttack = 0.15;
		return;
	}

	if (m_iSecondaryAmmoType == 101) // no CND... no shoot....
	{
		PlayEmptySound();
		return;
	}

	// calculate recoil!!
	if (m_pPlayer->pev->velocity.Length2D() > 160) // are we moving at 160 hammer units?
	{
		m_VecInaccuracy = VECTOR_CONE_4DEGREES; // max inaccuracy
	}
	else if (!(m_pPlayer->pev->flags & FL_ONGROUND)) // are we standing?
	{
		m_VecInaccuracy = VECTOR_CONE_3DEGREES; // medium inaccuracy
	}
	else if (m_pPlayer->pev->flags & FL_DUCKING) // are we ducking?
	{
		m_VecInaccuracy = VECTOR_CONE_2DEGREES; // minimal inaccuracy
	}
	else // are we jumping or falling?
	{
		m_VecInaccuracy = VECTOR_CONE_4DEGREES; // make the gun very inaccurate
	}


	// Size of the muzzle flash and how much volume in the world the firing sound produces
	m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = NORMAL_GUN_FLASH;
	m_pPlayer->pev->effects = (int)(m_pPlayer->pev->effects) | EF_MUZZLEFLASH;

	// player "shoot" animation
	m_pPlayer->SetAnimation(PLAYER_ATTACK1);

	Vector vecSrc = m_pPlayer->GetGunPosition();
	Vector vecAiming = m_pPlayer->GetAutoaimVector(AUTOAIM_5DEGREES);


	m_iRandomDamageCalculator = m_iRandomDamageCalculator + RANDOM_LONG(1, 16);

	switch (m_iRandomDamageCalculator)
	{
	case 1: // critical shits
		m_iWeaponDamageValue = 4;
		m_iRandomDamageCalculator = 0;
		break;
	case 2:
		m_iWeaponDamageValue = 11;
		m_iRandomDamageCalculator = 0;
		break;
	case 3:
	case 4:
	case 5:
		m_iWeaponDamageValue = 12;
		m_iRandomDamageCalculator = 0;
		break;
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:
	case 11:
		m_iWeaponDamageValue = 13;
		m_iRandomDamageCalculator = 0;
		break;
	case 12:
	case 13:
	case 14:
		m_iWeaponDamageValue = 14;
		m_iRandomDamageCalculator = 0;
		break;
	case 15: // mini crit
		m_iWeaponDamageValue = 18;
		m_iRandomDamageCalculator = 0;
		break;
	case 16:
		m_iWeaponDamageValue = 39;
		m_iRandomDamageCalculator = 0;
		ClientPrint(m_pPlayer->pev, HUD_PRINTCENTER, "Critical Hit!\n200 percent damage increase \n39 damage\n");
		EMIT_SOUND(edict(), CHAN_AUTO, "weapons/crit.wav", 1, ATTN_NORM);
		break;
	default:
		m_iWeaponDamageValue = 13;
		m_iRandomDamageCalculator = 0;
		ClientPrint(m_pPlayer->pev, HUD_PRINTCENTER, "Can't find a damage value...\nDealing normal damage \n13 damage\n");
		break;
	}

	Vector vecDir = m_pPlayer->FireBulletsPlayer(1, vecSrc, vecAiming, m_VecInaccuracy, 8192, BULLET_PLAYER_MP5, 1, m_iWeaponDamageValue, m_pPlayer->pev, m_pPlayer->random_seed);



	switch (m_iClip) // this code fucking sucks, i know there is a better way but yeah
	{
	case 34:
	case 32:
	case 30:
	case 28:
	case 26:
	case 24:
	case 22:
	case 20:
	case 18:
	case 16:
	case 14:
	case 12:
	case 10:
	case 8:
	case 6:
	case 4:
	case 2:
		SendWeaponAnim(ELITE_SHOOT1);
		EMIT_SOUND(edict(), CHAN_AUTO, "weapons/elite.wav", 1, ATTN_NORM);
		m_flForward = 10.8f;
		m_flRight = 6.7f;
		m_flUp = -2.6f;

		break;
	default:
		SendWeaponAnim(ELITE_SHOOT2);
		EMIT_SOUND(edict(), CHAN_AUTO, "weapons/elite.wav", 1, ATTN_NORM);
		m_flForward = 10.8f;
		m_flRight = -6.7f;
		m_flUp = -2.6f;
		m_iSecondaryAmmoType++;
		break;
	}

	// Eject the brass
	Vector vecShellVelocity = m_pPlayer->pev->velocity + gpGlobals->v_right * RANDOM_FLOAT(100, 200) + gpGlobals->v_up * RANDOM_FLOAT(100, 150) + gpGlobals->v_forward * 25;
	EjectBrass(pev->origin + m_pPlayer->pev->view_ofs + gpGlobals->v_up * m_flUp + gpGlobals->v_forward * m_flForward + gpGlobals->v_right * m_flRight, vecShellVelocity, pev->angles.y, m_iShell, TE_BOUNCE_SHELL);



	// start weapon condition message
	if (m_iSecondaryAmmoType == 1)
	{
		ClientPrint(m_pPlayer->pev, HUD_PRINTTALK, "Your Akimbo Cross Ventor 22 is starting to show some wear.\n");
	}
	if (m_iSecondaryAmmoType == 21)
	{
		ClientPrint(m_pPlayer->pev, HUD_PRINTTALK, "Your Akimbo Cross Ventor 22 has now 80 CND.\n");
	}
	if (m_iSecondaryAmmoType == 41)
	{
		ClientPrint(m_pPlayer->pev, HUD_PRINTTALK, "Your Akimbo Cross Ventor 22 has now 60 CND.\n");
	}
	if (m_iSecondaryAmmoType == 61)
	{
		ClientPrint(m_pPlayer->pev, HUD_PRINTTALK, "Your Akimbo Cross Ventor 22 has now 40 CND, try to find a replacement weapon or repair your current weapons.\n");
	}
	if (m_iSecondaryAmmoType == 81)
	{
		ClientPrint(m_pPlayer->pev, HUD_PRINTTALK, "Your Akimbo Cross Ventor 22 has now 20 CND, It will malfunction soon, so try to switch to a different weapon.\n");
	}
	if (m_iSecondaryAmmoType == 91)
	{
		ClientPrint(m_pPlayer->pev, HUD_PRINTTALK, "Your Akimbo Cross Ventor 22 has now 10 CND, It will malfunction, Switch to a different weapon.\n");
	}
	if (m_iSecondaryAmmoType == 101)
	{
		ClientPrint(m_pPlayer->pev, HUD_PRINTTALK, "Your Akimbo Cross Ventor 22 is now broken. Switch to a different weapon.\n");
	}


		// Punch the camera to simulate recoil
		m_pPlayer->pev->punchangle.x -= 1.52;
		// Remove a bullet

		m_iClip--;

		// Next time for attack and weapon idling
		m_flNextPrimaryAttack = 0.15; // 300
		m_flTimeWeaponIdle = 2;
	
}

void CElite::Reload()
{

	if (m_iClip >= 1) //do we have a bullet in the chamber....
	{
		DefaultReload(34, ELITE_RELOAD, 2.3);
	}
	else 
	{
		DefaultReload(34, ELITE_RELOAD_EMPTY, 3.4);
	}
}


void CElite::WeaponIdle()
{
	ResetEmptySound();

	// Not the time to idle
	if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase())
		return;

	// Play idle animation
	SendWeaponAnim(ELITE_IDLE);
}