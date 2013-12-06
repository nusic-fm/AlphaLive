/*
  ==============================================================================

    This file was auto-generated by the Jucer!

    It contains the basic startup code for a Juce application.

  ==============================================================================
*/

//  Created by Liam Meredith-Lacey on 14/09/2011.
//  Copyright 2011 nu desine.
//
//  This file is part of AlphaLive.
//
//  AlphaLive is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License, version 2, 
//  as published by the Free Software Foundation.
//  
//  AlphaLive is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

/*
 Current bug when launching AlphaLive on Mac via .alphalive file when
 "open last project on launch" is set to 'on'.
 
 If you launch AlphaLive on Windows by double-clicking a .alphalive file,
 the commandLine parameter of initialise() is equal to the file path of
 the file. Therefore when it gets down to the "if (selectedFile.getFileExtension() == ".alphalive")"
 statement it will load that project into AlphaLive.
 
 However, on mac this is handled different - the file path is passsed into
 the application using the anotherInstanceStarted() function which is called
 after initialise(). Therfore When the same is done on Mac, as the commandLine 
 parameter of initialise() does not equal the file path, when it gets down to
 the if/else statement it will launch the last opened project first,
 and then when it calls anotherInstanceStarted() afterwards and tries to load the clicked
 project it will ask the user if they want to save the last project first, which
 is not what we want at launch.
 
 This seems like a bug in JUCE to me (why would JUCE puropsely handle this
 different on different OS's, plus otherwise what would the commandLine parameter
 of initialise() be used for?), but until this is I need to prevent this problem myself.
 
 To overcome this problem I am using two int variables - one that gets a time
 value at the end of the initialise() function and another within the anotherInstanceStarted()
 function. If these two time values are less than a 1000ms apart, it means that
 anotherInstanceStarted() was most probably called at launch, and in that case
 we don't want to ask the user if they would like to save first. Else, the function has
 been called after the initial launch, so it is ok to show this alert to the user.
 There is still a small annoyance in this method though, in that you will
 breifly see the last project opened up before it opens the new project.
 
 //NEW OBSERVATION 8/3/13 - 
 //The if/else statement of init() causes an assertion fail on OS X 10.8. Maybe
 //the bug only applies to OSX 10.6/10.7 version and below? Worth investigating!
 
 */

#include "../../JuceLibraryCode/JuceHeader.h"
#include "Common.h"
#include "MainWindow.h"
#include "../Functionality Classes/AlphaLiveEngine.h"
#include "../File and Settings/AppSettings.h"
#include "../File and Settings/AppDocumentState.h"
#include "../GUI Classes/Initial Loading Window/LoadingWindow.h"
#include "../File and Settings/StoredSettings.h"
#include "MainMenuModel.h"
#include "../GUI Classes/Binary Data/BinaryDataNew.h"

//==============================================================================
class AlphaSoftApplication  :   public JUCEApplication
{
public:
    //==============================================================================
    AlphaSoftApplication()
    {
    }

    ~AlphaSoftApplication()
    {
    }

