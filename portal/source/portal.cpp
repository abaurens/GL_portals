#include "portal.hpp"

#include "Mesh.hpp"
#include "Camera.hpp"
#include "Shader.hpp"

#include <glm/glm.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <chrono>
#include <iostream>

using namespace std::chrono_literals;
namespace chrono = std::chrono;

glm::dvec2 savedCursorPos;
bool paused = true;

glm::mat4 proj;

Shader flat;
Camera camera;
std::vector<Mesh> meshes;

// window
GLFWwindow *window;

// callbacks
static void error_callback(int error, const char *description)
{
  fprintf(stderr, "Error: %s\n", description);
}

static void resize_callback(GLFWwindow *, int width, int height)
{
  float ratio;
  ratio = width / (float)height;

  glViewport(0, 0, width, height);
  proj = glm::perspective(70.0f, ratio, 0.1f, 50.0f);
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
  {
    if (!paused)
    {
      paused = true;
      glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
      glfwSetCursorPos(window, savedCursorPos.x, savedCursorPos.y);
    }
    else
      glfwSetWindowShouldClose(window, GLFW_TRUE);
  }

  if (action != GLFW_PRESS)
    return;

  if (key == GLFW_KEY_P)
  {
    std::cout << "Camera:\n"
      << "  x: " << camera.position.x << "\n"
      << "  y: " << camera.position.y << "\n"
      << "  z: " << camera.position.z << "\n"
      << "  yaw: " << camera.yaw << "\n"
      << "  pitch: " << camera.pitch << std::endl;
  }
}

static void btn_callback(GLFWwindow *window, int button, int action, int mods)
{
  if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_PRESS && paused)
  {
    paused = false;
    glfwGetCursorPos(window, &savedCursorPos.x, &savedCursorPos.y);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPos(window, 0, 0);
  }
}





// portal 
static glm::mat4 portal_view(glm::mat4 orig_view, Mesh *src, Mesh *dst)
{
  glm::mat4 mv = orig_view * src->transform;
  glm::mat4 portal_cam =
    // 3. transformation from source portal to the camera - it's the
    //    first portal's ModelView matrix:
    mv
    // 2. object is front-facing, the camera is facing the other way:
    * glm::rotate(glm::mat4(1.0), glm::radians(180.0f), glm::vec3(0.0, 1.0, 0.0))
    // 1. go the destination portal; using inverse, because camera
    //    transformations are reversed compared to object
    //    transformations:
    * glm::inverse(dst->transform)
    ;
  return portal_cam;
}

static void init_portals()
{
  meshes.resize(2);

  const glm::vec4 portal_vertices[] = {
    glm::vec4(-1, 0, -1, 1),
    glm::vec4( 1, 0, -1, 1),
    glm::vec4(-1, 0,  1, 1),
    glm::vec4( 1, 0,  1, 1),
  };

  const GLushort portal_elements[] = {
    0,1,2, 2,1,3,
  };

  for (unsigned int i = 0; i < sizeof(portal_vertices) / sizeof(portal_vertices[0]); i++)
  {
    meshes[0].vertices.push_back(portal_vertices[i]);
    meshes[1].vertices.push_back(portal_vertices[i]);
  }

  for (unsigned int i = 0; i < sizeof(portal_elements) / sizeof(portal_elements[0]); i++)
  {
    meshes[0].indicies.push_back(portal_elements[i]);
    meshes[1].indicies.push_back(portal_elements[i]);
  }

  // 90° angle + slightly higher
  meshes[0].transform = glm::translate(glm::mat4(1), glm::vec3(0, -2, -1));
  meshes[1].transform = glm::rotate(glm::mat4(1), -90.0f, glm::vec3(0, 0, 1)) * glm::translate(glm::mat4(1), glm::vec3(0, -2, -1.2));

  meshes[0].upload(flat);
  meshes[1].upload(flat);

  // set camera position so they both appear on start
  camera = { { 3.2, 2, 1.0 }, {{ 4.1, 0.5 }} };
}

// Init pipeline
static void init_shaders()
{
  //GLuint vertex_shader, fragment_shader;

  flat.addFile(Shader::Type::vertex, "./Shaders/flat.vert");
  flat.addFile(Shader::Type::fragment, "./Shaders/flat.frag");
  flat.compile();

  if (!flat.valid())
  {
    clean_quit();
    exit(42);
  }
}

static void init_geometry()
{
  init_portals();
}


// init window & opengl context
static void init_gl_context()
{
  glfwInit();

  glfwSetErrorCallback(error_callback);

  //glfwWindowHint(GLFW_DOUBLEBUFFER, true);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

  window = glfwCreateWindow(1280, 720, PROJECT_NAME, nullptr, nullptr);
  if (!window)
  {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glfwSetFramebufferSizeCallback(window, resize_callback);
  glfwSetKeyCallback(window, key_callback);
  glfwSetMouseButtonCallback(window, btn_callback);

  if (glfwRawMouseMotionSupported())
    glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

  glfwMakeContextCurrent(window);
  //gladLoadGL();
  gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

  int width, height;
  glfwGetFramebufferSize(window, &width, &height);
  resize_callback(window, width, height);
}






void clean_quit()
{
  for (Mesh &mesh : meshes)
    mesh.clear();
  meshes.clear();

  flat.clear();

  glfwDestroyWindow(window);
  glfwTerminate();
}

void init()
{
  init_gl_context();
  glfwSwapInterval(1);

  // Graphic pipeline setup
  //glEnable(GL_CULL_FACE);
  glDisable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);

  init_shaders();
  init_geometry();
}

void loop()
{
  chrono::steady_clock::time_point now;
  chrono::steady_clock::time_point last = chrono::steady_clock::now();
  // rendering loop
  while (!glfwWindowShouldClose(window))
  {
    now = chrono::steady_clock::now();
    const chrono::milliseconds millis = chrono::duration_cast<chrono::milliseconds>(now - last);
    last = now;

    //std::cout << "timestep : " << millis << std::endl;

    update((float)millis.count() / 1000.0f);
    render();

    glfwSwapBuffers(window);
    glfwPollEvents();
  }
}

void update(float timestep)
{
  if (!paused)
    camera.update(timestep, window);
}

void render()
{
  glm::mat4 mvp;

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glUseProgram(flat.id());
  for (const Mesh &mesh : meshes)
  {
    mvp = proj * camera.getView() * mesh.transform;
    glUniformMatrix4fv(flat.getUniform("MVP"), 1, GL_FALSE, (const GLfloat *)&mvp);

    glDrawElements(GL_TRIANGLES, (GLsizei)mesh.indicies.size(), GL_UNSIGNED_SHORT, &mesh.indicies[0]);
  }
}

int main()
{
  std::cout << "Hello Premake from " << PROJECT_NAME << " !" << std::endl;

  init();
  loop();
  clean_quit();
  return 0;
}
