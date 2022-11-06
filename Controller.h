#pragma once
#include <sstream>
#include <stack>
#include "MathUtility.h"
#include "SetUtility.h"
#include "KeyboardUtility.h"
#include "Model.h"
#include "ViewState.h"
#include "View.h"
#include "EntityState.h"
#include "Entity.h"

class Controller
{
protected:
	Model* model;
	View* view;

	int heuristicFunctionForShortest(Position pos, Position target)
	{
		return Position::difference(pos, target);
	}

	int heuristicFunctionForSafest(Position pos)
	{
		return pos.getX() + pos.getY() + model->getDangerLevel(pos);
	}

	class Console
	{
		std::vector<std::string> commands;
		std::vector<std::pair<int, std::function<void(int, int)>*>> callbacks;

	public:
		Console() {}

		Console(std::vector<std::string> commandsList)
		{
			commands = commandsList;
			callbacks = std::vector<std::pair<int, std::function<void(int, int)>*>>(commands.size());

			for (int i = 0; i < commands.size(); i++)
			{
				callbacks[i].first = 0;
				callbacks[i].second = nullptr;
			}
		}

		void setCallback(int commandNumber, int argumentCount, std::function<void(int, int)> callbackFunction)
		{
			if (commandNumber < 1 || commandNumber > commands.size())
				return;

			if (argumentCount < 0)
				return;

			callbacks[commandNumber - 1].first = argumentCount;
			callbacks[commandNumber - 1].second = new std::function<void(int, int)>(callbackFunction);
		}

		void addCommand(std::string commandName, int argumentCount, std::function<void(int, int)> callbackFunction)
		{
			commands.push_back(commandName);

			std::pair<int, std::function<void(int, int)> *> newCallback;

			newCallback.first = argumentCount;
			newCallback.second = new std::function<void(int, int)>(callbackFunction);

			callbacks.push_back(newCallback);
		}

		void applyCommand(int commandNumber)
		{
			if (callbacks[commandNumber - 1].first != 0)
				return;

			std::function<void(int, int)> callback = *callbacks[commandNumber - 1].second;
			callback(-1, -1);
		}

		void applyCommand(int commandNumber, int firstArg, int secondArg)
		{
			if (callbacks[commandNumber - 1].first != 2)
				return;

			std::function<void(int, int)> callback = *callbacks[commandNumber - 1].second;
			callback(firstArg, secondArg);
		}

		void applyCommandByName(std::string command, int firstArg, int secondArg)
		{
			int index;

			for (index = 0; index < commands.size(); index++)
				if (command == commands[index])
					break;
			
			if (index == commands.size())
				return;

			applyCommand(index + 1, firstArg, secondArg);
		}

		void applyCommandByName(std::string command)
		{
			int index;

			for (index = 0; index < commands.size(); index++)
				if (command == commands[index])
					break;

			if (index == commands.size())
				return;

			applyCommand(index + 1);
		}
	};

	Console consoleHandlers;

public:
	Controller(Model* m, View* v) : model(m), view(v) { initConsole(); }

	virtual void initConsole()
	{
		std::vector<std::string> commandsList =
		{"observe", "info", "addplanteatingmale", "addplanteatingfemale",
		 "addpredatormale", "addpredatorfemale", "addplant", "addfood",
		 "delete"};

		consoleHandlers = Console(commandsList);

		consoleHandlers.setCallback(1, 0, [=](int, int)
			{
				view->setObserveCommand();
			}
		);

		consoleHandlers.setCallback(2, 0, [=](int, int)
			{
				view->setInfoCommand();
			}
		);

		consoleHandlers.setCallback(3, 2, [=](int a, int b)
			{
				model->addPlantEatingMale(Position(a, b));
			}
		);

		consoleHandlers.setCallback(4, 2, [=](int a, int b)
			{
				model->addPlantEatingFemale(Position(a, b));
			}
		);

		consoleHandlers.setCallback(5, 2, [=](int a, int b)
			{
				model->addPredatorMale(Position(a, b));
			}
		);

		consoleHandlers.setCallback(6, 2, [=](int a, int b)
			{
				model->addPredatorFemale(Position(a, b));
			}
		);

		consoleHandlers.setCallback(7, 2, [=](int a, int b)
			{
				model->addPlant(Position(a, b));
			}
		);

		consoleHandlers.setCallback(8, 2, [=](int a, int b)
			{
				model->addFood(Position(a, b));
			}
		);

		consoleHandlers.setCallback(9, 2, [=](int a, int b)
			{
				Entity* ent = model->getIn(Position(a, b));

				handleDied(ent);

				model->getEntities().erase(ent);
			}
		);
	}

