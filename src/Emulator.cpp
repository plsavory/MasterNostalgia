#include "Emulator.h"
#include "ProjectInfo.h"

Emulator::Emulator() {
    system = nullptr;
    window = nullptr;
    config = new Config();
    inputInterface = new InputInterface(config);

    renderWidth = 256;
    renderHeight = 224;
}

Emulator::~Emulator() {
    if (system) {
        delete(system);
    }
}

void Emulator::init(const std::string &fileName) {
    // TODO detect ROM type and support multiple consoles if we ever get master system support fully working
    system = new MasterSystem(inputInterface);

    bool romLoadResult = system->init(fileName);

    if (!romLoadResult) {
        // TODO provide some more useful error handling
        throw GeneralException("Failed to load ROM file");
    }


}

void Emulator::run() {
    // Create SFML window for video output
    setVideoMode((unsigned int)config->getDisplayWidth(), (unsigned int)config->getDisplayHeight());
    setRenderingTexture();

//    bool hasPrintedVdpInfo = false;

    while (window->isOpen()) {

        sf::Event event;
        while (window->pollEvent(event)) {
            if (event.type == sf::Event::Closed || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)) {
                window->close();
                return;
            }
        }
        system->emulateFrame();

        videoOutputTexture.update(system->getVideoOutput());
        window->clear(sf::Color::Black);
        window->draw(videoOutputSprite);
        window->display();

        // Lazy way to debug the VDP...
//        if (!hasPrintedVdpInfo && sf::Keyboard::isKeyPressed(sf::Keyboard::V)) {
//            system->printVDPInformation();
//            hasPrintedVdpInfo = true;
//        }
//
//        if (hasPrintedVdpInfo && !sf::Keyboard::isKeyPressed(sf::Keyboard::V)) {
//            hasPrintedVdpInfo = false;
//        }

        // TODO determine if the render mode needs to be changed, if the console has changed window output mode and change accordingly
        unsigned short consoleDisplayWidth = system->getCurrentDisplayWidth();
        unsigned short consoleDisplayHeight = system->getCurrentDisplayHeight();

        if (consoleDisplayWidth != renderWidth || consoleDisplayHeight != renderHeight) {
            renderWidth = consoleDisplayWidth;
            renderHeight = consoleDisplayHeight;
            setRenderingTexture();
        }
    }
    delete(window);
    window = nullptr;
}

void Emulator::setVideoMode(unsigned int width, unsigned int height) {
    if (window) {
        window->close();
        delete(window);
    }

    window = new sf::RenderWindow(sf::VideoMode(width, height, 32), Utils::getVersionString(false), config->isFullScreenMode() ? sf::Style::Fullscreen : sf::Style::Default);
    window->setFramerateLimit(60);
    window->setVerticalSyncEnabled(true);
}

void Emulator::setRenderingTexture() {
    // TODO add a way to keep the aspect ratio of the console's display, to prevent scaling artifacts.
    float widthScale = (float)config->getDisplayWidth()/(float)renderWidth;
    float heightScale = (float)config->getDisplayHeight()/(float)renderHeight;


    videoOutputTexture.create((int)renderWidth, (int)renderHeight);
    videoOutputSprite.setTexture(videoOutputTexture);
    videoOutputSprite.setPosition(0.f, 0.f);
    videoOutputSprite.setScale(widthScale, heightScale);
}

