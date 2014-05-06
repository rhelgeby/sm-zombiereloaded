/*
 * ============================================================================
 *
 *  Zombie API
 *
 *  File:           teammanager.inc
 *  Type:           Include
 *  Description:    Virtual team API.
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

#if defined _zm_teammanager_included
 #endinput
#endif
#define _zm_teammanager_included

/*____________________________________________________________________________*/

/*

TERMINOLOGY

Team index:
This is the team index used by the Source engine. It is specific to the current
game (CS:S, TF2, HL2:DM, etc).

Virtual team:
A named virtual team with a unique team ID. The Zombie API use virtual team IDs
instead of team indexes.

Virtual team IDs are not constant and may be different each time the server is
restarted or the team manager plugin is reloaded. Plugins depending on the team
manager should look up team IDs during plugin start and cache them.

Team mapping:
A virtual team ID is mapped to a team index so that we can use lower-level
functions like SwitchTeam in SourceMod. The mapping is configured by server
admins.

*/

/*____________________________________________________________________________*/

/**
 * @section Predefined virtual team names.
 */

/**
 * Client isn't assigned to a team. This usually applies for clients that just
 * connected and haven't selected a team yet.
 */
#define ZM_TEAM_UNASSIGNED  "unassigned"

/**
 * Client is a spectator.
 */
#define ZM_TEAM_SPECTATOR   "spectator"

/**
 * Client is a human.
 */
#define ZM_TEAM_HUMAN       "human"

/**
 * Client is a zombie.
 */
#define ZM_TEAM_ZOMBIE      "zombie"

/**
 * Optional substitute team for players who don't like to wait for a new game.
 *
 * Client is a a special spectator that can walk on the map as a regular player,
 * but is unable to affect the gameplay in any way (invisible, no-block,
 * no weapons, can't push buttons, etc).
 */
#define ZM_TEAM_GHOST       "ghost"

/**
 * @endsection
 */

/*____________________________________________________________________________*/

/**
 * Virtual team ID.
 *
 * All teams are referred by their ID. The team name is just used for initial
 * lookup, and eventually as output to the user.
 */
enum ZMTeam
{
    INVALID_ZM_TEAM = -1,
    
    ZMTeam_Unassigned = 0,
    ZMTeam_Spectator,
    ZMTeam_Human,
    ZMTeam_Zombie,
    ZMTeam_Ghost
}

/*____________________________________________________________________________*/

/**
 * Gets the virtual team ID for the specified team.
 *
 * @param teamName      Team name to look up.
 *
 * @return              Virtual team ID if found, otherwise INVALID_ZM_TEAM.
 */
native ZMTeam:ZM_GetTeamByName(const String:teamName[]);

/*____________________________________________________________________________*/

/**
 * Creates a new named virtual team.
 *
 * By default, the virtual team is mapped to the "unassigned" team index.
 *
 * @param teamName      Name of the new team.
 *
 * @return              Virtual team ID if created, otherwise INVALID_ZM_TEAM.
 * @error               Team name already exists.
 */
native ZMTeam:ZM_CreateTeam(const String:teamName[]);

/*____________________________________________________________________________*/

/**
 * Returns a list of registered virtual team names.
 *
 * Note: Modifications on this list will not be reflected to the team manager.
 *
 * @return              Handle to ADT array of team name strings. Must be closed
 *                      when no longer in use.
 */
native Handle:ZM_GetTeamNames();

/*____________________________________________________________________________*/

/**
 * Returns a list of registered virtual team IDs.
 *
 * Note: Modifications to this list will not be reflected to the team manager.
 *
 * @return              Handle to ADT array of ZMTeam IDs. Must be closed when
 *                      no longer in use.
 */
native Handle:ZM_GetTeams();

/*____________________________________________________________________________*/

/**
 * Sets a mapping between a virtual team and a game specific team index.
 *
 * @param team          Virtual team to map.
 * @param teamIndex     Game specific team index to map virtual team to.
 *
 * @noreturn
 * @error               Invalid team ID.
 */
native ZM_SetTeamIndex(ZMTeam:teamID, teamIndex);

