#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "weapons.h"
#include "player.h"
#include "saverestore.h"

// These correspond directly to the sequences in the weapon's view model
enum sniper_rare_e
{
	SNIPER_RARE_IDLE1 = 0,
	SNIPER_RARE_IDLEEMPTY,
	SNIPER_RARE_SHOOT,
	SNIPER_RARE_SHOOTLAST,
	SNIPER_RARE_DRAW,
	SNIPER_RARE_DRAWEMPTY,
	SNIPER_RARE_HOLSTER,
	SNIPER_RARE_HOLSTEREMPTY,
	SNIPER_RARE_RELOAD,
	SNIPER_RARE_RELOADEMPTY,
	SNIPER_RARE_RELOADJAM,
	//SNIPER_RARE_STARTRELOAD, not a shotgun
	//SNIPER_RARE_ENDRELOAD,
	//SNIPER_RARE_ENDRELOADEMPTY,
	//SNIPER_RARE_ENDRELOADJAM,
	SNIPER_RARE_SHOOTJAM,
};

LINK_ENTITY_TO_CLASS(weapon_sniper_rare, CSniperRare)




void CSniperRare::Spawn()
{
	pev->classname = MAKE_STRING("weapon_sniper_rare"); //entity name for references and for console
	Precache(); //cache the weapon so the engine can find the weapon
	m_iId = WEAPON_SNIPERRARE; //name for weapons.h definitions
	SET_MODEL(ENT(pev), "models/weapons/sniper/w_sniper_r.mdl");//set world model for the weapon
	m_iDefaultAmmo = 3;// how many rounds is the weapon going to give?
	FallInit();// let gravity do its thing
}

void CSniperRare::Precache()
{
	//insert models below to precache
	PRECACHE_MODEL("models/weapons/sniper/v_sniper_r.mdl");//viewmodel
	PRECACHE_MODEL("models/weapons/sniper/w_sniper_r.mdl"); // worldmodel
	PRECACHE_MODEL("models/weapons/sniper/p_sniper_r.mdl"); // playermodel

	//insert sounds below to precache
	PRECACHE_SOUND("weapons/sniper_rare/rare/fire.wav");
	PRECACHE_SOUND("weapons/ui/ammolow.wav");

	//precache event file
	m_usFireSniperRare = PRECACHE_EVENT(1, "events/sniper_rare.sc");

}

bool CSniperRare::GetItemInfo(ItemInfo* p)
{
	p->pszName = STRING(pev->classname); //match classes so the engine can load hud elements
	p->pszAmmo1 = "bolts"; // ammo type (bolts mean heavy ammo)
	p->iMaxAmmo1 = BOLT_MAX_CARRY;		 // amount of max ammo
	p->pszAmmo2 = NULL;			   // secondary ammo? (aka durablity in game)
	p->iMaxAmmo2 = NULL;		 // max 2nd ammo
	p->iMaxClip = 3; // ammo in mag
	p->iSlot = 5;	  // hud slot
	p->iPosition = 2; // row slot
	p->iFlags = 0;	  // special weapon flags
	p->iId = m_iId = WEAPON_SNIPERRARE; //link id
	p->iWeight = 10; // auto switch

	return true;
}



void CSniperRare::SecondaryAttack()
{
	if (m_pPlayer->m_iFOV != 0)
	{
		m_pPlayer->m_iFOV = 0; // 0 means reset to default fov
	}
	else if (m_pPlayer->m_iFOV != 20)
	{
		m_pPlayer->m_iFOV = 20;
	}

	pev->nextthink = UTIL_WeaponTimeBase() + 0.1;
	m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 1.0;
}

