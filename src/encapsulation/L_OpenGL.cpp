
#include "encapsulation/L_OpenGL.hpp"

/////////////////////
//
//	INIT
//
/////////////////////


L_OpenGL::L_OpenGL(const std::string &mapVsPath, const std::string &mapFsPath,
                   const std::shared_ptr<Menu> &menu)
        : _menu(menu), _terrain(mapVsPath, mapFsPath)
{
    GLCall(glEnable(GL_DEPTH_TEST));

    _loadingThread = std::thread(&Terrain::generateTerrain, &_terrain);
}



void    L_OpenGL::initShader(const glm::mat4 &projection)
{
    _terrain._shader.use();
    _terrain._shader.setMat4("projection", projection);
}


void    L_OpenGL::updateShader(const glm::vec3 &dirLook, const glm::mat4 &view)
{
    if (!_linkDone)
    {
        _menu->_textShader.use();
        if (_terrain._isTerrainReady)
        {
            _loadingThread.join();
            _terrain.bindTerrain();
            _linkDone = true;
        }
    }
    else
    {
        _terrain._shader.use();
        _terrain._shader.setVec3("cameraDir", dirLook);
        _terrain._shader.setMat4("view", view);
    }
}

void    L_OpenGL::setTexture(int textureID)
{
    _menu->_textShader.use();
    _menu->_textShader.setInt("u_Texture", textureID);
}

void    L_OpenGL::display()
{
    GLCall(glClearColor(0.2f, 0.3f, 0.3f, 1.0f));
    GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

    if (!_linkDone)
    {
        GLCall(glBindVertexArray(_menu->_vArray._vArrayId));
        GLCall(glDrawArrays(GL_TRIANGLES, 0, _menu->_vertexNb));
    }
    else if (_linkDone)
    {
        GLCall(glBindVertexArray(_terrain._vArray._vArrayId));
        GLCall(glDrawArrays(GL_TRIANGLES, 0, _terrain._vertexNb));
    }
}

void    L_OpenGL::cleanUp()
{
}