	void nextStep()
	{
		view->render();
		controlUponState();
	}

	void controlUponState()
	{
		if (view->getState() != CONSOLE)
			KeyboardUtility::handleKeyboard();
		view->nextState();
		view->render();

		ViewState previousViewState = view->getPreviousState();
		ViewState viewState = view->getState();

		if (viewState == OBSERVATION && previousViewState != CONTINUEMENU && previousViewState != STARTMENU
			&& previousViewState != CONSOLE)
		{
			nextStateOfModel();
		}

		else if (viewState == CONSOLE)
		{
			handleConsole();
		}
	}

	void actOfPlant(Entity* entity)
	{
		EntityState state = entity->getState();

		if (state == IDLE)
			;

		else if (state == REPRODUCING)
		{
			reproduceByPlant(entity);
		}
	}

	void actOfFood(Entity* entity)
	{
		EntityState state = entity->getState();

		if (state == IDLE)
			;
	}

	void actOfPlantEating(Entity* entity)
	{
		EntityState state = entity->getState();

		if (state == IDLE)
			randomlyWalk(entity);

		else if (state == WAITINGFORPAIR)
		{
			if (entity->hasCall())
			{
				entity->setTarget(entity->getCallee());
				entity->getTarget()->setTarget(entity);
				entity->setCallee(nullptr);
			}

			Entity* possiblePair;

			if (entity->isMale())
				possiblePair = model->getClosest(SetUtility::Intersection(
					SetUtility::Intersection(model->getAlive(), model->getFemale()), model->getPlantEating()), entity);

			else
				possiblePair = model->getClosest(SetUtility::Intersection(
					SetUtility::Intersection(model->getAlive(), model->getMale()), model->getPlantEating()), entity);

			if (possiblePair)
				entity->call(possiblePair);
		}

		else if (state == SEARCHINGFOREAT)
		{
			if (entity->getTarget() == nullptr || entity->getTarget()->getState() == DIED)
				entity->setTarget(model->getClosest(
					SetUtility::Intersection(
						SetUtility::Intersection(model->getAlive(), model->getEatable()),
						SetUtility::Union(model->getFood(), model->getPlants()))

					, entity));

			if (entity->getTarget())
				moveToTarget(entity, entity->getTarget());
		}

		else if (state == EATING)
			eatByPlantEating(entity, entity->getTarget());

		else if (state == RUNAWAY)
			moveToSafePlace(entity);

		else if (state == SEARCHINGFORPAIR)
		{
			if (entity->getTarget() == nullptr || entity->getTarget()->getState() == DIED)
				entity->setTarget(
					model->getClosest(
						SetUtility::Intersection(
							SetUtility::Intersection(
								SetUtility::Intersection(
									model->getAlive(), model->getReproducable()), model->getAnimals()), model->getPlantEating())

						, entity));

			moveToTarget(entity, entity->getTarget());
		}

		else if (state == REPRODUCING)
		{
			reproduceByPlantEating(entity, entity->getTarget());
		}
	}

	void actOfPredator(Entity* entity)
	{
		EntityState state = entity->getState();

		if (state == IDLE)
			randomlyWalk(entity);

		else if (state == WAITINGFORPAIR)
		{
			if (entity->hasCall())
			{
				entity->setTarget(entity->getCallee());
				entity->getTarget()->setTarget(entity);
				entity->setCallee(nullptr);
			}

			Entity* possiblePair;

			if (entity->isMale())
				possiblePair = model->getClosest(SetUtility::Intersection(
					SetUtility::Intersection(model->getAlive(), model->getFemale()), model->getPredators()), entity);

			else
				possiblePair = model->getClosest(SetUtility::Intersection(
					SetUtility::Intersection(model->getAlive(), model->getMale()), model->getPredators()), entity);

			if (possiblePair)
				entity->call(possiblePair);
		}

		else if (state == SEARCHINGFOREAT)
		{
			if (entity->getTarget() == nullptr || entity->getTarget()->getState() == DIED)
				entity->setTarget(model->getClosest(
					SetUtility::Intersection(
						SetUtility::Intersection(model->getAlive(), model->getEatable()), 
						SetUtility::Union(model->getFood(), model->getPlantEating()))

					, entity));

			if (entity->getTarget())
				moveToTarget(entity, entity->getTarget());
		}

		else if (state == EATING)
			eatByPredator(entity, entity->getTarget());

		else if (state == RUNAWAY)
			moveToSafePlace(entity);

		else if (state == SEARCHINGFORPAIR)
		{
			if (entity->getTarget() == nullptr || entity->getTarget()->getState() == DIED)
				entity->setTarget(
					model->getClosest(
						SetUtility::Intersection(
							SetUtility::Intersection(
								SetUtility::Intersection(
									model->getAlive(), model->getReproducable()), model->getAnimals()), model->getPredators())

						, entity));

			moveToTarget(entity, entity->getTarget());
		}

		else if (state == REPRODUCING)
		{
			reproduceByPredator(entity, entity->getTarget());
		}
	}

