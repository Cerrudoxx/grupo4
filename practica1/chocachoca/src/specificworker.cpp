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
#include <ranges>
#include "specificworker.h"

#define Black "\x1B[30m"
#define Red "\x1B[31m"
#define Green "\x1B[32m"
#define Yellow "\x1B[33m"
#define Blue "\x1B[34m"
#define Magenta "\x1B[35m"
#define Cyan "\x1B[36m"
#define White "\x1B[37m"

/**
 * \brief Default constructor
 */
SpecificWorker::SpecificWorker(TuplePrx tprx, bool startup_check) : GenericWorker(tprx)
{
    this->startup_check_flag = startup_check;
    //	QLoggingCategory::setFilterRules("*.debug=false\n");
}

/**
 * \brief Default destructor
 */
SpecificWorker::~SpecificWorker()
{
    std::cout << "Destroying SpecificWorker" << std::endl;
}

bool SpecificWorker::setParams(RoboCompCommonBehavior::ParameterList params)
{
    return true;
}

void SpecificWorker::initialize()
{
    std::cout << "Initialize worker" << std::endl;
    if (this->startup_check_flag)
    {
        this->startup_check();
    }
    else
    {
        ///////////// Your code ////////
        // Viewer
        viewer = new AbstractGraphicViewer(this->frame, params.GRID_MAX_DIM);
        auto [r, e] = viewer->add_robot(params.ROBOT_WIDTH, params.ROBOT_LENGTH, 0, 100, QColor("Blue"));
        robot_draw = r;
        viewer->show();

        connect(pushButton_stop, &QPushButton::clicked, [this]()
                {
                try
                { omnirobot_proxy->setSpeedBase(0, 0, 0); }
                catch (const Ice::Exception &e)
                { std::cout << e << std::endl; }
                std::terminate(); });

        horizontalSlider_wall_distance->setValue(params.WALL_MIN_DISTANCE);
        lcdNumber_wall_distance->display(params.WALL_MIN_DISTANCE);
        // connect the slider with the wall distance using a lambda
        connect(horizontalSlider_wall_distance, &QSlider::valueChanged, [this](int value)
                {
                params.WALL_MIN_DISTANCE = value;
                lcdNumber_wall_distance->display(value); });
        ///////////////////////////////
#ifdef HIBERNATION_ENABLED
        hibernationChecker.start(500);
#endif

        this->setPeriod(STATES::Compute, 100);
    }
}

