#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "player.h"
#include "soundent.h"
#include "gamerules.h"
#include "UserMessages.h"


LINK_ENTITY_TO_CLASS(weapon_p90, CP90);

void CP90::Spawn()
{
	Precache();
	SET_MODEL(ENT(pev), "models/w_p90.mdl");
	m_iId = WEAPON_P90;
	m_iDefaultAmmo = 50; // How much ammo this weapon has on spawn
	FallInit();			 // get ready to fall down.
}

void CP90::Precache()
{
	PRECACHE_MODEL("models/v_p90.mdl");
	PRECACHE_MODEL("models/w_p90.mdl");
	PRECACHE_MODEL("models/p_p90.mdl");

	m_iShell = PRECACHE_MODEL("models/rshell.mdl"); // brass shell

	PRECACHE_SOUND("weapons/p90.wav");
}

bool CP90::GetItemInfo(ItemInfo* p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "9mm"; // Which ammo type this weapon use
	p->iMaxAmmo1 = _9MM_MAX_CARRY;	 // What's the max ammo quantity for that kind of ammo
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = NULL;
	p->iMaxClip = 50; // How many ammo this weapon's clip or magazine has
	p->iSlot = 2;	  // Which "slot" (column) in the HUD this weapon is located (2 = same slot as HL1 MP5, shotgun, crossbow)
	p->iPosition = 1; // Which "position" (row) in the HUD this weapon is located (4 = after quad shotgun)
	p->iFlags = 0;	  // Special flags this weapon has
	p->iId = m_iId = WEAPON_P90;
	p->iWeight = MP5_WEIGHT; // How much "priority" this weapon has when auto-switch is triggered

	return true;
}

bool CP90::Deploy()
{
	//  The last parameter is the animation set for the player model in thirdperson to use
	return DefaultDeploy("models/v_p90.mdl", "models/p_p90.mdl", P90_DRAW, "mp5");
}

void CP90::PrimaryAttack()
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
		1, 10, m_pPlayer->pev, m_pPlayer->random_seed);

	// Play view model animation and firing sound
	SendWeaponAnim(P90_SHOOT1 + RANDOM_LONG(0, 1));
	EMIT_SOUND(edict(), CHAN_AUTO, "weapons/p90.wav", 1, ATTN_NORM);


	// Eject the brass
	Vector vecShellVelocity = m_pPlayer->pev->velocity + gpGlobals->v_right * RANDOM_FLOAT(100, 200) +
							  gpGlobals->v_up * RANDOM_FLOAT(100, 150) + gpGlobals->v_forward * 25;
	EjectBrass(pev->origin + m_pPlayer->pev->view_ofs + gpGlobals->v_up * -3 + gpGlobals->v_forward * -16 +
				   gpGlobals->v_right * -6,
		vecShellVelocity, pev->angles.y, m_iShell, TE_BOUNCE_SHELL);

	// Punch the camera to simulate recoil
	m_pPlayer->pev->punchangle.x -= 1;
	// Remove a bullet
	m_iClip--;
	// Next time for attack and weapon idling
	m_flNextPrimaryAttack = 0.1;
	m_flTimeWeaponIdle = 4;
}

void CP90::Reload()
{
	DefaultReload(50, P90_RELOAD, 3.4);
}


void CP90::WeaponIdle()
{
	ResetEmptySound();

	// Not the time to idle
	if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase())
		return;

	// Play idle animation
	SendWeaponAnim(P90_IDLE);
}