    //==============================================================================
    void initialise (const String& commandLine)
    {
        SplashScreen* splash = new SplashScreen(String::empty,
                                                ImageFileFormat::loadFrom(BinaryDataNew::loading_png, BinaryDataNew::loading_pngSize),
                                                false);
        
        
        // Create temporary loading window. think about how to implement a realtime progress bar in here!
        //loadingWindow = new LoadingWindow();
        
         #if JUCE_MAC || DOXYGEN
         std::cout << "Running on Mac..." << std::endl;
         #endif
         
         #if JUCE_WINDOWS || DOXYGEN
         std::cout << "Running on Windows..." <<std::endl;
         #endif
        
        #if JUCE_LINUX || DOXYGEN
        std::cout << "Running on Linux..." << std::endl;
        #endif
        
        //Call this function incase it is the first time the app is run on a computer.
        //This function also calls the function that installs the basic demo project if needed.
        StoredSettings::getInstance()->setDefaultValues();
        
        commandManager = new ApplicationCommandManager();
        commandManager->registerAllCommandsForTarget (this);
   
        //create a single global instance of AppSettings
        p = AppSettings::Instance();
        
        //all functionality is done within this class!
        alphaLiveEngine = new AlphaLiveEngine();
        AppSettings::Instance()->setAlphaLiveEngineRef(alphaLiveEngine);
        
        //should this be created here? If not, where?
        appDocumentState = new AppDocumentState();
        
        //set menu bar
        menuModel = new MainMenuModel(*appDocumentState);
        
        //create main app window
        mainWindow = new MainAppWindow(*alphaLiveEngine, *appDocumentState, menuModel);
        
        appDocumentState->setMainAppWindowRef (mainWindow);
        
        #if JUCE_MAC
        PopupMenu menu;
        menu.addCommandItem (commandManager, CommandIDs::About);
        menu.addSeparator();
        menu.addCommandItem (commandManager, CommandIDs::Preferences);
            
        MenuBarModel::setMacMainMenu (menuModel, &menu);
        #endif
        
		//==== open any requested files/projects ====
		//See the comment at the top of this .cpp file for current issues here
        
        File selectedFile;
        
        // Without setting selectedFile within the below if statement an assertion failure
        // is caused on OS X 10.8 when running in debug mode as commandLine will
        // equal or contain "-NSDocumentRevisionsDebugMode YES"
        if (File::isAbsolutePath(commandLine.unquoted()))
            selectedFile = commandLine.unquoted();
        
        //check to see if the clicked file is a .alphalive file
        if (selectedFile.getFileExtension() == ".alphalive")
        {
            //load selected file
            appDocumentState->loadProject(false, selectedFile);   
        }
        else if (StoredSettings::getInstance()->launchTask == 2)
        {
            //Open the last project...
            if (StoredSettings::getInstance()->recentFiles.getFile(0) != File::nonexistent)
                appDocumentState->loadProject(false, StoredSettings::getInstance()->recentFiles.getFile(0));
        }
        
        //Tell the HidComms class that it can start recieving and processing pad and elite control reports.
        alphaLiveEngine->setAppHasInitialised();
        
        //set window visible here once everything has initilised and been displayed
        mainWindow->setVisible(true);
        //delete loading window now as everything will be loaded at this point
        //loadingWindow = 0;
        splash->deleteAfterDelay (RelativeTime::seconds (0), false);
        
        //If auto-check is on, check for software updates
        bool isUpdating = false;
        
        if (StoredSettings::getInstance()->autoCheckUpdates == 2)
        {
            //This needs to be called asyncronously instead, otherwise it will
            //interfere with anotherInstanceStarted() if called at launch and
            //not load up the opened project
            isUpdating = mainWindow->getMainComponent()->updateSoftware(true);
        }
        
        //If not currently updating software, check for firmware updates
        if (isUpdating == false)
            alphaLiveEngine->updateFirmware();
        
        initTime = Time::getMillisecondCounter();
    }

    
    void shutdown()
    {
        // Do your application's shutdown code here..
        #if JUCE_MAC
        MenuBarModel::setMacMainMenu (nullptr);
        #endif
        menuModel = nullptr;
        delete menuModel;
        
        mainWindow = 0;
        delete alphaLiveEngine;
        
        delete appDocumentState;
        //change the way the Singleton object gets destroyed?
        delete p;
        
        commandManager = nullptr;
        StoredSettings::deleteInstance();
        AlphaTheme::deleteInstance();
    }

    //==============================================================================
    void systemRequestedQuit()
    {
        bool shouldQuit = AlertWindow::showOkCancelBox(AlertWindow::WarningIcon, translate("Quit"), translate("Are you sure you want to quit?"));
        
        if (shouldQuit == true)
        {
            /*
            bool shouldSave = AlertWindow::showOkCancelBox(AlertWindow::WarningIcon, "Save Project", "Would you like to save?", "Yes", "No");
            
            if (shouldSave == true)
            {
                //save
                appDocumentState->savePerformance(0);
            }
             */
            
            //If the option is enabled in the prefs, the project will be automatically cleaned
            //on app shutdown.
            //There is a logical check which compares the number files in the audio files dircetory currently
            //to the number when the project was opened or last cleaned. If they don't match (which most
            //likely means new audio files were added), the project is cleaned.
            if (StoredSettings::getInstance()->cleanOnClose == 2)
            {
                int numOfFilesAtClose = File::getCurrentWorkingDirectory().getNumberOfChildFiles(2);
                
                if (numOfFilesAtClose != appDocumentState->getNumOfFilesAtStart())
                {
                    appDocumentState->cleanUpProject (true);
                }
            }
            
            quit();
            
        }
        
    }

    //==============================================================================
    const String getApplicationName()
    {
        return "AlphaLive";
    }

    const String getApplicationVersion()
    {
        return ProjectInfo::versionString;
    }

    bool moreThanOneInstanceAllowed()
    {
        return false;
    }

