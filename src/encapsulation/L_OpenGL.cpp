
#include "encapsulation/L_OpenGL.h"

/////////////////////
//
//	INIT
//
/////////////////////

L_OpenGL::L_OpenGL(const std::shared_ptr<Terrain> &terrain, const std::shared_ptr<Menu> &menu, const std::shared_ptr<ShadowMap> &shadowMap)
        : _terrain(terrain), _menu(menu), _shadowMap(shadowMap)
{
    GLCall(glEnable(GL_DEPTH_TEST));
}

void    L_OpenGL::initShader(const glm::mat4 &projection)
{
    _terrain->_projection = projection;
}

void    L_OpenGL::updateShader(std::shared_ptr<Camera> camera)
{
    if (!(_menu->_linkDone && _terrain->_isTerrainLinked))
    {
        _menu->useShader();
        if (_terrain->_isTerrainReady && !_terrain->_isTerrainLinked)
        {
            _terrain->_loadingThread.join();
            _terrain->bindTerrain();
            _terrain->_isTerrainLinked = true;
            _menu->useShader();
        }
    }
    else
    {
        _shadowMap->_shader.use();
        // Compute the MVP matrix from the light's point of view
        std::cout << camera->_width << " " << camera->_height << " " << camera->_deep << std::endl;
        glm::mat4 depthProjectionMatrix = glm::ortho<float>(camera->_width * -0.5f, camera->_width * 0.5f,
                                                            camera->_height * -0.5f, camera->_height * 0.5f,
                                                            camera->_deep * -0.5f, camera->_deep * 0.5f);
        glm::mat4 depthViewMatrix = glm::lookAt(camera->_centroid, camera->_centroid + glm::vec3(-1, -1, 0) , camera->_upSun);
        glm::mat4 depthModelMatrix = glm::mat4(1.0);
        glm::mat4 depthMVP = depthProjectionMatrix * depthViewMatrix * depthModelMatrix;
        _shadowMap->_shader.setMat4("mvp", depthMVP);

        _terrain->_shader.use();
        _terrain->_shader.setVec3("cameraDir", camera->_dirLook);
        _terrain->_mvp = _terrain->_projection * camera->_view;
        _terrain->_shader.setMat4("mvp", _terrain->_mvp);
    }
}

void    L_OpenGL::clear() const
{
    GLCall(glClearColor(0.2f, 0.3f, 0.3f, 1.0f));
    GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
}

void    L_OpenGL::display() const
{
    clear();

    if (!(_menu->_linkDone && _terrain->_isTerrainLinked))
    {
        _menu->_gui.draw();
    }
    else
    {
        _terrain->_shader.use();
        _terrain->draw();

        _shadowMap->draw();
    }
}