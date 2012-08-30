//
//  AppDocumentState.cpp
//  AlphaSoft
//
//  Created by Liam Meredith-Lacey on 07/11/2011.
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

#include "AppDocumentState.h"
#include "AppSettings.h"
#include "../Functionality Classes/Sequencer Mode/SequencerValues.h"
#include "../Application/MainWindow.h"
#include "StoredSettings.h"

#define PAD_SETTINGS AppSettings::Instance()->padSettings[i]
#define PAD_SETTINGS_pad AppSettings::Instance()->padSettings[currentlySelectedPad]
#define PAD_SETTINGS_pads AppSettings::Instance()->padSettings[padNum]

AppDocumentState::AppDocumentState()
{
    for (int i = 0; i <= NO_OF_SCENES-1; i++)
    {
        sceneData.insert(i, new XmlElement("SCENE_" + String(i)));
    }
    
    projectData = new XmlElement("PROJECT_SETTINGS");
    
    guiUpdateFlag = 0;
    sceneToUpdate = 0;
    sceneStatus = 0;
    
    currentProjectFile = File::nonexistent;
    
    mainAppWindowRef = nullptr;
    
    shouldDisplayAlertWindow = true;
    currentlySelectedScene = 0;
}


AppDocumentState::~AppDocumentState()
{

}

void AppDocumentState::setMainAppWindowRef (MainAppWindow *ref)
{
    mainAppWindowRef = ref;
}

void AppDocumentState::savePadSettings (int padNumber, XmlElement *padData)
{
    int i = padNumber;
    
    //put data into this the temp XmlElement
    padData->setAttribute("mode", PAD_SETTINGS->getMode());
    padData->setAttribute("pressureSensitivityMode", PAD_SETTINGS->getPressureSensitivityMode());
    padData->setAttribute("exclusiveMode", PAD_SETTINGS->getExclusiveMode());
    padData->setAttribute("exclusiveGroup", PAD_SETTINGS->getExclusiveGroup());
    padData->setAttribute("quantizeMode", PAD_SETTINGS->getQuantizeMode());
    
    //only save whats necessary
    if (PAD_SETTINGS->getMode() == 1) //midi mode
    {
        padData->setAttribute("midiNote", PAD_SETTINGS->getMidiNote());
        padData->setAttribute("midiVelocity", PAD_SETTINGS->getMidiVelocity());
        padData->setAttribute("midiChannel", PAD_SETTINGS->getMidiChannel());
        padData->setAttribute("midiMinPressureRange", PAD_SETTINGS->getMidiMinPressureRange());
        padData->setAttribute("midiMaxPressureRange", PAD_SETTINGS->getMidiMaxPressureRange());
        padData->setAttribute("midiPressureMode", PAD_SETTINGS->getMidiPressureMode());
        padData->setAttribute("midiTriggerMode", PAD_SETTINGS->getMidiTriggerMode());
        padData->setAttribute("midiIndestructible", PAD_SETTINGS->getMidiIndestructible());
        padData->setAttribute("midiSticky", PAD_SETTINGS->getMidiSticky());
        padData->setAttribute("midiPressureStatus", PAD_SETTINGS->getMidiPressureStatus());
        padData->setAttribute("midiNoteStatus", PAD_SETTINGS->getMidiNoteStatus());
        padData->setAttribute("midiCcController", PAD_SETTINGS->getMidiCcController());
    }
    else if (PAD_SETTINGS->getMode() == 2) //sampler mode
    {
        
        if (AppSettings::Instance()->getCopyExternalFiles() == true)
        {
            //if project currently allows audio files to be copied to project, only save the file name
            padData->setAttribute("samplerAudioFilePath", PAD_SETTINGS->getSamplerAudioFilePath().getFileName());
            
        }
        else if (AppSettings::Instance()->getCopyExternalFiles() == false)
        {
            //else save the full pathname
            padData->setAttribute("samplerAudioFilePath", PAD_SETTINGS->getSamplerAudioFilePath().getFullPathName());
        }
        
        padData->setAttribute("samplerTriggerMode", PAD_SETTINGS->getSamplerTriggerMode());
        padData->setAttribute("samplerShouldLoop", PAD_SETTINGS->getSamplerShouldLoop());
        padData->setAttribute("samplerIndestructible", PAD_SETTINGS->getSamplerIndestructible());
        padData->setAttribute("samplerShouldFinishLoop", PAD_SETTINGS->getSamplerShouldFinishLoop());
        padData->setAttribute("samplerSticky", PAD_SETTINGS->getSamplerSticky());
        padData->setAttribute("samplerEffect", PAD_SETTINGS->getSamplerEffect());
        padData->setAttribute("samplerPan", PAD_SETTINGS->getSamplerPan());
        padData->setAttribute("samplerGain", PAD_SETTINGS->getSamplerGain());
        
        if (PAD_SETTINGS->getSamplerEffect() == 1) //Gain and Pan
        {
            padData->setAttribute("samplerFxGainPanGain", PAD_SETTINGS->getSamplerFxGainPanGain());
            padData->setAttribute("samplerFxGainPanPan", PAD_SETTINGS->getSamplerFxGainPanPan());
            padData->setAttribute("samplerFxGainPanAlphaTouch", PAD_SETTINGS->getSamplerFxGainPanAlphaTouch());
            padData->setAttribute("samplerFxGainPanAtReverse", PAD_SETTINGS->getSamplerFxGainPanAtReverse());
            padData->setAttribute("samplerFxGainPanAtIntensity", PAD_SETTINGS->getSamplerFxGainPanAtIntensity());
        }
        else if (PAD_SETTINGS->getSamplerEffect() == 2) //LPF
        {
            padData->setAttribute("samplerFxLpfMix", PAD_SETTINGS->getSamplerFxLpfMix());
            padData->setAttribute("samplerFxLpfFreq", PAD_SETTINGS->getSamplerFxLpfFreq());
            padData->setAttribute("samplerFxLpfBandwidth", PAD_SETTINGS->getSamplerFxLpfBandwidth());
            padData->setAttribute("samplerFxLpfAlphaTouch", PAD_SETTINGS->getSamplerFxLpfAlphaTouch());
            padData->setAttribute("samplerFxLpfAtReverse", PAD_SETTINGS->getSamplerFxLpfAtReverse());
            padData->setAttribute("samplerFxLpfAtIntensity", PAD_SETTINGS->getSamplerFxLpfAtIntensity());
        }
        else if (PAD_SETTINGS->getSamplerEffect() == 3) //HPF
        {
            padData->setAttribute("samplerFxHpfMix", PAD_SETTINGS->getSamplerFxHpfMix());
            padData->setAttribute("samplerFxHpfFreq", PAD_SETTINGS->getSamplerFxHpfFreq());
            padData->setAttribute("samplerFxHpfBandwidth", PAD_SETTINGS->getSamplerFxHpfBandwidth());
            padData->setAttribute("samplerFxHpfAlphaTouch", PAD_SETTINGS->getSamplerFxHpfAlphaTouch());
            padData->setAttribute("samplerFxHpfAtReverse", PAD_SETTINGS->getSamplerFxHpfAtReverse());
            padData->setAttribute("samplerFxHpfAtIntensity", PAD_SETTINGS->getSamplerFxHpfAtIntensity());
        }
        else if (PAD_SETTINGS->getSamplerEffect() == 4) //BPF
        {
            padData->setAttribute("samplerFxBpfMix", PAD_SETTINGS->getSamplerFxBpfMix());
            padData->setAttribute("samplerFxBpfFreq", PAD_SETTINGS->getSamplerFxBpfFreq());
            padData->setAttribute("samplerFxBpfBandwidth", PAD_SETTINGS->getSamplerFxBpfBandwidth());
            padData->setAttribute("samplerFxBpfAlphaTouch", PAD_SETTINGS->getSamplerFxBpfAlphaTouch());
            padData->setAttribute("samplerFxBpfAtReverse", PAD_SETTINGS->getSamplerFxBpfAtReverse());
            padData->setAttribute("samplerFxBpfAtIntensity", PAD_SETTINGS->getSamplerFxBpfAtIntensity());
        }
        else if (PAD_SETTINGS->getSamplerEffect() == 7) //Delay
        {
            padData->setAttribute("samplerFxDelayMix", PAD_SETTINGS->getSamplerFxDelayMix());
            padData->setAttribute("samplerFxDelayTime", PAD_SETTINGS->getSamplerFxDelayTime());
            padData->setAttribute("samplerFxDelayFeedback", PAD_SETTINGS->getSamplerFxDelayFeedback());
            padData->setAttribute("samplerFxDelayLpfFreq", PAD_SETTINGS->getSamplerFxDelayLpfFreq());
            padData->setAttribute("samplerFxDelayHpfFreq", PAD_SETTINGS->getSamplerFxDelayHpfFreq());
            padData->setAttribute("samplerFxDelaySync", PAD_SETTINGS->getSamplerFxDelaySync());
            padData->setAttribute("samplerFxDelayTimeMenu", PAD_SETTINGS->getSamplerFxDelayTimeMenu());
            padData->setAttribute("samplerFxDelayAlphaTouch", PAD_SETTINGS->getSamplerFxDelayAlphaTouch());
            padData->setAttribute("samplerFxDelayAtReverse", PAD_SETTINGS->getSamplerFxDelayAtReverse());
            padData->setAttribute("samplerFxDelayAtIntensity", PAD_SETTINGS->getSamplerFxDelayAtIntensity());
        }
        else if (PAD_SETTINGS->getSamplerEffect() == 8) //Reverb
        {
            padData->setAttribute("samplerFxReverbMix", PAD_SETTINGS->getSamplerFxReverbMix());
            padData->setAttribute("samplerFxReverbRoomSize", PAD_SETTINGS->getSamplerFxReverbRoomSize());
            padData->setAttribute("samplerFxReverbDamping", PAD_SETTINGS->getSamplerFxReverbDamping());
            padData->setAttribute("samplerFxReverbWidth", PAD_SETTINGS->getSamplerFxReverbWidth());
            padData->setAttribute("samplerFxReverbFreezeMode", PAD_SETTINGS->getSamplerFxReverbFreezeMode());
            padData->setAttribute("samplerFxReverbAlphaTouch", PAD_SETTINGS->getSamplerFxReverbAlphaTouch());
            padData->setAttribute("samplerFxReverbAtReverse", PAD_SETTINGS->getSamplerFxReverbAtReverse());
            padData->setAttribute("samplerFxReverbAtIntensity", PAD_SETTINGS->getSamplerFxReverbAtIntensity());
        }
        else if (PAD_SETTINGS->getSamplerEffect() == 9) //Flanger
        {
            padData->setAttribute("samplerFxFlangerMix", PAD_SETTINGS->getSamplerFxFlangerMix());
            padData->setAttribute("samplerFxFlangerRate", PAD_SETTINGS->getSamplerFxFlangerRate());
            padData->setAttribute("samplerFxFlangerFeedback", PAD_SETTINGS->getSamplerFxFlangerFeedback());
            padData->setAttribute("samplerFxFlangerIntensity", PAD_SETTINGS->getSamplerFxFlangerIntensity());
            padData->setAttribute("samplerFxFlangerRateMenu", PAD_SETTINGS->getSamplerFxFlangerRateMenu());
            padData->setAttribute("samplerFxFlangerSync", PAD_SETTINGS->getSamplerFxFlangerSync());
            padData->setAttribute("samplerFxFlangerAlphaTouch", PAD_SETTINGS->getSamplerFxFlangerAlphaTouch());
            padData->setAttribute("samplerFxFlangerAtReverse", PAD_SETTINGS->getSamplerFxFlangerAtReverse());
            padData->setAttribute("samplerFxFlangerAtIntensity", PAD_SETTINGS->getSamplerFxFlangerAtIntensity());
        }
        else if (PAD_SETTINGS->getSamplerEffect() == 10) //Tremolo
        {
            padData->setAttribute("samplerFxTremoloDepth", PAD_SETTINGS->getSamplerFxTremoloDepth());
            padData->setAttribute("samplerFxTremoloRate", PAD_SETTINGS->getSamplerFxTremoloRate());
            padData->setAttribute("samplerFxTremoloShape", PAD_SETTINGS->getSamplerFxTremoloShape());
            padData->setAttribute("samplerFxTremoloSync", PAD_SETTINGS->getSamplerFxTremoloSync());
            padData->setAttribute("samplerFxTremoloRateMenu", PAD_SETTINGS->getSamplerFxTremoloRateMenu());
            padData->setAttribute("samplerFxTremoloAlphaTouch", PAD_SETTINGS->getSamplerFxTremoloAlphaTouch());
            padData->setAttribute("samplerFxTremoloAtReverse", PAD_SETTINGS->getSamplerFxTremoloAtReverse());
            padData->setAttribute("samplerFxTremoloAtIntensity", PAD_SETTINGS->getSamplerFxTremoloAtIntensity());
        }
        
    }
    else if (PAD_SETTINGS->getMode() == 3) //sequencer mode
    {
        padData->setAttribute("sequencerMode", PAD_SETTINGS->getSequencerMode());
        for (int seq = 0; seq <= NO_OF_SEQS-1; seq++)
        {
            padData->setAttribute("newSequencerData"+String(seq), PAD_SETTINGS->getSequencerDataString(seq));
        }
        
        padData->setAttribute("sequencerNumberOfSequences", PAD_SETTINGS->getSequencerNumberOfSequences());
        padData->setAttribute("sequencerTriggerMode", PAD_SETTINGS->getSequencerTriggerMode());
        padData->setAttribute("sequencerShouldLoop", PAD_SETTINGS->getSequencerShouldLoop());
        padData->setAttribute("sequencerIndestructible", PAD_SETTINGS->getSequencerIndestructible());
        padData->setAttribute("sequencerShouldFinishLoop", PAD_SETTINGS->getSequencerShouldFinishLoop());
        padData->setAttribute("sequencerSticky", PAD_SETTINGS->getSequencerSticky());
        padData->setAttribute("sequencerLength", PAD_SETTINGS->getSequencerLength());
        padData->setAttribute("sequencerRelativeTempoMode", PAD_SETTINGS->getSequencerRelativeTempoMode());
        padData->setAttribute("sequencerDynamicMode", PAD_SETTINGS->getSequencerDynamicMode());
        
        if (PAD_SETTINGS->getSequencerMode() == 1) //sequencer midi mode
        {
            for (int row = 0; row <= NO_OF_ROWS-1; row++)
            {
                padData->setAttribute("sequencerMidiNote"+String(row), PAD_SETTINGS->getSequencerMidiNote(row));
            }
            padData->setAttribute("sequencerMidiVelocity", PAD_SETTINGS->getSequencerMidiVelocity());
            padData->setAttribute("sequencerMidiChannel", PAD_SETTINGS->getSequencerMidiChannel());
            padData->setAttribute("sequencerMidiNoteLength", PAD_SETTINGS->getSequencerMidiNoteLength());
            padData->setAttribute("sequencerMidiMinPressureRange", PAD_SETTINGS->getSequencerMidiMinPressureRange());
            padData->setAttribute("sequencerMidiMaxPressureRange", PAD_SETTINGS->getSequencerMidiMaxPressureRange());
            padData->setAttribute("sequencerMidiPressureMode", PAD_SETTINGS->getSequencerMidiPressureMode());
            padData->setAttribute("sequencerMidiPressureStatus", PAD_SETTINGS->getSequencerMidiPressureStatus());
            padData->setAttribute("sequencerMidiCcController", PAD_SETTINGS->getSequencerMidiCcController());
            
        }
        else if (PAD_SETTINGS->getSequencerMode() == 2) //sequencer samples mode
        {
            for (int row = 0; row <= NO_OF_ROWS-1; row++)
            {
                if (AppSettings::Instance()->getCopyExternalFiles() == true)
                {
                    //if project currently allows external audio files to be copied to project, only save the file name
                    padData->setAttribute("sequencerSamplesAudioFilePath"+String(row), PAD_SETTINGS->getSequencerSamplesAudioFilePath(row).getFileName());
                }
                else
                {
                    //else save the full pathname
                    padData->setAttribute("sequencerSamplesAudioFilePath"+String(row), PAD_SETTINGS->getSequencerSamplesAudioFilePath(row).getFullPathName());
                }
            }
            padData->setAttribute("sequencerPan", PAD_SETTINGS->getSequencerPan());
            padData->setAttribute("sequencerGain", PAD_SETTINGS->getSequencerGain());
        }
    }
    
    else if (PAD_SETTINGS->getMode() == 4) //controller mode
    {
        padData->setAttribute("controllerControl", PAD_SETTINGS->getControllerControl());
        padData->setAttribute("controllerSceneNumber", PAD_SETTINGS->getControllerSceneNumber());
        padData->setAttribute("controllerOscIpAddress", PAD_SETTINGS->getControllerOscIpAddress());
        padData->setAttribute("controllerOscPortNumber", PAD_SETTINGS->getControllerOscPort());
        padData->setAttribute("controllerMidiProgramChangeNumber", PAD_SETTINGS->getControllerMidiProgramChangeNumber());
        padData->setAttribute("controllerMidiProgramChangeChannel", PAD_SETTINGS->getControllerMidiProgramChangeChannel());
    }
}







