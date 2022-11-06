#pragma once
#include <fstream>
#include "View.h"

class ViewExt : public View
{
protected:
	std::string saveName;

public:
	ViewExt(Model* model) : View(model), saveName("model.save") {}

	void save()
	{
		std::ofstream saveStream;

		saveStream.open(saveName, std::ios::out);

		if (saveStream.fail())
			return;

		saveStream << model->getSerealization();

		saveStream.close();
	}

	void load()
	{
		std::ifstream loadStream;

		loadStream.open(saveName);

		if (loadStream.fail())
			return;

		std::stringstream representation;

		representation << loadStream.rdbuf();

		loadStream.close();

		/*std::cout << representation.str();*/

		model->deserealizeRepresentation(representation.str());
	}

	virtual void initMenues()
	{
		std::vector<std::string> optionsForStartMenu = { "Start", "Load", "Exit" };
		startMenu = Menu(optionsForStartMenu);

		startMenu.setCallback(1, [this]()
			{
				this->toPlayIsChosen = true;
			}
		);

		startMenu.setCallback(2, [this]()
			{
				load();
			}
		);

		startMenu.setCallback(3, [this]()
			{
				this->toExitIsChosen = true;
			}
		);

		std::vector<std::string> optionsForContinueMenu = { "Continue", "Save", "Load", "Exit" };
		continueMenu = Menu(optionsForContinueMenu);

		continueMenu.setCallback(1, [this]()
			{
				this->toContinueIsChosen = true;
			}
		);

		continueMenu.setCallback(2, [=]()
			{
				save();
			}
		);

		continueMenu.setCallback(3, [=]()
			{
				load();
			}
		);

		continueMenu.setCallback(4, [this]()
			{
				this->toExitIsChosen = true;
			}
		);
	}

};