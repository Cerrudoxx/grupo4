/*
 *    Copyright (C) 2024 by YOUR NAME HERE
 *
 *    This file is part of RoboComp
 *
 *    RoboComp is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    RoboComp is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with RoboComp.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
	\brief
	@author authorname
*/

#ifndef SPECIFICWORKER_H
#define SPECIFICWORKER_H

#define HIBERNATION_ENABLED

//Aspirador
#include <genericworker.h>
#include <QGraphicsItem>
#include "abstract_graphic_viewer/abstract_graphic_viewer.h"
#include "Lidar3D.h"

//Nuevo
#include "Grid2D.h"

class SpecificWorker : public GenericWorker
{
	Q_OBJECT
public:
	SpecificWorker(TuplePrx tprx, bool startup_check);
	~SpecificWorker();
	bool setParams(RoboCompCommonBehavior::ParameterList params);

	RoboCompGrid2D::Result Grid2D_getPaths(RoboCompGrid2D::TPoint source, RoboCompGrid2D::TPoint target);

public slots:
	void initialize();
	void compute();
	void emergency();
	void restore();
	int startup_check();

private:

	AbstractGraphicViewer *viewer;


	// Activity 4
	enum class StateCell
	{
		EMPTY,
		OCCUPIED,
		UNKNOWN
	};

	struct TCell
	{
		int x;
		int y;
		StateCell state;
		QGraphicsItem *graphics_item;
	};

	void draw_lidar(auto &filtered_points, QGraphicsScene *scene);


};

#endif
