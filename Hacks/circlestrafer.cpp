#include "circlestrafer.h"

bool Settings::CircleStrafer::enabled = false;
ButtonCode_t Settings::CircleStrafer::key = ButtonCode_t::KEY_R;

float rotation;
int CircleFactor = 0;

void CircleStrafe(C_BasePlayer* localplayer, CUserCmd* cmd) 
{
    CircleFactor++;
    if (CircleFactor > 361)
        CircleFactor = 0;

    int rotBy = 3.0 * CircleFactor - globalVars->interval_per_tick;

    if (Settings::CircleStrafer::enabled && inputSystem->IsButtonDown(Settings::CircleStrafer::key))
	{
        float rotation = DEG2RAD(rotBy);
        float cos_rot = cos(rotation);
        float sin_rot = sin(rotation);

        float changeForward = (cos_rot * cmd->forwardmove) - (sin_rot * cmd->sidemove);
        float changeSide = (sin_rot * cmd->forwardmove) + (cos_rot * cmd->sidemove);

        cmd->forwardmove = changeForward;
        cmd->sidemove = changeSide;
	}
    else
	{
        CircleFactor = 0;
	}	
}


void CircleStrafer::CreateMove(CUserCmd* cmd)
{
	C_BasePlayer* localplayer = (C_BasePlayer*) entityList->GetClientEntity(engine->GetLocalPlayer());

	if (!localplayer->GetAlive())
		return;

	CircleStrafe(localplayer, cmd);
}