void AppDocumentState::loadPadSettings (int padNumber, XmlElement *padData)
{
    int i = padNumber;
    
    if (padData->hasAttribute("mode")) //WHICH IT SHOULD ALWAYS HAVE!
        PAD_SETTINGS->setMode(padData->getIntAttribute("mode"));
    if (padData->hasAttribute("pressureSensitivityMode"))
        PAD_SETTINGS->setPressureSensitivityMode(padData->getIntAttribute("pressureSensitivityMode"));
    
    if (padData->hasAttribute("exclusiveMode"))
        PAD_SETTINGS->setExclusiveMode(padData->getIntAttribute("exclusiveMode"));
    if (padData->hasAttribute("exclusiveGroup"))
        PAD_SETTINGS->setExclusiveGroup(padData->getIntAttribute("exclusiveGroup"));
    if (padData->hasAttribute("quantizeMode"))
        PAD_SETTINGS->setQuantizeMode(padData->getIntAttribute("quantizeMode"));
    
    //only load needed data to reduce loading times and CPU usage, plus
    //can not load settings into seq and sampler modes where the pads player objects don't exist (yet)
    
    //midi mode
    if (PAD_SETTINGS->getMode() == 1)
    {
        if (padData->hasAttribute("midiNote"))
            PAD_SETTINGS->setMidiNote(padData->getIntAttribute("midiNote"));
        if (padData->hasAttribute("midiVelocity"))
            PAD_SETTINGS->setMidiVelocity(padData->getIntAttribute("midiVelocity"));
        if (padData->hasAttribute("midiChannel"))
            PAD_SETTINGS->setMidiChannel(padData->getIntAttribute("midiChannel"));
        if (padData->hasAttribute("midiMinPressureRange"))
            PAD_SETTINGS->setMidiMinPressureRange(padData->getIntAttribute("midiMinPressureRange"));
        if (padData->hasAttribute("midiMaxPressureRange"))
            PAD_SETTINGS->setMidiMaxPressureRange(padData->getIntAttribute("midiMaxPressureRange"));
        if (padData->hasAttribute("midiPressureMode"))
            PAD_SETTINGS->setMidiPressureMode(padData->getIntAttribute("midiPressureMode"));
        if (padData->hasAttribute("midiTriggerMode"))
            PAD_SETTINGS->setMidiTriggerMode(padData->getIntAttribute("midiTriggerMode"));
        if (padData->hasAttribute("midiIndestructible"))
            PAD_SETTINGS->setMidiIndestructible(padData->getIntAttribute("midiIndestructible"));
        if (padData->hasAttribute("midiSticky"))
            PAD_SETTINGS->setMidiSticky(padData->getIntAttribute("midiSticky"));
        if (padData->hasAttribute("midiPressureStatus"))
            PAD_SETTINGS->setMidiPressureStatus(padData->getBoolAttribute("midiPressureStatus"));
        if (padData->hasAttribute("midiNoteStatus"))
            PAD_SETTINGS->setMidiNoteStatus(padData->getBoolAttribute("midiNoteStatus"));
        if (padData->hasAttribute("midiCcController"))
            PAD_SETTINGS->setMidiCcController(padData->getIntAttribute("midiCcController"));
        
    }
    
    //sampler mode
    else if (PAD_SETTINGS->getMode() == 2)
    {
        File newFile;
        String newFileString(String::empty);
        
        if (padData->hasAttribute("samplerAudioFilePath"))
        {
            newFileString = padData->getStringAttribute("samplerAudioFilePath");
            
            if (newFileString != String::empty) //to prevent trying to load in a file if there's nothing to load
            {
                if (File::isAbsolutePath(newFileString) == false)
                {
                    //check if the saved audio file path is just the file name
                    //if so, get it from the working directory and apply the full pathname to it
                    newFile = File::getCurrentWorkingDirectory().getFullPathName() + File::separatorString + newFileString;
                }
                else if (File::isAbsolutePath(newFileString) == true)
                {
                    //else, it should be the full path name already
                    newFile = newFileString;
                }
                
                PAD_SETTINGS->setSamplerAudioFilePath(newFile);
            }
            else
                PAD_SETTINGS->setSamplerAudioFilePath(File::nonexistent);
        }
        
        if (padData->hasAttribute("samplerTriggerMode"))
            PAD_SETTINGS->setSamplerTriggerMode(padData->getIntAttribute("samplerTriggerMode"));
        if (padData->hasAttribute("samplerShouldLoop"))
            PAD_SETTINGS->setSamplerShouldLoop(padData->getIntAttribute("samplerShouldLoop"));
        if (padData->hasAttribute("samplerIndestructible"))
            PAD_SETTINGS->setSamplerIndestructible(padData->getIntAttribute("samplerIndestructible"));
        if (padData->hasAttribute("samplerShouldFinishLoop"))
            PAD_SETTINGS->setSamplerShouldFinishLoop(padData->getIntAttribute("samplerShouldFinishLoop"));
        if (padData->hasAttribute("samplerSticky"))
            PAD_SETTINGS->setSamplerSticky(padData->getIntAttribute("samplerSticky"));
        if (padData->hasAttribute("samplerEffect"))
            PAD_SETTINGS->setSamplerEffect(padData->getIntAttribute("samplerEffect"));
        if (padData->hasAttribute("samplerPan"))
            PAD_SETTINGS->setSamplerPan(padData->getDoubleAttribute("samplerPan"));
        if (padData->hasAttribute("samplerGain"))
            PAD_SETTINGS->setSamplerGain(padData->getDoubleAttribute("samplerGain"));
        
        if (padData->hasAttribute("samplerEffect"))
        {
            if (PAD_SETTINGS->getSamplerEffect() == 1) //Gain and Pan
            {
                PAD_SETTINGS->setSamplerFxGainPanGain(padData->getDoubleAttribute("samplerFxGainPanGain"));
                PAD_SETTINGS->setSamplerFxGainPanPan(padData->getDoubleAttribute("samplerFxGainPanPan"));
                PAD_SETTINGS->setSamplerFxGainPanAlphaTouch(padData->getIntAttribute("samplerFxGainPanAlphaTouch"));
                PAD_SETTINGS->setSamplerFxGainPanAtReverse(padData->getIntAttribute("samplerFxGainPanAtReverse"));
                PAD_SETTINGS->setSamplerFxGainPanAtIntensity(padData->getDoubleAttribute("samplerFxGainPanAtIntensity"));
            }
            else if (PAD_SETTINGS->getSamplerEffect() == 2) //LPF
            {
                PAD_SETTINGS->setSamplerFxLpfMix(padData->getDoubleAttribute("samplerFxLpfMix"));
                PAD_SETTINGS->setSamplerFxLpfFreq(padData->getDoubleAttribute("samplerFxLpfFreq"));
                PAD_SETTINGS->setSamplerFxLpfBandwidth(padData->getDoubleAttribute("samplerFxLpfBandwidth"));
                PAD_SETTINGS->setSamplerFxLpfAlphaTouch(padData->getIntAttribute("samplerFxLpfAlphaTouch"));
                PAD_SETTINGS->setSamplerFxLpfAtReverse(padData->getIntAttribute("samplerFxLpfAtReverse"));
                PAD_SETTINGS->setSamplerFxLpfAtIntensity(padData->getDoubleAttribute("samplerFxLpfAtIntensity"));
                
            }
            else if (PAD_SETTINGS->getSamplerEffect() == 3) //HPF
            {
                PAD_SETTINGS->setSamplerFxHpfMix(padData->getDoubleAttribute("samplerFxHpfMix"));
                PAD_SETTINGS->setSamplerFxHpfFreq(padData->getDoubleAttribute("samplerFxHpfFreq"));
                PAD_SETTINGS->setSamplerFxHpfBandwidth(padData->getDoubleAttribute("samplerFxHpfBandwidth"));
                PAD_SETTINGS->setSamplerFxHpfAlphaTouch(padData->getIntAttribute("samplerFxHpfAlphaTouch"));
                PAD_SETTINGS->setSamplerFxHpfAtReverse(padData->getIntAttribute("samplerFxHpfAtReverse"));
                PAD_SETTINGS->setSamplerFxHpfAtIntensity(padData->getDoubleAttribute("samplerFxHpfAtIntensity"));
            }
            else if (PAD_SETTINGS->getSamplerEffect() == 4) //BPF
            {
                PAD_SETTINGS->setSamplerFxBpfMix(padData->getDoubleAttribute("samplerFxBpfMix"));
                PAD_SETTINGS->setSamplerFxBpfFreq(padData->getDoubleAttribute("samplerFxBpfFreq"));
                PAD_SETTINGS->setSamplerFxBpfBandwidth(padData->getDoubleAttribute("samplerFxBpfBandwidth"));
                PAD_SETTINGS->setSamplerFxBpfAlphaTouch(padData->getIntAttribute("samplerFxBpfAlphaTouch"));
                PAD_SETTINGS->setSamplerFxBpfAtReverse(padData->getIntAttribute("samplerFxBpfAtReverse"));
                PAD_SETTINGS->setSamplerFxBpfAtIntensity(padData->getDoubleAttribute("samplerFxBpfAtIntensity"));
            }
            else if (PAD_SETTINGS->getSamplerEffect() == 7) //Delay
            {
                PAD_SETTINGS->setSamplerFxDelayMix(padData->getDoubleAttribute("samplerFxDelayMix"));
                PAD_SETTINGS->setSamplerFxDelayTime(padData->getDoubleAttribute("samplerFxDelayTime"));
                PAD_SETTINGS->setSamplerFxDelayFeedback(padData->getDoubleAttribute("samplerFxDelayFeedback"));
                PAD_SETTINGS->setSamplerFxDelayLpfFreq(padData->getDoubleAttribute("samplerFxDelayLpfFreq"));
                PAD_SETTINGS->setSamplerFxDelayHpfFreq(padData->getDoubleAttribute("samplerFxDelayHpfFreq"));
                PAD_SETTINGS->setSamplerFxDelaySync(padData->getIntAttribute("samplerFxDelaySync"));
                PAD_SETTINGS->setSamplerFxDelayTimeMenu(padData->getIntAttribute("samplerFxDelayTimeMenu"));
                PAD_SETTINGS->setSamplerFxDelayAlphaTouch(padData->getIntAttribute("samplerFxDelayAlphaTouch"));
                PAD_SETTINGS->setSamplerFxDelayAtReverse(padData->getIntAttribute("samplerFxDelayAtReverse"));
                PAD_SETTINGS->setSamplerFxDelayAtIntensity(padData->getDoubleAttribute("samplerFxDelayAtIntensity"));
            }
            else if (PAD_SETTINGS->getSamplerEffect() == 8) //Reverb
            {
                PAD_SETTINGS->setSamplerFxReverbMix(padData->getDoubleAttribute("samplerFxReverbMix"));
                PAD_SETTINGS->setSamplerFxReverbRoomSize(padData->getDoubleAttribute("samplerFxReverbRoomSize"));
                PAD_SETTINGS->setSamplerFxReverbDamping(padData->getDoubleAttribute("samplerFxReverbDamping"));
                PAD_SETTINGS->setSamplerFxReverbWidth(padData->getDoubleAttribute("samplerFxReverbWidth"));
                PAD_SETTINGS->setSamplerFxReverbFreezeMode(padData->getDoubleAttribute("samplerFxReverbFreezeMode"));
                PAD_SETTINGS->setSamplerFxReverbAlphaTouch(padData->getIntAttribute("samplerFxReverbAlphaTouch"));
                PAD_SETTINGS->setSamplerFxReverbAtReverse(padData->getIntAttribute("samplerFxReverbAtReverse"));
                PAD_SETTINGS->setSamplerFxReverbAtIntensity(padData->getDoubleAttribute("samplerFxReverbAtIntensity"));
            }
            else if (PAD_SETTINGS->getSamplerEffect() == 9) //Flanger
            {
                PAD_SETTINGS->setSamplerFxFlangerMix(padData->getDoubleAttribute("samplerFxFlangerMix"));
                PAD_SETTINGS->setSamplerFxFlangerRate(padData->getDoubleAttribute("samplerFxFlangerRate"));
                PAD_SETTINGS->setSamplerFxFlangerFeedback(padData->getDoubleAttribute("samplerFxFlangerFeedback"));
                PAD_SETTINGS->setSamplerFxFlangerIntensity(padData->getDoubleAttribute("samplerFxFlangerIntensity"));
                PAD_SETTINGS->setSamplerFxFlangerSync(padData->getIntAttribute("samplerFxFlangerSync"));
                PAD_SETTINGS->setSamplerFxFlangerRateMenu(padData->getIntAttribute("samplerFxFlangerRateMenu"));
                PAD_SETTINGS->setSamplerFxFlangerAlphaTouch(padData->getIntAttribute("samplerFxFlangerAlphaTouch"));
                PAD_SETTINGS->setSamplerFxFlangerAtReverse(padData->getIntAttribute("samplerFxFlangerAtReverse"));
                PAD_SETTINGS->setSamplerFxFlangerAtIntensity(padData->getDoubleAttribute("samplerFxFlangerAtIntensity"));
            }
            
            else if (PAD_SETTINGS->getSamplerEffect() == 10) //Tremolo
            {
                PAD_SETTINGS->setSamplerFxTremoloDepth(padData->getDoubleAttribute("samplerFxTremoloDepth"));
                PAD_SETTINGS->setSamplerFxTremoloRate(padData->getDoubleAttribute("samplerFxTremoloRate"));
                PAD_SETTINGS->setSamplerFxTremoloShape(padData->getIntAttribute("samplerFxTremoloShape"));
                PAD_SETTINGS->setSamplerFxTremoloSync(padData->getIntAttribute("samplerFxTremoloSync"));
                PAD_SETTINGS->setSamplerFxTremoloRateMenu(padData->getIntAttribute("samplerFxTremoloRateMenu"));
                PAD_SETTINGS->setSamplerFxTremoloAlphaTouch(padData->getIntAttribute("samplerFxTremoloAlphaTouch"));
                PAD_SETTINGS->setSamplerFxTremoloAtReverse(padData->getIntAttribute("samplerFxTremoloAtReverse"));
                PAD_SETTINGS->setSamplerFxTremoloAtIntensity(padData->getDoubleAttribute("samplerFxTremoloAtIntensity"));
            }
        }
    }
    
    //sequencer mode
    else if (PAD_SETTINGS->getMode() == 3)
    {
        if (padData->hasAttribute("sequencerMode")) //WHICH IT SHOULD ALWAYS HAVE
            PAD_SETTINGS->setSequencerMode(padData->getIntAttribute("sequencerMode"));
        
        //should probably change this now as there's no 'new' data - just data
        
        if (padData->hasAttribute("newSequencerData0") == true) //new seq data format (0-127)
        {
            for (int seq = 0; seq <= NO_OF_SEQS-1; seq++)
            {
                PAD_SETTINGS->stringToSeqData(padData->getStringAttribute("newSequencerData"+String(seq)), seq);
            }
        }
        
        /*
         else if (padData->hasAttribute("sequencerData0") == true) //old seq data format (0-1)
         {
         for (int seq = 0; seq <= NO_OF_SEQS-1; seq++)
         {
         //convert any '1s' in the string to '110'
         PAD_SETTINGS->stringToSeqDataFormatConversion(padData->getStringAttribute("sequencerData"+String(seq)), seq);
         }
         }
         */
        
        if (padData->hasAttribute("sequencerNumberOfSequences"))
            PAD_SETTINGS->setSequencerNumberOfSequences(padData->getIntAttribute("sequencerNumberOfSequences"));
        if (padData->hasAttribute("sequencerTriggerMode"))
            PAD_SETTINGS->setSequencerTriggerMode(padData->getIntAttribute("sequencerTriggerMode"));
        if (padData->hasAttribute("sequencerShouldLoop"))
            PAD_SETTINGS->setSequencerShouldLoop(padData->getIntAttribute("sequencerShouldLoop"));
        if (padData->hasAttribute("sequencerIndestructible"))
            PAD_SETTINGS->setSequencerIndestructible(padData->getIntAttribute("sequencerIndestructible"));
        if (padData->hasAttribute("sequencerShouldFinishLoop"))
            PAD_SETTINGS->setSequencerShouldFinishLoop(padData->getIntAttribute("sequencerShouldFinishLoop"));
        if (padData->hasAttribute("sequencerSticky"))
            PAD_SETTINGS->setSequencerSticky(padData->getIntAttribute("sequencerSticky"));
        if (padData->hasAttribute("sequencerLength"))
            PAD_SETTINGS->setSequencerLength(padData->getIntAttribute("sequencerLength"));
        if (padData->hasAttribute("sequencerRelativeTempoMode"))
            PAD_SETTINGS->setSequencerRelativeTempoMode(padData->getIntAttribute("sequencerRelativeTempoMode"));
        if (padData->hasAttribute("sequencerDynamicMode"))
            PAD_SETTINGS->setSequencerDynamicMode(padData->getIntAttribute("sequencerDynamicMode"));
        
        //sequencer midi mode
        if (padData->getIntAttribute("sequencerMode") == 1)
        {
            for (int row = 0; row <= NO_OF_ROWS-1; row++)
            {
                if (padData->hasAttribute("sequencerMidiNote"+String(row)))
                    PAD_SETTINGS->setSequencerMidiNote(padData->getIntAttribute("sequencerMidiNote"+String(row)), row);
            }
            
            if (padData->hasAttribute("sequencerMidiVelocity"))
                PAD_SETTINGS->setSequencerMidiVelocity(padData->getIntAttribute("sequencerMidiVelocity"));
            if (padData->hasAttribute("sequencerMidiChannel"))
                PAD_SETTINGS->setSequencerMidiChannel(padData->getIntAttribute("sequencerMidiChannel"));
            if (padData->hasAttribute("sequencerMidiNoteLength"))
                PAD_SETTINGS->setSequencerMidiNoteLength(padData->getIntAttribute("sequencerMidiNoteLength"));
            if (padData->hasAttribute("sequencerMidiMinPressureRange"))
                PAD_SETTINGS->setSequencerMidiMinPressureRange(padData->getIntAttribute("sequencerMidiMinPressureRange"));
            if (padData->hasAttribute("sequencerMidiMaxPressureRange"))
                PAD_SETTINGS->setSequencerMidiMaxPressureRange(padData->getIntAttribute("sequencerMidiMaxPressureRange"));
            if (padData->hasAttribute("sequencerMidiPressureMode"))
                PAD_SETTINGS->setSequencerMidiPressureMode(padData->getIntAttribute("sequencerMidiPressureMode"));
            if (padData->hasAttribute("sequencerMidiPressureStatus"))
                PAD_SETTINGS->setSequencerMidiPressureStatus(padData->getBoolAttribute("sequencerMidiPressureStatus"));
            if (padData->hasAttribute("sequencerMidiCcController"))
                PAD_SETTINGS->setSequencerMidiCcController(padData->getIntAttribute("sequencerMidiCcController"));
        }
        
        //sequencer samples mode
        else if (padData->getIntAttribute("sequencerMode") == 2)
        {
            for (int row = 0; row <= NO_OF_ROWS-1; row++)
            {
                File newFile;
                String newFileString(String::empty);
                
                if (padData->hasAttribute("sequencerSamplesAudioFilePath"+String(row)))
                {
                    newFileString = padData->getStringAttribute("sequencerSamplesAudioFilePath"+String(row));
                    
                    if (newFileString != String::empty) //to prevent trying to load in a file if there's nothing to load
                    {
                        if (File::isAbsolutePath(newFileString) == false)
                        {
                            //check if the saved audio file path is just the file name
                            //if so, get it from the working directory and apply the full pathname to it
                            newFile = File::getCurrentWorkingDirectory().getFullPathName() + File::separatorString + newFileString;
                        }
                        else
                        {
                            //else, it should be the full path name already
                            newFile = newFileString;
                        }
                        
                        PAD_SETTINGS->setSequencerSamplesAudioFilePath(newFile, row);
                    }
                    else
                        PAD_SETTINGS->setSequencerSamplesAudioFilePath(File::nonexistent, row);
                }
            }
            
            if (padData->hasAttribute("sequencerPan"))
                PAD_SETTINGS->setSequencerPan(padData->getDoubleAttribute("sequencerPan"));
            if (padData->hasAttribute("sequencerGain"))
                PAD_SETTINGS->setSequencerGain(padData->getDoubleAttribute("sequencerGain"));
        }
    }
    
    //controller mode
    else if (PAD_SETTINGS->getMode() == 4)
    {
        if (padData->hasAttribute("controllerControl"))
            PAD_SETTINGS->setControllerControl(padData->getIntAttribute("controllerControl"));
        if (padData->hasAttribute("controllerSceneNumber"))
            PAD_SETTINGS->setControllerSceneNumber(padData->getIntAttribute("controllerSceneNumber"));
        
        if (padData->hasAttribute("controllerOscIpAddress"))
            PAD_SETTINGS->setControllerOscIpAddress(padData->getStringAttribute("controllerOscIpAddress"));
        if (padData->hasAttribute("controllerOscPortNumber"))
            PAD_SETTINGS->setControllerOscPort(padData->getIntAttribute("controllerOscPortNumber"));
        if (padData->hasAttribute("controllerMidiProgramChangeNumber"))
            PAD_SETTINGS->setControllerMidiProgramChangeNumber(padData->getIntAttribute("controllerMidiProgramChangeNumber"));
        if (padData->hasAttribute("controllerMidiProgramChangeChannel"))
            PAD_SETTINGS->setControllerMidiProgramChangeChannel(padData->getIntAttribute("controllerMidiProgramChangeChannel"));
        
    }
}






