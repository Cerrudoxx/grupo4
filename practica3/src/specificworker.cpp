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
#include "specificworker.h"
#include <cppitertools/enumerate.hpp>
#include <cppitertools/range.hpp>
#include <cppitertools/sliding_window.hpp>

#define RESET "\033[0m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define RED "\033[31m"
#define PURPLE "\033[35m"

/**
 * \brief Default constructor
 */
SpecificWorker::SpecificWorker(TuplePrx tprx, bool startup_check) : GenericWorker(tprx)
{
    std::locale::global(std::locale("C"));
    this->startup_check_flag = startup_check;
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
        // Viewer
        viewer = new AbstractGraphicViewer(this->frame, params.GRID_MAX_DIM);
        auto [r, e] = viewer->add_robot(params.ROBOT_WIDTH, params.ROBOT_LENGTH, 0, 100, QColor("Blue"));
        robot_draw = r;
        viewer->setStyleSheet("background-color: lightGray;");
        this->resize(800, 700);

        // connect stop button un UI with a lambda function
        connect(pushButton_stop, &QPushButton::clicked, [this]()
                {
            try
            { omnirobot_proxy->setSpeedBase(0, 0, 0); }
            catch (const Ice::Exception &e)
            { std::cout << e << std::endl; }
            pushButton_stop->setText(pushButton_stop->isChecked() ? "Track" : "Stop"); });
        viewer->show();

        this->setPeriod(STATES::Compute, 100);
        // this->setPeriod(STATES::Emergency, 500);
    }
}
void SpecificWorker::compute()
{
    /// read bpearl (lower) lidar and draw
    auto bpearl_points = read_lidar_bpearl();
    if (bpearl_points.empty())
    {
        qWarning() << __FUNCTION__ << "Empty bpearl lidar data";
        return;
    };
    // draw_lidar(ldata.points, &viewer->scene);

    auto helios_points = read_lidar_helios();
    if (helios_points.empty())
    {
        qWarning() << __FUNCTION__ << "Empty helios lidar data";
        return;
    };
    // draw_lidar(helios_points, &viewer->scene);

    /// detect walls
    auto lines = detect_wall_lines(helios_points, &viewer->scene);

    /// remove wall lines
    auto new_data = remove_wall_points(lines, bpearl_points);
    auto filtered_points = new_data;
    //  draw_lidar(filtered_points, &viewer->scene);

    // /// get walls as polygons
    std::vector<QPolygonF> obstacles = get_walls_as_polygons(lines, params.ROBOT_WIDTH / 2);
    // draw_obstacles(obstacles, &viewer->scene, Qt::darkRed);

    // /// get obstacles as polygons using DBSCAN
    auto obs = rc::dbscan(filtered_points, params.ROBOT_WIDTH, 2);
    obstacles.insert(obstacles.end(), obs.begin(), obs.end());
    draw_lidar(filtered_points, &viewer->scene);
    draw_obstacles(obstacles, &viewer->scene, Qt::darkRed);

    // /// check if there is new YOLO data in buffer
    std::expected<RoboCompVisualElementsPub::TObject, std::string> tp_person = std::unexpected("No person found");
    auto [data_] = buffer.read_first();
    if (data_.has_value())
        tp_person = find_person_in_data(data_.value().objects);

    // if no person, stop the robot and return
    if (not tp_person)
    {
        qWarning() << __FUNCTION__ << "No person found, searching";
        state = STATE::SEARCH;
    }

    if (tp_person)
    {
        // /// find the polygon that contains the person and remove it
        obstacles = find_person_polygon_and_remove(tp_person.value(), obstacles);
        auto enlargedObstacles = enlarge_polygons(obstacles, params.ROBOT_WIDTH / 2);  
        draw_obstacles(enlargedObstacles, &viewer->scene, Qt::darkYellow);

        /// compute an obstacle free path

        // If close to target, stop
        float dist = std::hypot(std::stof(tp_person.value().attributes.at("x_pos")), std::stof(tp_person.value().attributes.at("y_pos")));
        if (dist < params.PERSON_MIN_DIST)
        {
            qWarning() << __FUNCTION__ << "Path length: " << " Close to person. Stopping";
            stop_robot();
            return;
        }

         Eigen::Vector2f goal{std::stof(tp_person.value().attributes.at("x_pos")), std::stof(tp_person.value().attributes.at("y_pos"))};
        std::vector<Eigen::Vector2f> path = rc::VisibilityGraph().generate_path(Eigen::Vector2f::Zero(),
                                                                                goal,
                                                                                obstacles,
                                                                                params.ROBOT_WIDTH / 2,
                                                                                nullptr);

        draw_path_to_person(path, &viewer->scene);
    }

    std::vector<RoboCompLidar3D::TPoint> ldata;
    std::transform(filtered_points.begin(), filtered_points.end(), std::back_inserter(ldata), [](auto &a)
                   { return RoboCompLidar3D::TPoint{a.x(), a.y(), 0, 0}; });
    // call state machine to track the first point of the path
    const auto &[adv, rot] = state_machine(tp_person, ldata, room_model, obstacles);

      

    // plot on UI
    if(tp_person)
    {
        float d = std::hypot(std::stof(tp_person.value().attributes.at("x_pos")),
                                 std::stof(tp_person.value().attributes.at("y_pos")));
        plot_distance(running_average(d) - params.PERSON_MIN_DIST);
        lcdNumber_dist_to_person->display(d);
        lcdNumber_angle_to_person->display(atan2(std::stof(tp_person.value().attributes.at("x_pos")),
                                                 std::stof(tp_person.value().attributes.at("y_pos"))));
    }


    // move the robot
    try
    {
        omnirobot_proxy->setSpeedBase(0.f, adv, rot);
    }
    catch (const Ice::Exception &e)
    {
        std::cout << e << std::endl;
    }

    lcdNumber_adv->display(adv);
    lcdNumber_rot->display(rot);
}

