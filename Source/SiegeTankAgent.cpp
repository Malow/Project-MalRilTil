#include "SiegeTankAgent.h"
#include "AgentManager.h"
#include "PFManager.h"
#include "TargetingAgent.h"

SiegeTankAgent::SiegeTankAgent(Unit* mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	agentType = "SiegeTankAgent";
	//Broodwar->printf("SiegeTankAgent created (%s)", unit->getType().getName().c_str());
	
	goal = TilePosition(-1, -1);
}

void SiegeTankAgent::computeActions()
{
	bool defensive = false;

	bool staySieged = false;
	int distance = this->getUnit()->getDistance(Position(this->getGoal()));
	if(distance > 300)
	{
		if(unit->isSieged())
		{
			unit->unsiege();
		}
	}
	else if(distance < 200)
	{
		if(!unit->isSieged())
		{
			unit->siege();
		}
		staySieged = true;
	}

	if(this->enemyGroundUnitsWithinRange(400) > 3)
	{
		if(!unit->isSieged())
		{
			unit->siege();
		}
	}
	else if(this->enemyGroundUnitsWithinRange(500) == 0)
	{
		if(!staySieged)
		{
			if(unit->isSieged())
			{
				unit->unsiege();
			}
		}
	}
	PFManager::getInstance()->computeAttackingUnitActions(this, goal, defensive);
}