void AppDocumentState::saveProjectSettings()
{
    //reset/clear XmlElement.
    if (projectData != nullptr) //if there is data in the scene's XmlElement
        projectData->removeAllAttributes();
    
    projectData->setAttribute("copyExternalFiles", AppSettings::Instance()->getCopyExternalFiles());
    
}

void AppDocumentState::loadProjectSettings()
{
    if (projectData->hasAttribute("copyExternalFiles") == true)
        AppSettings::Instance()->setCopyExternalFiles(projectData->getIntAttribute("copyExternalFiles"));
    else
        AppSettings::Instance()->setCopyExternalFiles(true); //default value
    
}







void AppDocumentState::saveToScene (int sceneNumber)
{
    //reset/clear XmlElement.
    clearScene(sceneNumber);
    
    //===global settings===
    XmlElement *globalData  = new XmlElement ("GLOBAL_DATA");
    globalData->setAttribute("globalGain", AppSettings::Instance()->getGlobalGain());
    globalData->setAttribute("globalPan", AppSettings::Instance()->getGlobalPan());
    
    globalData->setAttribute("globalTempo", AppSettings::Instance()->getGlobalTempo());
    globalData->setAttribute("quantizationValue", AppSettings::Instance()->getQuantizationValue());
    globalData->setAttribute("beatsPerBar", AppSettings::Instance()->getBeatsPerBar());
    globalData->setAttribute("autoStartClock", AppSettings::Instance()->getAutoStartClock());
    
    
    
    //===pad settings (pad number = i)
    for (int i = 0; i <= 47; i++)
    {
        //create a 'temp' XmlElement to store data for a single pad
        XmlElement *padData  = new XmlElement ("PAD_DATA_" + String(i));
        
        savePadSettings (i, padData);
        
        //add temp xmlElement as a child element of the main scene XmlElement
        sceneData[sceneNumber]->addChildElement(new XmlElement(*padData));
        
        delete padData; 
        
    }
    
    //add globalData as child element.
    //WHY DO I HAVE TO DO THIS HERE AND NOT BEFORE DOING THE PADDATA STUFF?
    //caused hours of confusion and crashing if this was done first
    //UPDATE 29/8/12 - does the above comment still apply now?
    sceneData[sceneNumber]->addChildElement(new XmlElement(*globalData));
    
    delete globalData;
}




