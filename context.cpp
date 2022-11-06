#include "fairy_engine.hpp"

// has to be global so just put it here
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    std::vector<fairy::keyLink>* keyLinks;
    keyLinks = (std::vector<fairy::keyLink>*)glfwGetWindowUserPointer(window);
    for(auto &_keyLink : *keyLinks) {
        if(_keyLink.key == key) {
            if(action == GLFW_PRESS) {
                *_keyLink.var = 1;
            } else if(action == GLFW_RELEASE) {
                *_keyLink.var = 0;
            }
        }
    }
}

namespace fairy {
    context::context(int windowWidth, int windowHeight, const char* windowTitle, std::string vCode, std::string fCode) {
        glfwInit();

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

        window = glfwCreateWindow(windowWidth, windowHeight, windowTitle, NULL, NULL);
        glfwMakeContextCurrent(window);
        //glfwSwapInterval(0);

        gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
        glViewport(0, 0, windowWidth, windowHeight);
        
        glfwSetKeyCallback(window, keyCallback);

        _shader.vertex(vCode);
        _shader.fragment(fCode);
        _shader.program();
    }

    void context::createVertexArray(const char* name) {
        unsigned int VAO;
        glGenVertexArrays(1, &VAO);
        VAOs.push_back(vertexArray{name, VAO});
    }

    void context::bindVertexArray(const char* name) {
        glBindVertexArray(getObjectByName<vertexArray>(name, &VAOs)->id);
    }

    void context::createVertexBuffer(const char* vaoName, const char* name, GLenum type) {
        unsigned int VBO;
        glGenBuffers(1, &VBO);
        VBOs.push_back(vertexBuffer{vaoName, name, VBO, type});
    }

    void context::createVertexAttribute(const char* vaoName, int location, int size, GLenum type) {
        vertexAttributes.push_back(vertexAttribute{location, size, type, vaoName});
    }

    void context::setupVertexAttributes(const char* vaoName) {
        glBindVertexArray(getObjectByName<vertexArray>(vaoName, &VAOs)->id);
        int sum = 0;
        for(auto &vAttrib : vertexAttributes) {
            if(vAttrib.vertexArrayName == vaoName) {
                sum += vAttrib.size;
            }
        }
        for(auto &vAttrib : vertexAttributes) {
            if(vAttrib.vertexArrayName == vaoName) {
                glVertexAttribPointer(vAttrib.location, vAttrib.size, vAttrib.type, GL_FALSE, sum * sizeof(float), (void*)0);
                // size of .. should be configurable
                glEnableVertexAttribArray(vAttrib.location);
            }
        }
    }

    void context::clearFrame(float r, float g, float b, float a) {
        glClearColor(r, g, b, a);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    void context::linkKey(int key, int* var) {
        keyLinks.push_back(keyLink{key, var});
        glfwSetWindowUserPointer(window, (void *)&keyLinks);
    }

    bool context::loop() {
        return !glfwWindowShouldClose(window);
    }

    bool context::loop(float r, float g, float b, float a) {
        clearFrame(r, g, b, a);
        return !glfwWindowShouldClose(window);
    }

    void context::drawElements(const char* vaoName) {
        _shader.use();
        glBindVertexArray(getObjectByName<vertexArray>(vaoName, &VAOs)->id); 
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }

    void context::drawTriangles(const char* vaoName) {
        _shader.use();
        glBindVertexArray(getObjectByName<vertexArray>(vaoName, &VAOs)->id);
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

    void context::swapBuffers() {
        glfwSwapBuffers(window);
    }

    void context::pollEvents() {
        glfwPollEvents();
    }

    shader* context::getShader() {
        return &_shader;
    }

    context::~context() {}

    void context::createTransform(const char* name) {
        glm::mat4 _transform = glm::mat4(1.0f);
        transforms.push_back(transform{name, _transform});
    }

    void context::rotate(const char* name, float x, float y, float z) {
        glm::mat4 &_transform = getObjectByName<transform>(name, &transforms)->matrix;
        // WOAHH THIS WORKS??
        // !!!!!!!!!!!!!!!!!!
        _transform = glm::rotate(_transform, glm::radians(x), glm::vec3(1.0f, 0.0f, 0.0f));
        _transform = glm::rotate(_transform, glm::radians(y), glm::vec3(0.0f, 1.0f, 0.0f));
        _transform = glm::rotate(_transform, glm::radians(z), glm::vec3(0.0f, 0.0f, 1.0f));
    }

    void context::scale(const char* name, float size) {
        glm::mat4 _transform = getObjectByName<transform>(name, &transforms)->matrix;
        _transform = glm::scale(_transform, glm::vec3(size, size, size));
        getObjectByName<transform>(name, &transforms)->matrix = _transform;
    }

    void context::translate(const char* name, float x, float y, float z) {
        glm::mat4 _transform = getObjectByName<transform>(name, &transforms)->matrix;
        _transform = glm::translate(_transform, glm::vec3(x, y, z));
        getObjectByName<transform>(name, &transforms)->matrix = _transform;
    }

    void context::applyTransform(const char* name, const char* uniformName) {
        _shader.setMat4(getObjectByName<transform>(name, &transforms)->location, getObjectByName<transform>(name, &transforms)->matrix);
    }

    void context::projection(const char* name) {
        int width;
        int height;
        glfwGetFramebufferSize(window, &width, &height);
        glm::mat4 _transform = getObjectByName<transform>(name, &transforms)->matrix;
        _transform = glm::perspective(glm::radians(45.0f), (float)width/height, 0.1f, 100.0f);
        getObjectByName<transform>(name, &transforms)->matrix = _transform;
    }

    void context::getUniformLoc(const char* name, const char* uniformName) {
        getObjectByName<transform>(name, &transforms)->location = glGetUniformLocation(_shader.shaderProgram, uniformName);
    }
}