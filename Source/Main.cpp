#include <JuceHeader.h>
#include "MainComponent.h"

// Main application window that inherits from JUCE's DocumentWindow
// Handles window creation, configuration, and lifecycle management
class MainWindow : public juce::DocumentWindow {

    public:
        // Constructor: Sets up the window with title, color scheme, and button configuration
        MainWindow(juce::String name): DocumentWindow(
            name,
            // Get the default background color from the current look and feel
            juce::Desktop::getInstance()
                .getDefaultLookAndFeel()
                .findColour(juce::ResizableWindow::backgroundColourId),
            // Enable minimize, maximize, and close buttons
            DocumentWindow::allButtons){

                // Use the operating system's native title bar instead of JUCE's custom one
                setUsingNativeTitleBar(true);
                
                // Create and set the main synth component as the window's content
                setContentOwned(new MainComponent(), true);

                // Platform-specific window configuration
                #if JUCE_IOS || JUCE_ANDROID
                    // On mobile platforms, use fullscreen mode
                    setFullScreen(true);
                #else
					// On desktop platforms, setup a window with specific size and resizability
                    setResizable(true, false);
                    centreWithSize(getWidth(), getHeight());
                #endif

                setVisible(true);
        }

        // Requests the application to quit gracefully
        void closeButtonPressed() override{
            juce::JUCEApplication::getInstance()->systemRequestedQuit();
        }

    private:
        // JUCE macro that prevents copying and detects memory leaks in debug builds
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainWindow)
};


// Main application class that manages the application lifecycle
class CodingSoundsSynth : public juce::JUCEApplication {
    public:
        CodingSoundsSynth() {}

        // Application settings
        const juce::String getApplicationName() override { 
            return ProjectInfo::projectName; 
        }
        
        const juce::String getApplicationVersion() override { 
            return ProjectInfo::versionString; 
        }
        
        bool moreThanOneInstanceAllowed() override { 
            return true; 
        }

        // Called when the application starts
        void initialise(const juce::String& commandLine) override{
            // Create the main window 
            mainWindow.reset(new MainWindow(getApplicationName()));
        }

        // Called when the application is shutting down
        void shutdown() override{
            mainWindow = nullptr;
        }

        // Called when the operating system requests the application to quit
        void systemRequestedQuit() override{
            quit();
        }

    private:
        // Smart pointer to the main window, ensures proper cleanup when destroyed
        std::unique_ptr<MainWindow> mainWindow;
};


// JUCE macro that creates the application entry point and initializes the JUCE framework
START_JUCE_APPLICATION(CodingSoundsSynth)