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
	//check if yamato gun has been researched
	if(Broodwar->self()->hasResearched(TechTypes::Yamato_Gun))
	{
		//check if battlecruiser has enough mana to use it
		if(unit->getEnergy() >= TechTypes::Yamato_Gun.energyUsed())
		{
			//get units in range of yamato gun
			Unit* valkyrieUnit = NULL;
			Unit* scienceVesselUnit = NULL;
			Unit* siegeTankUnit = NULL;
			int radius = 10 * 32; //in pixels (10 = range of spell, 32 = pixels/rangeunit)
			bool yamatoGunUsed = false;
			for(set<Unit*>::iterator i = unit->getUnitsInRadius(radius).begin(); i != unit->getUnitsInRadius(radius).end(); i++)
			{
				if((*i)->exists())
				{
					if((*i)->getPlayer() == Broodwar->enemy()) //check if unit is enemy
					{
						//Note: battlecruisers are only used against a terran enemy
						//prioritize use of yamato gun on units with over 260 hitpoints(damage of yamato gun)(with some exceptions)
						if((*i)->getType() == UnitTypes::Terran_Battlecruiser && (*i)->getHitPoints() > 260)
						{
							unit->useTech(TechTypes::Yamato_Gun, (*i));
							Broodwar->printf("(DBG) yamato gun used on battlecruiser");
							yamatoGunUsed = true;
							break;
						}
						else if((*i)->getType() == UnitTypes::Terran_Valkyrie && (*i)->getHitPoints() == (*i)->getType().maxHitPoints())
						{
							valkyrieUnit= (*i);
						}
						else if((*i)->getType() == UnitTypes::Terran_Science_Vessel && (*i)->getHitPoints() == (*i)->getType().maxHitPoints())
						{
							scienceVesselUnit = (*i);
						}
						else if(((*i)->getType() == UnitTypes::Terran_Siege_Tank_Siege_Mode || (*i)->getType() == UnitTypes::Terran_Siege_Tank_Tank_Mode) && ((*i)->getHitPoints() == (*i)->getType().maxHitPoints())) 
						{
							siegeTankUnit = (*i);
						}
					}
				}
			}
			//if not used on a battlecruise, use yamato gun on other units(if found)
			if(!yamatoGunUsed)
			{
				if(scienceVesselUnit != NULL)
				{
					unit->useTech(TechTypes::Yamato_Gun, scienceVesselUnit);
					Broodwar->printf("(DBG) yamato gun used on science vessel");
				}
				else if(valkyrieUnit != NULL)
				{
					unit->useTech(TechTypes::Yamato_Gun, valkyrieUnit);
					Broodwar->printf("(DBG) yamato gun used on valkyrie");
				}
				else if(siegeTankUnit != NULL)
				{
					unit->useTech(TechTypes::Yamato_Gun, siegeTankUnit);
					Broodwar->printf("(DBG) yamato gun used on siege tank");
				}
			}
		}
	}

	bool defensive = false;
	PFManager::getInstance()->computeAttackingUnitActions(this, goal, defensive);
}
