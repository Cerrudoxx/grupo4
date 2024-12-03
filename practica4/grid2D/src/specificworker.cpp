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
std::tuple<int, int> SpecificWorker::realToGrid(float x, float y)
{
    int i = (static_cast<float>(GRID_SIZE) / GRID_DIMENSION_MM) * x + (GRID_SIZE / 2);
    int j = (static_cast<float>(GRID_SIZE) / GRID_DIMENSION_MM) * y + (GRID_SIZE / 2);
    // if (i < 0 || i >= GRID_SIZE || j < 0 || j >= GRID_SIZE)
    // {
    // 	return std::nullopt;
    // }

    return std::make_tuple(i, j);
}

QPointF SpecificWorker::gridToReal(int i, int j)
{

    float x = GRID_DIMENSION_MM / GRID_SIZE * i - (GRID_DIMENSION_MM / 2);
    float y = GRID_DIMENSION_MM / GRID_SIZE * j - (GRID_DIMENSION_MM / 2);

    return QPointF(x, y);
}

void SpecificWorker::initialize()
{
    std::cout << "Initialize worker" << std::endl;
    ///////////// Your code ////////
    // Viewer
    viewer = new AbstractGraphicViewer(this->frame, params.GRID_MAX_DIM);
    auto [r, e] = viewer->add_robot(params.ROBOT_WIDTH, params.ROBOT_LENGTH, 0, 100, QColor("Blue"));
    robot_draw = r;
    viewer->show();

    initializeGrid(grid, &viewer->scene);
    ///////////////////////////////
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
    updateGrid(p_filter, grid);
    DrawGrid(grid, &viewer->scene);
}
/**
 * Draws LIDAR points onto a QGraphicsScene.
 *
 * This method clears any existing graphical graphics_items from the scene, then iterates over the filtered
 * LIDAR points to add new graphics_items. Each LIDAR point is represented as a colored rectangle. The point
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
        // los dos primeros valores son la posicion inicial del rectangulo, y los otros dos son las dimesiones del mismo
        auto item = scene->addRect(-25, -25, 100, 50, color, brush);
        item->setPos(p.x, p.y);
        items.push_back(item);
    }
}

void SpecificWorker::initializeGrid(auto &grid, QGraphicsScene *scene)
{

    for (auto &&[i, row] : grid | iter::enumerate)
        for (auto &&[j, celda] : row | iter::enumerate)
        {

            celda.graphics_item = viewer->scene.addRect(TILE_SIZE_MM / 2, TILE_SIZE_MM / 2, TILE_SIZE_MM, TILE_SIZE_MM,
                                                        QPen(QColor("White"), 20), QBrush(QColor("Light Gray")));
            celda.graphics_item->setPos(gridToReal(i, j));
            celda.state = StateCell::EMPTY;
            celda.x = i;
            celda.y = j;
        }
}

void SpecificWorker::DrawGrid(auto &grid, QGraphicsScene *scene){
    for (auto &&[i, row] : grid | iter::enumerate)
        for (auto &&[j, celda] : row | iter::enumerate)
        {
            if(celda.state == StateCell::OCCUPIED){
                celda.graphics_item = viewer->scene.addRect(TILE_SIZE_MM / 2, TILE_SIZE_MM / 2, TILE_SIZE_MM, TILE_SIZE_MM,
                                                        QPen(QColor("White"), 20), QBrush(QColor("Black")));
                celda.graphics_item->setPos(gridToReal(celda.x, celda.y));
            }
                
        }
            
}

float euclideanDistance(float x, float y)
{
    return std::sqrt(x * x + y * y);
}

void SpecificWorker::updateGrid(auto lidarPoints, auto &grid)
{
    int robot_x = 0;
    int robot_y = 0;
    for (const auto &p : lidarPoints)
    {
        auto distancia = euclideanDistance(p.x, p.y);
        for(const auto &salto: iter::range(0.f, 1.f, distancia/TILE_SIZE_MM)){
            auto r= p.norm()*salto;
            auto [i, j] = realToGrid(r.x, r.y);
            grid[i][j].state = StateCell::EMPTY;
            grid[i][j].graphics_item->setBrush(QBrush(QColor("White")));
        }
        auto [i, j] = realToGrid(p.x, p.y);
        grid[i][j].state = StateCell::OCCUPIED;
        grid[i][j].graphics_item->setBrush(QBrush(QColor("Red")));



        
    }
}

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

std::vector<Eigen::Vector2f> SpecificWorker::read_lidar_bpearl()
{
    try
    {
        auto ldata = lidar3d_proxy->getLidarData("bpearl", 0, 2 * M_PI, 1);
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

RoboCompGrid2D::Result SpecificWorker::Grid2D_getPaths(RoboCompGrid2D::TPoint source, RoboCompGrid2D::TPoint target)
{
#ifdef HIBERNATION_ENABLED
    hibernation = true;
#endif
    // implementCODE
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
// From the RoboCompGrid2D you can use this types:
// RoboCompGrid2D::TPoint
// RoboCompGrid2D::Result
