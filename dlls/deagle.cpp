// HALF-LIFE: SILLYFIED
// Magnum Research Desert Eagle
// Changes: ammo 5.56 > 5.7, max 180 > 150, acc 5 > 3, rpm 0.15 > 0.125

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "player.h"
#include "soundent.h"
#include "gamerules.h"
#include "UserMessages.h"

LINK_ENTITY_TO_CLASS(weapon_deagle, CDeagle);

void CDeagle::Spawn()
{
	Precache();
	SET_MODEL(ENT(pev), "models/w_deagle.mdl");
	m_iId = WEAPON_DEAGLE;
	m_iDefaultAmmo = 27; // How much ammo this weapon has on spawn
	FallInit();			 // get ready to fall down.
}

void CDeagle::Precache()
{
	PRECACHE_MODEL("models/v_deagle.mdl");
	PRECACHE_MODEL("models/w_deagle.mdl");

	m_iShell = PRECACHE_MODEL("models/rshell_357.mdl"); // brass shell

	PRECACHE_SOUND("weapons/deagle.wav");
}

bool CDeagle::GetItemInfo(ItemInfo* p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "357"; // Which ammo type this weapon use
	p->iMaxAmmo1 = _357_MAX_CARRY;	 // What's the max ammo quantity for that kind of ammo
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = NULL;
	p->iMaxClip = 9;  // How many ammo this weapon's clip or magazine has
	p->iSlot = 1;	  // Which "slot" (column) in the HUD this weapon is located (2 = same slot as HL1 MP5, shotgun, crossbow)
	p->iPosition = 3; // Which "position" (row) in the HUD this weapon is located (4 = after quad shotgun)
	p->iFlags = 0;	  // Special flags this weapon has
	p->iId = m_iId = WEAPON_DEAGLE;
	p->iWeight = MP5_WEIGHT; // How much "priority" this weapon has when auto-switch is triggered

	return true;
}

bool CDeagle::Deploy()
{
	//  The last parameter is the animation set for the player model in thirdperson to use
	return DefaultDeploy("models/v_deagle.mdl", "models/p_crowbar.mdl", DEAGLE_DRAW, "mp5");
}

void CDeagle::PrimaryAttack()
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

	// Size of the muzzle flash and how much volume in the world the firing sound produces
	m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = NORMAL_GUN_FLASH;
	m_pPlayer->pev->effects = (int)(m_pPlayer->pev->effects) | EF_MUZZLEFLASH;

	// player "shoot" animation
	m_pPlayer->SetAnimation(PLAYER_ATTACK1);

	Vector vecSrc = m_pPlayer->GetGunPosition();
	Vector vecAiming = m_pPlayer->GetAutoaimVector(AUTOAIM_5DEGREES);
	Vector vecDir = m_pPlayer->FireBulletsPlayer(1, vecSrc, vecAiming, VECTOR_CONE_3DEGREES, 8192, BULLET_PLAYER_MP5,
		1, 25, m_pPlayer->pev, m_pPlayer->random_seed); // dam 23 > 25 (+2) acc 5 > 3 (+2)

	// Play view model animation and firing sound
	SendWeaponAnim(DEAGLE_SHOOT1 + RANDOM_LONG(0, 2));
	EMIT_SOUND(edict(), CHAN_AUTO, "weapons/deagle.wav", 1, ATTN_NORM);

	// Eject the brass
	Vector vecShellVelocity = m_pPlayer->pev->velocity + gpGlobals->v_right * RANDOM_FLOAT(100, 200) +
							  gpGlobals->v_up * RANDOM_FLOAT(100, 150) + gpGlobals->v_forward * 10;
	EjectBrass(pev->origin + m_pPlayer->pev->view_ofs + gpGlobals->v_up * -12 + gpGlobals->v_forward * -1 +
				   gpGlobals->v_right * 5,
		vecShellVelocity, pev->angles.y, m_iShell, TE_BOUNCE_SHELL);

	// Punch the camera to simulate recoil
	m_pPlayer->pev->punchangle.x -= 2.6;
	// Remove a bullet
	m_iClip--;
	// Next time for attack and weapon idling
	m_flNextPrimaryAttack = 0.22;
	m_flTimeWeaponIdle = 2;
}

void CDeagle::Reload()
{
	// Reload 30 bullets, play the AK47_RELOAD animation, reload duration is 2.5 seconds
	DefaultReload(9, DEAGLE_RELOAD, 2.2);
}


void CDeagle::WeaponIdle()
{
	ResetEmptySound();

	// Not the time to idle
	if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase())
		return;

	// Play idle animation
	SendWeaponAnim(DEAGLE_IDLE);
}