void AppDocumentState::loadFromScene (int sceneNumber)
{
    
    if (sceneData[sceneNumber] != nullptr && sceneData[sceneNumber]->hasTagName("SCENE_"+String(sceneNumber)))
    {
        //===global settings===
        XmlElement *globalData = new XmlElement(*sceneData[sceneNumber]->getChildByName("GLOBAL_DATA"));
        
        AppSettings::Instance()->setGlobalGain(globalData->getDoubleAttribute("globalGain"));
        AppSettings::Instance()->setGlobalPan(globalData->getDoubleAttribute("globalPan"));
        
        AppSettings::Instance()->setGlobalTempo(globalData->getDoubleAttribute("globalTempo"));
        AppSettings::Instance()->setQuantizationValue(globalData->getIntAttribute("quantizationValue"));
        AppSettings::Instance()->setBeatsPerBar(globalData->getIntAttribute("beatsPerBar"));
        AppSettings::Instance()->setAutoStartClock(globalData->getIntAttribute("autoStartClock"));
        
        delete globalData;
        
        //===pad settings (pad number = i)
        for (int i = 0; i <= 47; i++)
        {
            //creates a deep copy of sceneData, not just a copy of the pointer
            XmlElement *padData = new XmlElement(*sceneData[sceneNumber]->getChildByName("PAD_DATA_"+String(i))); 
            
            loadPadSettings(i, padData);
            
            delete padData;
        }
        
    }
    
    //update GUI - calls update() in mainComponent
    notifyObs();
}


void AppDocumentState::clearScene (int sceneNumber)
{
    
    //reset/clear XmlElement.
    if (sceneData[sceneNumber] != nullptr) //if there is data in the scene's XmlElement
    {
        sceneData[sceneNumber]->deleteAllChildElements();
    }
}









void AppDocumentState::createNewProject()
{
    //========== ask user if they would like to save the current project first =========
    int modeCheck = 0; //don't show 'save?' alert
    int shouldSave = 2; //don't save
    
    //check to see if the user might want to save anything first by looking for a pad
    //with a mode set to it
    //IDEALLY WE NEED A BETTER METHOD OF CHECKING WHETHER THE USER MIGHT WANT TO SAVE
    for (int i = 0; i <= 47; i++)
    {
        modeCheck = PAD_SETTINGS->getMode();
        if (modeCheck > 0)
            break;
    }
    
    //if found a pad with a mode set to it, ask if user wants to save first
    if (modeCheck != 0)
    {
        shouldSave = AlertWindow::showYesNoCancelBox(AlertWindow::WarningIcon, translate("Create New Project"), translate("Would you like to save the current project first?"));
    }
    
    //if the user didn't press cancel on the alertwindow ('cancel load command')
    if (shouldSave != 0) 
    {
        
        if (shouldSave == 1) //'yes'
        {
            shouldDisplayAlertWindow = false;
            saveProject();
        }
        
        
        //============= reset all settings =================
        
        AppSettings::Instance()->resetData();
        
        for (int i = 0; i <= 47; i++)
            PAD_SETTINGS->resetData(0);
        
        currentProjectFile = File::nonexistent;
        
        File::getSpecialLocation(File::tempDirectory).deleteRecursively();
        File::getSpecialLocation(File::tempDirectory).setAsCurrentWorkingDirectory();
        
        //========= clear all XmlElement objects and update the sceneComponent display ===========
        
        //let the observer know it will need to update the sceneComponent GUI next time it is 'notified'
        guiUpdateFlag = 1;
        
        projectData->removeAllAttributes();
        
        for (int i = 0; i <= NO_OF_SCENES-1; i++)
        {
            //accessed by observer in order to update the relevent scene slot's GUI
            sceneToUpdate = i;
            
            //reset the scene data by saving the default settings to the scene
            saveToScene(i);
            
            //display GUI scene slot as empty
            sceneStatus = 0;
        
            //set the first scene to be display as 'selected'
            if (i == 0)
            {
                sceneStatus = 2;
                setCurrentlySelectedScene(0);
            }
            
            //update display
            notifyObs();
        }
        
        //let the observer know it will need to update the settings display next time it is 'notified'
        guiUpdateFlag = 0;
        
        //save the reset settings, then call update the main display
        saveToScene(0);
        notifyObs();
        
        //change the window title bar text
        mainAppWindowRef->setTitleBarText("untitled");
        
    }
}


void AppDocumentState::saveProject()
{
    if (currentProjectFile == File::nonexistent) //if no performance has been saved yet
    {
        saveProjectAs();
    }
    
    else //replace currentProjectFile
    {
        //first, need to save the current project and scene settings
        saveProjectSettings();
        saveToScene(currentlySelectedScene);
        
        currentProjectFile.deleteFile();
        currentProjectFile.create(); //create the file

        XmlElement performanceSettings("ALPHALIVE_PROJECT_VERSION_1");

        performanceSettings.addChildElement(projectData);
        
        for (int i = 0; i <= NO_OF_SCENES-1; i++)
        {
            performanceSettings.addChildElement(sceneData[i]);
        }
        
        String xmlDoc = performanceSettings.createDocument(String::empty, false);
        currentProjectFile.appendText(xmlDoc);
        
        //remove projectData and sceneData child elements from performanceSettings so that they aren't deleted when
        //performanceSettings goes out of scope.
        
        performanceSettings.removeChildElement(projectData, false);
        
        for (int i = 0; i <= NO_OF_SCENES-1; i++)
        {
            performanceSettings.removeChildElement(sceneData[i], false);
        }
        
        /*
        if (shouldDisplayAlertWindow == true)
        {
            AlertWindow::showMessageBoxAsync(AlertWindow::InfoIcon, "Project Saved", "The project settings have been successfully saved to file");
        }
        shouldDisplayAlertWindow = true;
         */
        
        
        //add the file to the 'recent files' list
        registerRecentFile (currentProjectFile);
        
    }
}