//////////////////////////////////////////////////////////////////
/// YOUR CODE HERE
//////////////////////////////////////////////////////////////////
std::vector<Eigen::Vector2f> SpecificWorker::read_lidar_bpearl()
{
    try
    {
        auto ldata = lidar3d1_proxy->getLidarData("bpearl", 0, 2 * M_PI, 1);
        // filter points according to height and distance
        std::vector<Eigen::Vector2f> p_filter;
        for (const auto &a : ldata.points)
        {
            if (a.z < 500 and a.distance2d > 200)
                p_filter.emplace_back(a.x, a.y);
        }
        return p_filter;
    }
    catch (const Ice::Exception &e)
    {
        std::cout << e << std::endl;
    }
    return {};
}
std::vector<Eigen::Vector2f> SpecificWorker::read_lidar_helios()
{
    try
    {

        auto ldata = lidar3d_proxy->getLidarData("helios", 0, 2 * M_PI, 2);
        // filter points according to height and distance
        std::vector<Eigen::Vector2f> p_filter;
        for (const auto &a : ldata.points)
        {
            if (a.z > 1300 and a.distance2d > 200)
                p_filter.emplace_back(a.x, a.y);
        }

        return p_filter;
    }
    catch (const Ice::Exception &e)
    {
        std::cout << e << std::endl;
    }
    return {};
}
void SpecificWorker::update_room_model(const auto &points, QGraphicsScene *scene)
{
    // transform points to a std::vector<Eigen::Vector2f>
    std::vector<Eigen::Vector2f> points_eigen;
    std::ranges::transform(points, std::back_inserter(points_eigen),
                           [](auto &a)
                           { return Eigen::Vector2f(a.x, a.y); });

    // compute room features
    const auto &[_, __, corners, triple_corners] = room_detector.compute_features(points_eigen, &viewer->scene);

    // if triple_corners is empty, stick with the previous room model
    if (triple_corners.empty())
    {
        qWarning() << __FUNCTION__ << "Empty triple corners";
        return;
    }

    // update current room model with new measures
    const auto &[c1, c2, c3, c4] = triple_corners[0];

    room_model.update(c1, c2, c3, c4);
    room_model.set_valid(true);
}
/* Removes points that are on the walls from the given set of points.
 *
 * This function filters out points that are on the walls based on the room model.
 * It transforms the points to Eigen vectors, computes room lines, and filters out
 * points that are on these lines. The remaining points are returned as a list of polygons.
 *
 * param points The set of points to be filtered.
 * return A vector of polygons representing the filtered points.
 */
