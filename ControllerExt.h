#pragma once
#include "Controller.h"
#include "Mountain.h"

class ControllerExt : public Controller
{
public:
	ControllerExt(Model* model, View* view) : Controller(model, view)
	{
		initConsoleExt();
	}

	void initConsoleExt()
	{
		consoleHandlers.addCommand("addmountain", 2, [=](int a, int b)
			{
				model->addEntity(new Mountain(model->getEntities().size(), Position(a, b)));
			}
		);
	}
};