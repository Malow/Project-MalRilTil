#include "ExplorationSquad.h"
#include "UnitAgent.h"
#include "ExplorationManager.h"

ExplorationSquad::ExplorationSquad(int mId, string mName, int mPriority)
{
	this->id = mId;
	this->type = EXPLORER;
	this->moveType = AIR;
	this->name = mName;
	this->priority = mPriority;
	activePriority = priority;
	active = false;
	required = false;
	goal = Broodwar->self()->getStartLocation();
	goalSetFrame = 0;
	currentState = STATE_NOT_SET;
	this->startLocations = Broodwar->getStartLocations();
	this->enemyBaseExplored = false;
	this->timesSenseExploredBase = 0;
}

bool ExplorationSquad::isActive()
{
	return active;
}

void ExplorationSquad::defend(TilePosition mGoal)
{

}

void ExplorationSquad::attack(TilePosition mGoal)
{
	TilePosition nGoal = Broodwar->self()->getStartLocation();
	if (nGoal.x() >= 0)
	{
		this->goal = nGoal;
		setMemberGoals(goal);
	}
}

void ExplorationSquad::assist(TilePosition mGoal)
{

}

void ExplorationSquad::computeActions()
{
	if(this->isUnderAttack() == true)
	{
		this->currentState = STATE_DEFEND;
	}
	if (!active)
	{
		if (isFull())
		{
			active = true;
		}
		return;
	}

	//First, remove dead agents
	for(int i = 0; i < (int)agents.size(); i++)
	{
		if(!agents.at(i)->isAlive())
		{
			agents.erase(agents.begin() + i);
			i--;
		}
	}

	//All units dead, go back to inactive
	if ((int)agents.size() == 0)
	{
		this->startLocations.clear();
		TilePosition nGoal = ExplorationManager::getInstance()->getNextToExplore(this);
		if (nGoal.x() >= 0)
		{
			this->goal = nGoal;
			setMemberGoals(goal);
		}
		this->clearGoal();
		active = false;
		return;
	}
	
	if (active)
	{
		TilePosition nGoal;
		if(this->timesSenseExploredBase > 100)
		{
			this->startLocations = Broodwar->getStartLocations();
			this->timesSenseExploredBase = 0;
		}
		if (activePriority != priority)
		{
			priority = activePriority;
		}

		if(startLocations.size() == 0)
		{
			this->timesSenseExploredBase = this->timesSenseExploredBase + 1; 
			nGoal = ExplorationManager::getInstance()->getNextToExplore(this);
		}
		else
		{
			set<TilePosition>::iterator go;
			go=startLocations.begin();
			startLocations.erase(go);
			if(*go != Broodwar->self()->getStartLocation())
			{
				nGoal = *go;
			}
			else
			{
				nGoal = ExplorationManager::getInstance()->getNextToExplore(this);
			}
		}
		Broodwar->drawTextScreen(300, 40, "TimeSenseStartLocExplore %d", this->timesSenseExploredBase); // For testing
		if (nGoal.x() >= 0)
		{
			this->goal = nGoal;
			setMemberGoals(goal);
		}
	}
}

void ExplorationSquad::printInfo()
{
	string f = "NotFull";
	if (isFull())
	{
		f = "Full";
	}
	string a = "Inactive";
	if (isActive())
	{
		a = "Active";
	}

	Broodwar->printf("[ExplorationSquad %d] (%s, %s) Goal: (%d,%d) prio: %d", id, f.c_str(), a.c_str(), goal.x(), goal.y(), priority);
}

void ExplorationSquad::clearGoal()
{
	
}

TilePosition ExplorationSquad::getGoal()
{
	return goal;
}

bool ExplorationSquad::hasGoal()
{
	if (goal.x() < 0 || goal.y() < 0)
	{
		return false;
	}
	return true;
}
