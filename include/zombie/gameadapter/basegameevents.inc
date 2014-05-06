/*
 * ============================================================================
 *
 *  Zombie API
 *
 *  File:           basegameevents.inc
 *  Description:    Provides base round and player events. Some events may be
 *                  faked if the game doesn't have native support for them.
 *
 *  Copyright (C) 2014  Richard Helgeby
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * ============================================================================
 */

#if defined _zm_basegameevents_included
 #endinput
#endif
#define _zm_basegameevents_included

/*____________________________________________________________________________*/

/**
 * @section Predefined event names.
 */
#define ZM_BASEEVENTS_ON_ROUND_START        "OnRoundStart"
#define ZM_BASEEVENTS_ON_ROUND_FREEZE_END   "OnRoundFreezeEnd"
#define ZM_BASEEVENTS_ON_ROUND_END          "OnRoundEnd"
#define ZM_BASEEVENTS_ON_PLAYER_TEAM_PRE    "OnPlayerTeamPre"
#define ZM_BASEEVENTS_ON_PLAYER_TEAM        "OnPlayerTeam"
#define ZM_BASEEVENTS_ON_PLAYER_SPAWN       "OnPlayerSpawn"
#define ZM_BASEEVENTS_ON_PLAYER_HURTH       "OnPlayerHurt"
#define ZM_BASEEVENTS_ON_PLAYER_DEATH       "OnPlayerDeath"
#define ZM_BASEEVENTS_ON_PLAYER_JUMP        "OnPlayerJump"
#define ZM_BASEEVENTS_ON_PLAYER_FIRE_WEAPON "OnPlayerFireWeapon"
/**
 * @endsection
 */

/*____________________________________________________________________________*/

/**
 * The round started.
 *
 * @noreturn
 */
functag ZMOnRoundStart();

/*____________________________________________________________________________*/

/**
 * The round freeze time is up.
 *
 * @noreturn
 */
functag ZMOnRoundFreezeEnd();

/*____________________________________________________________________________*/

/**
 * The round ended.
 *
 * @param winner        Winning team.
 */
functag ZMOnRoundEnd(ZMTeam:winner);

/*____________________________________________________________________________*/

/**
 * A player joined a team. Pre event hook to allow modification.
 *
 * @param client        Client index.
 * @param team          The team player joined.
 * @param oldTeam       Old team before changing team.
 * @param disconnect    Team changed because player disconnected.
 *
 * @return              Plugin_Handled to block event broadcast. Does not block
 *                      actual team change.
 *                      Plugin_Continue to continue event broadcast.
 */
functag Action:ZMOnPlayerTeamPre(
        client,
        ZMTeam:team,
        ZMTeam:oldTeam,
        bool:disconnect);

/*____________________________________________________________________________*/

/**
 * A player joined a team.
 *
 * @param client        Client index.
 * @param team          The team player joined.
 * @param oldTeam       Old team before changing team.
 * @param disconnect    Team change because player disconnects.
 *
 * @noreturn
 */
functag ZMOnPlayerTeam(client, ZMTeam:team, ZMTeam:oldTeam, bool:disconnect);

/*____________________________________________________________________________*/

/**
 * Player has spawned.
 *
 * Note: This event is NOT called when client is spawning in the game (the first
 *       time they see the map, before selecting a team).
 *
 * @param client        Client index.
 *
 * @noreturn
 */
functag ZMOnPlayerSpawn(client);

/*____________________________________________________________________________*/

/**
 * Player has been damaged.
 *
 * @param victim        Victim client index.
 * @param attacker      Attacker client index. It may be the world.
 * @param health        Remaining health of victim.
 * @param armor         Remaining armor of victim.
 * @param weapon        Weapon class name used to hurt the victim. No "weapon_"
 *                      prefix.
 * @param dmgHealth     Amount of health the victim lost.
 * @param dmgArmor      Amount of armor the victim lost.
 * @param hitgroup      Index of hit group on the victim that was damaged.
 *
 * @noreturn
 */
functag ZMOnPlayerHurt(
        victim,
        attacker,
        health,
        armor,
        const String:weapon[],
        dmgHealth,
        dmgArmor,
        hitgroup);

/*____________________________________________________________________________*/

/**
 * Player died.
 *
 * @param victim        Client who died.
 * @param attacker      Attacker client index. It may be the world.
 * @param weapon        Weapon class name used to kill the victim. No "weapon_"
 *                      prefix.
 * @param headshot      Whether the victim died of a headshot.
 *
 * @noreturn
 */
functag ZMOnPlayerDeath(victim, attacker, const String:weapon[], bool:headshot);

/*____________________________________________________________________________*/

/**
 * Player jumped.
 *
 * @param client        Client index.
 *
 * @noreturn
 */
functag ZMOnPlayerJump(client);

/*____________________________________________________________________________*/

/**
 * Player has fired a weapon.
 *
 * @param client        Client who fired weapon.
 * @param weapon        Weapon class name. No "weapon_" prefix.
 */
functag Action:ZMOnPlayerFireWeapon(client, const String:weapon[]);

/*____________________________________________________________________________*/

/**
 * @section Adds an event hook on a specific event.
 *
 * @param callback      Event callback.
 *
 * @return              Event ID.
 * @error               Invalid callback, or caller is not a registered module.
 */
native ZMEvent:ZM_HookRoundStart(ZMOnRoundStart:callback);
native ZMEvent:ZM_HookRoundFreezeEnd(ZMOnRoundFreezeEnd:callback);
native ZMEvent:ZM_HookRoundEnd(ZMOnRoundEnd:callback);
native ZMEvent:ZM_HookPlayerTeamPre(ZMOnPlayerTeamPre:callback);
native ZMEvent:ZM_HookPlayerTeam(ZMOnPlayerTeam:callback);
native ZMEvent:ZM_HookPlayerSpawn(ZMOnPlayerSpawn:callback);
native ZMEvent:ZM_HookPlayerHurt(ZMOnPlayerHurt:callback);
native ZMEvent:ZM_HookPlayerDeath(ZMOnPlayerDeath:callback);
native ZMEvent:ZM_HookPlayerJump(ZMOnPlayerJump:callback);
native ZMEvent:ZM_HookPlayerFireWeapon(ZMOnPlayerFireWeapon:callback);
/**
 * @endsection
 */