void AppDocumentState::saveProjectAs()
{
    //navigate to app directory
    FileChooser saveFileChooser(translate("Create a AlphaLive project to save..."), 
                                StoredSettings::getInstance()->appProjectDir, 
                                "*.alphalive");
    
    if (saveFileChooser.browseForFileToSave(false))
    {
        //create a project directory
        File savedDirectory (saveFileChooser.getResult());
        
        //create folder to hold the projects audio files (if it doesn't already exist, which it shouldnt (?))
        File audioFileDirectory = (savedDirectory.getFullPathName() + File::separatorString + "Audio Files");
        
        if (AppSettings::Instance()->getCopyExternalFiles() == true)
        {
            //copy current working directory to the audio files directory
            File::getCurrentWorkingDirectory().copyDirectoryTo(audioFileDirectory);
        }
        else
            audioFileDirectory.createDirectory();  
        //set the audio files directory as the new working directory so when audio files are imported they go straight into here
        audioFileDirectory.setAsCurrentWorkingDirectory();
        
        //create file
        File savedFile (savedDirectory.getFullPathName() + File::separatorString + savedDirectory.getFileName()); //get file that the user has 'saved'
        String stringFile = savedFile.getFullPathName(); //get the filepath name of the file as a string
        stringFile = stringFile + ".alphalive"; //append an extension name to the filepath name
        savedFile = (stringFile); //set the file to this name
        
        bool overwrite = true; //by default true
        
        //how do i check for overriding here?
        
        //delete the file if it exists & write the new data
        if (savedFile.exists())
        {
            overwrite = AlertWindow::showOkCancelBox(AlertWindow::WarningIcon, translate("This File Already Exists!"), translate("Are you sure you want to overwrite this file?"));
        }
        
        if (overwrite == true)
        {
            //first, need to save the current project and scene settings
            saveToScene(currentlySelectedScene);
            saveProjectSettings();
            
            savedDirectory.createDirectory();
            savedFile.deleteFile();
            savedFile.create(); //create the file
    
            XmlElement performanceSettings("ALPHALIVE_PROJECT_VERSION_1");
            
            performanceSettings.addChildElement(projectData);
            
            for (int i = 0; i <= NO_OF_SCENES-1; i++)
            {
                performanceSettings.addChildElement(sceneData[i]);
            }
            
            String xmlDoc = performanceSettings.createDocument(String::empty, false);
            savedFile.appendText(xmlDoc);
            
            //remove projectData and sceneData child elements from performanceSettings so that they aren't deleted when
            //performanceSettings goes out of scope.
            
            performanceSettings.removeChildElement(projectData, false);
            
            for (int i = 0; i <= NO_OF_SCENES-1; i++)
            {
                performanceSettings.removeChildElement(sceneData[i], false);
            }
            
            //set the current open document to the document just saved
            currentProjectFile = savedFile;
            //change the window title bar text
            mainAppWindowRef->setTitleBarText(currentProjectFile.getFileNameWithoutExtension());
            
            /*
            if (shouldDisplayAlertWindow == true)
                AlertWindow::showMessageBoxAsync(AlertWindow::InfoIcon, "Project Saved", "The project settings have been successfully saved to file");
            shouldDisplayAlertWindow = true;
             */
            
            //add the file to the 'recent files' list
            registerRecentFile (currentProjectFile);
            
        }
        
    }
    
}

void AppDocumentState::loadProject (bool openBrowser, File fileToOpen)
{
    //openBrower will be true when the 'Load' button is clicked, and false when a .alphalive file is clicked
    //fileToOpen will be equal to File::nonexistent when Load is click, and the file path when a .alphalive
    //file is clicked
    
    //========== NEW: ask user if they would like to save the current project first =========
    int modeCheck = 0; //don't show 'save?' alert
    int shouldSave = 2; //don't save
    
    //check to see if the user might want to save anything first by looking for a pad
    //with a mode set to it
    //IDEALLY WE NEED A BETTER METHOD OF CHECKING WHETHER THE USER MIGHT WANT TO SAVE
    for (int i = 0; i <= 47; i++)
    {
        modeCheck = PAD_SETTINGS->getMode();
        if (modeCheck > 0)
            break;
    }
    
    //if found a pad with a mode set to it, ask if user wants to save first
    if (modeCheck != 0)
    {
        shouldSave = AlertWindow::showYesNoCancelBox(AlertWindow::WarningIcon, translate("Load New Project"), translate("Would you like to save the current project first?"));
    }
    
    //if the user didn't press cancel on the alertwindow ('cancel load command')
    if (shouldSave != 0) 
    {
        
        if (shouldSave == 1) //'yes'
        {
            shouldDisplayAlertWindow = false;
            saveProject();
        }
        
        // ========================== 'LOAD PROJECT' CODE ==================================
        
        //navigate to app directory
        FileChooser loadFileChooser(translate("Select a .alphalive file to open..."), 
                                    StoredSettings::getInstance()->appProjectDir, 
                                    "*.alphalive");
        
        bool shouldLoad;
        
        if (openBrowser == true)
            shouldLoad = loadFileChooser.browseForFileToOpen(); //open file browser
        
        if (shouldLoad || openBrowser == false)
        {
            //let the observer know it will need to update the sceneComponent GUI next time it is 'notified'
            guiUpdateFlag = 1;
            
            //get file
            File loadedFile;
            if (openBrowser == true)
                loadedFile = loadFileChooser.getResult();
            else
                loadedFile = fileToOpen;
            
            //parse file into xml file
            ScopedPointer<XmlElement> loadedXml (XmlDocument::parse(loadedFile));
            
            
            if (loadedXml != nullptr && loadedXml->hasTagName("ALPHALIVE_PROJECT_VERSION_1"))
            {
                
                //get the folder that holds the projects audio files
                File audioFileDirectory = (loadedFile.getParentDirectory().getFullPathName() + File::separatorString + "Audio Files");
                
                //if so some strange reason the directory doesn't exist, create it
                if (audioFileDirectory.exists() == false)
                    audioFileDirectory.createDirectory();
                
                //set the Audio Files directory as the new working directory so loaded audio files can be found
                audioFileDirectory.setAsCurrentWorkingDirectory();
                
                //=====================load projectData settings=======================
                
                //reset/clear XmlElement.
                if (projectData != nullptr)
                    projectData->removeAllAttributes();
                
                //put the loaded xml data into the xmlelement for the project settings
                XmlElement *projSettingsXml = loadedXml->getChildByName("PROJECT_SETTINGS");
                
                //check to see if the project settings child element actually exists (it won't within older AlphaLive Projects)
                if (loadedXml->containsChildElement(projSettingsXml) == true)
                    projectData = new XmlElement(*projSettingsXml);
                
                //apply the settings to AppSettings variables, even if "PROJECT_SETTINGS" doesn't exist (old file format), as default values should then be set
                loadProjectSettings();
                
                
                //WHY DON'T I NEED TO delete or remove projSettingsXml here?
                //I think it's because above I created a hard copy of projSettingsXml when applying it to projectData,
                //therefore when loadedXml goes out of scope and delete's projSettingsXml, it doesn't effect projectData
                //in anyway.
                
                //=========load the child elements of loadedXml and put them in the sceneData objects===========
                for (int scene = 0; scene <= NO_OF_SCENES-1; scene++)
                {
                    //accessed by observer in order to update the relevent scene slot's GUI
                    sceneToUpdate = scene;
                    
                    //clear the xmlelement for the current scene number
                    clearScene(scene);
                    
                    //put the loaded xml data into the xmlelement for the current scene
                    XmlElement* childToInsert = loadedXml->getChildByName("SCENE_" + String(scene));
                    sceneData.insert (scene, childToInsert);
                    //remove sceneData childelement from loadedXml so it isn't deleted when loadedXml goes out of scope!
                    loadedXml->removeChildElement (childToInsert, false);
                    
                    //determine the status of the scene
                    for (int i = 0; i < 48; i++)
                    {
                        sceneStatus = 0;
                        
                        if (sceneData[scene]->getChildByName("PAD_DATA_"+String(i))->getIntAttribute("mode") != 0)
                        {
                            sceneStatus = 1;
                            break;
                        }
                    }
                     
                    //set the first scene to be display as 'selected'
                    if (scene == 0)
                    {
                        sceneStatus = 2;
                        setCurrentlySelectedScene(0);
                    }
                    
                    //update display
                    notifyObs();
                }
                
                //let the observer know it will need to update the settings display next time it is 'notified'
                guiUpdateFlag = 0;
                
                //call loadFromScene to load the settings of scene 0 into the application
                loadFromScene(0);
                
                currentProjectFile = loadedFile;
                
                //=====================================================
                //==============NEW - reset unused mode settings=======
                //=====================================================
                /*
                 Here, the settings of the modes that aren't being used for each pad are reset to their default values.
                 */
                for (int i = 0; i <=47; i++)
                {
                    PAD_SETTINGS->resetData(PAD_SETTINGS->getMode());
                }
                //=====================================================
                
                //change the window title bar text
                mainAppWindowRef->setTitleBarText(currentProjectFile.getFileNameWithoutExtension());
                
                //add the file to the 'recent files' list
                registerRecentFile (currentProjectFile);
            }
            else if (loadedXml != nullptr && loadedXml->hasTagName("PERFORMANCE"))
            {
               AlertWindow::showMessageBoxAsync(AlertWindow::InfoIcon, translate("Cannot Open File"), translate("The selected AlphaLive Project file cannot be opened with this version of AlphaLive."));  
            }
            else
            {
                AlertWindow::showMessageBoxAsync(AlertWindow::InfoIcon, translate("Cannot Open File"), translate("The selected AlphaLive Project file seems to be corrupt."));
            }
            
        }
        
    }
     
}