std::vector<Eigen::Vector2f> SpecificWorker::remove_wall_points(const std::vector<QLineF> &Lines, const auto &bpearl)
{
    std::vector<Eigen::Vector2f> points_inside;

    for (const auto &p : bpearl)
    {
        bool dentro = true;
        for (const auto &line : Lines)
        {
            auto param_line = Eigen::ParametrizedLine<float, 2>::Through(Eigen::Vector2f(line.p1().x(), line.p1().y()),
                                                                         Eigen::Vector2f(line.p2().x(), line.p2().y()));
            if (param_line.distance(p) < params.POINT_LINE_MEMBERSHIP_THRESHOLD)
            {
                dentro = false;
                break;
            }
        }
        if (dentro)
            points_inside.emplace_back(p);
    }

    return points_inside;
}

std::vector<QLineF> SpecificWorker::detect_wall_lines(const auto &helios, QGraphicsScene *escena)
{
    std::vector<QLineF> lines;
    const auto &[Lineas, _, __, ___] = room_detector.compute_features(helios, escena);
    for (const auto &l : Lineas)
        lines.emplace_back(l.second);
    return lines;
}

std::expected<RoboCompVisualElementsPub::TObject, std::string>
SpecificWorker::find_person_in_data(const std::vector<RoboCompVisualElementsPub::TObject> &objects)
{
    if (objects.empty())
        return std::unexpected("Empty objects in method <find_person_in_data>");
    if (auto p_ = std::ranges::find_if(objects, [](auto &a)
                                       { return a.id == 0 and std::stof(a.attributes.at("score")) > 0.6; });
        p_ == std::end(objects))
        return std::unexpected("No person found in method <find_person_in_data>");
    else
    {
        draw_person(const_cast<RoboCompVisualElementsPub::TObject &>(*p_), &viewer->scene);
        return *p_;
    }
}
std::vector<QPolygonF> SpecificWorker::find_person_polygon_and_remove(const RoboCompVisualElementsPub::TObject &person, std::vector<QPolygonF> &obstacles)
{
    QPointF pp = QPointF(std::stof(person.attributes.at("x_pos")), std::stof(person.attributes.at("y_pos")));
    // compute 8 point around pp in circular configuration
    std::vector<QPointF> ppoly;
    for (auto i : iter::range(0.0, 2 * M_PI, M_PI / 2))
        ppoly.push_back(pp + QPointF(200 * cos(i), 200 * sin(i)));
    // check if any polygon contains the person and remove it
    std::vector<QPolygonF> new_obs(obstacles);
    if (auto res = std::ranges::find_if(obstacles, [ppoly](auto &p)
                                        {
                    // check if any point of ppoly is inside the polygon p
                    for (const auto &pp: ppoly)
                        if (p.containsPoint(pp, Qt::OddEvenFill))
                            return true;
                    return false; });
        res != std::end(obstacles))
        obstacles.erase(res);
    return obstacles;
}

std::vector<QPolygonF> SpecificWorker::enlarge_polygons(const std::vector<QPolygonF> &polygons, float amount)
{
    std::vector<QPolygonF> enlargedPolygons;
    for (const auto &poly : polygons)
    {
        QPolygonF exp_poly; // expanded polygon
        if (poly.size() < 3)
            continue;              // skip polygons with less than 3 points
        QPolygonF copy_poly(poly); // copy of the polygon to insert the first point at the end
        copy_poly << poly[0] << poly[1];
        for (const auto &p : iter::sliding_window(copy_poly, 3))
        {
            const auto p1 = Eigen::Vector2f{p[0].x(), p[0].y()};
            const auto p2 = Eigen::Vector2f{p[1].x(), p[1].y()};
            const auto p3 = Eigen::Vector2f{p[2].x(), p[2].y()};
            const auto bisectrix = ((p1 - p2).normalized() + (p3 - p2).normalized()).normalized();
            const Eigen::Vector2f np2 = p2 - amount * bisectrix;
            exp_poly << QPointF{np2.x(), np2.y()};
        }
        enlargedPolygons.emplace_back(exp_poly);
    }
    return enlargedPolygons;
}

