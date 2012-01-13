#include "BattlecruiserAgent.h"
#include "PFManager.h"
#include "AgentManager.h"
#include "TargetingAgent.h"

BattlecruiserAgent::BattlecruiserAgent(Unit* mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	agentType = "BattlecruiserAgent";
	//Broodwar->printf("BattlecruiserAgent created (%s)", unit->getType().getName().c_str());
	
	goal = TilePosition(-1, -1);
}

void BattlecruiserAgent::computeActions()
{
	// sniping Tanks, Goliaths, Turrets and Battlecruisers,

	//check if yamato gun has been researched
	if(Broodwar->self()->hasResearched(TechTypes::Yamato_Gun))
	{
		//check if battlecruiser has enough mana to use it
		if(unit->getEnergy() >= TechTypes::Yamato_Gun.energyUsed())
		{
			//get units in range of yamato gun
			Unit* tankUnit = NULL;
			int radius = 10 * 32; //in pixels (10 = range of spell, 32 = pixels/rangeunit)
			for(set<Unit*>::iterator i = unit->getUnitsInRadius(radius).begin(); i != unit->getUnitsInRadius(radius).end(); i++)
			{
				if((*i)->exists())
				{
					if((*i)->getPlayer() == Broodwar->enemy()) //check if unit is enemy
					{
						//prioritize use of yamato gun on other battlecruiser with more than 260 hitpoints(damage of yamato gun)
						if((*i)->getType() == UnitTypes::Terran_Battlecruiser && (*i)->getHitPoints() > 260)
						{
							unit->useTech(TechTypes::Yamato_Gun, (*i));
						}//otherwise use it to snipe tanks with full health(both modes)
						else if((*i)->getType() == UnitTypes::Terran_Siege_Tank_Siege_Mode || (*i)->getType() == UnitTypes::Terran_Siege_Tank_Tank_Mode) 
						{
							if((*i)->getHitPoints() == (*i)->getType().maxHitPoints())
							{
								tankUnit = (*i);
							}
						}
					}
				}
			}
			if(tankUnit != NULL)
			{
				unit->useTech(TechTypes::Yamato_Gun, tankUnit);
			}
		}
	}

	bool defensive = false;
	PFManager::getInstance()->computeAttackingUnitActions(this, goal, defensive);
}