void AppDocumentState::saveSceneToDisk (int sceneNumber)
{
    saveToScene(sceneNumber);
    
    //navigate to app directory
    FileChooser saveFileChooser(translate("Create an AlphaLive Scene file to save..."), 
                                StoredSettings::getInstance()->appProjectDir, 
                                "*.alphascene");
    if (saveFileChooser.browseForFileToSave(false))
    {
        //create a project directory
        File savedDirectory (saveFileChooser.getResult());
        File savedDirectoryName = savedDirectory;
        String directoryString = savedDirectory.getFullPathName() + " (SCENE)";
        savedDirectory = directoryString;
        
        File savedFile (savedDirectory.getFullPathName() + File::separatorString + savedDirectoryName.getFileName()); //get file that the user has 'saved'
        String stringFile = savedFile.getFullPathName(); //get the filepath name of the file as a string
        stringFile = stringFile + ".alphascene"; //append an extension name to the filepath name
        savedFile = stringFile; //set the file to this name
        
        bool overwrite = true; //by default true
        
        //delete the file if it exists &write the new data
        if (savedFile.existsAsFile())
        {
            overwrite = AlertWindow::showOkCancelBox(AlertWindow::WarningIcon, translate("This File Already Exists!"), translate("Are you sure you want to overwrite this file?"));
        }
        
        if (overwrite == true)
        {
            //------
            //MOVE ANY NEEDED AUDIO FILES INTO THE DEDICATED AUDIO FILES DIRECTORY FOR THIS SCENE
            //DO NOT CHANGE THE WORKING DIRECTORY HERE
            //------
            
            //create folder to hold the projects audio files
            File audioFileDirectory = (savedDirectory.getFullPathName() + File::separatorString + "Audio Files");
            audioFileDirectory.createDirectory();
            
            
            
            
            if (AppSettings::Instance()->getCopyExternalFiles() == true)
            {
                
                //search through all pads looking for audio files that need copying with the scene
                for (int i = 0; i <= 47; i++)
                {
                    //look for sampler audio files
                    if (sceneData[sceneNumber]->getChildByName("PAD_DATA_"+String(i))->getIntAttribute("mode") == 2)
                    {
                        String newFileName(sceneData[sceneNumber]->getChildByName("PAD_DATA_"+String(i))->getStringAttribute("samplerAudioFilePath"));
                        
                        if (newFileName != String::empty) //if there is 'something'
                        {
                            File originalFile;
                            
                            //Need to check here if the saved file path is just file name, which 
                            //means it exists in the audio files directory and needs copying.
                            //else, nothing needs to be done.
                            
                            if (File::isAbsolutePath(newFileName) == false)
                            {
                                //get the original audio file in question
                                originalFile = File::getCurrentWorkingDirectory().getFullPathName()+ File::separatorString + newFileName;
                                
                                //create an new file in scenes "Audio Files" folder;
                                File newFile (audioFileDirectory.getFullPathName() + File::separatorString + newFileName);
                                
                                if (originalFile.existsAsFile() == true) //if the file exists (which it should)
                                {
                                    if (newFile.existsAsFile() == false) //if it doesn't already exist
                                        originalFile.copyFileTo(newFile);
                                }
                                else
                                {
                                    AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon, translate("File not found!"), newFileName + " " + translate("could not be found."));
                                    //do something here so the loaded data is string::empty and NOT the missing audio file name
                                    //can i do this here or does it need to be done in the loadforscene method?
                                }
                            
                            }
                        }
                    }
                    //look for sequencer audio files
                    else if (sceneData[sceneNumber]->getChildByName("PAD_DATA_"+String(i))->getIntAttribute("mode") == 3)
                    {
                        for (int row = 0; row <= NO_OF_ROWS-1; row++)
                        {
                            String newFileName(sceneData[sceneNumber]->getChildByName("PAD_DATA_"+String(i))->getStringAttribute("sequencerSamplesAudioFilePath"+String(row)));
                            
                            if (newFileName != String::empty) //if there is 'something'
                            {
                                File originalFile;
                                
                                //Need to check here if the saved file path is just file name, which 
                                //means it exists in the audio files directory and needs copying.
                                //else, nothing needs to be done.
                                
                                if (File::isAbsolutePath(newFileName) == false)
                                {
                                    //get the original audio file in question
                                    originalFile = File::getCurrentWorkingDirectory().getFullPathName()+ File::separatorString + newFileName;
                                    
                                    //create an new file in scenes "Audio Files" folder;
                                    File newFile (audioFileDirectory.getFullPathName() + File::separatorString + newFileName);
                                    
                                    if (originalFile.existsAsFile() == true) //if the file exists (which it should)
                                    {
                                        if (newFile.existsAsFile() == false) //if it doesn't already exist
                                            originalFile.copyFileTo(newFile);
                                    }
                                    else
                                    {
                                        AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon, translate("File not found!"), newFileName + " " + translate("could not be found."));
                                        //do something here so the loaded data is string::empty and NOT the missing audio file name
                                        //can i do this here or does it need to be done in the loadforscene method?
                                    }
                                    
                                }
                            }
                        }
                    }
                }
            }
             
            //----
            
            //----
            //SAVE THE FILE TO DISK
            //----
            savedFile.deleteFile();
            savedFile.create(); //create the file
            
            //can't just save the sceneData[sceneNumber] xmlelement straight into file, as the tag name needs to be different, so a new xmlelement must be
            //created which imports the child elements from sceneData[sceneNumber]
            
            //create xmlelement to be saved
            XmlElement *toBeSaved = new XmlElement("ALPHALIVE_SCENE_VERSION_1");
            
            //import child elements
            for (int i = 0; i <= 47; i++)
            {
                toBeSaved->addChildElement(new XmlElement(*sceneData[sceneNumber]->getChildByName("PAD_DATA_"+String(i))));
            }
            toBeSaved->addChildElement(new XmlElement(*sceneData[sceneNumber]->getChildByName("GLOBAL_DATA")));
            
            //save to file
            String xmlDoc = toBeSaved->createDocument(String::empty, false);
            savedFile.appendText(xmlDoc);
            
            delete toBeSaved;
            
            //----
            
        }
    }
    
}




bool AppDocumentState::loadSceneFromDisk(int sceneNumber, bool openBrowser, File fileToOpen)
{
    //navigate to app directory
    FileChooser loadFileChooser(translate("Select a .alphascene file to load..."), 
                                StoredSettings::getInstance()->appProjectDir, 
                                "*.alphascene");
    
    bool shouldLoad;
    
    if (openBrowser == true)
        shouldLoad = loadFileChooser.browseForFileToOpen(); //open file browser
    
    if (shouldLoad || openBrowser == false)
    {
        File loadedFile;
        
        if (openBrowser == true)
            loadedFile = loadFileChooser.getResult();
        else
            loadedFile = fileToOpen;
        
        //parse file into xml file
        ScopedPointer<XmlElement> loadedXml (XmlDocument::parse(loadedFile));
        
        if (loadedXml != nullptr && loadedXml->hasTagName("ALPHALIVE_SCENE_VERSION_1"))
        {
            //clear the xmlelement for the currently selected scene number
            clearScene(sceneNumber);
            
            //put the loaded xml data into the xmlelement for the current scene
            //howcome i need to load each child individually here but not anywhere else (where i just load/save first child and it weird does the same for the others)??? (21/8/12 - does this comment still apply now?)
            for ( int i = 0; i <= 47; i++)
            {
                XmlElement* childToInsert = loadedXml->getChildByName("PAD_DATA_"+String(i));
                sceneData[sceneNumber]->addChildElement(childToInsert);
                loadedXml->removeChildElement(childToInsert, false);
            }
            XmlElement* childToInsert = loadedXml->getChildByName("GLOBAL_DATA");
            sceneData[sceneNumber]->addChildElement(childToInsert);
            
            //remove sceneData childelement from loadedXml so it isn't deleted when loadedXml goes out of scope!
            loadedXml->removeChildElement (childToInsert, false);
            
            
            
            //------------
            if (AppSettings::Instance()->getCopyExternalFiles() == true)
            {
                
                //get the folder that holds the projects audio files
                File audioFileDirectory = (loadedFile.getParentDirectory().getFullPathName() + File::separatorString + "Audio Files");
                
                //copy contents of audioFileDirectory into the working directory's Audio Files folder
                //This must be done by searching through the loaded data for audio file names and copying the files individually if they exist
                for (int i = 0; i <= 47; i++)
                {
                    //look for sampler audio files
                    if (sceneData[sceneNumber]->getChildByName("PAD_DATA_"+String(i))->getIntAttribute("mode") == 2)
                    {
                        String newFileName(sceneData[sceneNumber]->getChildByName("PAD_DATA_"+String(i))->getStringAttribute("samplerAudioFilePath"));
                        
                        if (newFileName != String::empty) //if there is 'something'
                        {
                            File originalFile(File::nonexistent);
                            File newFile(File::nonexistent);
                            
                            //Need to check here if the saved file path is just the file
                            //name or the full path, and handle the situation appropriatly
                            
                            if (File::isAbsolutePath(newFileName) == false)
                            {
                                //File should be an internal file
                                originalFile = audioFileDirectory.getFullPathName()+ File::separatorString + newFileName;
                                newFile = File::getCurrentWorkingDirectory().getFullPathName() + File::separatorString + newFileName;
                            }
                            
                            else if (File::isAbsolutePath(newFileName) == true)
                            {
                                //file is an external file.
                                originalFile = newFileName;
                                newFile = File::getCurrentWorkingDirectory().getFullPathName() + File::separatorString + originalFile.getFileName();
                                
                                //re-set the path name within sceneData from the full path name to just the file name
                                sceneData[sceneNumber]->getChildByName("PAD_DATA_"+String(i))->setAttribute("samplerAudioFilePath", newFile.getFileName());
                            }
                            
                            
                            if (originalFile.existsAsFile() == true) //if the file exists (which it should)
                            {
                                if (newFile.existsAsFile() == false) //if it doesn't already exist
                                {
                                    originalFile.copyFileTo(newFile); 
                                    std::cout << "Copying audio file from loaded scene's dir to current working dir!\n";
                                }
                            }
                            else
                            {
                                AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon, translate("File not found!"), newFileName + " " + translate("could not be found."));
                                //do something here so the loaded data is string::empty and NOT the missing audio file name
                            }
                        }
                    }
                    
                    
                    
                    //look for sequencer audio files
                    else if (sceneData[sceneNumber]->getChildByName("PAD_DATA_"+String(i))->getIntAttribute("mode") == 3)
                    {
                        for (int row = 0; row <= NO_OF_ROWS-1; row++)
                        {
                            
                            String newFileName(sceneData[sceneNumber]->getChildByName("PAD_DATA_"+String(i))->getStringAttribute("sequencerSamplesAudioFilePath"+String(row)));
                            
                            if (newFileName != String::empty) //if there is 'something'
                            {
                                File originalFile(File::nonexistent);
                                File newFile(File::nonexistent);
                                
                                //Need to check here if the saved file path is just the file
                                //name or the full path, and handle the situation appropriatly
                                
                                if (File::isAbsolutePath(newFileName) == false)
                                {
                                    //File should be an internal file
                                    originalFile = audioFileDirectory.getFullPathName()+ File::separatorString + newFileName;
                                    newFile = File::getCurrentWorkingDirectory().getFullPathName() + File::separatorString + newFileName;
                                }
                                
                                else if (File::isAbsolutePath(newFileName) == true)
                                {
                                    //file is an external file.
                                    originalFile = newFileName;
                                    newFile = File::getCurrentWorkingDirectory().getFullPathName() + File::separatorString + originalFile.getFileName();
                                    
                                    //re-set the path name within sceneData from the full path name to just the file name
                                    sceneData[sceneNumber]->getChildByName("PAD_DATA_"+String(i))->setAttribute("sequencerSamplesAudioFilePath"+String(row), newFile.getFileName());
                                }
                                
                                
                                if (originalFile.existsAsFile() == true) //if the file exists (which it should)
                                {
                                    if (newFile.existsAsFile() == false) //if it doesn't already exist
                                    {
                                        originalFile.copyFileTo(newFile); 
                                        std::cout << "Copying audio file from loaded scene's dir to current working dir!\n";
                                    }
                                }
                                else
                                {
                                    AlertWindow::showMessageBoxAsync(AlertWindow::WarningIcon, translate("File not found!"), newFileName + " " + translate("could not be found."));
                                    //do something here so the loaded data is string::empty and NOT the missing audio file name
                                }
                                
                            }
                        }
                    }
                }
            }
            else if (AppSettings::Instance()->getCopyExternalFiles() == false)
            {
                //This else if statement is needed in case the option to copy 
                //external files is currently false when importing scenes
                //which have their audio files stored in the directory 
                //(and hence only the file NAME (not path) is saved), as the audio files
                //will not be found once in the new project. The names need 
                //to be converted from just the file name to the full path to work
                
                //get the folder that holds the projects audio files
                File audioFileDirectory = (loadedFile.getParentDirectory().getFullPathName() + File::separatorString + "Audio Files");
                
                for (int i = 0; i <= 47; i++)
                {
                    //look for sampler audio files
                    if (sceneData[sceneNumber]->getChildByName("PAD_DATA_"+String(i))->getIntAttribute("mode") == 2)
                    {
                        String newFileString(sceneData[sceneNumber]->getChildByName("PAD_DATA_"+String(i))->getStringAttribute("samplerAudioFilePath"));
                        
                        if (newFileString != String::empty) //if there is 'something'
                        {
    
                            if (File::isAbsolutePath(newFileString) == false)
                            {
                                //File should be an internal file
                                File newFile(audioFileDirectory.getFullPathName()+ File::separatorString + newFileString); //should be a String?
                                
                                //set the saved file name to be the full path...
                                sceneData[sceneNumber]->getChildByName("PAD_DATA_"+String(i))->setAttribute("samplerAudioFilePath", newFile.getFullPathName()); 
                            }
                        }
                    }
                    
                    //look for sequencer audio files
                    else if (sceneData[sceneNumber]->getChildByName("PAD_DATA_"+String(i))->getIntAttribute("mode") == 3)
                    {
                        for (int row = 0; row <= NO_OF_ROWS-1; row++)
                        {
                            
                            String newFileString(sceneData[sceneNumber]->getChildByName("PAD_DATA_"+String(i))->getStringAttribute("sequencerSamplesAudioFilePath"+String(row)));
                            if (newFileString != String::empty) //if there is 'something'
                            {
                                //if the string saved is just the file name...
                                if (File::isAbsolutePath(newFileString) == false)
                                {
                                    //... get the full path name of the file...
                                    File newFile(audioFileDirectory.getFullPathName()+ File::separatorString + newFileString); //should be a String?
                                    //... a re-set the path name within sceneData, NOT PadSettings as this data hasn't been loaded
                                    //into PadSettings yet
                                    sceneData[sceneNumber]->getChildByName("PAD_DATA_"+String(i))->setAttribute("sequencerSamplesAudioFilePath"+String(row), newFile.getFullPathName());
                                }
                            }
                        }
                    }
                }
                
            }
           
            return true;
        }
        else
        {
            AlertWindow::showMessageBoxAsync(AlertWindow::InfoIcon, translate("Cannot Open File"), translate("The selected AlphaLive Scene file seems to be corrupt."));
            
            return false;
        } 
    }
    
    else // 'cancel' button pressed
    {
        return false;
    }
    
}




