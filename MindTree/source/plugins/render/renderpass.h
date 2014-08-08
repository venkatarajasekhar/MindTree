#ifndef MT_GL_RENDERPASS_H
#define MT_GL_RENDERPASS_H

#include "memory"
#include "mutex"
#include "vector"
#include "queue"
#include "utility"
#include "../datatypes/Object/object.h"

namespace MindTree {
namespace GL {

class ShaderRenderNode;

class Texture2D;
class Renderer;
class VAO;
class FBO;
class Renderbuffer;
class RenderConfig;

class RenderPass
{
public:
    RenderPass();
    virtual ~RenderPass();
    void render(const RenderConfig &config);

    void setCamera(CameraPtr camera);
    void setRenderersFromGroup(std::shared_ptr<Group> group);
    void addRendererFromObject(std::shared_ptr<GeoObject> obj);
    void addRenderer(Renderer *renderer);

    void setTarget(std::shared_ptr<FBO> target);
    std::shared_ptr<FBO> getTarget();

    std::vector<std::string> getRenderbufferOutNames();

    enum DepthOutput {
        TEXTURE,
        RENDERBUFFER,
        NONE
    };

    void setDepthOutput(std::shared_ptr<Texture2D> output);
    void setDepthOutput(std::shared_ptr<Renderbuffer> output);

    void addOutput(std::shared_ptr<Texture2D> tex);
    void addOutput(std::shared_ptr<Renderbuffer> rb);

    std::vector<std::shared_ptr<Texture2D>> getOutputTextures();
    std::shared_ptr<Texture2D> getOutDepthTexture();

    std::vector<std::shared_ptr<ShaderRenderNode>> getShaderNodes();

    CameraPtr getCamera();

    void setSize(int width, int height);
    glm::ivec2 getResolution();
    std::vector<glm::vec4> readPixel(std::vector<std::string> name, glm::ivec2 pos);

private:
    void processPixelRequests();
    void addGeometryRenderer(Renderer *renderer);

    std::vector<glm::vec4> _requestedPixels;
    std::queue<std::pair<std::string, glm::ivec2>> _pixelRequests;
    std::mutex _pixelRequestsLock;

    friend class RenderManager;
    void init();

    int _width, _height;
    bool _viewportChanged, _initialized;
    std::shared_ptr<Camera> _camera;
    std::shared_ptr<FBO> _target;
    std::mutex _sizeLock;
    std::mutex _geometryLock;

    std::vector<std::shared_ptr<ShaderRenderNode>> _shadernodes;
    std::vector<std::shared_ptr<ShaderRenderNode>> _geometryShaderNodes;

    std::vector<std::shared_ptr<Texture2D>> _outputTextures;
    std::vector<std::shared_ptr<Renderbuffer>> _outputRenderbuffers;

    std::shared_ptr<Texture2D> _depthTexture;
    std::shared_ptr<Renderbuffer> _depthRenderbuffer;
    DepthOutput _depthOutput;
};

}
}
#endif
