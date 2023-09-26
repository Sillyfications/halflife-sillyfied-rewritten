// Half-Life: Sillyfied weapon file
// This file can be used as an template for shotgun based reloading custom weapons

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "player.h"
#include "soundent.h"
#include "gamerules.h"
#include "UserMessages.h"

LINK_ENTITY_TO_CLASS(weapon_ksg, CKSG);

void CKSG::Spawn()
{
	Precache();
	SET_MODEL(ENT(pev), "models/w_ksg.mdl");
	m_iId = WEAPON_KSG;
	m_iDefaultAmmo = 18; // How much ammo this weapon has on spawn
	FallInit();			// get ready to fall down.
}

void CKSG::Precache()
{
	PRECACHE_MODEL("models/v_ksg.mdl");
	PRECACHE_MODEL("models/w_ksg.mdl");
	PRECACHE_MODEL("models/p_ksg.mdl");

	m_iShell = PRECACHE_MODEL("models/shotgunshell.mdl"); // brass shell

	PRECACHE_SOUND("weapons/ksg.wav");
	// PRECACHE_SOUND("weapons/la/reload.wav"); no longer needed
}

bool CKSG::GetItemInfo(ItemInfo* p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "10ga"; // Which ammo type this weapon use
	p->iMaxAmmo1 = 54;		  // What's the max ammo quantity for that kind of ammo
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = NULL;
	p->iMaxClip = 18;  // How many ammo this weapon's clip or magazine has
	p->iSlot = 4;	  // Which "slot" (column) in the HUD this weapon is located (2 = same slot as HL1 MP5, shotgun, crossbow)
	p->iPosition = 3; // Which "position" (row) in the HUD this weapon is located (4 = after quad shotgun)
	p->iFlags = 0;	  // Special flags this weapon has
	p->iId = m_iId = WEAPON_KSG;
	p->iWeight = MP5_WEIGHT; // How much "priority" this weapon has when auto-switch is triggered

	return true;
}

bool CKSG::Deploy()
{
	//  The last parameter is the animation set for the player model in thirdperson to use
	return DefaultDeploy("models/v_ksg.mdl", "models/p_ksg.mdl", KSG_DRAW, "crossbow");
	m_fInSpecialReload = 0;
}

void CKSG::PrimaryAttack()
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

	if (m_iClip <= 0) // out of ammo? time to... wait i am reloaded
	{
		Reload();
		if (m_iClip == 0)
			PlayEmptySound();
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
	Vector vecDir = m_pPlayer->FireBulletsPlayer(3, vecSrc, vecAiming, VECTOR_CONE_6DEGREES, 8192, BULLET_PLAYER_BUCKSHOT,
		1, 20, m_pPlayer->pev, m_pPlayer->random_seed);

	// Play view model animation and firing sound
	SendWeaponAnim(KSG_FIRE);
	EMIT_SOUND(edict(), CHAN_AUTO, "weapons/ksg.wav", 1, ATTN_NORM);


	// Eject the brass
	Vector vecShellVelocity = m_pPlayer->pev->velocity + gpGlobals->v_right * RANDOM_FLOAT(100, 200) +
							  gpGlobals->v_up * RANDOM_FLOAT(100, 150) + gpGlobals->v_forward * 25;
	EjectBrass(pev->origin + m_pPlayer->pev->view_ofs + gpGlobals->v_up * -3 + gpGlobals->v_forward * 16 +
				   gpGlobals->v_right * 6,
		vecShellVelocity, pev->angles.y, m_iShell, TE_BOUNCE_SHELL);

	// Punch the camera to simulate recoil
	m_pPlayer->pev->punchangle.x -= 10;
	// Remove a bullet

	m_iClip--;

	// if (m_iClip != 0)
	m_flPumpTime = gpGlobals->time + 0.5;
	// Next time for attack and weapon idling
	m_flNextPrimaryAttack = 1.2; // 300
	m_flTimeWeaponIdle = 1.2;
	m_fInSpecialReload = 0;
}

// void CKSG::SecondaryAttack() // secondary attack? nuh uh its a reloading button
//{
// if (m_iPrimaryAmmoType == 0) //check if player has actually ammo
//{
//	return; // no ammo whoopie no reloading for you!
// }

// if (m_iClip == 4) // is the mag tube full?
//{
//	return; // no more reloading for you!
// }


// DefaultReload(0, KSG_RELOAD, 1.3);// alternative option, maybe this stops with the "hold down the button to reload thing"
// m_iClip++; //add one bullet to the clip
// m_iPrimaryAmmoType--; //remove one bullet from the reserve
// m_flNextPrimaryAttack = 1.35; // prevent player from attacking IN the reload
// m_flNextSecondaryAttack = 1.35; // prevent player from reloading before the cooldown
// SendWeaponAnim(KSG_RELOAD); // play reload animation
// m_fInSpecialReload = 1;
//}


void CKSG::Reload()
{
	// DefaultReload(1, KSG_RELOAD, 1.3);
	if (m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0 || m_iClip == 18) // checks if the weapon is full, the LA has 4 round cap
		return;

	// don't reload until recoil is done
	if (m_flNextPrimaryAttack > UTIL_WeaponTimeBase())
		return;



	// check to see if we're ready to reload
	if (m_fInSpecialReload == 0)
	{
		SendWeaponAnim(KSG_START_RELOAD); // starting position
		m_fInSpecialReload = 1;
		m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.57; // time to for the weapon to start
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.57;
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

		//	if (RANDOM_LONG(0, 1))
		//		EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/la/reload.wav", 1, ATTN_NORM, 0, 85 + RANDOM_LONG(0, 0x1f));
		//	else
		//		EMIT_SOUND_DYN(ENT(m_pPlayer->pev), CHAN_ITEM, "weapons/la/reload.wav", 1, ATTN_NORM, 0, 85 + RANDOM_LONG(0, 0x1f)); //we are playing sound in the QC no need to play it here

		SendWeaponAnim(KSG_RELOAD);
		float m_flNextReload;
		m_flNextReload = UTIL_WeaponTimeBase() + 0.357;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.357;
	}
	else
	{
		// Add them to the clip
		m_iClip += 1;
		m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] -= 1;
		m_fInSpecialReload = 1;
	}
}


void CKSG::WeaponIdle()
{
	ResetEmptySound();

	m_pPlayer->GetAutoaimVector(AUTOAIM_5DEGREES);

	// Moved to ItemPostFrame
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
			if (m_iClip != 18 && 0 != m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType])
			{
				Reload();
			}
			else
			{
				// reload debounce has timed out
				SendWeaponAnim(KSG_PUMP);


				m_fInSpecialReload = 0;
				m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.0;
			}
		}
		else
		{
			int iAnim;
			float flRand = UTIL_SharedRandomFloat(m_pPlayer->random_seed, 0, 1);
			if (flRand <= 0.8)
			{
				iAnim = KSG_IDLE_DEEP;
				m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + (60.0 / 12.0); // * RANDOM_LONG(2, 5);
			}
			else if (flRand <= 0.95)
			{
				iAnim = KSG_IDLE;
				m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + (20.0 / 9.0);
			}
			else
			{
				iAnim = KSG_IDLE4;
				m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + (20.0 / 9.0);
			}
			SendWeaponAnim(iAnim);
		}
	}
}
