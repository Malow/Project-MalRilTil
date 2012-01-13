#include "ScienceVesselAgent.h"
#include "PFManager.h"
#include "AgentManager.h"

ScienceVesselAgent::ScienceVesselAgent(Unit* mUnit)
{
	unit = mUnit;
	type = unit->getType();
	unitID = unit->getID();
	agentType = "ScienceVesselAgent";
	//Broodwar->printf("ScienceVesselAgent created (%s)", unit->getType().getName().c_str());

	goal = TilePosition(-1, -1);
}

void ScienceVesselAgent::computeActions()
{
	//prioritize offensive spells
	bool usedOffensiveSpell = false;
	if(Broodwar->enemy()->getRace() == Races::Protoss) 	//check if enemy is protoss
	{
		//check if EMP shockwave has been researched
		if(Broodwar->self()->hasResearched(TechTypes::EMP_Shockwave))
		{
			//check if science vessel has enough mana for EMP
			if(unit->getEnergy() >= TechTypes::EMP_Shockwave.energyUsed())
			{
				int radius = 8 * 32; //in pixels (8 = range of spell, 32 = pixels/radiusunit)
				Unit* archonUnit = NULL;
				//get surrounding units in range of spell
				for(set<Unit*>::iterator i = unit->getUnitsInRadius(radius).begin(); i != unit->getUnitsInRadius(radius).end(); i++)
				{
					if((*i)->exists())
					{
						if((*i)->getPlayer() == Broodwar->enemy()) //check if it's one of our enemies' units
						{
							//use EMP on first arbiter/high templar/dark archon seen as a threat(has enough energy to cast a spell)
							if((*i)->getType() == UnitTypes::Protoss_Arbiter)
							{
								if((*i)->getEnergy() >= TechTypes::Stasis_Field.energyUsed())
								{
									unit->useTech(TechTypes::EMP_Shockwave, (*i));
									usedOffensiveSpell = true;
									break; //********
								}
							}
							else if((*i)->getType() == UnitTypes::Protoss_High_Templar)
							{
								if((*i)->getEnergy() >= TechTypes::Psionic_Storm.energyUsed())
								{
									unit->useTech(TechTypes::EMP_Shockwave, (*i));
									usedOffensiveSpell = true;
									break; //********
								}
							}
							else if((*i)->getType() == UnitTypes::Protoss_Dark_Archon)
							{
								if((*i)->getEnergy() >= TechTypes::Feedback.energyUsed()) 
								{
									unit->useTech(TechTypes::EMP_Shockwave, (*i));
									usedOffensiveSpell = true;
									break; //********
								}
							}
							//if none found, use on archon with full shield
							else if((*i)->getType() == UnitTypes::Protoss_Archon && (*i)->getShields() == (*i)->getType().maxShields())
							{
								archonUnit = (*i);
							}
						}
					}
				}
				if(archonUnit != NULL)
				{
					unit->useTech(TechTypes::EMP_Shockwave, archonUnit);
					usedOffensiveSpell = true;
				}
			}
		}
	}
	else if(Broodwar->enemy()->getRace() == Races::Zerg) //check if enemy is zerg
	{
		//check if irradiate has been researched
		if(Broodwar->self()->hasResearched(TechTypes::Irradiate))
		{
			//check if unit has enough mana for irradiate
			if(unit->getEnergy() >= TechTypes::Irradiate.energyUsed())
			{
				int radius = 9 * 32; //in pixels (9 = range of spell, 32 = pixels/radiusunit)
				//get surrounding units in range of spell
				for(set<Unit*>::iterator i = unit->getUnitsInRadius(radius).begin(); i != unit->getUnitsInRadius(radius).end(); i++)
				{
					if((*i)->exists())
					{
						if((*i)->getPlayer() == Broodwar->enemy()) //check if it's one of our enemies' units
						{
							//use irradiate on first found defiler seen as a threat(has enough energy to cast spells & is not already afflicted by irradiate)
							if((*i)->getType() == UnitTypes::Zerg_Defiler && (*i)->getEnergy() >= TechTypes::Dark_Swarm && !(*i)->isIrradiated())
							{
								unit->useTech(TechTypes::Irradiate, (*i)); 
								usedOffensiveSpell = true;
								break; //***********
							}
						}
					}
				}
			}
		}
	}
	//Note: Terran not handled, since we will be using battlecruiser instead of science vessel.
	//if an offensive spell was not used, check if a unit need a defensive matrix
	if(!usedOffensiveSpell)
	{
		//no need to check if defensive matrix has been researched since it needs no research
		//check if science vessel has enough energy for defensive matrix
		if(unit->getEnergy() >= TechTypes::Defensive_Matrix.energyUsed())
		{
			//get surrounding units in range of spell
			int radius = 10 * 32; //in pixels (10 = range of spell, 32 = pixels/radiusunit)
			for(set<Unit*>::iterator i = unit->getUnitsInRadius(radius).begin(); i != unit->getUnitsInRadius(radius).end(); i++)
			{
				if((*i)->exists())
				{
					if((*i)->getPlayer() == Broodwar->self()) //check if it's one of our units
					{
						//check if unit is important(in our case, siege tanks(both modes), and other science vessels)
						if((*i)->getType() == UnitTypes::Terran_Science_Vessel || (*i)->getType() == UnitTypes::Terran_Siege_Tank_Tank_Mode || (*i)->getType() == UnitTypes::Terran_Siege_Tank_Siege_Mode)
						{
							//use on first found unit in need of a defensive matrix(low health, under attack & not already defense matrixed)
							//**************HP-funktioner kanske inte fungerar************
							if((*i)->isUnderAttack() && ((*i)->getHitPoints() < ((*i)->getType().maxHitPoints() / 5)) && !(*i)->isDefenseMatrixed())
							{
								unit->useTech(TechTypes::Defensive_Matrix, (*i));
								break; //*************
							}
						}
					} 
				}
			}
		}
	}





	bool defensive = true;
	PFManager::getInstance()->computeAttackingUnitActions(this, goal, defensive);
}