void CSniperRare::PrimaryAttack()
{
	// Don't fire underwater - waterlevel 3 indicates that the player's head is underwater
	if (m_pPlayer->pev->waterlevel == 3)
	{
		// Play a "click" and don't allow another primary attack for a short time
		PlayEmptySound();
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.15;
		return;
	}

	// Check if the clip is empty
	if (m_iClip <= 0)
	{
		if (!m_fInReload && m_fFireOnEmpty)
		{
			// If the player has fired previously, but is still holding the attack button down,
			// just play the empty "click" sound until the player releases the button.
			PlayEmptySound();
			m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.2;

			// check if player has weapon messages enabled
			float msg = CVAR_GET_FLOAT("sillyfied_hud_show_wpn_center");

			if (msg >= 1)
			{
				// send a message to player
				ClientPrint(m_pPlayer->pev, HUD_PRINTCENTER, "No ammo!");
			}
		}

		return;
	}



	//check if the weapon has less than 33% of ammo and add the ammo count by one
	if (m_iClip == 1 + 1)
	{
		//play sound
		EMIT_SOUND(edict(), CHAN_AUTO, "weapons/ui/ammolow.wav", 1, ATTN_NORM);
		

		// check if player has weapon messages enabled
		float msg = CVAR_GET_FLOAT("sillyfied_hud_show_wpn_center");

		if (msg >= 1)
		{
			// send a message to player
			ClientPrint(m_pPlayer->pev, HUD_PRINTCENTER, "Ammo low! Reload!");
		}

	}

	//CND calculations start
	//check if the player allows the cnd feature
	//check if player has weapon messages enabled and the debug
	float allowcnd = CVAR_GET_FLOAT("sillyfied_weapon_cnd");
	float allowdebug = CVAR_GET_FLOAT("sillyfied_weapon_cnd");

	if (allowcnd == 1)
	{
		//player allows cnd so lets run the rng
		int m_iRandom = RANDOM_LONG(0, m_iCNDMaxValue);//generate a random number between 0 and the weapons own threshold (1500)
		
		if (m_iCNDStoredValue > m_iRandom) {//is the current CND higher than the random number
			m_bJamWeapon = true;//set jam weapon to true
			m_iCNDStoredValue = 0;//reset the current CND
		}//did we not hit the number? run it again for the reload
		int m_iCNDValTwo = m_iCNDStoredValue + m_iCNDStoredValue;//double the CND value
		if (m_iCNDValTwo > m_iRandom)//is the doubled CND higher than the random number
		{
			m_bJamWeaponReload = true;//jam while reloading and do not reset
		}

		if (allowdebug == 1) {// is the debug CVAR on?
			
			
			float n = m_iCNDStoredValue;
			float r = m_iRandom;
			ALERT(at_console, "CND is %f\n", n);
			ALERT(at_console, "RNG is %f\n", r);
		}
	}

	

	


	// calculate recoil (used for sniper weapons)
	if (m_pPlayer->m_iFOV == 20) // are we scoped in
	{
		m_VecInaccuracy = VECTOR_CONE_2DEGREES; // max accuracy
	}
	else // are we not?
	{
		m_VecInaccuracy = VECTOR_CONE_10DEGREES; // make the gun inaccurate
	}

	// If we get to this point - we're shooting!

	m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = NORMAL_GUN_FLASH;

	// Decrease the number of bullets in the clip
	// but check the +1 first and do not use any bullets

	if (m_bChamber == true) {
		m_bChamber = false; // we used our bullet soo back to false

		// check if player has weapon messages enabled
		float msg = CVAR_GET_FLOAT("sillyfied_hud_show_wpn_center");

		if (msg >= 1)
		{
			// send a message to player
			ClientPrint(m_pPlayer->pev, HUD_PRINTCENTER, "+1 not active\nBack to magazine.");
		}
	}
	else {
		m_iClip--; // use normal ammo
	}
	m_iCNDStoredValue = m_iCNDStoredValue + m_iWeaponDamage;

	// Add a muzzleflash to the player effects
	m_pPlayer->pev->effects |= EF_MUZZLEFLASH;

	// Player "shoot" animation
	m_pPlayer->SetAnimation(PLAYER_ATTACK1);

	// Set global vectors in the engine (don't ask)
	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);

	// Shoot bullets!
	Vector vecSrc = m_pPlayer->GetGunPosition();
	Vector vecAiming = m_pPlayer->GetAutoaimVector(AUTOAIM_10DEGREES);
	Vector vecDir = m_pPlayer->FireBulletsPlayer(
		1,					   // Number of bullets to shoot
		vecSrc,				   // The source of the bullets (i.e. the gun)
		vecAiming,			   // The direction to fire in (i.e. where the player is pointing)
		m_VecInaccuracy,	   // The accuracy spread of the weapon
		8192,				   // The distance the bullet can go (8192 is the limit for the engine)
		BULLET_PLAYER_357,	   // The type of bullet being fired
		10,					   // Number of tracer bullets to fire (none in this case)
		m_iWeaponDamage,	   // Set to non-zero to override the amount of damage (usually, leave this as 0)
		m_pPlayer->pev,		   // Attacker entity
		m_pPlayer->random_seed // The random seed
	);

	int flags;
#if defined(CLIENT_WEAPONS)
	flags = FEV_NOTHOST;
#else
	flags = 0;
