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

    sf::Keyboard::Key pauseKey = sf::Keyboard::Unknown;
    sf::Keyboard::Key exitKey = sf::Keyboard::Unknown;

    if (config->getGeneralControlConfig() && config->getGeneralControlConfig()->getKeyboardConfig()) {
        exitKey = config->getGeneralControlConfig()->getKeyboardConfig()->getExitKey();
        pauseKey = config->getGeneralControlConfig()->getKeyboardConfig()->getPauseKey();
    }

//    bool hasPrintedVdpInfo = false;

    while (window->isOpen()) {

        sf::Event event;
        while (window->pollEvent(event)) {

            if (event.type == sf::Event::Closed || (event.type == sf::Event::KeyPressed && exitKey != sf::Keyboard::Unknown && event.key.code == exitKey)) {
                window->close();
                return;
            }

            if (event.type == sf::Event::KeyPressed && pauseKey != sf::Keyboard::Unknown && event.key.code == pauseKey) {
                system->sendPauseInterrupt();
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

    float displayWidth = (float)config->getDisplayWidth();
    float displayHeight = (float)config->getDisplayHeight();

    float widthScale;
    float heightScale;

    videoOutputTexture.create((int)renderWidth, (int)renderHeight);
    videoOutputSprite.setTexture(videoOutputTexture);

    float xPosition = 0.f;
    float yPosition = 0.f;

    if (config->getPreserveAspectRatio()) {
        // Preserving original aspect ratio - determine the largest scale that we can fit inside the window
        widthScale = heightScale = std::min(displayWidth / (float)renderWidth, displayHeight / (float)renderHeight);

        // Position the display so that it appears in the middle of the screen
        xPosition += std::abs(((float)renderWidth * widthScale) - displayWidth)/2;
        yPosition += std::abs(((float)renderHeight * heightScale) - displayHeight)/2;

    } else {
        // Not preserving the original aspect ratio - stretch the image to the full size of the screen/window.
        widthScale = displayWidth/(float)renderWidth;
        heightScale = displayHeight/(float)renderHeight;
    }


    videoOutputSprite.setScale(widthScale, heightScale);
    videoOutputSprite.setPosition(xPosition, yPosition);
}

