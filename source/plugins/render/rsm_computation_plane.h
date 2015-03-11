#ifndef RSM_COMPUTATION_PLANE_H
#define RSM_COMPUTATION_PLANE_H

#include "light_accumulation_plane.h"
namespace MindTree {
namespace GL {

class RSMIndirectPlane : public LightAccumulationPlane
{
public:
    RSMIndirectPlane();

protected:
    void init(std::shared_ptr<ShaderProgram> program);
    void drawLight(const LightPtr light, 
                   std::shared_ptr<ShaderProgram> program) const;

private:
    std::shared_ptr<Texture2D> _samplingPattern;
};

}
}
#endif
