#include "IntroState.h"
#include "GlobalIncludes.h"

using namespace std;
using namespace glm;

void IntroState::pause(){}
void IntroState::resume(){}
void IntroState::finalize(){}
void IntroState::handle_events(){}

void IntroState::initialize()
{
    // Initialize resources
    model = make_unique<Model>("Assets/house/house.obj");

    texture = make_unique<Texture>();
    texture->load("Assets/house/houseTexture.jpg");

    hudTexture = make_unique<Texture>();
    hudTexture->load("Assets/skybox/front.jpg");

    modelProgram = make_shared<ShaderProgram>();
    modelProgram->create_program();
    modelProgram->add_shader_from_file("Shaders/modelVert.glsl", GL_VERTEX_SHADER);
    modelProgram->add_shader_from_file("Shaders/modelFrag.glsl", GL_FRAGMENT_SHADER);
    modelProgram->link_program();

    triProgram = make_shared<ShaderProgram>();
    triProgram->create_program();
    triProgram->add_shader_from_file("Shaders/triVert.glsl", GL_VERTEX_SHADER);
    triProgram->add_shader_from_file("Shaders/triFrag.glsl", GL_FRAGMENT_SHADER);
    triProgram->link_program();

    GLfloat vertices[] = {
        // Positions          // Colors           // Texture Coords
        0.25f - 0.5f,  0.25f - 0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // Top Right
        0.25f - 0.5f, -0.25f - 0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // Bottom Right
        -0.25f - 0.5f, -0.25f - 0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // Bottom Left
        -0.25f - 0.5f,  0.25f - 0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // Top Left 
    };
    GLuint indices[] = {  // Note that we start from 0!
        0, 1, 3,   // First Triangle
        1, 2, 3    // Second Triangle
    };

    triVAO.create();
    triVBO.create();
    triIBO.create();

    triVAO.bind();

    triVBO.bind();
    triVBO.add_data(&vertices, sizeof(vertices));
    triVBO.upload_data();

    triIBO.bind(GL_ELEMENT_ARRAY_BUFFER);
    triIBO.add_data(&indices, sizeof(indices));
    triIBO.upload_data();

    triVAO.enable_attribute(0);
    triVAO.configure_attribute(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);
    triVAO.enable_attribute(1);
    triVAO.configure_attribute(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    triVAO.enable_attribute(2);
    triVAO.configure_attribute(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));

    triVAO.unbind();

    camera = make_unique<Camera>(game_, vec3(0.0, 25.0, -70.0), glm::vec3(0), 5.0);
    camera->enable_input();

    SkyboxParameters skyboxParameters;
    skyboxParameters.right = "Assets/skybox/right.jpg";
    skyboxParameters.left = "Assets/skybox/left.jpg";
    skyboxParameters.top = "Assets/skybox/top.jpg";
    skyboxParameters.bottom = "Assets/skybox/bottom.jpg";
    skyboxParameters.front = "Assets/skybox/front.jpg";
    skyboxParameters.back = "Assets/skybox/back.jpg";

    skybox = make_unique<Skybox>(skyboxParameters);

    angle = 0.0f;
}

void IntroState::update()
{
    angle += game_->time_delta() * float(pi<float>()) / 2;
    camera->update(game_);
}

void IntroState::draw()
{
    modelProgram->use();
    texture->bind();
    for(int i = 0; i < 10; i++)
    {
        for(int j = 0; j < 10; j++)
        {
            model->transform_manager()->translate(glm::mat4(1.0f), vec3(50.0f * i, 50.0f * j, 0.0f));
            model->transform_manager()->rotate(model->transform_manager()->model_matrix(), angle, vec3(0.0f, 1.0f, 0.0f));
            mat4 mvp = camera->make_mvp(model->transform_manager()->model_matrix());
            modelProgram->set_uniform("mvpMatrix", &mvp);
            model->draw(modelProgram.get(), camera.get());
        }
    }

    triProgram->use();
    triVAO.bind();
    hudTexture->bind();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    triVAO.unbind();

    skybox->draw(camera.get());
}

