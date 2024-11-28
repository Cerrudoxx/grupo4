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
std::pair<int, int> SpecificWorker::realToGrid(float x, float y)
{
    int i = static_cast<int>((x + (GRID_SIZE / 2) * TILE_SIZE_MM) / TILE_SIZE_MM);
    int j = static_cast<int>((y + (GRID_SIZE / 2) * TILE_SIZE_MM) / TILE_SIZE_MM);
    return {i, j};
}

std::pair<float, float> SpecificWorker::gridToReal(int i, int j)
{
    float x = (i - GRID_SIZE / 2) * TILE_SIZE_MM + TILE_SIZE_MM / 2;
    float y = (j - GRID_SIZE / 2) * TILE_SIZE_MM + TILE_SIZE_MM / 2;
    return {x, y};
}

void SpecificWorker::inicializarGrid(std::array<std::array<TCell, GRID_SIZE>, GRID_SIZE> &grid, QGraphicsScene *scene, auto &filtered_points)
{
    for (const auto &p : filtered_points)
    {
        auto [i, j] = realToGrid(p.x, p.y);
        grid[i][j].state = StateCell::EMPTY;
        grid[i][j].graphics_item = nullptr;
    }
}


void SpecificWorker::initialize()
{
    qDebug() << "Initialize Grid2D";

    // visor
    viewer = new AbstractGraphicViewer(this->frame, QRectF(-5000, 2500, 10000, -5000));
    viewer->setWindowTitle("Grid2D");

    // initialize grid
    RoboCompLidar3D::TData ldata = lidar3d_proxy->getLidarData("helios", 0, 2 * M_PI, 1);
     RoboCompLidar3D::TPoints p_filter;
    std::ranges::copy_if(ldata.points, std::back_inserter(p_filter),
                         [](auto &a)
                         { return a.z < 500 and a.distance2d > 200; });
    inicializarGrid(grid, &viewer->scene, p_filter);

    qDebug() << "Viewer created";
}

void SpecificWorker::compute()
{
    RoboCompLidar3D::TData ldata;
    try
    {
        ldata = lidar3d_proxy->getLidarData("helios", 0, 2 * M_PI, 1);
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
    draw_grid(grid, &viewer->scene);
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
}

void SpecificWorker::draw_grid(auto &grid, QGraphicsScene *scene)
{
    for (int i = 0; i < GRID_SIZE; i++)
    {
        for (int j = 0; j < GRID_SIZE; j++)
        {
            printf("");
        }
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