void SpecificWorker::stop_robot()
{
    try
    {
        omnirobot_proxy->setSpeedBase(0.f, 0.f, 0.f);
    }
    catch (const Ice::Exception &e)
    {
        std::cout << e << std::endl;
    }
}
std::vector<QPolygonF> SpecificWorker::get_walls_as_polygons(const vector<QLineF> &lines, float robot_width)
{
    std::vector<QPolygonF> obstacles;
    for (const auto &l : lines)
    {
        // create line
        QLineF line = l;
        // Calculate the direction vector of the line
        QPointF direction = line.p2() - line.p1();
        // Calculate the normal vector of the line
        QPointF normal = QPointF(-direction.y(), direction.x());
        // Normalize the normal vector
        normal /= sqrt(normal.x() * normal.x() + normal.y() * normal.y());
        // Create the polygon
        QPolygonF poly;
        poly << line.p1() + normal * robot_width / 2 << line.p2() + normal * robot_width / 2
             << line.p2() - normal * robot_width / 2 << line.p1() - normal * robot_width / 2;
        obstacles.push_back(poly);
    }
    return obstacles;
}

//////////////////////////////////////////////////////////////////
/// STATE  MACHINE
//////////////////////////////////////////////////////////////////
SpecificWorker::RobotSpeed SpecificWorker::state_machine(const TPerson &tp_person,
                                                         const RoboCompLidar3D::TPoints &points,
                                                         const rc::Room &room_model,
                                                         const std::vector<QPolygonF> &obstacles)
{
    // call the appropriate state function
    RetVal res;
    if (pushButton_stop->isChecked()) // stop if buttom is pressed
        state = STATE::WAIT;

    switch (state)
    {
    case STATE::SEARCH:
        res = search(tp_person);
        label_state->setText("SEARCH");
        break;
    case STATE::TRACK:
        res = track(tp_person, points, room_model, obstacles);
        label_state->setText("TRACK");
        break;
    case STATE::WAIT:
        res = wait(tp_person);
        label_state->setText("WAIT");
        break;
    case STATE::STOP:
        res = stop();
        label_state->setText("STOP");
        break;
    }
    auto &[st, speed, rot] = res;
    state = st;
    return {speed, rot};
}

bool compare_floats(float a, float b, float tolerance)
{
    return std::fabs(a - b) <= tolerance;
}

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
SpecificWorker::RetVal SpecificWorker::track(const TPerson &tp_person,
                                             auto &points,
                                             const rc::Room &room_model,
                                             const std::vector<QPolygonF> &obstacles)
{
    qDebug() << BLUE;
    qDebug() << "TRACK";
    qDebug() << RESET;
    if (not tp_person)
    {
        qDebug() << "Persona no detectada en TRACK";
        return RetVal(STATE::SEARCH, 0.f, 0.f);
    }

    auto distance = std::hypot(std::stof(tp_person.value().attributes.at("x_pos")), std::stof(tp_person.value().attributes.at("y_pos")));
    lcdNumber_dist_to_person->display(distance);

    float x = std::stof(tp_person.value().attributes.at("x_pos"));
    float y = std::stof(tp_person.value().attributes.at("y_pos"));
    float angle_to_person = std::atan2(y, x);

    qDebug() << "Angle to person: " << angle_to_person;
    float corrector = 0.9;
    float maxAngle = M_PI;
    float minAngle = 0;
    float center = 1.6;

    auto calculate_vel_giro = [angle_to_person, center, minAngle, maxAngle, corrector]() -> float
    {
        return compare_floats(angle_to_person, center, 0.1) ? 0 : (angle_to_person < center && angle_to_person > minAngle) ? ((1.6 - angle_to_person) / (1.6 - 0.5)) * corrector
                                                              : (angle_to_person > center && angle_to_person < maxAngle)   ? -((angle_to_person - 1.7) / (2.5 - 1.7)) * corrector
                                                                                                                           : 0;
    };

    float vel_giro = std::clamp(calculate_vel_giro(), -1.5f, 1.5f);
    qDebug() << "Vel giro: " << vel_giro;

    auto calculate_vel_forward = [distance, corrector]() -> float
    {
        return distance > 700 ? std::min(1000.f, ((distance - 600) * corrector)) : 0;
    };

    float vel_forward = calculate_vel_forward();
    qDebug() << "Vel forward: " << vel_forward;

    return distance > 700 ? RetVal(STATE::TRACK, vel_forward, vel_giro) : RetVal(STATE::TRACK, 0.f, vel_giro);
}

SpecificWorker::RetVal SpecificWorker::search(const TPerson &person)
{
    // qDebug() << __FUNCTION__ ;
    //  check if the person is further than a threshold

    qDebug() << YELLOW << "SEARCH" << RESET;
    if (not person)
    {
        return RetVal(STATE::SEARCH, 0.f, 1.5f);
    }
    else
    {
        return RetVal(STATE::TRACK, 0.f, 0.0f);
    }
}

