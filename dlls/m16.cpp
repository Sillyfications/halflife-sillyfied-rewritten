#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "player.h"
#include "soundent.h"
#include "gamerules.h"
#include "UserMessages.h"


LINK_ENTITY_TO_CLASS(weapon_m16, CM16);

void CM16::Spawn()
{
	Precache();
	SET_MODEL(ENT(pev), "models/w_m16.mdl");
	m_iId = WEAPON_M16;
	m_iDefaultAmmo = 30; // How much ammo this weapon has on spawn
	FallInit();			 // get ready to fall down.
}

void CM16::Precache()
{
	PRECACHE_MODEL("models/v_m16.mdl");
	PRECACHE_MODEL("models/w_m16.mdl");
	PRECACHE_MODEL("models/p_m16.mdl");

	m_iShell = PRECACHE_MODEL("models/rshell_big.mdl"); // brass shell
	m_iMag = PRECACHE_MODEL("models/stanag.mdl");

	PRECACHE_SOUND("weapons/m16.wav");
}

bool CM16::GetItemInfo(ItemInfo* p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "357";		   // Which ammo type this weapon use
	p->iMaxAmmo1 = _357_MAX_CARRY; // What's the max ammo quantity for that kind of ammo
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = NULL;
	p->iMaxClip = 30; // How many ammo this weapon's clip or magazine has
	p->iSlot = 3;	  // Which "slot" (column) in the HUD this weapon is located (2 = same slot as HL1 MP5, shotgun, crossbow)
	p->iPosition = 0; // Which "position" (row) in the HUD this weapon is located (4 = after quad shotgun)
	p->iFlags = 0;	  // Special flags this weapon has
	p->iId = m_iId = WEAPON_M16;
	p->iWeight = MP5_WEIGHT; // How much "priority" this weapon has when auto-switch is triggered

	return true;
}

bool CM16::Deploy()
{
	//  The last parameter is the animation set for the player model in thirdperson to use
	return DefaultDeploy("models/v_m16.mdl", "models/p_m16.mdl", M16_DRAW, "mp5");
	m_pPlayer->m_iFOV = 0 + 40;
}

void CM16::PrimaryAttack()
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
	// semi auto firing
	// if m1 is in attack, dont do anything
	if (m_pPlayer->m_afButtonLast & IN_ATTACK)
	{
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
		1, 15, m_pPlayer->pev, m_pPlayer->random_seed); 

	// Play view model animation and firing sound
	SendWeaponAnim(M16_SHOOT1 + RANDOM_LONG(0, 1));
	EMIT_SOUND(edict(), CHAN_AUTO, "weapons/m16.wav", 1, ATTN_NORM);


	// Eject the brass
	Vector vecShellVelocity = m_pPlayer->pev->velocity + gpGlobals->v_right * RANDOM_FLOAT(100, 200) +
							  gpGlobals->v_up * RANDOM_FLOAT(100, 150) + gpGlobals->v_forward * 25;
	EjectBrass(pev->origin + m_pPlayer->pev->view_ofs + gpGlobals->v_up * -3 + gpGlobals->v_forward * -16 +
				   gpGlobals->v_right * -6,
		vecShellVelocity, pev->angles.y, m_iShell, TE_BOUNCE_SHELL);

	// Punch the camera to simulate recoil
	m_pPlayer->pev->punchangle.x -= 2;
	// Remove a bullet
	m_iClip--;
	// Next time for attack and weapon idling
	m_flNextPrimaryAttack = 0.001; 
	m_flTimeWeaponIdle = 4;
}

void CM16::SecondaryAttack()
{
	

	// Size of the muzzle flash and how much volume in the world the firing sound produces
	m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = NORMAL_GUN_FLASH;
	m_pPlayer->pev->effects = (int)(m_pPlayer->pev->effects) | EF_MUZZLEFLASH;

	// player "shoot" animation
	m_pPlayer->SetAnimation(PLAYER_ATTACK1);

	Vector vecSrc = m_pPlayer->GetGunPosition();
	Vector vecAiming = m_pPlayer->GetAutoaimVector(AUTOAIM_5DEGREES);
	Vector vecDir = m_pPlayer->FireBulletsPlayer(1, vecSrc, vecAiming, VECTOR_CONE_5DEGREES, 64, BULLET_PLAYER_CROWBAR,
		1, 40, m_pPlayer->pev, m_pPlayer->random_seed);

	// Play view model animation and firing sound
	SendWeaponAnim(M16_SWING);


	

	// Punch the camera to simulate recoil
	m_pPlayer->pev->punchangle.y -= 2;
	
	// Next time for attack and weapon idling
	m_flNextSecondaryAttack = 1.9;
	m_flTimeWeaponIdle = 4;
}

void CM16::Reload()
{
	DefaultReload(30, M16_RELOAD, 3.6);

}


void CM16::WeaponIdle()
{
	ResetEmptySound();

	// Not the time to idle
	if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase())
		return;

	// Play idle animation
	SendWeaponAnim(M16_IDLE);
}