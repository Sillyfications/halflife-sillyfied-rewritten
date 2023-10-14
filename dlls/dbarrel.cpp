//Half-Life: Sillyfied Double Barrel Shotgun

// The Lenton Arms SxS 4
// The Lenton Arms SxS 4 is a double-barreled shotgun that does high damage, but very inaccurate. It shoots two 4 gauge shells at once. The most effective way to use this weapon are for short range only.
// Ammo: 4 GAUGE
// Damage: 18x13 (234)
// Firerate: Single shot before reloading, can't measure
// Magazine: Barrel fed, 2/10
// DPS: 0-234
// CND: 10 shots before malfunction
// Accuracy: 20 degrees 
// Recoil: 20 upwards

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "player.h"
#include "soundent.h"
#include "gamerules.h"
#include "UserMessages.h"

LINK_ENTITY_TO_CLASS(weapon_dbarrel, CDB);

void CDB::Spawn()
{
	Precache();
	SET_MODEL(ENT(pev), "models/w_quad.mdl");
	m_iId = WEAPON_DBARREL;
	m_iDefaultAmmo = 2; // ammo on spawn
	m_iSecondaryAmmoType -= 27; // reset CND
	FallInit();
}

void CDB::Precache()
{
	PRECACHE_MODEL("models/v_quad.mdl");
	PRECACHE_MODEL("models/w_quad.mdl");
	PRECACHE_MODEL("models/p_quad.mdl");
	PRECACHE_SOUND("weapons/quad.wav");

	m_iShell = PRECACHE_MODEL("models/shotgunshell.mdl"); // brass shell
}

bool CDB::GetItemInfo(ItemInfo* p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "4 gauge";		   // its a shotgun, of course 12G
	p->iMaxAmmo1 = 10; // 125
	p->pszAmmo2 = "CNDDbarrel";
	p->iMaxAmmo2 = 27;
	p->iMaxClip = 2;			  // 2 barrels
	p->iSlot = 4;				  // 2 = smg, shotgun, crossbow (BUG! sprite doesn't show in game? gotta ask for help)
	p->iPosition = 1;			  // below m40a1 scopeless		(BUG! same here)
	p->iFlags = 0;				  // no idea
	p->iId = m_iId = WEAPON_DBARREL; // errm errm also no idea but must be important
	p->iWeight = MP5_WEIGHT;

	return true;
}

bool CDB::Deploy()
{
	return DefaultDeploy("models/v_quad.mdl", "models/p_quad.mdl", DB_DRAW, "shotgun");
}

// tweak some stuff before release
void CDB::PrimaryAttack()
{
	// don't fire underwater
	if (m_pPlayer->pev->waterlevel == 3)
	{
		PlayEmptySound();
		m_flNextPrimaryAttack = 0.15;
		return;
	}

	// don't fire if empty
	if (m_iClip <= 0)
	{
		PlayEmptySound();
		m_flNextPrimaryAttack = 0.15;
		return;
	}

	if (m_iSecondaryAmmoType == 27) // no DP... no shoot....
	{
		PlayEmptySound();
		return;
	}

	// nuzzle flash and sound volume
	m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = NORMAL_GUN_FLASH;
	m_pPlayer->pev->effects = (int)(m_pPlayer->pev->effects) | EF_MUZZLEFLASH;

	// player "shoot" animation
	m_pPlayer->SetAnimation(PLAYER_ATTACK1);

	Vector vecSrc = m_pPlayer->GetGunPosition();
	Vector vecAiming = m_pPlayer->GetAutoaimVector(AUTOAIM_5DEGREES);
	Vector vecDir = m_pPlayer->FireBulletsPlayer(18, vecSrc, vecAiming, VECTOR_CONE_20DEGREES, 8192, BULLET_PLAYER_BUCKSHOT,
		1, 13, m_pPlayer->pev, m_pPlayer->random_seed); // 9 per shell

	// play view model animation and firing sound
	SendWeaponAnim(DB_SHOOT1 + RANDOM_LONG(0, 2));
	EMIT_SOUND(edict(), CHAN_AUTO, "weapons/quad.wav", 1, ATTN_NORM);




	// recoil
	m_pPlayer->pev->punchangle.x -= 8;
	m_iClip--;
	m_iClip--;
	m_iSecondaryAmmoType++;

	// start weapon condition message
	if (m_iSecondaryAmmoType == 17)
	{
		ClientPrint(m_pPlayer->pev, HUD_PRINTTALK, "Your Lenton Arms SxS 4 is starting to show some wear.");
	}
	if (m_iSecondaryAmmoType == 19)
	{
		ClientPrint(m_pPlayer->pev, HUD_PRINTTALK, "Your Lenton Arms SxS 4 has now 80 CND.");
	}
	if (m_iSecondaryAmmoType == 21)
	{
		ClientPrint(m_pPlayer->pev, HUD_PRINTTALK, "Your Lenton Arms SxS 4 has now 60 CND.");
	}
	if (m_iSecondaryAmmoType == 23)
	{
		ClientPrint(m_pPlayer->pev, HUD_PRINTTALK, "Your Lenton Arms SxS 4 has now 40 CND, try to find a replacement weapon or repair your current weapon.");
	}
	if (m_iSecondaryAmmoType == 25)
	{
		ClientPrint(m_pPlayer->pev, HUD_PRINTTALK, "Your Lenton Arms SxS 4 has now 20 CND, It will malfunction soon, so try to switch to a different weapon.");
	}
	if (m_iSecondaryAmmoType == 26)
	{
		ClientPrint(m_pPlayer->pev, HUD_PRINTTALK, "Your Lenton Arms SxS 4 has now 10 CND, It will malfunction, Switch to a different weapon.");
	}
	if (m_iSecondaryAmmoType == 27)
	{
		ClientPrint(m_pPlayer->pev, HUD_PRINTTALK, "Your Lenton Arms SxS 4 is now broken. Switch to a different weapon.");
	}
	// end weapon condition messages

	m_flNextPrimaryAttack = 0.1;
	m_flTimeWeaponIdle = 1.85;
}

// tweak some stuff before release
void CDB::SecondaryAttack()
{
	return;
}

void CDB::Reload()
{
	DefaultReload(2, DB_RELOAD, 1.7);

	// Eject the brass while reloading
	Vector vecShellVelocity = m_pPlayer->pev->velocity + gpGlobals->v_right * RANDOM_FLOAT(250, 500) +
							  gpGlobals->v_up * RANDOM_FLOAT(250, 500) + gpGlobals->v_forward * 25;
	EjectBrass(pev->origin + m_pPlayer->pev->view_ofs + gpGlobals->v_up * -12 + gpGlobals->v_forward * 9 +
				   gpGlobals->v_right * 6,
		vecShellVelocity, pev->angles.y, m_iShell, TE_BOUNCE_SHELL);
	EjectBrass(pev->origin + m_pPlayer->pev->view_ofs + gpGlobals->v_up * -12 + gpGlobals->v_forward * 9 +
				   gpGlobals->v_right * 6,
		vecShellVelocity, pev->angles.y, m_iShell, TE_BOUNCE_SHELL);
}


void CDB::WeaponIdle()
{
	ResetEmptySound();


	if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase())
		return;


	SendWeaponAnim(DB_IDLE);
}