	void actUponState(Entity* entity)
	{
		if (entity->isPlantEating())
			actOfPlantEating(entity);

		else if (entity->isPredator())
			actOfPredator(entity);

		else if (entity->isPlant())
			actOfPlant(entity);

		else if (entity->isFood())
			actOfFood(entity);

		increaseHunger(entity, 1);
		increaseOld(entity, 1);
	}

	void makeActiveAllBorn()
	{
		std::set<Entity*> bornEntities = model->getInactive();

		for (Entity* entity : bornEntities)
			entity->makeActive();
	}

	void nextStateOf(Entity* entity)
	{
		if (entity->isAnimal())
			nextStateOfAnimal(entity);

		else if (entity->isPlant())
			nextStateOfPlant(entity);

		else if (entity->isFood())
			nextStateOfFood(entity);
	}

	void nextStateOfModel()
	{
		for (Entity* entity : model->getEntities())
		{
			if (!entity->isActive())
				continue;

			nextStateOf(entity);
			actUponState(entity);
		}

		handleAllDied();
		makeActiveAllBorn();
	}

	void nextStateOfAnimal(Entity* entity)
	{
		EntityState previousState = entity->getState();
		EntityState state = previousState;

		if (state == IDLE)
		{
			if (entity->isOld())
				state = DIED;

			else if (!entity->isPredator() && !model->isSafe(entity))
				state = RUNAWAY;

			else if (entity->isHunger() || entity->hasLowHealth())
				state = SEARCHINGFOREAT;

			else if (entity->isReproducable())
				state = WAITINGFORPAIR;
		}

		else if (state == SEARCHINGFOREAT)
		{
			if (entity->isOld() || entity->hasKillingHunger())
				state = DIED;

			else if (!entity->isPredator() && !model->isSafe(entity))
				state = RUNAWAY;

			else if (entity->getTarget() && model->isAdjacent(entity, entity->getTarget()))
				state = EATING;
		}

		else if (state == EATING)
		{
			if (entity->isOld())
				state = DIED;

			else if (!entity->isPredator() && !model->isSafe(entity))
				state = RUNAWAY;

			else if (entity->hasAdmissibleHunger())
				state = IDLE;

			else if (!entity->getTarget() || !model->isAdjacent(entity, entity->getTarget()))
				state = SEARCHINGFOREAT;
		}

		else if (state == RUNAWAY)
		{
			if (entity->isOld() || entity->hasZeroHealth() || entity->hasKillingHunger())
				state = DIED;

			else if (model->isSafe(entity))
				state = IDLE;
		}

		else if (state == WAITINGFORPAIR)
		{
			if (!entity->isPredator() && !model->isSafe(entity))
				state = RUNAWAY;

			else if (entity->isHunger())
				state = SEARCHINGFOREAT;

			else if (!entity->isReproducable())
				state == IDLE;

			else if (entity->hasPair())
				state = SEARCHINGFORPAIR;
		}

		else if (state == SEARCHINGFORPAIR)
		{
			if (!entity->isPredator() && !model->isSafe(entity))
				state = RUNAWAY;

			else if (entity->isHunger())
				state = SEARCHINGFOREAT;

			else if (!entity->isReproducable())
				state = IDLE;

			else if (!entity->hasPair())
				state = WAITINGFORPAIR;

			else if (model->isAdjacent(entity, entity->getTarget()))
				state = REPRODUCING;
		}

		else if (state == REPRODUCING)
		{
			if (!entity->isPredator() && !model->isSafe(entity))
				state = RUNAWAY;

			else if (entity->isHunger())
				state = SEARCHINGFOREAT;

			else if (!entity->isReproducable())
				state = IDLE;

			else if (!entity->hasPair())
				if (entity->isMale())
					state = WAITINGFORPAIR;
		}

		if (previousState == REPRODUCING && previousState != state
			|| previousState == SEARCHINGFORPAIR && previousState != state && state != REPRODUCING
			|| previousState == WAITINGFORPAIR && previousState != state && state != SEARCHINGFORPAIR)
		{
			entity->byeTarget();
		}

		if (state == DIED)
		{
			for (Entity* entity : model->getEntities())
			{
				if (entity->getTarget() == entity)
					entity->setTarget(nullptr);
			}
		}

		entity->setState(state);
	}

