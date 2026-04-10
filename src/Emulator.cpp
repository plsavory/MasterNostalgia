#include "Emulator.h"
#include "ProjectInfo.h"

Emulator::Emulator() {
    system = nullptr;
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

    // Initialise SDL
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        SDL_Log("SDL could not initialize! SDL_Error: %s", SDL_GetError());
        throw GeneralException();
    }

    system = new MasterSystem(inputInterface, config);

    bool romLoadResult = system->init(fileName);

    if (!romLoadResult) {
        // TODO provide some more useful error handling
        throw GeneralException("Failed to load ROM file");
    }


}

void Emulator::run() {

    // Create window for video output
    if (!SDL_CreateWindowAndRenderer(Utils::getVersionString(false).c_str(), config->getDisplayWidth(), config->getDisplayHeight(), 0, &window, &renderer)) {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return;
    }

    setRenderingTexture();

    SDL_Scancode pauseKey = SDL_SCANCODE_UNKNOWN;
    SDL_Scancode exitKey = SDL_SCANCODE_UNKNOWN;

    if (config->getGeneralControlConfig() && config->getGeneralControlConfig()->getKeyboardConfig()) {
        exitKey = config->getGeneralControlConfig()->getKeyboardConfig()->getExitKey();
        pauseKey = config->getGeneralControlConfig()->getKeyboardConfig()->getPauseKey();
    }

    bool pauseEmulationWhenNotInFocus = config->getPauseEmulationWhenNotInFocus();

    bool isFullscreen = config->isFullScreenMode();

    if (isFullscreen) {
        SDL_SetWindowFullscreen(window, isFullscreen);
    }

//    bool hasPrintedVdpInfo = false;

    bool hasFocus = true;
    bool running = true;

    SDL_Event event;

    // TODO handle PAL (50.03 Hz)
    Uint64 targetNS = 1000000000 / 60.08;

    VDPBorderColour borderColour = VDPBorderColour{0, 0, 0};
    while (running) {

        Uint64 startNS = SDL_GetTicksNS();

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) { // Note the 'SDL_EVENT_' prefix
                running = false;
            }

            if (event.type == SDL_EVENT_WINDOW_FOCUS_GAINED) {
                hasFocus = true;
            }

            if (event.type == SDL_EVENT_WINDOW_FOCUS_LOST) {
                hasFocus = false;
            }

            if (event.type == SDL_EVENT_KEY_DOWN) {
                if (event.key.scancode == pauseKey) {
                    system->sendPauseInterrupt();
                }

                if (event.key.scancode == exitKey) {
                    running = false;
                }

                if (event.key.scancode == SDL_SCANCODE_F11) {
                    isFullscreen = !isFullscreen;
                    SDL_SetWindowFullscreen(window, isFullscreen);
                }
            }
        }

        // Lazy way to debug the VDP...
//        if (!hasPrintedVdpInfo && sf::Keyboard::isKeyPressed(sf::Keyboard::V)) {
//            system->printVDPInformation();
//            hasPrintedVdpInfo = true;
//        }
//
//        if (hasPrintedVdpInfo && !sf::Keyboard::isKeyPressed(sf::Keyboard::V)) {
//            hasPrintedVdpInfo = false;
//        }

        SDL_SetRenderDrawColor(renderer, borderColour.r, borderColour.g, borderColour.b, 255);
        SDL_RenderClear(renderer);

        int consoleDisplayWidth = system->getCurrentDisplayWidth();
        int consoleDisplayHeight = system->getCurrentDisplayHeight();

        // Reflect any screen size changes if needed
        if (consoleDisplayWidth != renderWidth || consoleDisplayHeight != renderHeight) {
            renderWidth = consoleDisplayWidth;
            renderHeight = consoleDisplayHeight;
            setRenderingTexture();
        }

        if (hasFocus || !pauseEmulationWhenNotInFocus) {
            system->emulateFrame(hasFocus);

            VDPFrame vdpFrame = system->getVideoOutput();

            if (!config->getUseStaticBackgroundColour()) {
                borderColour = vdpFrame.borderColor;
            }

            void* pixels;
            int pitch;
            SDL_LockTexture(m_texture, nullptr, &pixels, &pitch);
            uint8_t* dst = (uint8_t*)pixels;
            uint8_t* src = (uint8_t*)vdpFrame.pixels;

            for (int y = 0; y < consoleDisplayHeight; ++y) {
                memcpy(dst + (y * pitch), src + (y * (256 * 4)), 256 * 4);
            }

            SDL_UnlockTexture(m_texture);
            SDL_RenderTexture(renderer, m_texture, nullptr, nullptr);
        }

        // Final frame swap
        SDL_RenderPresent(renderer);

        Uint64 elapsedNS = SDL_GetTicksNS() - startNS;

        if (targetNS > elapsedNS) {
            SDL_DelayNS(targetNS - elapsedNS);
        }

    }

    SDL_Quit();
}

void Emulator::setRenderingTexture() {

    if (m_texture) {
        SDL_DestroyTexture(m_texture);
    }

    m_texture = SDL_CreateTexture(renderer,
                                  SDL_PIXELFORMAT_XRGB8888,
                                  SDL_TEXTUREACCESS_STREAMING,
                                  renderWidth, renderHeight);

    SDL_SetRenderLogicalPresentation(
            renderer,
            renderWidth, renderHeight,
            config->getPreserveAspectRatio() ? SDL_LOGICAL_PRESENTATION_LETTERBOX : SDL_LOGICAL_PRESENTATION_STRETCH
//            ,SDL_SCALEMODE_NEAREST
    );
}