void SpecificWorker::compute()
{
    RoboCompLidar3D::TData ldata;
    try
    {
        ldata = lidar3d_proxy->getLidarData("bpearl", 0, 2 * M_PI, 1);
    }
    catch (const Ice::Exception &e)
    {
        std::cout << e << std::endl;
    }

    RoboCompLidar3D::TPoints p_filter;
    std::ranges::copy_if(ldata.points, std::back_inserter(p_filter),
                         [](auto &a)
                         { return a.z < 500 and a.distance2d > 200; });

    draw_lidar(p_filter, &viewer->scene);

    /// Add State machine with your sweeping logic
    RetVal ret_val;

    switch (state)
    {
    case STATE::AWAY_WALL:
    {
        ret_val = away_wall(p_filter);
        break;
    }
    case STATE::FOLLOW_WALL:
    {
        ret_val = follow_wall(p_filter);
        break;
    }
    case STATE::SPIRAL:
    {
        ret_val = spiral(p_filter);
        break;
    }
    case STATE::FORWARD:
    {
        ret_val = forward(p_filter);
        break;
    }
    case STATE::TURN:
    {
        ret_val = turn(p_filter);
        break;
    }
    }
    /// unpack  the tuple
    auto [st, adv, rot] = ret_val;
    state = st;

    /// Send movements commands to the robot
    try
    {
        omnirobot_proxy->setSpeedBase(0, adv, rot);
    }
    catch (const Ice::Exception &e)
    {
        std::cout << e << std::endl;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Analyzes the filtered points to determine whether to continue moving forward or to stop and turn.
 *
 * This method examines the central part of the `filtered_points` vector to find the minimum distance
 * point within that range. If the minimum distance is less than the width of the robot, it indicates
 * an obstacle is too close, prompting a state change to `TURN` and stopping motion. Otherwise,
 * the robot continues to move forward.
 *
 * @param filtered_points A vector of filtered points representing the robot's perception of obstacles.
 * @return A `RetVal` tuple consisting of the state (`FORWARD` or `TURN`), speed, and rotation.
 */
SpecificWorker::RetVal SpecificWorker::forward(auto &points)
{
    // check if the central part of the filtered_points vector has a minimum lower than the size of the robot
    auto offset_begin = closest_lidar_index_to_given_angle(points, -params.LIDAR_FRONT_SECTION);
    auto offset_end = closest_lidar_index_to_given_angle(points, params.LIDAR_FRONT_SECTION);

    qDebug() << Green << "Forward";

    qDebug() << "Follow Wall Counter: " << params.FOLLOW_WALL_COUNTER;
    qDebug() << "Follow Wall Counter Reset: " << params.FOLLOW_WALL_COUNTER_RESET;
    int minFollowWallCounter = params.MIN_FOLLOW_WALL_COUNTER;
    params.FOLLOW_WALL_COUNTER_RESET--;
    if (params.FOLLOW_WALL_COUNTER_RESET == 0)
    {
        params.FOLLOW_WALL_COUNTER = (std::rand() % minFollowWallCounter) + minFollowWallCounter;
        params.FOLLOW_WALL_COUNTER_RESET = params.MIN_FOLLOW_WALL_COUNTER_RESET;
        qDebug() << "RESET FOLLOW WALL COUNTER to: " << params.FOLLOW_WALL_COUNTER;
    }

    // float umbralEspiral = 1400;
    if (offset_begin and offset_end)
    {
        auto min_point = std::min_element(std::begin(points) + offset_begin.value(), std::begin(points) + offset_end.value(), [](auto &a, auto &b)
                                          { return a.distance2d < b.distance2d; });
        float angulo = min_point->phi;

        if (min_point->distance2d < params.TURN_THRESHOLD && angulo < 0.1 && angulo > -0.1)
        {
            return RetVal(STATE::TURN, 0.f, 0.f); // stop and change state if obstacle detected
        }
        else if (min_point->distance2d < params.START_FOLLOW_WALL_THRESHOLD)
        {
            if (angulo > 0)
            {
                if (params.FOLLOW_WALL_COUNTER > 0)
                {
                    qDebug() << "Follow Wall Derecha";
                    params.FOLLOW_WALL_COUNTER--;
                    return RetVal(STATE::FOLLOW_WALL, 0.f, -0.5f);
                }
                else
                {
                    qDebug() << "Cooldown Follow Wall!!, Turn Izquierda";
                    return RetVal(STATE::AWAY_WALL, 0.f, -0.5f);
                }
            }
            else
            {
                if (params.FOLLOW_WALL_COUNTER > 0)
                {
                    qDebug() << "Follow Wall Izquierda";
                    params.FOLLOW_WALL_COUNTER--;
                    return RetVal(STATE::FOLLOW_WALL, 0.f, 0.5f);
                }
                else
                {
                    qDebug() << "Cooldown Follow Wall!!, Turn Derecha";
                    return RetVal(STATE::AWAY_WALL, 0.f, 0.5f);
                }
            }
        }
        else if (min_point != points.end() and min_point->distance2d < params.SPIRAL_THRESHOLD)
        {
            return RetVal(STATE::FORWARD, params.MAX_ADV_SPEED, 0.f);
        }
        else
        {
            return RetVal(STATE::SPIRAL, params.MAX_ADV_SPEED, 0.3f);
        }
    }
    else // no valid readings
    {
        qWarning() << "No valid readings. Stopping";
        return RetVal(STATE::FORWARD, 0.f, 0.f);
    }
}

/**
 * @brief Checks if the central part of the provided filtered points is free to proceed and determines the next state.
 *
 * This function inspects the central third of the filtered points vector to find the point with the minimum distance.
 * If the minimum distance in this central region is greater than twice the robot's width, the robot will switch to
 * the FORWARD state. Otherwise, it will continue to TURN.
 *
 * @param filtered_points A vector containing points with distance information used for making navigation decisions.
 * @returns A tuple containing the next state (FORWARD or TURN), and speed values.
 */
SpecificWorker::RetVal SpecificWorker::turn(auto &points)
{
    // Instantiate the random number generator and distribution
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<int> dist(0, 1);
    static bool first_time = true;
    static int sign = 1;
    qDebug() << Yellow << "Turn";

    /// check if the narrow central part of the filtered_points vector is free to go. If so stop turning and change state to FORWARD
    auto offset_begin = closest_lidar_index_to_given_angle(points, -params.LIDAR_FRONT_SECTION);
    auto offset_end = closest_lidar_index_to_given_angle(points, params.LIDAR_FRONT_SECTION);

    qDebug() << "Follow Wall Counter: " << params.FOLLOW_WALL_COUNTER;
    qDebug() << "Follow Wall Counter Reset: " << params.FOLLOW_WALL_COUNTER_RESET;
    int minFollowWallCounter = params.MIN_FOLLOW_WALL_COUNTER;
    params.FOLLOW_WALL_COUNTER_RESET--;
    if (params.FOLLOW_WALL_COUNTER_RESET == 0)
    {
        params.FOLLOW_WALL_COUNTER = (std::rand() % minFollowWallCounter) + minFollowWallCounter;
        params.FOLLOW_WALL_COUNTER_RESET = params.MIN_FOLLOW_WALL_COUNTER_RESET;
        qDebug() << "RESET FOLLOW WALL COUNTER to: " << params.FOLLOW_WALL_COUNTER;
    }

    // exit if no valid readings
    if (not offset_begin or not offset_end)
    {
        qWarning() << "No valid readings. Stopping";
        return RetVal(STATE::FORWARD, 0.f, 0.f);
    }

    auto min_point = std::min_element(std::begin(points) + offset_begin.value(), std::begin(points) + offset_end.value(), [](auto &a, auto &b)
                                      { return a.distance2d < b.distance2d; });
    if (min_point != std::end(points) and min_point->distance2d > params.ADVANCE_THRESHOLD)
    {
        first_time = true;
        return RetVal(STATE::FORWARD, 0.f, 0.f);
    }

    /// Keep doing my business
    // get the min_element for all points range anc check if angle is greater, less or closer to zero to choose the direction
    auto min_point_all = std::ranges::min_element(points, [](auto &a, auto &b)
                                                  { return a.distance2d < b.distance2d; });
    // if min_point_all phi is negative, turn right, otherwise turn left. If it is close to zero, turn randomly
    if (first_time)
    {
        if (min_point_all->phi < 0.1 and min_point_all->phi > -0.1)
        {
            sign = dist(gen);
            if (sign == 0)
                sign = -1;
            else
                sign = 1;
        }
        else
            sign = min_point_all->phi > 0 ? -1 : 1;
        first_time = false;
    }
    return RetVal(STATE::TURN, 0.f, sign * params.MAX_ROT_SPEED);
}


/**
 * @brief Checks if the robot is close to an obstacle and decides whether to continue spiraling or to stop and turn.
 *
 * This function examines the filtered points to determine if the robot is close to an obstacle. If the minimum distance
 * point is within the TURN_THRESHOLD, the robot will stop and change state to TURN. If the distance is less than the SPIRAL_THRESHOLD,
 * the robot will continue to spiral. Otherwise, it will adjust its speed and rotation to continue spiraling.
 *
 * @param points A vector of filtered points representing the robot's perception of obstacles.
 * @returns A tuple containing the next state (TURN or SPIRAL), and speed and rotation values.
 */
SpecificWorker::RetVal SpecificWorker::spiral(auto &points)
{
    auto offset_begin = closest_lidar_index_to_given_angle(points, -params.LIDAR_FRONT_SECTION);
    auto offset_end = closest_lidar_index_to_given_angle(points, params.LIDAR_FRONT_SECTION);
    // qDebug () << "Spiral";
    static bool first_time = true; // flag to indicate the first time the robot enters the SPIRAL state
    qDebug() << Blue << "Spiral";

    qDebug() << "Follow Wall Counter: " << params.FOLLOW_WALL_COUNTER;
    qDebug() << "Follow Wall Counter Reset: " << params.FOLLOW_WALL_COUNTER_RESET;
    int minFollowWallCounter = params.MIN_FOLLOW_WALL_COUNTER;
    params.FOLLOW_WALL_COUNTER_RESET--;
    //
    if (params.FOLLOW_WALL_COUNTER_RESET == 0)
    {
        params.FOLLOW_WALL_COUNTER = (std::rand() % minFollowWallCounter) + minFollowWallCounter;
        params.FOLLOW_WALL_COUNTER_RESET = params.MIN_FOLLOW_WALL_COUNTER_RESET;
        qDebug() << "RESET FOLLOW WALL COUNTER to: " << params.FOLLOW_WALL_COUNTER;
    }

    if (offset_begin and offset_end)
    {
        auto min_point = std::min_element(std::begin(points) + offset_begin.value(), std::begin(points) + offset_end.value(), [](auto &a, auto &b)
                                          { return a.distance2d < b.distance2d; });
        if (min_point != points.end() and min_point->distance2d < params.TURN_THRESHOLD) // obstacle detected
        {
            params.ROT_ACTUAL = params.ROT_INICIAL;
            params.VEL_ACTUAL = params.VEL_INICIAL;
            first_time = true;
            return RetVal(STATE::TURN, 0.f, 0.f); // stop and change state if obstacle detected
        }
        else if (min_point != points.end() and min_point->distance2d < params.SPIRAL_THRESHOLD / 3)
        {
            params.ROT_ACTUAL = params.ROT_INICIAL;
            params.VEL_ACTUAL = params.VEL_INICIAL;
            first_time = true;
            return RetVal(STATE::FORWARD, params.MAX_ADV_SPEED, 0.f);
        }
        else
        {
            if (first_time) // first time entering the SPIRAL state
            {
                first_time = false;
                return RetVal(STATE::SPIRAL, 200, 1.0f);
            }
            else
            {
                float alfa = params.ALFA_GIRO;
                float incVel = (params.MAX_ADV_SPEED / 1000.f) * alfa;
                float incRot = (params.MAX_ROT_SPEED / 1000.f) * alfa;
                qDebug() << "rot: " << params.ROT_ACTUAL << " vel: " << params.VEL_ACTUAL;
                if (std::abs(params.ROT_ACTUAL) > 0.45)
                {
                    params.ROT_ACTUAL -= incRot; // decrease rotation
                }
                if (params.VEL_ACTUAL < params.MAX_ADV_SPEED)
                {
                    params.VEL_ACTUAL += incVel;  // increase speed
                }

                return RetVal(STATE::SPIRAL, params.VEL_ACTUAL, params.ROT_ACTUAL);
            }
        }
    }
    else // no valid readings
    {
        qWarning() << "No valid readings. Stopping";
        return RetVal(STATE::FORWARD, 0.f, 0.f);
    }
}

/**
 * @brief Compares two floating-point numbers for equality within a specified percentage error.
 *
 * This function checks if the absolute difference between two floating-point numbers is within
 * a given percentage of the larger of the two numbers.
 *
 * @param a The first floating-point number.
 * @param b The second floating-point number.
 * @param percentage_error The allowable percentage error for the comparison.
 * @return true if the numbers are equal within the specified percentage error, false otherwise.
 */
bool are_floats_equal(float a, float b, float percentage_error)
{
    int valA = a;
    int valB = b;
    bool iguales = false;
    if (valA == valB)
    {
        iguales = true;
    }
    else
    {
        float diff = std::abs(valA - valB);
        float maxVal = std::max(valA, valB);
        float error = (diff / maxVal) * 100;
        qDebug() << "Error A y B: " << error;

        if (error < percentage_error)
        {
            iguales = true;
        }
    }
    return iguales;
}

/**
 * @brief Checks if the robot is close to a wall and decides whether to move away or continue following it.
 *
 * This function examines the filtered points to determine if the robot is close to a wall. If the minimum distance
 * point is within the AWAY_WALL_THRESHOLD, the robot will move away from the wall. If the distance is greater than
 * the AWAY_WALL_THRESHOLD, the robot will continue to follow the wall. If the distance to the wall is less than the
 * AWAY_WALL_SECURITY_BACK, the robot will stop and move away from the wall.
 *
 * @param points A vector of filtered points representing the robot's perception of obstacles.
 * @returns A tuple containing the next state (AWAY_WALL or FORWARD), and speed and rotation values.
 */
SpecificWorker::RetVal SpecificWorker::away_wall(auto &points)
{

    auto offset_begin = closest_lidar_index_to_given_angle(points, -params.LIDAR_FRONT_SECTION);
    auto offset_end = closest_lidar_index_to_given_angle(points, params.LIDAR_FRONT_SECTION);

    auto offset_begin_back = closest_lidar_index_to_given_angle(points, -(M_PI - params.LIDAR_FRONT_SECTION));
    auto offset_end_back = closest_lidar_index_to_given_angle(points, M_PI - params.LIDAR_FRONT_SECTION);

    auto min_point_back_left = std::min_element(std::begin(points), std::begin(points) + offset_begin_back.value(), [](auto &a, auto &b)
                                                { return a.distance2d < b.distance2d; });
    auto min_point_back_right = std::min_element(std::begin(points) + offset_end_back.value(), std::end(points), [](auto &a, auto &b)
                                                 { return a.distance2d < b.distance2d; });
    
    auto min_point_back = min_point_back_right;
    if (min_point_back_left < min_point_back_right)
    {
        min_point_back = min_point_back_left;
    }



    qDebug() << Cyan << "Away Wall";
    qDebug() << "Follow Wall Counter: " << params.FOLLOW_WALL_COUNTER;
    qDebug() << "Follow Wall Counter Reset: " << params.FOLLOW_WALL_COUNTER_RESET;
    int minFollowWallCounter = params.MIN_FOLLOW_WALL_COUNTER;
    params.FOLLOW_WALL_COUNTER_RESET--;
    if (params.FOLLOW_WALL_COUNTER_RESET == 0) // reset the follow wall counter
    {
        params.FOLLOW_WALL_COUNTER = (std::rand() % minFollowWallCounter) + minFollowWallCounter;
        params.FOLLOW_WALL_COUNTER_RESET = params.MIN_FOLLOW_WALL_COUNTER_RESET;
        qDebug() << "RESET FOLLOW WALL COUNTER to: " << params.FOLLOW_WALL_COUNTER;
    }

    if (offset_begin and offset_end)
    {
        auto min_point = std::min_element(std::begin(points) + offset_begin.value(), std::begin(points) + offset_end.value(), [](auto &a, auto &b)
                                          { return a.distance2d < b.distance2d; });

        float angulo = min_point->phi;

        auto primerPunto = std::begin(points) + offset_begin.value();
        auto ultimoPunto = std::begin(points) + offset_end.value();
        float distPrimerPunto = primerPunto->distance2d;
        float distUltimoPunto = ultimoPunto->distance2d;
        qDebug() << "Distancia Primer Punto: " << distPrimerPunto;
        qDebug() << "Distancia Ultimo Punto: " << distUltimoPunto;
        if (are_floats_equal(distPrimerPunto, distUltimoPunto, 20)) // recto con pared
        {
            qDebug() << "Recto con pared";
            qDebug() << "Distancia a la pared (frontal): " << min_point->distance2d;
            qDebug() << "Distancia a la pared (trasera): " << min_point_back->distance2d;
            if (min_point->distance2d > params.AWAY_WALL_THRESHOLD) //start to move away from wall
            {
                return RetVal(STATE::FORWARD, 0.f, 0.f);
            }
            else
            {
                if(min_point_back->distance2d > params.AWAY_WALL_SECURITY_BACK) //check if there is obstacles behind
                {
                    qDebug() << "Distancia a la pared trasera:" << min_point_back->distance2d << "Distancia de seguridad: " << params.AWAY_WALL_SECURITY_BACK;
                    qDebug() << "Alejandose de la pared";
                    
                    return RetVal(STATE::AWAY_WALL, -params.MAX_ADV_SPEED, 0);
                }
                else
                {
                    qDebug() << Red;
                    qDebug() << "Acerquandose a la pared trasera-> STOP";
                    return RetVal(STATE::FORWARD, 0.f, 0.f);
                }
                
            }
        }
        else // perpendicular to the wall
        {
            qDebug() << "Poniendose perpendicular a la pared";
            if (angulo > 0)
            {
                return RetVal(STATE::AWAY_WALL, 15, 1.f);
            }
            else
            {
                return RetVal(STATE::AWAY_WALL, 15, -1.f);
            }
        }
    }
    else // no valid readings
    {
        qWarning() << "No valid readings. Stopping";
        return RetVal(STATE::FORWARD, 0.f, 0.f);
    }
}

/**
 * @brief Checks if the robot is close to a wall and decides whether to follow it or to spiral.
 *
 * This function examines the filtered points to determine if the robot is close to a wall. If the minimum distance
 * point is within the STOP_FOLLOW_WALL_THRESHOLD, the robot will stop following the wall and change state to TURN.
 * If the distance is less than the SPIRAL_THRESHOLD, the robot will continue to spiral. Otherwise, it will adjust
 * its speed and rotation to continue following the wall.
 *
 * @param points A vector of filtered points representing the robot's perception of obstacles.
 * @returns A tuple containing the next state (TURN or SPIRAL), and speed and rotation values.
 */
SpecificWorker::RetVal SpecificWorker::follow_wall(auto &points)
{
    auto offset_begin = closest_lidar_index_to_given_angle(points, -params.LIDAR_FRONT_SECTION);
    auto offset_end = closest_lidar_index_to_given_angle(points, params.LIDAR_FRONT_SECTION);

    auto offset_begin_right = closest_lidar_index_to_given_angle(points, params.LIDAR_RIGHT_SIDE_SECTION);
    auto offset_end_right = closest_lidar_index_to_given_angle(points, params.LIDAR_RIGHT_SIDE_SECTION + 0.1f);


    static bool first_time = true;
    qDebug() << Magenta << "Follow Wall";
    bool derecha = false;
    qDebug() << "Follow Wall Counter: " << params.FOLLOW_WALL_COUNTER;
    qDebug() << "Follow Wall Counter Reset: " << params.FOLLOW_WALL_COUNTER_RESET;
    int minFollowWallCounter = params.MIN_FOLLOW_WALL_COUNTER;
    if (params.FOLLOW_WALL_COUNTER_RESET == 0)
    {
        params.FOLLOW_WALL_COUNTER = (std::rand() % minFollowWallCounter) + minFollowWallCounter;
        params.FOLLOW_WALL_COUNTER_RESET = params.MIN_FOLLOW_WALL_COUNTER_RESET;
        qDebug() << "RESET FOLLOW WALL COUNTER to: " << params.FOLLOW_WALL_COUNTER;
    }

    if (offset_begin and offset_end)
    {
        auto min_point = std::min_element(std::begin(points) + offset_begin.value(), std::begin(points) + offset_end.value(), [](auto &a, auto &b)
                                          { return a.distance2d < b.distance2d; });

        if (min_point->distance2d > params.SPIRAL_THRESHOLD) // continue spiraling
        {
            return RetVal(STATE::SPIRAL, 0.f, 0.f);
        }

        // CONDICION FINAL FOLLOW WALL
        if (min_point->distance2d < params.STOP_FOLLOW_WALL_THRESHOLD)
        {
            qDebug() << "STOP FOLLOW WALL";
            return RetVal(STATE::TURN, 0.f, 0.f);
        }

        // ZONA DE CORRECCION
        float distAct = min_point->distance2d;
        float distMin = params.MIN_FOLLOW_WALL_DISTANCE;
        float distMax = params.MAX_FOLLOW_WALL_DISTANCE;
        float distIdeal = (distMax + distMin) / 2;
        float anchoIdeal = (distMax - distMin) / 2;
        float error = 1.f;
        float angulo = min_point->phi;
        float speed = params.MAX_ADV_SPEED;
        float correccion = 0.3f;
        qDebug() << "Distancia Actual: " << distAct << "distancia Ideal: " << distIdeal << "Ancho Ideal: " << anchoIdeal;

        if (angulo > 0)    // derecha
        {
            derecha = true;
            if (params.FOLLOW_WALL_COUNTER > 0)
            {
                params.FOLLOW_WALL_COUNTER--;
                if (distAct > distMin && distAct < distMax)
                {
                    return RetVal(STATE::FOLLOW_WALL, speed, 0);
                }
                else if (distAct <= distMin)
                {
                    return RetVal(STATE::FOLLOW_WALL, speed, -correccion);
                }
                else if (distAct >= distMax)
                {
                    return RetVal(STATE::FOLLOW_WALL, speed, correccion);
                }
            }
            else
            {
                qDebug() << "Cooldown Follow Wall!!, alejarse de la pared";
                return RetVal(STATE::AWAY_WALL, 0.f, 0.f);
            }
        }
        else
        {
            derecha = false;
            if (params.FOLLOW_WALL_COUNTER > 0)
            {
                params.FOLLOW_WALL_COUNTER--;

                if (distAct > distMin && distAct < distMax)
                {
                    return RetVal(STATE::FOLLOW_WALL, speed, 0);
                }
                else if (distAct <= distMin)
                {
                    return RetVal(STATE::FOLLOW_WALL, speed, correccion);
                }
                else if (distAct >= distMax)
                {
                    return RetVal(STATE::FOLLOW_WALL, speed, -correccion);
                }
            }
            else
            {
                qDebug() << "Cooldown Follow Wall!!, alejarse de la pared";
                return RetVal(STATE::AWAY_WALL, 0.f, 0.0f);
            }
        }
    }
    else // no valid readings
    {
        qWarning() << "No valid readings. Stopping";
        return RetVal(STATE::FORWARD, 0.f, 0.f);
    }
}

/**
 * Draws LIDAR points onto a QGraphicsScene.
 *
 * This method clears any existing graphical items from the scene, then iterates over the filtered
 * LIDAR points to add new items. Each LIDAR point is represented as a colored rectangle. The point
 * with the minimum distance is highlighted in red, while the other points are drawn in green.
 *
 * @param filtered_points A collection of filtered points to be drawn, each containing the coordinates
 *                        and distance.
 * @param scene A pointer to the QGraphicsScene where the points will be drawn.
 */
void SpecificWorker::draw_lidar(auto &filtered_points, QGraphicsScene *scene)
{
    static std::vector<QGraphicsItem *> items; // store items so they can be shown between iterations

    // remove all items drawn in the previous iteration
    for (auto i : items)
    {
        scene->removeItem(i);
        delete i;
    }
    items.clear();

    auto color = QColor(Qt::green);
    auto brush = QBrush(QColor(Qt::green));
    for (const auto &p : filtered_points)
    {
        auto item = scene->addRect(-50, -50, 100, 100, color, brush);
        item->setPos(p.x, p.y);
        items.push_back(item);
    }

    // compute and draw minimum distance point in frontal range
    auto offset_begin = closest_lidar_index_to_given_angle(filtered_points, -params.LIDAR_FRONT_SECTION);
    auto offset_end = closest_lidar_index_to_given_angle(filtered_points, params.LIDAR_FRONT_SECTION);
    if (not offset_begin or not offset_end)
    {
        std::cout << offset_begin.error() << " " << offset_end.error() << std::endl;
        return;
    } // abandon the ship
    auto min_point = std::min_element(std::begin(filtered_points) + offset_begin.value(), std::begin(filtered_points) + offset_end.value(), [](auto &a, auto &b)
                                      { return a.distance2d < b.distance2d; });
    QColor dcolor;
    if (min_point->distance2d < params.TURN_THRESHOLD)
        dcolor = QColor(Qt::red);
    else
        dcolor = QColor(Qt::magenta);
    auto ditem = scene->addRect(-100, -100, 200, 200, dcolor, QBrush(dcolor));
    ditem->setPos(min_point->x, min_point->y);
    items.push_back(ditem);

    // compute and draw minimum distance point to wall
    auto wall_res_right = closest_lidar_index_to_given_angle(filtered_points, params.LIDAR_RIGHT_SIDE_SECTION);
    auto wall_res_left = closest_lidar_index_to_given_angle(filtered_points, params.LIDAR_LEFT_SIDE_SECTION);
    if (not wall_res_right or not wall_res_left) // abandon the ship
    {
        qWarning() << "No valid lateral readings" << QString::fromStdString(wall_res_right.error()) << QString::fromStdString(wall_res_left.error());
        return;
    }
    auto right_point = filtered_points[wall_res_right.value()];
    auto left_point = filtered_points[wall_res_left.value()];
    // compare both to get the one with minimum distance
    auto min_obj = (right_point.distance2d < left_point.distance2d) ? right_point : left_point;
    auto item = scene->addRect(-100, -100, 200, 200, QColor(QColorConstants::Svg::orange), QBrush(QColor(QColorConstants::Svg::orange)));
    item->setPos(min_obj.x, min_obj.y);
    items.push_back(item);

    // draw a line from the robot to the minimum distance point
    auto item_line = scene->addLine(QLineF(QPointF(0.f, 0.f), QPointF(min_obj.x, min_obj.y)), QPen(QColorConstants::Svg::orange, 10));
    items.push_back(item_line);

    // update UI
    lcdNumber_minangle->display(atan2(min_obj.x, min_obj.y));
    lcdNumber_mindist->display(min_obj.distance2d);

    // Draw two lines coming out from the robot at angles given by params.LIDAR_OFFSET
    // Calculate the end points of the lines
    // float angle1 = params.LIDAR_FRONT_SECTION / 2.f;
    auto res_right = closest_lidar_index_to_given_angle(filtered_points, params.LIDAR_FRONT_SECTION);
    auto res_left = closest_lidar_index_to_given_angle(filtered_points, -params.LIDAR_FRONT_SECTION);
    if (not res_right or not res_left)
    {
        std::cout << res_right.error() << " " << res_left.error() << std::endl;
        return;
    }

    float right_line_length = filtered_points[res_right.value()].distance2d;
    float left_line_length = filtered_points[res_left.value()].distance2d;
    float angle1 = filtered_points[res_left.value()].phi;
    float angle2 = filtered_points[res_right.value()].phi;
    QLineF line_left{QPointF(0.f, 0.f),
                     robot_draw->mapToScene(left_line_length * sin(angle1), left_line_length * cos(angle1))};
    QLineF line_right{QPointF(0.f, 0.f),
                      robot_draw->mapToScene(right_line_length * sin(angle2), right_line_length * cos(angle2))};
    QPen left_pen(Qt::blue, 10); // Blue color pen with thickness 3
    QPen right_pen(Qt::red, 10); // Blue color pen with thickness 3
    auto line1 = scene->addLine(line_left, left_pen);
    auto line2 = scene->addLine(line_right, right_pen);
    items.push_back(line1);
    items.push_back(line2);

   
}

/**
 * @brief Calculates the index of the closest lidar point to the given angle.
 *
 * This method searches through the provided list of lidar points and finds the point
 * whose angle (phi value) is closest to the specified angle. If a matching point is found,
 * the index of the point in the list is returned. If no point is found that matches the condition,
 * an error message is returned.
 *
 * @param points The collection of lidar points to search through.
 * @param angle The target angle to find the closest matching point.
 * @return std::expected<int, string> containing the index of the closest lidar point if found,
 * or an error message if no such point exists.
 */
std::expected<int, string> SpecificWorker::closest_lidar_index_to_given_angle(const auto &points, float angle)
{
    // search for the point in points whose phi value is closest to angle
    auto res = std::ranges::find_if(points, [angle](auto &a)
                                    { return a.phi > angle; });
    if (res != std::end(points))
        return std::distance(std::begin(points), res);
    else
        return std::unexpected("No closest value found in method <closest_lidar_index_to_given_angle>");
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void SpecificWorker::emergency()
{
    std::cout << "Emergency worker" << std::endl;
    // computeCODE
    //
    // if (SUCCESSFUL)
    //   emmit goToRestore()
}
// Execute one when exiting to emergencyState
void SpecificWorker::restore()
{
    std::cout << "Restore worker" << std::endl;
    // computeCODE
    // Restore emergency component
}
int SpecificWorker::startup_check()
{
    std::cout << "Startup check" << std::endl;
    QTimer::singleShot(200, qApp, SLOT(quit()));
    return 0;
}

/**************************************/
// From the RoboCompLidar3D you can call this methods:
// this->lidar3d_proxy->getLidarData(...)
// this->lidar3d_proxy->getLidarDataArrayProyectedInImage(...)
// this->lidar3d_proxy->getLidarDataProyectedInImage(...)
// this->lidar3d_proxy->getLidarDataWithThreshold2d(...)

/**************************************/
// From the RoboCompLidar3D you can use this types:
// RoboCompLidar3D::TPoint
// RoboCompLidar3D::TDataImage
// RoboCompLidar3D::TData

/**************************************/
// From the RoboCompOmniRobot you can call this methods:
// this->omnirobot_proxy->correctOdometer(...)
// this->omnirobot_proxy->getBasePose(...)
// this->omnirobot_proxy->getBaseState(...)
// this->omnirobot_proxy->resetOdometer(...)
// this->omnirobot_proxy->setOdometer(...)
// this->omnirobot_proxy->setOdometerPose(...)
// this->omnirobot_proxy->setSpeedBase(...)
// this->omnirobot_proxy->stopBase(...)

/**************************************/
// From the RoboCompOmniRobot you can use this types:
// RoboCompOmniRobot::TMechParams
