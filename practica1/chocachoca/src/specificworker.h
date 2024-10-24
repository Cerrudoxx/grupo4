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

// #define HIBERNATION_ENABLED

#include <genericworker.h>
#include "abstract_graphic_viewer/abstract_graphic_viewer.h"
#include <expected>
#include <random>

class SpecificWorker : public GenericWorker
{
    Q_OBJECT
public:
    SpecificWorker(TuplePrx tprx, bool startup_check);
    ~SpecificWorker();
    bool setParams(RoboCompCommonBehavior::ParameterList params);

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
        float MAX_ROT_SPEED = 1;    // rad/s

        float TURN_THRESHOLD = ROBOT_LENGTH + 70; // mm 530 Distnacia minima para girar (evita chocar con pared)

        // SPIRAL THRESHOLD
        float SPIRAL_THRESHOLD = 1500; // mm Distancia minima alrededor para empezar a girar en espiral

        // FOLLOW_WALL THRESHOLDS
        float STOP_FOLLOW_WALL_THRESHOLD = TURN_THRESHOLD;               // mm Distancia mininma para dejar de seguir la pared (ahora igual a TURN_THRESHOLD)
        float START_FOLLOW_WALL_THRESHOLD = 850;                         // mm Distancia minima para empezar a seguir la pared
        float MIN_FOLLOW_WALL_DISTANCE = TURN_THRESHOLD + 170;           // mm 660 Distancia minima a la pared para seguir la pared (ancho del robot + margen)
        float MAX_FOLLOW_WALL_DISTANCE = MIN_FOLLOW_WALL_DISTANCE + 300; // mm 960 Distancia maxima a la pared para seguir la pared (ancho del robot + margen)

        // FOLLOW_WALL COUNTER
        int MIN_FOLLOW_WALL_COUNTER = 200;                                 // Numero de iteraciones minimo para seguir la pared
        int FOLLOW_WALL_COUNTER = MIN_FOLLOW_WALL_COUNTER + 200;           // Numero de iteraciones maximo para seguir la pared
        int MIN_FOLLOW_WALL_COUNTER_RESET = FOLLOW_WALL_COUNTER ;       // Numero de iteraciones para resetear el contador de seguir la pared
        int FOLLOW_WALL_COUNTER_RESET = MIN_FOLLOW_WALL_COUNTER_RESET * 2; // Numero de iteraciones para resetear el contador de seguir la pared

        float ADVANCE_THRESHOLD = ROBOT_WIDTH * 1.2; // mm

        // AWAY_FROM_WALL THRESHOLD
        float AWAY_WALL_THRESHOLD = 2.5 * 1000; // mm Distancia minima para alejarse de la pared
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

    bool startup_check_flag;
    AbstractGraphicViewer *viewer;

    // state machine
    enum class STATE
    {
        SPIRAL,
        FORWARD,
        TURN,
        FOLLOW_WALL,
        AWAY_WALL
    };
    STATE state = STATE::FORWARD;

    using RetVal = std::tuple<STATE, float, float>;

    RetVal forward(auto &filtered_points);
    RetVal turn(auto &filtered_points);
    RetVal spiral(auto &filtered_points);
    RetVal follow_wall(auto &filtered_points);
    RetVal away_wall(auto &filtered_points);

    void draw_lidar(auto &filtered_points, QGraphicsScene *scene);
    QGraphicsPolygonItem *robot_draw;
    std::expected<int, string> closest_lidar_index_to_given_angle(const auto &points, float angle);

    // random number generator
    std::random_device rd;
};

#endif