    void anotherInstanceStarted (const String& commandLine)
    {
        //This function gets called anytime the user tries to open a second instance of AlphaLive
        //or when a .alphalive file is opened. commandLine is the file path of the clicked file
        
        File selectedFile(commandLine.unquoted());
        
        //check to see if the clicked file is a .alphalive file
        if (selectedFile.getFileExtension() == ".alphalive")
        {
            //Figure out if this function was called from app launch on mac,
            //by comparing the two time values.
            //If so, don't ask if you want to save the last project.
            projectLaunchTime = Time::getMillisecondCounter();
            
            if (projectLaunchTime - initTime < 1000)
            {
                //load selected file, not showing 'save?' alert window
                appDocumentState->loadProject(false, selectedFile, false);
            }
            else
            {
                //load selected file
                appDocumentState->loadProject(false, selectedFile);
            }
        }
        
    }
    
    
    //==============================================================================
    //removed the nested MainMenuModel class and put it in a seperate file,
    //due to BAD_ACCESS when trying to call AppDocumentState::loadPerformance
    //from it
    //==============================================================================
    
    
    //==============================================================================
    void getAllCommands (Array <CommandID>& commands)
    {
        //JUCEApplication::getAllCommands (commands);
        
        const CommandID ids[] = 
        {	
            CommandIDs::New,
            CommandIDs::Open,
            CommandIDs::Save,
            CommandIDs::SaveAs,
            CommandIDs::CleanUpProject,
            StandardApplicationCommandIDs::quit,
            CommandIDs::EnableLed,
            CommandIDs::EnableLedPressure,
            CommandIDs::EnableLedClock,
            CommandIDs::EnableLedMidiMode
        };
        
        commands.addArray (ids, numElementsInArray (ids));
    }
    
    void getCommandInfo (CommandID commandID, ApplicationCommandInfo& result)
    {
        const int cmd = ModifierKeys::commandModifier;
        const int shift = ModifierKeys::shiftModifier;
        const int alt = ModifierKeys::altModifier;
        
        //within 'setInfo()' below, the name sets the String that appears in the Menu bar,
        //and the description sets what would appear in the tooltip if the command is set to a button
        //and the tooltip parameter is set to 'true'
        
        if(commandID == CommandIDs::New)
        {
            result.setInfo (translate("New..."),
                            "Creates a new project.",
                            CommandCategories::FileCommands, 0);
            result.defaultKeypresses.add (KeyPress ('n', cmd, 0));
        }
        else if(commandID == CommandIDs::Open)
        {
            result.setInfo (translate("Open..."),
                            "Opens an AlphaLive Project file (.alphalive).",
                            CommandCategories::FileCommands, 0);
            result.defaultKeypresses.add (KeyPress ('o', cmd, 0));
        }
        else if(commandID == CommandIDs::Save)
        {
            result.setInfo (translate("Save"),
                            "Saves the current settings to an AlphaLive Project file (.alphalive).",
                            CommandCategories::FileCommands, 0);
            result.defaultKeypresses.add (KeyPress ('s', cmd, 0));
        }
        else if(commandID == CommandIDs::SaveAs)
        {
            result.setInfo (translate("Save As..."),
                            "Saves the current settings to an AlphaLive Project file (.alphalive).",
                            CommandCategories::FileCommands, 0);
            result.defaultKeypresses.add (KeyPress ('s', cmd|shift, 0));
        }
        else if (commandID == CommandIDs::CleanUpProject)
        {
            result.setInfo (translate("Clean Up Project..."),
                            "Removes any unused audio files from the projects 'Audio Files' directory.",
                            CommandCategories::FileCommands, 0);
        }
        else if (commandID == StandardApplicationCommandIDs::quit)
        {
            result.setInfo (translate("Quit"),
                            "Quits the application",
                            CommandCategories::OtherCommands, 0);
            
            result.defaultKeypresses.add (KeyPress ('q', cmd, 0));
        }
        else if (commandID == CommandIDs::EnableLed)
        {
            result.setInfo (translate("Enable LED"),
                            "Sets the status of the LED in the AlphaSphere",
                            CommandCategories::HardwareCommands, 0);
            result.defaultKeypresses.add (KeyPress ('l', cmd|alt, 0));
            
            result.setTicked (AppSettings::Instance()->getHardwareLedStatus());
            result.setActive (!(AppSettings::Instance()->getHardwareLedMode()));
            //result.setActive (alphaLiveEngine->getDeviceStatus() != 0); // << not currently working
        }
        else if (commandID == CommandIDs::EnableLedPressure)
        {
            result.setInfo (translate("Enable LED Pressure Interaction"),
                            "Sets the status of the LED pressure interaction",
                            CommandCategories::HardwareCommands, 0);
            result.defaultKeypresses.add (KeyPress ('p', cmd|alt, 0));
            
            result.setTicked (AppSettings::Instance()->getHardwareLedPressureStatus());
            result.setActive (AppSettings::Instance()->getHardwareLedStatus() &&
                             !(AppSettings::Instance()->getHardwareLedMode()) /*&&
                             alphaLiveEngine->getDeviceStatus() != 0*/); // << not currently working
        }
        else if (commandID == CommandIDs::EnableLedClock)
        {
            result.setInfo (translate("Enable LED Clock Interaction"),
                            "Sets the status of the LED clock interaction",
                            CommandCategories::HardwareCommands, 0);
            result.defaultKeypresses.add (KeyPress ('c', cmd|alt, 0));
            
            result.setTicked (AppSettings::Instance()->getHardwareLedClockStatus());
            result.setActive (AppSettings::Instance()->getHardwareLedStatus() &&
                             !(AppSettings::Instance()->getHardwareLedMode()) /*&&
                             alphaLiveEngine->getDeviceStatus() != 0*/); // << not currently working
        }
        else if (commandID == CommandIDs::EnableLedMidiMode)
        {
            result.setInfo (translate("Enable LED MIDI CC Control"),
                            "Sets the status of the LED MIDI CC Control mode",
                            CommandCategories::HardwareCommands, 0);
            result.defaultKeypresses.add (KeyPress ('m', cmd|alt, 0));
            
            result.setTicked (AppSettings::Instance()->getHardwareLedMode());
            //result.setActive (alphaLiveEngine->getDeviceStatus() != 0); // << not currently working
        }
        
    }
    