void AppDocumentState::savePadToDisk (int padNumber)
{
    //This function will be similar to 'saveSceneToDisk()'
    //and will extract the pad data element of sceneData[currentlySelectedScene]
    //and put it int a new XmlElment which is then saved as an external file.
    //The only reason I'm not implementing that right now is because,
    //like with exporting scenes, the audio files need to be copied to
    //an associated audio files directory. Also when we implement this function/feature,
    //the 'loadPadFromDisk()' function will need to import any associated
    //audio files into the current projects audio files directory.
    
    //Like with loadFromScene(), all the code that access padData within saveToScene()
    //has been moved to a seperate function which will be called winthin this function
    //to get the right data from PadSettings.
    
    //Importing and exporting pad data should be accessible from right-clicking
    //on a pad, as well as from the menu bar (within the menu bar export settings
    //will only be selectable when a single pad is selected).
    
}

void AppDocumentState::loadPadFromDisk (Array<int> selectedPads_, bool openBrowser, File fileToOpen)
{
    //navigate to app directory
    FileChooser loadFileChooser(translate("Select a .alphapad file to load..."), 
                                StoredSettings::getInstance()->appProjectDir, 
                                "*.alphapad");
    
    bool shouldLoad;
    
    if (openBrowser == true)
        shouldLoad = loadFileChooser.browseForFileToOpen(); //open file browser
    
    if (shouldLoad || openBrowser == false)
    {
        File loadedFile;
        
        if (openBrowser == true)
            loadedFile = loadFileChooser.getResult();
        else
            loadedFile = fileToOpen;
        
        //parse file into xml file
        ScopedPointer<XmlElement> loadedXml (XmlDocument::parse(loadedFile));
        
        if (loadedXml != nullptr && loadedXml->hasTagName("ALPHALIVE_PAD_SETTINGS_VERSION_1"))
        {
            //saveToScene(currentlySelectedScene);
            
            XmlElement* padData = loadedXml->getChildByName("PAD_DATA");
            
            for (int i = 0; i < selectedPads_.size(); i++)
            {
                loadPadSettings(selectedPads_[i], padData);
            }
            
            loadedXml->removeChildElement (padData, true);
            
            //update GUI - calls update() in mainComponent
            
            //SHOULD I CREATE A NEW GUIUPDATEFLAG VALUE THAT ONLY ALLOWS A MINIMUM AMOUNT OF
            //THE GUI T0 BE UPDATED? HOWEVER THERE WILL PROBABLY BE A DIFFERENCE BETWEEN WHETHER
            //IF THIS FUNCTION WAS CALLED FROM THE TOOLBOX OR BY MANUALLY LOADING A PADS SETTINGS.
            guiUpdateFlag = 0;
            notifyObs();
        }
        
    }
}







void AppDocumentState::saveSequence (int currentlySelectedSeqNumber, int currentlySelectedPad)
{
    //navigate to app directory
    FileChooser saveFileChooser(translate("Create a single sequence file to save..."), 
                                StoredSettings::getInstance()->appProjectDir, 
                                "*.alphaseq");
    
    if (saveFileChooser.browseForFileToSave(false))
    {
        //Surely there's a easier way to do the following code? FileBasedDocument
        File savedFile (saveFileChooser.getResult()); //get file that the user has 'saved'
        String stringFile = savedFile.getFullPathName(); //get the filepath name of the file as a string
        stringFile = stringFile + ".alphaseq"; //append an extension name to the filepath name
        savedFile = stringFile; //set the file to this name
        
        bool overwrite = true; //by default true
        
        //delete the file if it exists &write the new data
        if (savedFile.existsAsFile())
        {
            overwrite = AlertWindow::showOkCancelBox(AlertWindow::WarningIcon, translate("This File Already Exists!"), translate("Are you sure you want to overwrite this file?"));
        }
        
        if (overwrite == true)
        {
            savedFile.deleteFile();
            savedFile.create(); //create the file
            
            XmlElement sequenceDataXml("SEQUENCE_DATA");
            
            //get single sequence data string based on currently selected sequencer number slider value
            sequenceDataXml.setAttribute("sequenceData", PAD_SETTINGS_pad->getSequencerDataString(currentlySelectedSeqNumber));
            
            String xmlDoc = sequenceDataXml.createDocument(String::empty);
            savedFile.appendText(xmlDoc);
            
            std::cout << savedFile.getFullPathName() << std::endl;
            
            //AlertWindow::showMessageBoxAsync(AlertWindow::InfoIcon, "Single Sequence Saved", "The sequence has been successfully saved to file");
        }
    }
    
}


void AppDocumentState::loadSequence (int currentlySeletedSeqNumber, 
                                     Array<int> selectedPads_,
                                     bool openBrowser, 
                                     File fileToOpen)
{
    //navigate to app directory
    FileChooser loadFileChooser(translate("Select a .alphaseq file to load..."), 
                                StoredSettings::getInstance()->appProjectDir, 
                                "*.alphaseq");
    
    bool shouldLoad;
    
    if (openBrowser == true)
        shouldLoad = loadFileChooser.browseForFileToOpen(); //open file browser
    
    if (shouldLoad || openBrowser == false)
    {
        //File loadedFile (loadFileChooser.getResult());
        File loadedFile;
        
        if (openBrowser == true)
            loadedFile = loadFileChooser.getResult();
        else
            loadedFile = fileToOpen;
        
        XmlElement* xml = XmlDocument::parse(loadedFile);
        if (xml != nullptr && xml->hasTagName("SEQUENCE_DATA"))
        {
            for (int i = 0; i < selectedPads_.size(); i++)
            {
                int padNum = selectedPads_[i];
                //get the saved string and call stringToSeqData() to convert it back int a stream of int's
                PAD_SETTINGS_pads->stringToSeqData(xml->getStringAttribute("sequenceData"), currentlySeletedSeqNumber);
            }
            

        }
        
        delete xml;
        
        //update GUI
        //SHOULD I CREATE A NEW GUIUPDATEFLAG VALUE THAT ONLY ALLOWS A MINIMUM AMOUNT OF
        //THE GUI T0 BE UPDATED?
        notifyObs();
    }
}




void AppDocumentState::saveSequenceSet(int currentlySelectedPad)
{
    //navigate to app directory
    FileChooser saveFileChooser(translate("Create a sequence set file to save..."), 
                                StoredSettings::getInstance()->appProjectDir, 
                                "*.alphaseqset");
    
    if (saveFileChooser.browseForFileToSave(false))
    {
        //Surely there's a easier way to do the following code? FileBasedDocument
        File savedFile (saveFileChooser.getResult()); //get file that the user has 'saved'
        String stringFile = savedFile.getFullPathName(); //get the filepath name of the file as a string
        stringFile = stringFile + ".alphaseqset"; //append an extension name to the filepath name
        savedFile = stringFile; //set the file to this name
        
        bool overwrite = true; //by default true
        
        //delete the file if it exists &write the new data
        if (savedFile.existsAsFile())
        {
            overwrite = AlertWindow::showOkCancelBox(AlertWindow::WarningIcon, translate("This File Already Exists!"), translate("Are you sure you want to overwrite this file?"));
        }
        
        if (overwrite == true)
        {
            savedFile.deleteFile();
            savedFile.create(); //create the file
            
            XmlElement sequenceDataXml("SEQUENCE_DATA");
            
            //get all sequence data strings 
            for (int i = 0; i <= NO_OF_SEQS-1; i++)
            {
                sequenceDataXml.setAttribute("sequenceData"+String(i), PAD_SETTINGS_pad->getSequencerDataString(i));
            }
            
            String xmlDoc = sequenceDataXml.createDocument(String::empty);
            savedFile.appendText(xmlDoc);
            
        }
    }
}





void AppDocumentState::loadSequenceSet(Array<int> selectedPads_,
                                       bool openBrowser,
                                       File fileToOpen)
{
    //navigate to app directory
    FileChooser loadFileChooser(translate("Select a .alphaseqset file to load..."), 
                                StoredSettings::getInstance()->appProjectDir, 
                                "*.alphaseqset");
    
    bool shouldLoad;
    
    if (openBrowser == true)
        shouldLoad = loadFileChooser.browseForFileToOpen(); //open file browser
    
    if (shouldLoad || openBrowser == false)
    {
        //File loadedFile (loadFileChooser.getResult());
        File loadedFile;
        
        if (openBrowser == true)
            loadedFile = loadFileChooser.getResult();
        else
            loadedFile = fileToOpen;
        
        XmlElement* xml = XmlDocument::parse(loadedFile);
        if (xml != nullptr && xml->hasTagName("SEQUENCE_DATA"))
        {
            for (int i = 0; i < selectedPads_.size(); i++)
            {
                int padNum = selectedPads_[i];
                //get the saved string for each of the seq and call stringToSeqData() to convert them back int a stream of int's
                for (int seqNumber = 0; seqNumber <= NO_OF_SEQS-1; seqNumber++)
                {
                    PAD_SETTINGS_pads->stringToSeqData(xml->getStringAttribute("sequenceData"+String(seqNumber)), seqNumber);
                }
            }
            
        }
        
        delete xml;
        
        //update GUI
        //SHOULD I CREATE A NEW GUIUPDATEFLAG VALUE THAT ONLY ALLOWS A MINIMUM AMOUNT OF
        //THE GUI T0 BE UPDATED? 
        notifyObs();
        
    }
}