	void nextStateOfPlant(Entity* entity)
	{
		EntityState previousState = entity->getState();
		EntityState state = previousState;

		if (state == IDLE)
		{
			if (entity->hasZeroHealth() || entity->isOld())
				state = DIED;

			else if (entity->isReproducable())
				state = REPRODUCING;
		}

		else if (state == REPRODUCING)
		{
			if (entity->hasZeroHealth())
				state = DIED;

			else if (!entity->isReproducable())
				state = IDLE;
		}

		if (previousState == REPRODUCING && previousState != state
			|| previousState == SEARCHINGFORPAIR && previousState != state && state != REPRODUCING)
			entity->byeTarget();

		if (state == DIED)
		{
			for (Entity* entity : model->getEntities())
			{
				if (entity->getTarget() == entity)
					entity->setTarget(nullptr);
			}
		}

		entity->setState(state);
	}

	void nextStateOfFood(Entity* entity)
	{
		EntityState previousState = entity->getState();
		EntityState state = previousState;

		if (state == IDLE)
		{
			if (entity->hasZeroHealth() || entity->isOld())
				state = DIED;
		}

		if (state == DIED)
		{
			for (Entity* entity : model->getEntities())
			{
				if (entity->getTarget() == entity)
					entity->setTarget(nullptr);
			}
		}

		entity->setState(state);
	}

	void handleAllDied()
	{
		Entity* toDelete = nullptr;
		for (Entity* entity : model->getEntities())
		{
			if (toDelete)
			{
				for (Entity* enity : model->getEntities())
					handleDied(toDelete);

				model->getEntities().erase(toDelete);
				toDelete = nullptr;
			}

			if (entity->getState() == DIED)
				toDelete = entity;
		}

		if (toDelete)
		{
			handleDied(toDelete);
			model->getEntities().erase(toDelete);
		}
	}

	void handleDied(Entity* entity)
	{
		for (Entity* another : model->getEntities())
		{
			if (another->getTarget() == entity)
				another->setTarget(nullptr);

			if (another->getCallee() == entity)
				another->setCallee(nullptr);
		}
	}

	void handleConsole()
	{
		std::string command;
		std::getline(std::cin, command);

		if (command.empty() || command == "\n")
			return;

		std::stringstream commandTokens = std::stringstream(command);
		std::vector<std::string> tokens;

		std::string token;
		while (std::getline(commandTokens, token, ' '))
			tokens.push_back(token);

		command = tokens[0];

		int argumentCount = tokens.size() - 1;

		if (argumentCount == 2)
			consoleHandlers.applyCommandByName(command, std::stoi(tokens[1]), std::stoi(tokens[2]));

		else if (argumentCount == 0)
			consoleHandlers.applyCommandByName(command);
	}

	void increaseHealth(Entity* eating, int healthAddition)
	{
		int health = eating->getHealth();

		health += healthAddition;

		if (health > eating->getMaxHealth())
			health = eating->getMaxHealth();

		eating->setHealth(health);
	}

	void decreaseHealth(Entity* eating, int healthDecreasion)
	{
		int health = eating->getHealth();

		health -= healthDecreasion;

		if (health < 0)
			health = 0;

		eating->setHealth(health);
	}

	void increaseHunger(Entity* eating, int hungerDecreasion)
	{
		int hunger = eating->getHunger();

		hunger += hungerDecreasion;

		if (hunger > eating->getMaxHunger())
			hunger = eating->getMaxHunger();

		eating->setHunger(hunger);
	}

	void decreaseHunger(Entity* eating, int hungerDecreasion)
	{
		int hunger = eating->getHunger();

		hunger -= hungerDecreasion;

		if (hunger < 0)
			hunger = 0;

		eating->setHunger(hunger);
	}

