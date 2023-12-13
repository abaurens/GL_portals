#include "portal.hpp"

#include "Mesh.hpp"

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

static const char *vertex_shader_text = R"(
  #version 110

  uniform mat4 MVP;
  attribute vec2 vPos;
  varying vec3 color;

  void main()
  {
      gl_Position = MVP * vec4(vPos, 0.0, 1.0);
      color = vec3(1, 1, 1);
  }
)";

static const char *fragment_shader_text = R"(
  #version 110

  varying vec3 color;

  void main()
  {
      gl_FragColor = vec4(color, 1.0);
  }
)";

// shader
GLuint program;

// shader uniforms
GLint mvp_location, vpos_location, vcol_location;

// mesh
//Mesh portals[2];
std::vector<Mesh> meshes;

// window
GLFWwindow *window;

static void error_callback(int error, const char *description)
{
  fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GLFW_TRUE);
}


// portal 
glm::mat4 portal_view(glm::mat4 orig_view, Mesh *src, Mesh *dst)
{
  glm::mat4 mv = orig_view * src->object2world;
  glm::mat4 portal_cam =
    // 3. transformation from source portal to the camera - it's the
    //    first portal's ModelView matrix:
    mv
    // 2. object is front-facing, the camera is facing the other way:
    * glm::rotate(glm::mat4(1.0), glm::radians(180.0f), glm::vec3(0.0, 1.0, 0.0))
    // 1. go the destination portal; using inverse, because camera
    //    transformations are reversed compared to object
    //    transformations:
    * glm::inverse(dst->object2world)
    ;
  return portal_cam;
}

static void init_portals()
{
  meshes.resize(2);

  const glm::vec4 portal_vertices[] = {
    glm::vec4(-1, -1, 0, 1),
    glm::vec4(1, -1, 0, 1),
    glm::vec4(-1,  1, 0, 1),
    glm::vec4(1,  1, 0, 1),
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
  meshes[0].object2world = glm::translate(glm::mat4(1), glm::vec3(0, 1, -2));
  meshes[1].object2world = glm::rotate(glm::mat4(1), -90.0f, glm::vec3(0, 1, 0)) * glm::translate(glm::mat4(1), glm::vec3(0, 1.2, -2));

  meshes[0].upload(vpos_location);
  meshes[1].upload(vpos_location);
}

// Init pipeline
static void init_shaders()
{
  GLuint vertex_shader, fragment_shader;

  vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
  glCompileShader(vertex_shader);

  fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
  glCompileShader(fragment_shader);

  program = glCreateProgram();
  glAttachShader(program, vertex_shader);
  glAttachShader(program, fragment_shader);
  glLinkProgram(program);

  mvp_location = glGetUniformLocation(program, "MVP");
  vpos_location = glGetAttribLocation(program, "vPos");
  //vcol_location = glGetAttribLocation(program, "vCol");
}

static void init_geometry()
{
  init_portals();
}

void render(GLFWwindow *window)
{
  float ratio;
  int width, height;
  glm::mat4 m, v, p, mvp;

  glfwGetFramebufferSize(window, &width, &height);
  ratio = width / (float)height;

  glViewport(0, 0, width, height);
  glClear(GL_COLOR_BUFFER_BIT);

  m = glm::identity<glm::mat4>();
  //m = glm::rotate(m, (float)glfwGetTime(), glm::vec3(0, 0, 1));
  v = glm::lookAt(glm::vec3(5, 5, 5), glm::vec3(0, 0, 0), glm::vec3(0, 0, 1));
  p = glm::perspective(70.0f, ratio, 0.1f, 1000.0f);
  //p = glm::ortho(-ratio, ratio, -1.f, 1.f, 1.f, -1.f);
  mvp = p * v;

  glUseProgram(program);
  for (const Mesh &mesh : meshes)
  {
    mvp = p * v * mesh.object2world;
    glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat *)&mvp);

    glDrawElements(GL_TRIANGLES, mesh.indicies.size(), GL_UNSIGNED_SHORT, &mesh.indicies[0]);
    //glDrawElements(GL_TRIANGLES, mesh.indicies.size() / 3, GL_UNSIGNED_SHORT, &mesh.indicies[0]);
  }
}

// init window & opengl context

void init_gl_context()
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

  glfwSetKeyCallback(window, key_callback);

  glfwMakeContextCurrent(window);
  //gladLoadGL();
  gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
}

int main()
{
  std::cout << "Hello Premake from " << PROJECT_NAME << " !" << std::endl;

  init_gl_context();
  glfwSwapInterval(1);
  


  // Graphic pipeline setup
  //glEnable(GL_CULL_FACE);
  glDisable(GL_CULL_FACE);

  init_shaders();
  init_geometry();

  
  
  // rendering loop
  while (!glfwWindowShouldClose(window))
  {
    render(window);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // cleanup
  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}