#endif

	PLAYBACK_EVENT_FULL(flags, m_pPlayer->edict(), m_usFireSniperRare, 0.0, (float*)&g_vecZero, (float*)&g_vecZero, vecDir.x, vecDir.y, 0, 0, (m_iClip == 0) ? 1 : 0, 0);

	// If the clip is now empty and there's no more ammo available, update the HEV
	if (!m_iClip && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
	{
		// HEV suit - indicate out of ammo condition
		m_pPlayer->SetSuitUpdate("!HEV_AMO0", false, 0);
	}

	if (m_pPlayer->m_iFOV != 0)
	{
		SecondaryAttack();
	}

	// The desert eagle can fire quite quickly with no laser spot, so use a 250ms delay
	if (m_bJamWeapon == true) {
		m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 3.55;
		m_bJamWeapon = false;
	}
	else {
		m_flNextPrimaryAttack = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 1.68;
	}
	

	// Set the time until the weapon should start idling again
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat(m_pPlayer->random_seed, 10, 15);
}

bool CSniperRare::Deploy()
{
	return DefaultDeploy(
		"models/weapons/sniper/v_sniper_r.mdl", // weapon viewmodel
		"models/weapons/sniper/p_sniper_r.mdl", // world viewmodel
		SNIPER_RARE_DRAW,				// play the draw animation
		"bow",						// 3rd person animation
		pev->body						// body pointer
	);
	m_bChamber = false; // tell game to not start with putting +1
	
	

}

void CSniperRare::Holster()
{
	m_fInReload = false; //cancel reload

	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5; // edge i mean delay the weapon so it can play the holster animation

	if (m_pPlayer->m_iFOV != 0)
	{
		SecondaryAttack();
	}

	if (m_bChamber == true) { //check if player has the +1 buff
		m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]++; //refund the round
		m_bChamber = false;

		// check if player has weapon messages enabled
		float msg = CVAR_GET_FLOAT("sillyfied_hud_show_wpn_center");

		if (msg >= 1)
		{
			// send a message to player
			ClientPrint(m_pPlayer->pev, HUD_PRINTCENTER, "Refunded 1 heavy ammo.");
		}
	}

	SendWeaponAnim(SNIPER_RARE_HOLSTER);
}

void CSniperRare::Reload()
{
	// Don't reload if the player doesn't have any ammo
	if (m_pPlayer->ammo_bolts <= 0)
		return;
	if (m_iClip == 3)
		return;

	int iResult;

	// The view model has two different animations depending on if there are any bullets in the clip
	if (m_iClip == 0)
	{
		if (m_bJamWeaponReload == true) {
			iResult = DefaultReload(3, SNIPER_RARE_RELOADJAM, 5.26);
			m_bJamWeaponReload = false;
		}
		else
		{
			iResult = DefaultReload(3, SNIPER_RARE_RELOADEMPTY, 3.02);
		}
		
	}
	else
	{


		if (m_pPlayer->ammo_bolts > 5)
		{
			m_bChamber = true;
			m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]--;

			// check if player has weapon messages enabled
			float msg = CVAR_GET_FLOAT("sillyfied_hud_show_wpn_center");

			if (msg >= 1)
			{
				// send a message to player

				ClientPrint(m_pPlayer->pev, HUD_PRINTCENTER, "+1 active");
			}
		}
		else {

			// check if player has weapon messages enabled
			float msg = CVAR_GET_FLOAT("sillyfied_hud_show_wpn_center");

			if (msg >= 1)
			{
				// send a message to player
				ClientPrint(m_pPlayer->pev, HUD_PRINTCENTER, "+1 disabled\nLow reserve.");
			}
		}

		iResult = DefaultReload(3, SNIPER_RARE_RELOAD, 2.22);

	}
		

	if (iResult)
	{
		// If the reload happened, then reset the weapon's idle time
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat(m_pPlayer->random_seed, 10, 15);
	}
}

void CSniperRare::WeaponIdle()
{
	// This is used in conjunction with the PlayEmptySound function.
	// This resets a flag so the "click" for an empty weapon can be replayed after a short delay
	ResetEmptySound();

	// Despite the name, this will SET the autoaim vector.
	// 10 degrees is what the magnum uses, so we'll use the same.
	m_pPlayer->GetAutoaimVector(AUTOAIM_10DEGREES);

	// Exit out of the method if the weapon time hasn't passed yet or if the clip is empty
	if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase() || m_iClip <= 0)
		return;

	// play idle
	int iAnim;

	iAnim = SNIPER_RARE_IDLE1;
	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + (91.0 / 60.0);
}