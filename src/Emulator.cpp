#include "Emulator.h"

Emulator::Emulator() {
    system = nullptr;
    window = nullptr;
}

Emulator::~Emulator() {
    if (system) {
        delete(system);
    }
}

void Emulator::init(const std::string &fileName) {
    // TODO detect ROM type and support multiple consoles if we ever get master system support fully working
    system = new MasterSystem();

    bool romLoadResult = system->init(fileName);

    if (!romLoadResult) {
        // TODO provide some more useful error handling
        throw GeneralException("Failed to load ROM file");
    }


}

void Emulator::run() {
    // Create SFML window for video output
    float width = 256;
    float height = 224;
    float displayScale = 3;

    int windowWidth = (int)(width * displayScale);
    int windowHeight = (int)(height * displayScale);

    setVideoMode(windowWidth, windowHeight);

    videoOutputTexture.create((int)width, (int)height);
    videoOutputSprite.setTexture(videoOutputTexture);
    videoOutputSprite.setPosition(0.f, 0.f);
    videoOutputSprite.setScale(displayScale, displayScale);

    while (window->isOpen()) {

        sf::Event event;
        while (window->pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window->close();
                return;
            }
        }
        system->emulateFrame();
        videoOutputTexture.update(system->getVideoOutput());
        window->clear(sf::Color::Black);
        window->draw(videoOutputSprite);
        window->display();
    }
    delete(window);
    window = nullptr;
}

void Emulator::setVideoMode(unsigned int width, unsigned int height) {
    if (window) {
        window->close();
        delete(window);
    }

    window = new sf::RenderWindow(sf::VideoMode(width, height, 32), "Mastalgia", sf::Style::Default);
}

