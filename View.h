#pragma once
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <functional>
#include "ViewState.h"
#include "Model.h"
#include "Entity.h"

class View
{
protected:
	Model* model;
	ViewState previousState;
	ViewState state;

	static const int consoleWidth = 80;
	static const int consoleHeight = 80;

	bool toPlayIsChosen;
	bool toContinueIsChosen;
	bool toExitIsChosen;
	bool isObserve;
	bool isInfo;

	class Menu
	{
		int currentOption;
		std::vector<std::string> options;
		std::vector<std::function<void()> *> callbacks;

	public:
		Menu() : currentOption(1) {}
		Menu(std::vector<std::string> opts) : currentOption(1), options(opts)
		{
			callbacks = std::vector<std::function<void()> *>(options.size());

			for (int i = 0; i < options.size(); i++)
				callbacks[i] = nullptr;
		}

		~Menu()
		{
			for (int i = 0; i < options.size(); i++)
				if (callbacks[i])
					delete callbacks[i];
		}

		void selectAbove()
		{
			currentOption--;

			if (currentOption < 1)
				currentOption = 1;
		}

		void selectBelow()
		{
			currentOption++;

			if (currentOption > options.size())
				currentOption = options.size();
		}

		void setCallback(int optionNumber, std::function<void()> callbackFunction)
		{
			if (optionNumber < 1 || optionNumber > options.size())
				return;

			callbacks[optionNumber - 1] = new std::function<void()>(callbackFunction);
		}

		void drawMenu()
		{
			for (int i = 0; i < 10; i++)
				std::cout << "\n";

			for (int i = 0; i < options.size(); i++)
			{
				std::string option = options[i];
				int offset = 0;

				if (i == currentOption - 1)
				{
					offset = 2;
					option = std::string("* ") + option;
				}

				std::cout << marginLeft(option, getMarginAmountToCenter(option)) << "\n";
			}
		}

		void makeSelection()
		{
			std::function<void()> callback = *callbacks[currentOption - 1];
			callback();
		}
	};

	Menu startMenu;
	Menu continueMenu;

public:
	View(Model* model) : state(STARTMENU), previousState(STARTMENU), model(model)
	{
		toPlayIsChosen = false;
		toContinueIsChosen = false;
		toExitIsChosen = false;

		isObserve = false;
		isInfo = false;
	}

	virtual void initMenues()
	{
		std::vector<std::string> optionsForStartMenu = { "Start", "Exit" };
		startMenu = Menu(optionsForStartMenu);

		startMenu.setCallback(1, [this]()
			{
				this->toPlayIsChosen = true;
			}
		);
		
		startMenu.setCallback(2, [this]()
			{
				this->toExitIsChosen = true;
			}
		);

		std::vector<std::string> optionsForContinueMenu = { "Continue", "Exit" };
		continueMenu = Menu(optionsForContinueMenu);

		continueMenu.setCallback(1, [this]()
			{
				this->toContinueIsChosen = true;
			}
		);

		continueMenu.setCallback(2, [this]()
			{
				this->toExitIsChosen = true;
			}
		);
	}

	void resetEvents()
	{
		toPlayIsChosen = false;
		toContinueIsChosen = false;
		toExitIsChosen = false;

		isObserve = false;
		isInfo = false;
	}

	bool isObserveCommand()
	{
		return isObserve;
	}

	bool isControlCommand()
	{
		return isInfo;
	}

	void setObserveCommand() { isObserve= true; }

	void setInfoCommand() { isInfo = true; }

	ViewState getPreviousState() { return previousState; }

	ViewState getState() { return state; }

	void setState(ViewState st)
	{ 
		previousState = state;
		state = st;
	}

	static std::string marginLeft(std::string str, int amount)
	{
		std::string result = str;

		for (int i = 0; i < amount; i++)
			result = std::string(" ") + result;

		return result;
	}

	static int getMarginAmountToCenter(std::string str)
	{
		return (consoleWidth - str.size()) / 2;
	}

