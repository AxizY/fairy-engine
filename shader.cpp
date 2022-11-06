#include "fairy_engine.hpp"

namespace fairy {
    void shader::vertex(std::string vCode) {
        vertexShader = glCreateShader(GL_VERTEX_SHADER);
        const char* vCodeC = vCode.c_str();
        glShaderSource(vertexShader, 1, &vCodeC, NULL);
        glCompileShader(vertexShader);
        int success;
        char infoLog[512];
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
        if(!success)
        {
            glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
            std::cout << "Vertex Error \n" << infoLog << std::endl;
        };
    }

    void shader::fragment(std::string fCode) {
        fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        const char* fCodeC = fCode.c_str();
        glShaderSource(fragmentShader, 1, &fCodeC, NULL);
        glCompileShader(fragmentShader);
        int success;
        char infoLog[512];
        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
        if(!success)
        {
            glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
            std::cout << "Fragment Error \n" << infoLog << std::endl;
        };
    }

    void shader::program() {
        shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);
        int success;
        char infoLog[512];
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
        if(!success)
        {
            glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
            std::cout << "Shader Program Error \n" << infoLog << std::endl;
        }
    }

    void shader::setFloat(const char* name, float value) {
        glUniform1f(glGetUniformLocation(shaderProgram, name), value);
    }

    void shader::setMat4(unsigned int loc, glm::mat4 value) {
        glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(value));
    }

    void shader::use() {
        glUseProgram(shaderProgram);
    }
}