void AppDocumentState::removeUneededAudioFiles()
{
    
    if (currentProjectFile != File::nonexistent) //if there is currently an open project
    {
        
        bool shouldCleanUp = AlertWindow::showOkCancelBox(AlertWindow::WarningIcon, translate("Clean Up Project"), translate("This command will go through the current projects Audio Files directory and delete any files which aren't currently being used. Over time this will prevent an excessive build-up of redundant data. It was also reset any unused mode settings to default values. Please note that you can not undo this command!"));
        if (shouldCleanUp == true)
        {
            //this function must check all the settings of all the sceneData elements,
            //and delete any audio files in the project Audio Files directory that aren't included in these settings.
            //need to move all the currently used audio files somewhere, delete any that are left, and move the used ones back
            //Is is the best way to do it?
            
            //first must save the current settings into the current scene to prevent missing audio files errors once the clean up has been completed.
            //For example, if you imported in a new audio file and then instantly cleaned up without saving the new settings it might delete the current
            //audio file as a reference to it wouldn't be found in any of the sceneData elements, so when the clean up is complete the audio file would 
            //now be missing.
            //instead of saving, you could load up the scene data for the current scene which would delete the current settings that havent been saved. What would be more natural?
            saveToScene(currentlySelectedScene);
            
            File tempAudioDirectory = File::getCurrentWorkingDirectory().getParentDirectory().getFullPathName() + File::separatorString + "tempDir";
            tempAudioDirectory.createDirectory();
            
            //search through all scenes
            for (int sceneNumber = 0; sceneNumber <= NO_OF_SCENES-1; sceneNumber++)
            {
                
                if (sceneData[sceneNumber]->getNumChildElements() > 0) //if the scene XmlElement contains data
                {
                    
                    //search through all pads looking for audio files 
                    for (int i = 0; i <= 47; i++)
                    {
                        //look for sampler audio files
                        if (sceneData[sceneNumber]->getChildByName("PAD_DATA_"+String(i))->getIntAttribute("mode") == 2)
                        {
                            String originalFile(sceneData[sceneNumber]->getChildByName("PAD_DATA_"+String(i))->getStringAttribute("samplerAudioFilePath"));
                            
                            if (originalFile != String::empty) //if there is 'something'
                            {
                                File audioFile(File::nonexistent);
                                
                                //Look for internal files, which are saved just by just their file name
                                
                                if (File::isAbsolutePath(originalFile) == false)
                                {
                                    //File should be an internal file
                                    audioFile = File::getCurrentWorkingDirectory().getFullPathName() + File::separatorString + originalFile;
                                    
                                    if (audioFile.existsAsFile() == true)
                                    {
                                        File newFile = tempAudioDirectory.getFullPathName()+ File::separatorString + originalFile;
                                        if (newFile.existsAsFile() == false) //if it doesn't already exist
                                        {
                                            audioFile.copyFileTo(newFile);//copy the file
                                        }
                                    }
                                    
                                }
                                else
                                {
                                    //external file.
                                    //check to see if the external file is actually stored internally,
                                    //which could be caused by importing files to project, 
                                    //and then selecting 'don't copy' afterwards.
                                    
                                    audioFile = originalFile;
                                    
                                    if (audioFile.isAChildOf(File::getCurrentWorkingDirectory()) == true)
                                    {
                                        originalFile = audioFile.getFileName();
                                        File newFile = tempAudioDirectory.getFullPathName()+ File::separatorString + originalFile;
                                        if (newFile.existsAsFile() == false) //if it doesn't already exist
                                        {
                                            audioFile.copyFileTo(newFile);//copy the file
                                        } 
                                        
                                        //would it be a good idea to 'fix' the saved file path here so that the file
                                        //is now recignised as being internal instead of external?
                                    }
                                }
                            }
                        }
                        //look for sequencer audio files
                        else if (sceneData[sceneNumber]->getChildByName("PAD_DATA_"+String(i))->getIntAttribute("mode") == 3)
                        {
                            for (int row = 0; row <= NO_OF_ROWS-1; row++)
                            {
                                String originalFile(sceneData[sceneNumber]->getChildByName("PAD_DATA_"+String(i))->getStringAttribute("sequencerSamplesAudioFilePath"+String(row)));
                                
                                if (originalFile != String::empty) //if there is 'something'
                                {
                                    File audioFile(File::nonexistent);
                                    
                                    //Look for internal files, which are saved just by just their file name
                                    
                                    if (File::isAbsolutePath(originalFile) == false)
                                    {
                                        //File should be an internal file
                                        audioFile = File::getCurrentWorkingDirectory().getFullPathName() + File::separatorString + originalFile;
                                        
                                        if (audioFile.existsAsFile() == true)
                                        {
                                            File newFile = tempAudioDirectory.getFullPathName()+ File::separatorString + originalFile;
                                            if (newFile.existsAsFile() == false) //if it doesn't already exist
                                            {
                                                audioFile.copyFileTo(newFile);//copy the file
                                            }
                                        }
                                        
                                    }
                                    else
                                    {
                                        //external file.
                                        //check to see if the external file is actually stored internally,
                                        //which could be caused by importing files to project, 
                                        //and then selecting 'don't copy' afterwards.
                                        
                                        audioFile = originalFile;
                                        
                                        if (audioFile.isAChildOf(File::getCurrentWorkingDirectory()) == true)
                                        {
                                            originalFile = audioFile.getFileName();
                                            File newFile = tempAudioDirectory.getFullPathName()+ File::separatorString + originalFile;
                                            if (newFile.existsAsFile() == false) //if it doesn't already exist
                                            {
                                                audioFile.copyFileTo(newFile);//copy the file
                                            } 
                                            
                                            //would it be a good idea to 'fix' the saved file path here so that the file
                                            //is now recignised as being internal instead of external?
                                        }
                                    }
                                    
                                }
                            }
                        }
                    }
                }
            }
            //create a copy of the working dir (Audio Files dir)
            File audioFileDirectory = File::getCurrentWorkingDirectory();
            //delete the current working dir
            File::getCurrentWorkingDirectory().deleteRecursively();
            //rename the temp dir (which should hold all the needed audio files) to 'Audio Files' so it can be used as the working dir when the project is next loaded up
            tempAudioDirectory.moveFileTo(audioFileDirectory);
            
            //set the currentWorkingDirectory
            audioFileDirectory.setAsCurrentWorkingDirectory();
            
            //=====================================================
            //==============NEW - reset unused mode settings=======
            //=====================================================
            /*
             Here, the settings of the modes that aren't being used for each pad are reset to their default values.
             */
            for (int i = 0; i <=47; i++)
            {
                PAD_SETTINGS->resetData(PAD_SETTINGS->getMode());
            }
            //=====================================================
            
            //automatically save the new settings
            shouldDisplayAlertWindow = false;
            saveProject();
            
            AlertWindow::showMessageBox(AlertWindow::InfoIcon, translate("Project Cleaned Up!"), translate("All redundant files and settings have been deleted and reset."));
        }
    }
    
    else
    {
        AlertWindow::showMessageBox(AlertWindow::InfoIcon, translate("No project currently open!"), translate("There is no project open to clean up."));
    }
}


void AppDocumentState::importAudioFiles()
{
    //==============================================================================================
    //this function is called if copyExternalFilesSwitch is set to on/true
    //==============================================================================================
    
    saveToScene(currentlySelectedScene);
    
    //search through all scenes
    for (int sceneNumber = 0; sceneNumber <= NO_OF_SCENES-1; sceneNumber++)
    {
        if (sceneData[sceneNumber]->getNumChildElements() > 0) //if the scene XmlElement contains data
        {
            //search through all pads looking for audio files 
            for (int i = 0; i <= 47; i++)
            {
                //look for sampler audio files
                if (sceneData[sceneNumber]->getChildByName("PAD_DATA_"+String(i))->getIntAttribute("mode") == 2)
                {
                    File currentFile;
                    File newFile;
                    String fileName = sceneData[sceneNumber]->getChildByName("PAD_DATA_"+String(i))->getStringAttribute("samplerAudioFilePath");
                    
                    if (fileName != String::empty)
                    {
                        //Here, we are looking for external files. If we find one, we need to copy it
                        if (File::isAbsolutePath(fileName) == true)
                        {
                            //if we are looking for external files and the pads file is an external file...
                            currentFile = fileName;
                        }
                        else
                        {
                            //no match...
                            break;
                        }
                        
                        newFile = File::getCurrentWorkingDirectory().getFullPathName() + File::separatorString + currentFile.getFileName();
                        
                        if (newFile.existsAsFile() == false) //if it doesn't yet exist
                        {
                            //copy the added audio file into the newly created file
                            currentFile.copyFileTo(newFile);
                        }
                        
                        else if (newFile.existsAsFile() == true) //if it already exists 
                        {
                            Array<File> matchingFilesArray;
                            String fileWildCard (newFile.getFileNameWithoutExtension()+"*");
                            bool importedFileNeedsCopying = true;
                            
                            //Find all possible duplicates of the imported file using fileWildCard
                            //and add reference of all possible files to matchingFilesArray.
                            File::getCurrentWorkingDirectory().findChildFiles(matchingFilesArray, 2, false, fileWildCard);
                            
                            for (int i = 0; i < matchingFilesArray.size(); i++)
                            {
                                if (currentFile.hasIdenticalContentTo(matchingFilesArray[i]) == true)
                                {
                                    //if it finds a duplicate, flag that the file doesn't need copying
                                    importedFileNeedsCopying = false;
                                    //set the file
                                    newFile = matchingFilesArray[i];
                                    //break out of the for loop once a duplicate has been found to prevent unnecessary checks
                                    break;
                                }
                            }
                            
                            if (importedFileNeedsCopying == true) 
                            {
                                //if no duplicate was found...
                                //... copy the added file with an appended name
                                newFile = newFile.getNonexistentSibling();
                                currentFile.copyFileTo(newFile); 
                            }
                            
                        }
                        
                        
                        sceneData[sceneNumber]->getChildByName("PAD_DATA_"+String(i))->setAttribute("samplerAudioFilePath", newFile.getFileName());
                        
                        if (sceneNumber == currentlySelectedScene)
                            AppSettings::Instance()->padSettings[i]->setSamplerAudioFilePath(newFile);
                    }
                }
                
                //look for sequencer audio files
                else if (sceneData[sceneNumber]->getChildByName("PAD_DATA_"+String(i))->getIntAttribute("mode") == 3)
                {
                    for (int row = 0; row <= NO_OF_ROWS-1; row++)
                    {
                        File currentFile;
                        File newFile;
                        String fileName = sceneData[sceneNumber]->getChildByName("PAD_DATA_"+String(i))->getStringAttribute("sequencerSamplesAudioFilePath"+String(row));
                        
                        if (fileName != String::empty)
                        {
                            //Here, we are looking for external files. If we find one, we need to copy it
                            if (File::isAbsolutePath(fileName) == true)
                            {
                                //if we are looking for external files and the pads file is an external file...
                                currentFile = fileName;
                            }
                            else
                            {
                                //no match...
                                break;
                            }
                            
                            newFile = File::getCurrentWorkingDirectory().getFullPathName() + File::separatorString + currentFile.getFileName();
                            
                            if (newFile.existsAsFile() == false) //if it doesn't yet exist
                            {
                                //copy the added audio file into the newly created file
                                currentFile.copyFileTo(newFile);
                            }
                            
                            else if (newFile.existsAsFile() == true) //if it already exists 
                            {
                                Array<File> matchingFilesArray;
                                String fileWildCard (newFile.getFileNameWithoutExtension()+"*");
                                bool importedFileNeedsCopying = true;
                                
                                //Find all possible duplicates of the imported file using fileWildCard
                                //and add reference of all possible files to matchingFilesArray.
                                File::getCurrentWorkingDirectory().findChildFiles(matchingFilesArray, 2, false, fileWildCard);
                                
                                for (int i = 0; i < matchingFilesArray.size(); i++)
                                {
                                    if (currentFile.hasIdenticalContentTo(matchingFilesArray[i]) == true)
                                    {
                                        //if it finds a duplicate, flag that the file doesn't need copying
                                        importedFileNeedsCopying = false;
                                        //set the file
                                        newFile = matchingFilesArray[i];
                                        //break out of the for loop once a duplicate has been found to prevent unnecessary checks
                                        break;
                                    }
                                }
                                
                                if (importedFileNeedsCopying == true) 
                                {
                                    //if no duplicate was found...
                                    //... copy the added file with an appended name
                                    newFile = newFile.getNonexistentSibling();
                                    currentFile.copyFileTo(newFile); 
                                }
                                
                            }
                            
                            
                            sceneData[sceneNumber]->getChildByName("PAD_DATA_"+String(i))->setAttribute("sequencerSamplesAudioFilePath"+String(row), newFile.getFileName());
                            
                            if (sceneNumber == currentlySelectedScene)
                                AppSettings::Instance()->padSettings[i]->setSequencerSamplesAudioFilePath(newFile, row);
                        }
                    }
                }
            }
        }
    }
    
    //loadFromScene(currentlySelectedScene);
}

void AppDocumentState::registerRecentFile (const File& file)
{
    RecentlyOpenedFilesList::registerRecentFileNatively (file);
    StoredSettings::getInstance()->recentFiles.addFile (file);
    StoredSettings::getInstance()->flush();
}


void AppDocumentState::setCurrentlySelectedScene(int value)
{
    currentlySelectedScene = value;
    std::cout << "Currently Selected Scene = " << currentlySelectedScene << std::endl;
}


int AppDocumentState::getGuiUpdateFlag()
{
    return guiUpdateFlag;
}
int AppDocumentState::getSceneToUpdate()
{
    return sceneToUpdate;
}
int AppDocumentState::getSceneStatus()
{
    return sceneStatus;
}
