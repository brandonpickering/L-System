
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/common.hpp>
#include <glm/mat2x2.hpp>
#include <glm/vec2.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <L-System/lsystem.hpp>

#include <fstream>
#include <stack>


using namespace glm;


class Turtle2D {
public:
    struct State {
        vec2 pos = vec2(0, 0);
        vec2 face = vec2(0, 1);
    };
    
    State state;
    std::stack<State> stateStack;

    void apply(lsystem::Symbol command) {
        command.args.push_back(0);

        if (command.name == "[") {
            stateStack.push(state);

        } else if (command.name == "]") {
            if (!stateStack.empty()) {
                state = stateStack.top();
                stateStack.pop();
            }

        } else if (command.name == "seg") {
            glBegin(GL_LINES);
            glVertex2f(state.pos.x, state.pos.y);
            state.pos += state.face * (float) command.args[0];
            glVertex2f(state.pos.x, state.pos.y);
            glEnd();
        
        } else if (command.name == "rot") {
            float a = (float) (command.args[0] * 3.14159265358979/180.0);
            mat2 rot = mat2(cos(a), sin(a), -sin(a), cos(a));
            state.face = rot * state.face;
        }
    }
};


class Turtle3D {
public:
    const vec3 gravity = vec3(0, -1, 0);
    
    struct State {
        vec3 pos = vec3(0, 0, 0);
        vec3 face = vec3(0, 1, 0);
        vec3 head = vec3(0, 0, 1);
    };

    State state;
    std::stack<State> stateStack;

    void apply(lsystem::Symbol command) {
        command.args.push_back(0);
        float a = (float) (command.args[0] * 3.14159265358979/180.0);

        if (command.name == "[") {
            stateStack.push(state);

        } else if (command.name == "]") {
            if (!stateStack.empty()) {
                state = stateStack.top();
                stateStack.pop();
            }

        } else if (command.name == "seg") {
            glBegin(GL_LINES);
            glVertex3f(state.pos.x, state.pos.y, state.pos.z);
            state.pos += state.face * (float) command.args[0];
            glVertex3f(state.pos.x, state.pos.y, state.pos.z);
            glEnd();
        
        } else if (command.name == "yaw") {
            mat4 rot = rotate(mat4(), a, state.head);
            state.face = vec3(rot * vec4(state.face, 1));
        
        } else if (command.name == "roll") {
            mat4 rot = rotate(mat4(), -a, state.face);
            state.head = vec3(rot * vec4(state.head, 1));
        
        } else if (command.name == "pitch") {
            vec3 right = cross(state.face, state.head);
            mat4 rot = rotate(mat4(), a, right);
            state.face = vec3(rot * vec4(state.face, 1));
            state.head = vec3(rot * vec4(state.head, 1));
        
        } else if (command.name == "bal") {
            vec3 balRight = cross(gravity, state.face);
            vec3 balHead = cross(balRight, state.face);
            state.head = normalize(balHead);
        }
    }
};


static int n = 5;
static double t = 0;

static lsystem::System lsys;


static void load() {
    std::ifstream file("tree.lsys");
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(file, line))
        lines.push_back(line);

    std::vector<lsystem::ParseError> errors = lsys.interpret(lines);
    for (auto error : errors)
        printf("%d:%d: %s\n", error.line, error.column, error.message.c_str());
}


static void draw() {
    lsystem::String commands = lsys.iterate(lsys.seed(), n);

    glLoadIdentity();
    glFrustum(-1, 1, -1, 1, 1, 100);
    glTranslatef(0, -4.5, -5);
    glRotatef(0.1 * t * 360, 0, 1, 0);
    glScalef(1.2, 1.2, 1.2);

    Turtle3D turtle;
    for (lsystem::Symbol command : commands)
        turtle.apply(command);
}


static void draw2D() {
    lsystem::String commands = lsys.iterate(lsys.seed(), n);

    glLoadIdentity();
    float size = 10;
    glOrtho(-size/2, size/2, 0, size, -size/2, size/2);

    Turtle2D turtle;
    for (lsystem::Symbol command : commands)
        turtle.apply(command);
}


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_UP && action == GLFW_PRESS)
        n += 1;
    if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
        n -= 1;
    n = max(n, 0);

    // if (key == GLFW_KEY_ENTER && action == GLFW_PRESS)
    //     load();
}


int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
    GLFWwindow *window = glfwCreateWindow(512, 512, "", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

    glfwSetKeyCallback(window, key_callback);
    glfwSetTime(0);

    while (!glfwWindowShouldClose(window)) {
        t = glfwGetTime();
        load();

        glClear(GL_COLOR_BUFFER_BIT);

        draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
