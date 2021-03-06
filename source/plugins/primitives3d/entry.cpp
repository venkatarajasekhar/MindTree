#include "sphere.h"
#include "cube.h"
#include "plane.h"
#include "cylinder.h"
#include "torus.h"

#include "boost/python.hpp"

#include "data/cache_main.h"

using namespace MindTree;

BOOST_PYTHON_MODULE(prim3d)
{
    auto cubeproc = [] (DataCache *cache) {
        float scale = cache->getData(0).getData<double>();
        auto cube = prim3d::createCubeMesh(scale);
        cache->pushData(cube);
    };

    auto planeproc = [] (DataCache *cache) {
        float scale = cache->getData(0).getData<double>();
        auto plane = prim3d::createPlaneMesh(scale);
        cache->pushData(plane);
    };

    DataCache::addProcessor(new CacheProcessor("OBJECTDATA", "CUBE", cubeproc));
    DataCache::addProcessor(new CacheProcessor("OBJECTDATA", "PLANE", planeproc));
}