	void increaseOld(Entity* eating, int oldIncreasion)
	{
		int old = eating->getOld();

		old += oldIncreasion;

		if (old > eating->getMaxOld())
			old = eating->getMaxOld();

		eating->setOld(old);
	}

	void randomlyWalk(Entity* entity)
	{
		std::set<Position> freeAdjacentPositions = model->getFreeAdjacent(entity->getPosition());

		if (!freeAdjacentPositions.empty())
		{
			Position pos = SetUtility::randomFrom(freeAdjacentPositions);

			entity->setPosition(pos);
			//moveTo(entity, pos);
		}
	}

	void moveToTarget(Entity* entity, Entity* target)
	{
		std::set<Position> freeAdjacent = model->getFreeAdjacent(entity->getPosition());


		if (!freeAdjacent.empty())
		{
			Position min = *freeAdjacent.begin();

			for (Position pos : freeAdjacent)
				if (heuristicFunctionForShortest(pos, target->getPosition()) < heuristicFunctionForShortest(min, target->getPosition()))
					min = pos;

			entity->setPosition(min);
		}
	}

	void moveToSafePlace(Entity* entity)
	{
		Position pos = model->getClosest(model->getSafePlaces(), entity->getPosition());

		if (pos != Position(-1, -1))
		{
			std::set<Position> freeAdjacent = model->getFreeAdjacent(entity->getPosition());

			if (!freeAdjacent.empty())
			{
				Position min = *freeAdjacent.begin();

				for (Position position : freeAdjacent)
					if (heuristicFunctionForShortest(position, pos) < heuristicFunctionForShortest(min, pos))
						min = position;

				entity->setPosition(min);
			}
		}
	}

	void eatByPlantEating(Entity* eating, Entity* eatable)
	{
		int healthAddition = 0;
		int hungerDecreasion = 0;

		if (eatable->isPlant())
		{
			healthAddition = 5;
			hungerDecreasion = 10;
		}

		else if (eatable->isFood())
		{
			healthAddition = 2;
			hungerDecreasion = 4;
		}

		increaseHealth(eating, healthAddition);
		decreaseHunger(eating, hungerDecreasion);

		decreaseHealth(eatable, healthAddition);
		nextStateOf(eatable);
	}

	void reproduceByPlantEating(Entity* one, Entity* another)
	{
		std::set<Position> freePositions = model->getFreeAdjacent(one->getPosition());

		if (freePositions.empty())
			return;

		int randomNumber = MathUtility::randomInt(1, 4);

		if (randomNumber == 1)
		{
			Position pos = SetUtility::randomFrom(freePositions);

			int whetherMale = MathUtility::randomInt(0, 1);

			if (whetherMale)
				model->bornNewPlantEatingMale(pos);

			else
				model->bornNewPlantEatingFemale(pos);
		}
	}

	void eatByPredator(Entity* eating, Entity* eatable)
	{
		int healthAddition = 0;
		int hungerDecreasion = 0;

		if (eatable->isPlantEating())
		{
			healthAddition = 6;
			hungerDecreasion = 5;
		}

		else if (eatable->isFood())
		{
			healthAddition = -2;
			hungerDecreasion = 4;
		}

		increaseHealth(eating, healthAddition);
		decreaseHunger(eating, hungerDecreasion);

		decreaseHealth(eatable, healthAddition);
		nextStateOf(eatable);
	}

	void reproduceByPredator(Entity* one, Entity* another)
	{
		std::set<Position> freePositions = model->getFreeAdjacent(one->getPosition());

		if (freePositions.empty())
			return;

		Position pos = SetUtility::randomFrom(freePositions);

		int randomNumber = MathUtility::randomInt(1, 8);

		if (randomNumber == 1)
		{
			int whetherMale = MathUtility::randomInt(0, 1);

			if (whetherMale)
				model->bornNewPredatorMale(pos);

			else
				model->bornNewPredatorFemale(pos);
		}
	}

	void reproduceByPlant(Entity* one)
	{
		std::set<Position> freePositions = model->getFreeAdjacent(one->getPosition());

		if (freePositions.empty())
			return;

		Position pos = SetUtility::randomFrom(freePositions);

		int randomNumber = MathUtility::randomInt(1, 8);

		if (randomNumber == 1)
		{
			model->bornNewPlant(pos);
		}
	}
};