	void drawMap()
	{
		std::vector<std::vector<std::string>> map;

		for (int i = 0; i < model->getMap()->getHeight(); i++)
		{
			std::vector<std::string> row;
			for (int j = 0; j < model->getMap()->getWidth(); j++)
				row.push_back(std::string("-"));

			map.push_back(row);
		}

		for (Entity* entity : model->getEntities())
		{
			int rowCoordinate = entity->getPosition().getY();
			int colCoordinate = entity->getPosition().getX();

			std::string symbol = entity->getSymbolNotation();

			map[rowCoordinate - 1][colCoordinate - 1] = symbol;
		}

		for (int i = model->getMap()->getHeight() - 1; i >= 0; i --)
		{
			if (i + 1 < 10)
				std::cout << (i + 1) << "  ";

			else
				std::cout << (i + 1) << " ";

			for (int j = 0; j < model->getMap()->getWidth(); j++)
				std::cout << map[i][j] << "  ";

			std::cout << "\n";
		}

		std::cout << "   ";
		for (int j = 0; j < model->getMap()->getWidth(); j++)
			if (j + 1 < 10)
				std::cout << (j + 1) << "  ";

			else
				std::cout << (j + 1) << " ";

		std::cout << "\n";
	}

	void drawInfo()
	{
		std::cout << std::setw(10) << "ID"
			<< std::setw(5) << "OLD"
			<< std::setw(8) << "HEALTH"
			<< std::setw(8) << "HUNGER"
			<< std::setw(20) << "TYPE"
			<< std::setw(8) << "SEX"
			<< std::setw(5) << "XPOS"
			<< std::setw(5) << "YPOS" << "\n";
		
		for (Entity* entity : model->getEntities())
			std::cout << std::setw(10) << entity->getId()
			<< std::setw(5) << entity->getOld()
			<< std::setw(8) << entity->getHealth()
			<< std::setw(8) << entity->getHunger()
			<< std::setw(20) << entity->typeName()
			<< std::setw(8) << entity->getSexNotation()
			<< std::setw(5) << entity->getPosition().getX()
			<< std::setw(5) << entity->getPosition().getY() << "\n";
	}

	void drawMapWithConsole()
	{
		drawMap();

		std::cout << "\n\n:";
	}

	void drawMapWithPlayerWithConsole()
	{
		drawInfo();

		std::cout << "\n\n:";
	}

	void nextState()
	{
		ViewState state = getState();

		if (state == STARTMENU)
		{
			if (KeyboardUtility::onEnter())
				startMenu.makeSelection();

			if (toPlayIsChosen)
				state = OBSERVATION;

			else if (toExitIsChosen)
				exit(0);
		}

		else if (state == OBSERVATION)
		{
			if (KeyboardUtility::onEsc())
				state = CONTINUEMENU;

			else if (KeyboardUtility::onSemicolon())
				state = CONSOLE;
		}

		else if (state == CONTINUEMENU)
		{
			if (KeyboardUtility::onEnter())
				continueMenu.makeSelection();

			if (toContinueIsChosen)
				state = OBSERVATION;

			else if (toExitIsChosen)
				std::exit(0);
		}

		else if (state == CONSOLE)
		{
			if (KeyboardUtility::onEsc() && getPreviousState() == OBSERVATION)
				state = OBSERVATION;

			else if (isObserve)
				state = OBSERVATION;

			else if (isInfo)
				state = INFO;
		}

		else if (state == INFO)
		{
			if (KeyboardUtility::onEsc())
				state = CONSOLE;
		}

		if (state == STARTMENU)
		{
			if (KeyboardUtility::onS())
				startMenu.selectBelow();

			else if (KeyboardUtility::onW())
				startMenu.selectAbove();
		}

		else if (state == CONTINUEMENU)
		{
			if (KeyboardUtility::onS())
				continueMenu.selectBelow();

			else if (KeyboardUtility::onW())
				continueMenu.selectAbove();
		}

		resetEvents();
		setState(state);
	}

	void render()
	{
		system("cls");

		if (state == STARTMENU)
			startMenu.drawMenu();

		else if (state == OBSERVATION)
			drawMap();

		else if (state == INFO)
			drawInfo();

		else if (state == CONSOLE)
			drawMapWithConsole();

		else if (state == CONTINUEMENU)
			continueMenu.drawMenu();
	}
};