/*____________________________________________________________________________*/

/**
 * Gets the game specific team index the virtual team is mapped to.
 *
 * @param team          Virtual team to get team index from.
 *
 * @noreturn
 * @error               Invalid team ID.
 */
native ZM_GetTeamIndex(ZMTeam:teamID);

/*____________________________________________________________________________*/

/**
 * Sets the client's team.
 *
 * @param client        Client index.
 * @param teamID        Team to set.
 */
native ZM_SetClientTeam(client, ZMTeam:teamID);

/*____________________________________________________________________________*/

/**
 * Gets the client's team.
 *
 * @param client        Client index.
 *
 * @return              Current client team.
 */
native ZMTeam:ZM_GetClientTeam(client);

/*____________________________________________________________________________*/

/**
 * Switches the specified player's team. If the player already is on that team,
 * nothing is done.
 *
 * @param client        Player's index.
 * @param team          Team ID.
 *
 * @noreturn
 */
native ZM_SwitchTeam(client, ZMTeam:teamID);

/*____________________________________________________________________________*/

/**
 * Returns whether the client is on the specified team.
 *
 * @param client        Client index.
 *
 * @return              True if client is on the team, false otherwise.
 */
stock bool:ZM_IsClientOnTeam(client, ZMTeam:teamID)
{
    return ZM_GetClientTeam(client) == teamID;
}

/*____________________________________________________________________________*/

/**
 * Returns whether the client has not yet been assigned to a team.
 *
 * @param client        Client index.
 *
 * @return              True if unassigned, false otherwise.
 */
stock bool:ZM_IsClientUnassigned(client)
{
    return ZM_GetClientTeam(client) == ZMTeam_Unassigned;
}

/*____________________________________________________________________________*/

/**
 * Returns whether the client is on the spectator team.
 *
 * @param client        Client index.
 *
 * @return              True if spectator, false otherwise.
 */
stock bool:ZM_IsClientSpectator(client)
{
    return ZM_GetClientTeam(client) == ZMTeam_Spectator;
}

/*____________________________________________________________________________*/

/**
 * Returns whether the client is on the human team.
 *
 * @param client        Client index.
 *
 * @return              True if human, false otherwise.
 */
stock bool:ZM_IsClientHuman(client)
{
    return ZM_GetClientTeam(client) == ZMTeam_Human;
}

/*____________________________________________________________________________*/

/**
 * Returns whether the client is on the zombie team.
 *
 * @param client        Client index.
 *
 * @return              True if zombie, false otherwise.
 */
stock bool:ZM_IsClientZombie(client)
{
    return ZM_GetClientTeam(client) == ZMTeam_Zombie;
}

/*____________________________________________________________________________*/

/**
 * Returns whether the client is on the ghost team.
 *
 * @param client        Client index.
 *
 * @return              True if ghost, false otherwise.
 */
stock bool:ZM_IsClientGhost(client)
{
    return ZM_GetClientTeam(client) == ZMTeam_Ghost;
}

/*____________________________________________________________________________*/

/**
 * Returns whether the specified team is a team for players.
 *
 * @param teamID        Team to check.
 *
 * @return              True if team is zombie or human, false otherwise.
 */
stock bool:ZM_IsPlayerTeam(ZMTeam:teamID)
{
    return teamID == ZMTeam_Human || teamID == ZMTeam_Zombie;
}

/*____________________________________________________________________________*/

/**
 * Counts clients on the specified team.
 * 
 * @param teamID    The team to count.
 * @param alive     (Optional) Only count alive clients. Default is false.
 * 
 * @return          The number of clients on the specified team.  
 */
stock ZM_CountTeam(ZMTeam:teamID, bool:alive = false)
{
    new count = 0;
    
    for (new client = 1; client < MaxPlayers; client++)
    {
        if (!IsClientInGame(client))
        {
            continue;
        }
        
        if (alive && !IsPlayerAlive(client))
        {
            continue;
        }
        
        if (ZM_GetClientTeam(client) == teamID)
        {
            count++;
        }
    }
    
    return count;
}