    bool perform (const InvocationInfo& info)
    {
        if(info.commandID == CommandIDs::New)
        {
            appDocumentState->createNewProject();
            return true;
        }
        else if(info.commandID == CommandIDs::Open)
        {
            appDocumentState->loadProject(true);
            return true;
        }
        
        else if(info.commandID == CommandIDs::Save)
        {
            appDocumentState->saveProject();
            return true;
        }
        
        else if(info.commandID == CommandIDs::SaveAs)
        {
            appDocumentState->saveProjectAs();
            return true;
        }
        
        else if(info.commandID == CommandIDs::CleanUpProject)
        {
            appDocumentState->cleanUpProject (false);
            return true;
        }
        
        else if(info.commandID == CommandIDs::EnableLed)
        {
            uint8 status;
            
            if (AppSettings::Instance()->getHardwareLedStatus() == 1)
            {
                //dissable LED
                status = 0;
            }
            else
            {
                //enable LED
                status = 1;
            }
            
            AppSettings::Instance()->setHardwareLedStatus(status);
            //update the menu bar items status if this was called not from the menu bar
            commandManager->commandStatusChanged();
            return true;
        }
        
        else if(info.commandID == CommandIDs::EnableLedPressure)
        {
            int status;
            
            if (AppSettings::Instance()->getHardwareLedPressureStatus())
            {
                //dissable LED pressure interaction
                status = 0;
            }
            else
            {
                //enable LED presure interaction
                status = 1;
            }
            
            AppSettings::Instance()->setHardwareLedPressureStatus(status);
            //update the menu bar items status if this was called not from the menu bar
            commandManager->commandStatusChanged();
            return true;
        }
        
        else if(info.commandID == CommandIDs::EnableLedClock)
        {
            int status;
            
            if (AppSettings::Instance()->getHardwareLedClockStatus())
            {
                //dissable LED clock interaction
                status = 0;
            }
            else
            {
                //enable LED clock interaction
                status = 1;
            }
            
            AppSettings::Instance()->setHardwareLedClockStatus(status);
            //update the menu bar items status if this was called not from the menu bar
            commandManager->commandStatusChanged();
            return true;
        }
        
        else if(info.commandID == CommandIDs::EnableLedMidiMode)
        {
            int mode;
            
            if (AppSettings::Instance()->getHardwareLedMode())
            {
                //set to normal mode
                mode = 0;
            }
            else
            {
                //set to MIDI controlled mode
                mode = 1;
            }
            
            AppSettings::Instance()->setHardwareLedMode(mode);
            //update the menu bar items status if this was called not from the menu bar
            commandManager->commandStatusChanged();
            return true;
        }
        
        else if (info.commandID == StandardApplicationCommandIDs::quit)
        {
            systemRequestedQuit();
            return true;
        }

        return false;
    }
    
    
   
    //==============================================================================
private:
    ScopedPointer <MainAppWindow> mainWindow;
    ScopedPointer<LoadingWindow> loadingWindow;
    AlphaLiveEngine *alphaLiveEngine;
    AppSettings *p;
    AppDocumentState *appDocumentState;
    
    //ScopedPointer<MainMenuModel> menuModel;
    MainMenuModel *menuModel;
    
    //these are used to hackily prevent the bug where
    //launching AlphaLive using a .alphalive file while
    //'open last project on launch' is on causes the
    //new project to not open automatically.
    //See top comment for more details.
    int initTime, projectLaunchTime;
};

//==============================================================================
// This macro generates the main() routine that starts the app.
START_JUCE_APPLICATION(AlphaSoftApplication)

