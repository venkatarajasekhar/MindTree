#include "GL/glew.h"
#include "QGLContext"
#include "glm/gtc/matrix_transform.hpp"
#include "glwrapper.h"
#include "chrono"
#include "render.h"
#include "renderpass.h"
#include "shader_render_node.h"
#include "rendertree.h"

using namespace MindTree::GL;

void RenderConfig::setDrawPoints(bool draw)    
{
    _drawPoints = draw;
}

void RenderConfig::setDrawEdges(bool draw)    
{
    _drawEdges = draw;
}

void RenderConfig::setDrawPolygons(bool draw)    
{
    _drawPolygons = draw;
}

void RenderConfig::setShowFlatShaded(bool b)
{
    _flatShading = b;
}

bool RenderConfig::drawPoints()    const 
{
    return _drawPoints;
}

bool RenderConfig::drawEdges()    const 
{
    return _drawEdges;
}

bool RenderConfig::drawPolygons()    const 
{
    return _drawPolygons;
}

bool RenderConfig::flatShading() const
{
    return _flatShading;
}

bool RenderThread::_rendering = false;
std::mutex RenderThread::_renderingLock;
std::thread RenderThread::_renderThread;
std::vector<RenderTree*> RenderThread::_renderQueue;

void RenderThread::addManager(RenderTree* manager)
{
    auto it = std::find(begin(_renderQueue), end(_renderQueue), manager);
    if(it == end(_renderQueue))
        _renderQueue.push_back(manager);

    if(!isRendering()) start();
}

void RenderThread::removeManager(RenderTree *manager)
{
    auto it = std::find(begin(_renderQueue), end(_renderQueue), manager);
    if(it != end(_renderQueue))
        _renderQueue.erase(it);
    if(_renderQueue.empty()) stop();
}

bool RenderThread::isRendering()
{
    std::lock_guard<std::mutex> lock(_renderingLock);
    return _rendering;
}

void RenderThread::start()
{
    if(isRendering()) stop();

    _rendering = true;

    auto renderLoop = [] {
        while(RenderThread::isRendering()) {
            for(auto *manager : _renderQueue) {
                manager->draw();
            }
        }
    };

    _renderThread = std::thread(renderLoop);
}

void RenderThread::stop()
{
    std::cout << "stop rendering" << std::endl;
    {
        std::lock_guard<std::mutex> lock(_renderingLock);
        _rendering = false;
    }
    if (_renderThread.joinable()) _renderThread.join();
}

std::shared_ptr<ResourceManager> RenderTree::_resourceManager;

RenderTree::RenderTree(QGLContext *context)
    : _initialized(false), _context(context)
{
    if(!_resourceManager) _resourceManager = std::make_shared<ResourceManager>();
}

RenderTree::~RenderTree()
{
}

std::shared_ptr<ResourceManager> RenderTree::getResourceManager()
{
    return _resourceManager;
}

void RenderTree::setDirty()
{
    std::lock_guard<std::mutex> lock(_managerLock);
    _initialized = false;
}

void RenderTree::setCustomTextureNameMapping(std::string realname, std::string newname)
{
    {
        std::lock_guard<std::mutex> lock(_managerLock);
        _textureNameMappings[realname] = newname;
    }
    setDirty();
}

std::string RenderTree::getTextureName(std::shared_ptr<Texture> tex) const
{
    std::string realName = tex->getName();
    if(_textureNameMappings.find(realName) != end(_textureNameMappings))
        return _textureNameMappings.at(realName);
    else
        return realName;
}

void RenderTree::clearCustomTextureNameMapping()
{
    {
        std::lock_guard<std::mutex> lock(_managerLock);
        _textureNameMappings.clear();
    }
    setDirty();
}

void RenderTree::init()
{
    RenderThread::asrt();
    if(_initialized) return;

    static bool glewInitialized = false;

    if(!glewInitialized) glewInit();

    _initialized = true;
    glClearColor( 0., 0., 0., 0. );

    //connect output textures to all following passes
    uint i=0;
    for(auto &pass : passes){
        pass->init();
        if(i > 0) {
            for (uint j = 0; j < i; ++j){
                auto lastPass = passes[j];
                auto textures = lastPass->getOutputTextures();

                auto shadernodes = pass->getShaderNodes();
                for (auto shadernode : shadernodes) {
                    for(auto texture : textures) {
                        shadernode->program()->setTexture(texture, getTextureName(texture));
                    }
                    if(lastPass->_depthOutput == RenderPass::TEXTURE)
                        shadernode->program()->setTexture(lastPass->getOutDepthTexture());
                }
            }
        }
        ++i;
    }
}

std::vector<std::string> RenderTree::getAllOutputs() const
{
    std::vector<std::string> outputs;
    for(const auto &pass : passes) {
        auto textures = pass->getOutputTextures();
        for(const auto &tex : textures) {
            outputs.push_back(tex->getName());
        }
    }
    return outputs;
}

void RenderTree::setConfig(RenderConfig cfg)    
{
    std::lock_guard<std::mutex> lock(_managerLock);
    config = cfg;
}

RenderConfig RenderTree::getConfig()    
{
    std::lock_guard<std::mutex> lock(_managerLock);
    return config;
}

void RenderTree::addPass(std::shared_ptr<RenderPass> pass)
{
    std::lock_guard<std::mutex> lock(_managerLock);
    _initialized = false;
    passes.push_back(pass);
}

void RenderTree::insertPassBefore(std::weak_ptr<RenderPass> ref_pass, std::shared_ptr<RenderPass> pass)
{
    std::lock_guard<std::mutex> lock(_managerLock);
    _initialized = false;
    auto it = std::find(begin(passes), end(passes), ref_pass.lock());
    passes.insert(it, pass);
}

void RenderTree::insertPassAfter(std::weak_ptr<RenderPass> ref_pass, std::shared_ptr<RenderPass> pass)
{
    std::lock_guard<std::mutex> lock(_managerLock);
    _initialized = false;
    auto it = std::find(begin(passes), end(passes), ref_pass.lock());
    ++it;
    passes.insert(it, pass);
}

void RenderTree::removePass(std::weak_ptr<RenderPass> pass)
{
    std::lock_guard<std::mutex> lock(_managerLock);
    auto it = std::find(begin(passes), end(passes), pass.lock());
    if(it != passes.end()) {
        passes.erase(it);
        _initialized = false;
    }
    else {
        std::cout << "could not remove renderpass" << std::endl;
    }
}

RenderPass* RenderTree::getPass(uint index)
{
    std::lock_guard<std::mutex> lock(_managerLock);
    return std::next(begin(passes), index)->get();
}

void RenderTree::draw()
{
    ContextBinder binder(_context);
    RenderThread::asrt();

    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_POLYGON_OFFSET_POINT);

    auto start = std::chrono::steady_clock::now();
    {
        std::lock_guard<std::mutex> lock(_managerLock);
        if(!_initialized) {
            init();
        }
        int i = 0;
        for(auto &pass : passes){
            pass->render(config);
        }
        _resourceManager->cleanUp();
    }


    glDisable(GL_POINT_SMOOTH);
    glDisable(GL_PROGRAM_POINT_SIZE);
    glDisable(GL_POLYGON_OFFSET_POINT);
    _context->swapBuffers();

    glFinish();

    auto end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    renderTime = duration.count() / 1000000000.0;
    //std::cout << "Rendering took " << seconds << "s"
    //    << " ==> " << 1.0 / seconds << "fps" << std::endl;
}