SpecificWorker::RetVal SpecificWorker::wait(const TPerson &tp_person)
{
    // qDebug() << __FUNCTION__ ;
    //  check if the person is further than a threshold
    if (not tp_person)
    {
        qWarning() << __FUNCTION__ << "No person found";
        return RetVal(STATE::WAIT, 0.f, 0.f);
    }

    if (std::hypot(std::stof(tp_person.value().attributes.at("x_pos")), std::stof(tp_person.value().attributes.at("y_pos"))) > params.PERSON_MIN_DIST + 100)
        return RetVal(STATE::TRACK, 0.f, 0.f);

    return RetVal(STATE::WAIT, 0.f, 0.f);
}
/**
 * @brief Determines the robot's behavior when following a wall.
 *
 * This method analyzes the filtered points to determine the robot's behavior when following a wall.
 * It first checks if the robot is about to crash into an obstacle, in which case it stops and changes
 * the state to TURN. If no obstacle is detected, it then calculates the distance to the wall on the
 * robot's side and computes the necessary speed and rotation to maintain a safe distance from the wall.
 *
 * @param filtered_points A vector containing points with distance information used for making navigation decisions.
 * @returns A tuple containing the next state (WALL), and speed values.
 */
SpecificWorker::RetVal SpecificWorker::stop() // TODO: release to let the joysitck control the robot
{
    // qDebug() << __FUNCTION__ ;
    //  Check the status of the pushButton_stop
    if (not pushButton_stop->isChecked())
        return RetVal(STATE::TRACK, 0.f, 0.f);

    return RetVal(STATE::STOP, 0.f, 0.f);
}
float SpecificWorker::running_average(float dist)
{
    static float avg = 0;
    static int count = 0;
    avg = (avg * count + dist) / (count + 1);
    count++;
    return avg;
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

    auto color = QColor(Qt::darkGreen);
    auto brush = QBrush(QColor(Qt::darkGreen));
    for (const auto &p : filtered_points)
    {
        auto item = scene->addRect(-25, -25, 50, 50, color, brush);
        item->setPos(p.x(), p.y());
        items.push_back(item);
    }
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
void SpecificWorker::draw_person(RoboCompVisualElementsPub::TObject &person, QGraphicsScene *scene) const
{
    static std::vector<QGraphicsItem*> items;
    // remove all items drawn in the previous iteration
    for(auto i: items)
    {
        scene->removeItem(i);
        delete i;
    }
    items.clear();

    // draw a circle around the person
    float radius = 300;
    auto person_draw = scene->addEllipse(-radius, -radius, radius*2, radius*2, QPen(Qt::magenta, 30));
    person_draw->setPos(std::stof(person.attributes["x_pos"]), std::stof(person.attributes["y_pos"]));
    items.push_back(person_draw);

    // draw a radius inside the ellipse to indicate the person's orientation
    auto x = std::stof(person.attributes.at("x_pos"));
    auto y = std::stof(person.attributes.at("y_pos"));
    auto angle = std::stof(person.attributes.at("orientation")) + M_PI;
    auto item_radius = scene->addLine(QLineF(QPointF(x, y),
                                                                    QPointF( x - radius * sin(angle),y + radius * cos(angle))),
                                                         QPen(Qt::magenta, 20));
    items.push_back(item_radius);

    // draw a line from the robot to the person circle but ending on the circunference. The end point is the exterior of the circle
    // I need a line from the robot to the person x,y but it has to be 300mm shorter
    auto len = std::hypot(x, y);
    auto item_line = scene->addLine(QLineF(QPointF(0.f, 0.f),
                                                                   QPointF((len -radius) *x/len, (len - radius)*y/len )),
                                                           QPen(Qt::magenta, 20));
    items.push_back(item_line);
}
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
QPolygonF SpecificWorker::shrink_polygon(const QPolygonF &polygon, qreal amount)
{
    if (polygon.isEmpty())
        return QPolygonF();

    // Calculate the centroid of the polygon
    QPointF centroid(0, 0);
    for (const QPointF &point : polygon)
        centroid += point;
    centroid /= polygon.size();

    // Move each point towards the centroid
    QPolygonF shrunkPolygon;
    for (const QPointF &point : polygon)
    {
        QPointF direction = centroid - point;
        qreal length = std::sqrt(direction.x() * direction.x() + direction.y() * direction.y());
        if (length > 0)
        {
            QPointF offset = direction / length * amount;
            shrunkPolygon << (point + offset);
        }
        else
            shrunkPolygon << point; // If the point is at the centroid, leave it unchanged
    }
    return shrunkPolygon;
}

void SpecificWorker::draw_obstacles(const vector<QPolygonF> &list_poly, QGraphicsScene *scene, const QColor &color) const
{
    static std::vector<QGraphicsItem*> items;
    // remove all items drawn in the previous iteration
    for(auto i: items)
    {
        scene->removeItem(i);
        delete i;
    }
    items.clear();

    for(const auto &poly : list_poly)
    {
        auto item = scene->addPolygon(poly, QPen(color, 50));
        items.push_back(item);
    }
}
void SpecificWorker::draw_path_to_person(const auto &points, QGraphicsScene *scene)
{
    if(points.empty())
        return;

    // remove all items drawn in the previous iteration
    static std::vector<QGraphicsItem*> items;
    for(auto i: items)
    {
        scene->removeItem(i);
        delete i;
    }
    items.clear();

    // draw the path as a series of lines with dots in between
    for (auto i : iter::range(0UL, points.size() - 1))
    {
        auto line = scene->addLine(QLineF(QPointF(points[i].x(), points[i].y()), QPointF(points[i+1].x(), points[i+1].y())),
                                   QPen(Qt::blue, 40));
        items.push_back(line);
        auto dot = scene->addEllipse(-30, -30, 60, 60, QPen(Qt::darkBlue, 40));
        dot->setPos(points[i].x(), points[i].y());
       items.push_back(dot);
    }
}

void SpecificWorker::plot_distance(double distance)
{
    // add value to plot
    static int key = 0;
    plot->graph(0)->addData(key++, distance);
    // Remove data points if there are more than X
    if (plot->graph(0)->dataCount() > params.MAX_DIST_POINTS_TO_SHOW)
        plot->graph(0)->data()->removeBefore(key - params.MAX_DIST_POINTS_TO_SHOW);
    // plot
    plot->rescaleAxes();  plot->replot();
}
//////////////////////////////////////////////////////////////////
/// SUBSCRIPTIONS (called in a different thread)
//////////////////////////////////////////////////////////////////
// SUBSCRIPTION to setVisualObjects method from VisualElementsPub interface. This is called in a different thread.
void SpecificWorker::VisualElementsPub_setVisualObjects(RoboCompVisualElementsPub::TData data)
{
    // std::cout << "VisualElements_setVisualObjects" << std::endl;
    //    for(auto object : data.objects)
    //        std::cout << "Object type: " << object.id << std::endl;
    //    qDebug() << "Size: " << data.objects.size();
    auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    buffer.put<0>(std::move(data), timestamp); // inserts the laser data value to the queue 0.
}

//////////////////////////////////////////////////////////////////
/// AUXILIARY FUNCTIONS
//////////////////////////////////////////////////////////////////
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

/**************************************/
// From the RoboCompVisualElements you can call this methods:
// this->visualelements_proxy->getVisualObjects(...)
// this->visualelements_proxy->setVisualObjects(...)

/**************************************/
// From the RoboCompVisualElements you can use this types:
// RoboCompVisualElements::TRoi
// RoboCompVisualElements::TObject
// RoboCompVisualElements::TObjects

// Instantiate the random number generator and distribution
//    static std::mt19937 gen(rd());
//    static std::uniform_int_distribution<int> dist(0, 1);
//    static bool first_time = true;
//    static int sign = 1;

//        auto poly = room_model.get_qt_polygon();
//        // filter out point on wall and outside room
//        poly = shrink_polygon(poly, params.ROBOT_WIDTH);
//        std::ranges::copy_if(points, std::back_inserter(points_inside), [poly](auto &a)
//        { return poly.containsPoint(QPointF(a.x, a.y), Qt::FillRule::OddEvenFill); });
//        if(points_inside.empty()) { qWarning() << __FUNCTION__ << "Empty points inside"; return {}; }
// cluster remaining points
// auto list_poly = rc::dbscan(points_inside, params.ROBOT_WIDTH, 2);