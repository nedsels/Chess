#include <thread>

#include "game.h"

GLFWwindow* initializeWindow(int width, int height, const char* name);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
void mouse_button_callback(GLFWwindow* window, int button, int action,
                           int mods);
void framedelay();

float lastFrame = 0.0f;
float framerate = 60.0f;

int main() {
  GLFWwindow* window = initializeWindow(SCR_WIDTH, SCR_HEIGHT, "Chess");

  stbi_set_flip_vertically_on_load(true);

  Game game = Game();

  while (!glfwWindowShouldClose(window)) {
    processInput(window);

    game.update();

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    framedelay();

    game.draw();

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}

GLFWwindow* initializeWindow(int width, int height, const char* name) {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

  GLFWwindow* window = glfwCreateWindow(width, height, name, NULL, NULL);

  if (window == NULL) {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    exit(1);
  }

  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetMouseButtonCallback(window, mouse_button_callback);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    exit(1);
  }

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  // glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);
  // glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE);

  return window;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
  // Closing program
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
  mouseX = (float)xposIn;
  mouseY = (float)yposIn;
}

void mouse_button_callback(GLFWwindow* window, int button, int action,
                           int mods) {
  if (clicking && action == GLFW_RELEASE) {
    clicking = false;
  }

  if (!clicking) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
      leftClick = true;
      clicking = true;
    }

    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
      rightClick = true;
      clicking = true;
    }
  }
}

void framedelay() {
  float deltaTime = (float)glfwGetTime() - lastFrame;
  float tick = 1.0f / framerate;

  if (deltaTime < tick) {
    std::this_thread::sleep_for(
        std::chrono::nanoseconds((int)((tick - deltaTime) * pow(10, 9))));
  }

  lastFrame = (float)glfwGetTime();
}
