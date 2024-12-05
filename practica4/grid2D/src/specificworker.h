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

//#define HIBERNATION_ENABLED

// Aspirador
#include <genericworker.h>
#include <QGraphicsItem>
#include "abstract_graphic_viewer/abstract_graphic_viewer.h"
#include "Lidar3D.h"

// Nuevo
#include "Grid2D.h"
#include <vector>
#include <Eigen/Dense>
#include <queue>
#include <cppitertools/enumerate.hpp>
#include <cppitertools/range.hpp>
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
	struct Params
	{
		float ROBOT_WIDTH = 460;  // mm
		float ROBOT_LENGTH = 480; // mm

		float MAX_ADV_SPEED = 1000; // mm/s
		float MAX_ROT_SPEED = 1;	// rad/s

		float TURN_THRESHOLD = ROBOT_LENGTH + 70; // mm 530 Distnacia minima para girar (evita chocar con pared)

		// SPIRAL THRESHOLD
		float SPIRAL_THRESHOLD = 1500; // mm Distancia minima alrededor para empezar a girar en espiral

		// FOLLOW_WALL THRESHOLDS
		float STOP_FOLLOW_WALL_THRESHOLD = TURN_THRESHOLD;				 // mm Distancia mininma para dejar de seguir la pared (ahora igual a TURN_THRESHOLD)
		float START_FOLLOW_WALL_THRESHOLD = 850;						 // mm Distancia minima para empezar a seguir la pared
		float MIN_FOLLOW_WALL_DISTANCE = TURN_THRESHOLD + 170;			 // mm 660 Distancia minima a la pared para seguir la pared (ancho del robot + margen)
		float MAX_FOLLOW_WALL_DISTANCE = MIN_FOLLOW_WALL_DISTANCE + 300; // mm 960 Distancia maxima a la pared para seguir la pared (ancho del robot + margen)

		// FOLLOW_WALL COUNTER
		int MIN_FOLLOW_WALL_COUNTER = 200;								   // Numero de iteraciones minimo para seguir la pared
		int FOLLOW_WALL_COUNTER = MIN_FOLLOW_WALL_COUNTER + 200;		   // Numero de iteraciones maximo para seguir la pared
		int MIN_FOLLOW_WALL_COUNTER_RESET = FOLLOW_WALL_COUNTER;		   // Numero de iteraciones para resetear el contador de seguir la pared
		int FOLLOW_WALL_COUNTER_RESET = MIN_FOLLOW_WALL_COUNTER_RESET * 2; // Numero de iteraciones para resetear el contador de seguir la pared

		float ADVANCE_THRESHOLD = ROBOT_WIDTH * 1.2; // mm

		// AWAY_FROM_WALL THRESHOLD
		float AWAY_WALL_THRESHOLD = 2.5 * 1000;			   // mm Distancia minima para alejarse de la pared
		float AWAY_WALL_SECURITY_BACK = ROBOT_LENGTH + 50; // mm Distancia minima para no chocar con la pared al alejarse

		// SPIRAL PARAMS
		float VEL_INICIAL = 300.f; // m/s usados para reset de velocidad
		float ROT_INICIAL = 0.85f; // rad/s usados para reset de velocidad de giro

		float VEL_ACTUAL = VEL_INICIAL; // m/s usado en espiral
		float ROT_ACTUAL = ROT_INICIAL; // rad/s usado en espiral

		float ALFA_GIRO = 1.15; // multiplicador de velocidad de giro en espiral

		// LIDAR 3D
		float LIDAR_OFFSET = 9.f / 10.f; // eight tenths of vector's half size
		float LIDAR_FRONT_SECTION = 0.5; // rads, aprox 30 degrees

		float LIDAR_RIGHT_SIDE_SECTION = M_PI / 3; // rads, 90 degrees
		float LIDAR_LEFT_SIDE_SECTION = -M_PI / 3; // rads, 90 degrees

		std::string LIDAR_NAME_LOW = "bpearl";
		std::string LIDAR_NAME_HIGH = "helios";
		QRectF GRID_MAX_DIM{-5000, 2500, 10000, -5000};

		float WALL_MIN_DISTANCE = 1000; // mm
	};
	Params params;

	AbstractGraphicViewer *viewer;
	// GRID
	static constexpr int GRID_DIMENSION_MM = 5000;
	static constexpr int TILE_SIZE_MM = 100;
	static constexpr int GRID_SIZE = GRID_DIMENSION_MM / TILE_SIZE_MM;

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
		QGraphicsRectItem *graphics_item;
	};

	std::array<std::array<TCell, GRID_SIZE>, GRID_SIZE> grid;

	void draw_lidar(auto &filtered_points, QGraphicsScene *scene);

	// lidar
	std::vector<Eigen::Vector2f> read_lidar_bpearl();
	void initializeGrid(auto &grid, QGraphicsScene *scene);
	void updateGrid(auto lidarPoints, auto &grid, auto &gridMoves);
	std::optional<std::tuple<int, int>> realToGrid(float x, float y);
	void DrawGrid(auto &grid, QGraphicsScene *scene);
	QPointF gridToReal(int i, int j);

void rutaDijkstra(int x, int y, int x2, int y2, std::array<std::array<TCell, GRID_SIZE>, GRID_SIZE> grid, std::vector<std::vector<bool>> &gridMoves)
;
	QGraphicsPolygonItem *robot_draw;
};

#endif
