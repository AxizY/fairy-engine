#ifndef FAIRY_ENGINE_HPP
#define FAIRY_ENGINE_HPP
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#define arrayBuffer GL_ARRAY_BUFFER
#define elementArrayBuffer GL_ELEMENT_ARRAY_BUFFER

namespace fairy {
    struct vertexAttribute {
        int location;
        int size;
        GLenum type;
        const char* vertexArrayName;
    };

    struct vertexArray {
        const char* name;
        unsigned int id;
    };

    struct vertexBuffer {
        const char* vertexArrayName;
        const char* name;
        unsigned int id;
        GLenum type;
    };

    struct keyLink {
        int key;
        int* var;
    };

    struct transform {
        const char* name;
        glm::mat4 matrix;
        unsigned int location;
    };

    class shader {
        private:
            unsigned int vertexShader;
            unsigned int fragmentShader;
        public:
            unsigned int shaderProgram;
            void vertex(std::string vCode);
            void fragment(std::string fCode);
            void program();
            void use();
            void setFloat(const char* name, float value);
            void setMat4(unsigned int loc, glm::mat4 value);
    };

    class context {
        private:
            GLFWwindow* window;
            shader _shader;
            std::vector<vertexAttribute> vertexAttributes;
            std::vector<vertexArray> VAOs;
            std::vector<vertexBuffer> VBOs;
            std::vector<keyLink> keyLinks;
            std::vector<transform> transforms;
            
            template <typename T>
            inline T* getObjectByName(const char* name, std::vector<T>* array) {
                for(auto &_object : *array) {
                    if(_object.name == name) {
                        return &_object;
                    }
                }
            }
        public:
            context(int windowWidth, int windowHeight, const char* windowTitle, std::string vCode, std::string fCode);
            ~context();
            void createVertexArray(const char* name);
            void bindVertexArray(const char* name);

            void createVertexBuffer(const char* vaoName, const char* name, GLenum type);
            void createVertexAttribute(const char* vaoName, int location, int size, GLenum type);
            void setupVertexAttributes(const char* vaoName);

            void linkKey(int key, int* var);            

            bool loop(); // overloading: choose between auto clear or not
            bool loop(float r, float g, float b, float a);
            void clearFrame(float r, float g, float b, float a);
            void drawElements(const char* vaoName);
            void drawTriangles(const char* vaoName);
            void pollEvents();
            void swapBuffers();
            
            void createTransform(const char* name);
            void rotate(const char* name, float x, float y, float z);
            void translate(const char* name, float x, float y, float z);
            void scale(const char* name, float size);

            void getUniformLoc(const char* name, const char* uniformName);
            void applyTransform(const char* name, const char* uniformName);
            void projection(const char* name);
            
            shader* getShader();
            
            template <typename T>
            inline void sendVertexBuffer(const char* name, T input, unsigned int size) {
                vertexBuffer vbo = (*getObjectByName<vertexBuffer>(name, &VBOs));
                glBindVertexArray(getObjectByName<vertexArray>(vbo.vertexArrayName, &VAOs)->id);
                glBindBuffer(vbo.type, vbo.id);
                glBufferData(vbo.type, size, input, GL_STATIC_DRAW); 
                // GL_STATIC_DRAW might need be need to be an option
            }
    };
}
#endif