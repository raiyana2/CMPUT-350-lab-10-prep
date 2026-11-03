#include <filesystem>
#include <iostream>

#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

int main(int argc, char** argv) {
    std::filesystem::path executablePath = argc > 0 ? std::filesystem::path{argv[0]} : std::filesystem::path{};
    const std::filesystem::path shaderDirectory = executablePath.has_parent_path() ? executablePath.parent_path() : std::filesystem::current_path();

    const auto mainVertexShader = shaderDirectory / "main.vert";
    const auto mainFragmentShader = shaderDirectory / "main.frag";
    const auto blurFragmentShader = shaderDirectory / "blur.frag";

    if (!std::filesystem::exists(mainVertexShader) || !std::filesystem::exists(mainFragmentShader) ||
        !std::filesystem::exists(blurFragmentShader)) {
        std::cerr << "Shader files were not found in the executable directory: " << shaderDirectory << '\n';
        return 1;
    }

    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.stencilBits = 8;
    settings.majorVersion = 2;
    settings.minorVersion = 1;

    sf::RenderWindow window(sf::VideoMode({1280, 720}), "Shaders", sf::State::Windowed, settings);
    const auto& actualSettings = window.getSettings();
    std::cout << "OpenGL version " << actualSettings.majorVersion << "."
              << actualSettings.minorVersion << " flags: " << actualSettings.attributeFlags << '\n';

    sf::VertexArray fullscreenTriangle(sf::PrimitiveType::Triangles, 3);
    fullscreenTriangle[0].position = {-1.0f, -1.0f};
    fullscreenTriangle[1].position = {3.0f, -1.0f};
    fullscreenTriangle[2].position = {-1.0f, 3.0f};
    fullscreenTriangle[0].texCoords = {0.0f, 0.0f};
    fullscreenTriangle[1].texCoords = {2.0f, 0.0f};
    fullscreenTriangle[2].texCoords = {0.0f, 2.0f};

    sf::Shader sceneShader(mainVertexShader, mainFragmentShader);
    sf::Shader blurShader(mainVertexShader, blurFragmentShader);

    sf::RenderTexture sceneTexture({1280u, 720u});

    sf::Sprite sceneSprite(sceneTexture.getTexture());
    sceneSprite.setTexture(sceneTexture.getTexture(), true);

    sf::Vector2f texelSize(1.0f / static_cast<float>(sceneTexture.getSize().x),
                          1.0f / static_cast<float>(sceneTexture.getSize().y));

    while (window.isOpen()) {
        while (auto e = window.pollEvent()) {
            if (e->is<sf::Event::Closed>()) {
                return 0;
            }
        }

        sceneTexture.clear(sf::Color::Black);
        sceneTexture.draw(fullscreenTriangle, &sceneShader);
        sceneTexture.display();

        blurShader.setUniform("texture", sceneTexture.getTexture());
        blurShader.setUniform("texelSize", texelSize);

        window.clear(sf::Color::Black);
        window.draw(sceneSprite, &blurShader);
        window.display();
    }

    return 0;
}
