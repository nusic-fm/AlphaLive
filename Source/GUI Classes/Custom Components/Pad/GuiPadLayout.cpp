/*
 *  GuiPadLayout.cpp
 *  sdaJuce
 *
 *  Created by Sam Davies on 22/09/2011.
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
 */

#include "GuiPadLayout.h"
#include "../../Binary Data/PadBinaryData.h"
#include "../../Views/MainComponent.h"

GuiPadLayout::GuiPadLayout(AlphaLiveEngine &subject, MainComponent &ref)
                            :   Component ("GuiPadLayout"),
                                mSubject(subject), //set the classes subject to the alphaLiveEngine ref
                                mainComponentRef(ref) //take in a reference of MainComponent
                                

{
    
    //attach this class to the subject class
    mSubject.attach(this);
    
    //adds buttons to select rows using custom button GuiPadRow
    row6Sb =  new GuiPadRow(40);
    row5Sb =  new GuiPadRow(62);
    row4Sb =  new GuiPadRow(78);
    row3Sb =  new GuiPadRow(103);
    row2Sb =  new GuiPadRow(83);
    row1Sb =  new GuiPadRow(55);
    
    row1Sb->addListener(this);
	addAndMakeVisible(row1Sb);
	//row1Sb->toBehind(globalSb);
    row1Sb->addMouseListener(this, true);
	
	row2Sb->addListener(this);
	addAndMakeVisible(row2Sb);
	//row2Sb->toBehind(row1Sb);
    row2Sb->addMouseListener(this, true);
	
	row3Sb->addListener(this);
	addAndMakeVisible(row3Sb);
	//row3Sb->toBehind(row2Sb);
    row3Sb->addMouseListener(this, true);
	
	row4Sb->addListener(this);
	addAndMakeVisible(row4Sb);
	//row4Sb->toBehind(row3Sb);
    row4Sb->addMouseListener(this, true);
	
	row5Sb->addListener(this);
	addAndMakeVisible(row5Sb);
	//row5Sb->toBehind(row4Sb);
    row5Sb->addMouseListener(this, true);
	
	row6Sb->addListener(this);
	addAndMakeVisible(row6Sb);
	//row6Sb->toBehind(row5Sb);
    row6Sb->addMouseListener(this, true);
    
    globalSb = new GuiPadRow(58);
	globalSb->addListener(this);
	addAndMakeVisible(globalSb);
	//globalSb->toBehind(pads[0]);
    globalSb->addMouseListener(this, true);
	
	row7Sb = new GuiPadRow(65);
	row7Sb->addListener(this);
	addAndMakeVisible(row7Sb);
	//globalSb->toBehind(pads[0]);
    row7Sb->addMouseListener(this, true);
    
	
	//adds 48 instances of GuiPad component with toggle-state active
	for (int i = 0; i <= 47; i++) 
    {
		pads.insert(i, new GuiPad(i, *this));
		addAndMakeVisible(pads[i]);
		pads[i]->getButton()->setClickingTogglesState(true);
        pads[i]->addMouseListener(this, true);
	}
	
	//path for hit test - DO WE NEED A HIT TEST FOR THIS COMPONENT?
	hitPath.addEllipse (220.0f, 40.0f, 570.0f, 570.0f);
	
	pStore = 0;
	rotateValue = 0;
	pivotX = 0;
	pivotY = 0;
	s = 0;	
	
	sin1 = 0.3826834324;
	sin2 = 0.9238795325;
	sinN1 = -sin1;
	sinN2 = -sin2;
	
	sin3 = 0.7071067812;
	sinN3 = -sin3;
	
	sin4 = 0.2588190451;
	sin5 = 0.8660254038;
	sin6 = 0.9659258263;
	sin7 = 0.5;
	sinN4 = -sin4;
	sinN5 = -sin5;
	sinN6 = -sin6;
	sinN7 = -sin7;
	
	sin8 = 0.4539904997;
	sin9 = 0.9510565163;
	sin10 = 0.8910065242;
	sin11 = 0.3090169944;
	sinN8 = -sin8;
	sinN9 = -sin9;
	sinN10 = -sin10;
	sinN11 = -sin11;
	
	
	cos1 = sin2;
	cos2 = sin1;
	cosN1 = -sin2;
	cosN2 = -sin1;
	
	cos3 = sin3;
	cosN3 = -sin3;
	
	cos4 = -sin6;
	cos5 = -sin7;
	cos6 = -sin4;
	cos7 = -sin5;
	cosN4 = -cos4;
	cosN5 = -cos5;
	cosN6 = -cos6;
	cosN7 = -cos7;
	
	cos8 = -sin10;
	cos9 = -sin11;
	cos10 = -sin8;
	cos11 = -sin9;
	cosN8 = -cos8;
	cosN9 = -cos9;
	cosN10 = -cos10;
	cosN11 = -cos11;
	
    //trigger the global button being clicked to set a default set of selected pads!
	//globalSb->triggerClick();
    
    shouldDisplaySettings = false;
    //currentlySelectedPad = 99;
    selectedPads.clear(); //should I call clearQuick() instead?
	
}

GuiPadLayout::~GuiPadLayout()
{
	
	delete globalSb;
	delete row1Sb;
	delete row2Sb;
	delete row3Sb;
	delete row4Sb;
	delete row5Sb;
	delete row6Sb;
    delete row7Sb;
    
    //detach this class from the subject class
    mSubject.detach(this);

}


void GuiPadLayout::resized()
{
	
	setCentrePosition(384, 295);
	globalSb->centreWithSize(58, 58);
    
    row6Sb->centreWithSize(95, 95);
	row5Sb->centreWithSize(161, 161);
	row4Sb->centreWithSize(241, 241);
	row3Sb->centreWithSize(352, 352);
	row2Sb->centreWithSize(439, 439);
	row1Sb->centreWithSize(496, 496);
	row7Sb->centreWithSize(557, 557);
	
	
    //adds 48 pads using an equation that takes centre point, distance from centre and angle
    pads[45]->setBounds((((getWidth() *0.5) - 18) + (48 * sin1)), (((getHeight() *0.5) - 18) + (48 * cos1)) ,36, 36);
	pads[46]->setBounds((((getWidth() *0.5) - 18) + (48 * sin2)), (((getHeight() *0.5) - 18) + (48 * cos2)) ,36, 36);	
	pads[47]->setBounds((((getWidth() *0.5) - 18) + (48 * sin2)), (((getHeight() *0.5) - 18) + (48 * cosN2)) ,36, 36);
	pads[40]->setBounds((((getWidth() *0.5) - 18) + (48 * sin1)), (((getHeight() *0.5) - 18) + (48 * cosN1)) ,36, 36);	
	pads[41]->setBounds((((getWidth() *0.5) - 18) + (48 * sinN1)), (((getHeight() *0.5) - 18) + (48 * cosN1)) ,36, 36);
	pads[42]->setBounds((((getWidth() *0.5) - 18) + (48 * sinN2)), (((getHeight() *0.5) - 18) + (48 * cosN2)) ,36, 36);
	pads[43]->setBounds((((getWidth() *0.5) - 18) + (48 * sinN2)), (((getHeight() *0.5) - 18) + (48 * cos2)) ,36, 36);
	pads[44]->setBounds((((getWidth() *0.5) - 18) + (48 * sinN1)), (((getHeight() *0.5) - 18) + (48 * cos1)) ,36, 36);
	
	pads[36]->setBounds((((getWidth() *0.5) - 22) + (82 * sin3)), (((getHeight() *0.5) - 22) + (82 * cos3)) ,44, 44);
	pads[37]->setBounds((((getWidth() *0.5) - 22) + (82 * 1)), (((getHeight() *0.5) - 22) + (82 * 0)) ,44, 44);
	pads[38]->setBounds((((getWidth() *0.5) - 22) + (82 * sin3)), (((getHeight() *0.5) - 22) + (82 * cosN3)) ,44, 44);
	pads[39]->setBounds((((getWidth() *0.5) - 22) + (82 * 0)), (((getHeight() *0.5) - 22) + (82 * -1)) ,44, 44);
	pads[32]->setBounds((((getWidth() *0.5) - 22) + (82 * sinN3)), (((getHeight() *0.5) - 22) + (82 * cosN3)) ,44, 44);
	pads[33]->setBounds((((getWidth() *0.5) - 22) + (82 * -1)), (((getHeight() *0.5) - 22) + (82 * 0)) ,44, 44);
	pads[34]->setBounds((((getWidth() *0.5) - 22) + (82 * sinN3)), (((getHeight() *0.5) - 22) + (82 * cos3)) ,44, 44);
	pads[35]->setBounds((((getWidth() *0.5) - 22) + (82 * 0)), (((getHeight() *0.5) - 22) + (82 * 1)) ,44, 44);
	
	pads[27]->setBounds((((getWidth() *0.5) - 34) + (124 * sin2)), (((getHeight() *0.5) - 34) + (124 * cos2)) ,68, 68);
	pads[28]->setBounds((((getWidth() *0.5) - 34) + (124 * sin2)), (((getHeight() *0.5) - 34) + (124 * cosN2)) ,68, 68);
	pads[29]->setBounds((((getWidth() *0.5) - 34) + (124 * sin1)), (((getHeight() *0.5) - 34) + (124 * cosN1)) ,68, 68);
	pads[30]->setBounds((((getWidth() *0.5) - 34) + (124 * sinN1)), (((getHeight() *0.5) - 34) + (124 * cosN1)) ,68, 68);
	pads[31]->setBounds((((getWidth() *0.5) - 34) + (124 * sinN2)), (((getHeight() *0.5) - 34) + (124 * cosN2)) ,68, 68);
	pads[24]->setBounds((((getWidth() *0.5) - 34) + (124 * sinN2)), (((getHeight() *0.5) - 34) + (124 * cos2)) ,68, 68);
	pads[25]->setBounds((((getWidth() *0.5) - 34) + (124 * sinN1)), (((getHeight() *0.5) - 34) + (124 * cos1)) ,68, 68);
	pads[26]->setBounds((((getWidth() *0.5) - 34) + (124 * sin1)), (((getHeight() *0.5) - 34) + (124 * cos1)) ,68, 68);
	
	pads[18]->setBounds((((getWidth() *0.5) - 42) + (179 * 1)), (((getHeight() *0.5) - 42) + (179 * 0)) ,84, 84);
	pads[19]->setBounds((((getWidth() *0.5) - 43) + (179 * sin3)), (((getHeight() *0.5) - 43) + (179 * cosN3)) ,84, 84);
	pads[20]->setBounds((((getWidth() *0.5) - 43) + (179 * 0)), (((getHeight() *0.5) - 43) + (179 * -1)) ,84, 84);
	pads[21]->setBounds((((getWidth() *0.5) - 43) + (179 * sinN3)), (((getHeight() *0.5) - 43) + (179 * cosN3)) ,84, 84);
	pads[22]->setBounds((((getWidth() *0.5) - 43) + (179 * -1)), (((getHeight() *0.5) - 43) + (179 * 0)) ,84, 84);
	pads[23]->setBounds((((getWidth() *0.5) - 43) + (179 * sinN3)), (((getHeight() *0.5) - 43) + (179 * cos3)) ,84, 84);
	pads[16]->setBounds((((getWidth() *0.5) - 43) + (179 * 0)), (((getHeight() *0.5) - 43) + (179 * 1)) ,84, 84);
	pads[17]->setBounds((((getWidth() *0.5) - 43) + (179 * sin3)), (((getHeight() *0.5) - 43) + (179 * cos3)) ,84, 84);
	
	pads[9]->setBounds((((getWidth() *0.5) - 34) + (217 * sin2)), (((getHeight() *0.5) - 34) + (217 * cosN2)) ,68, 68);
	pads[10]->setBounds((((getWidth() *0.5) - 34) + (217 * sin1)), (((getHeight() *0.5) - 34) + (217 * cosN1)) ,68, 68);
	pads[11]->setBounds((((getWidth() *0.5) - 34) + (217 * sinN1)), (((getHeight() *0.5) - 34) + (217 * cosN1)) ,68, 68);
	pads[12]->setBounds((((getWidth() *0.5) - 34) + (217 * sinN2)), (((getHeight() *0.5) - 34) + (217 * cosN2)) ,68, 68);
	pads[13]->setBounds((((getWidth() *0.5) - 34) + (217 * sinN2)), (((getHeight() *0.5) - 34) + (217 * cos2)) ,68, 68);
	pads[14]->setBounds((((getWidth() *0.5) - 34) + (217 * sinN1)), (((getHeight() *0.5) - 34) + (217 * cos1)) ,68, 68);
	pads[15]->setBounds((((getWidth() *0.5) - 34) + (217 * sin1)), (((getHeight() *0.5) - 34) + (217 * cos1)) ,68, 68);
	pads[8]->setBounds((((getWidth() *0.5) - 34) + (217 * sin2)), (((getHeight() *0.5) - 34) + (217 * cos2)) ,68, 68);
    
	pads[0]->setBounds((((getWidth() *0.5) - 22) + (250 * sin3)), (((getHeight() *0.5) - 22) + (250 * cosN3)) ,44, 44);
	pads[1]->setBounds((((getWidth() *0.5) - 22) + (250 * 0)), (((getHeight() *0.5) - 22) + (250 * -1)) ,44, 44);
	pads[2]->setBounds((((getWidth() *0.5) - 22) + (250 * sinN3)), (((getHeight() *0.5) - 22) + (250 * cosN3)) ,44, 44);
	pads[3]->setBounds((((getWidth() *0.5) - 22) + (250 * -1)), (((getHeight() *0.5) - 22) + (250 * 0)) ,44, 44);
	pads[4]->setBounds((((getWidth() *0.5) - 22) + (250 * sinN3)), (((getHeight() *0.5) - 22) + (250 * cos3)) ,44, 44);
	pads[5]->setBounds((((getWidth() *0.5) - 22) + (250 * 0)), (((getHeight() *0.5) - 22) + (250 * 1)) ,44, 44);
	pads[6]->setBounds((((getWidth() *0.5) - 22) + (250 * sin3)), (((getHeight() *0.5) - 22) + (250 * cos3)) ,44, 44);
	pads[7]->setBounds((((getWidth() *0.5) - 22) + (250 * 1)), (((getHeight() *0.5) - 22) + (250 * 0)) ,44, 44);
	
}


//observers update function, called everytime an OSC message is recieved
bool GuiPadLayout::update(const Subject& theChangedSubject)
{
    if (&theChangedSubject == &mSubject)
    {
        if (mSubject.getGuiUpdateFlag() == 0) //update gradient from pressure
        {
            //if 'show pad settings when pressed' control is set to on, the pad settings are currently
            //not being displayed, display current settings of the pressed pad
            /*
            if (shouldDisplaySettings == true)
            {
                
                if (pStore != mSubject.getRecievedPad()) 
                {
                    turnOff(pStore);
                    pStore = mSubject.getRecievedPad();
                    mainComponentRef.setCurrentlySelectedPad(pStore);
                
                    pads[mSubject.getRecievedPad()]->turnOn();
                    pads[mSubject.getRecievedPad()]->toggleChange();
                }
            }
             */
            
            //update gradient
            pads[mSubject.getRecievedPad()]->setGradient(mSubject.getRecievedValue());
            
            
        }
        else if (mSubject.getGuiUpdateFlag() == 1) //update pad playing status visual signifier
        {
            pads[mSubject.getPadNumberForPlayingStatus()]->setPadPlayingState(mSubject.getPlayingStatus());
            mSubject.setGuiUpdateFlag(0);   // this is needed so that the pad gradient can be set to 0
                                            // when the playing status is being changed at the same time.
                                            // Without this line the pad will hang orange.
            //However is strange how this doesn't apply when using alt-click to emulate mouse clicks!!

        }
        
    }
    
    return true;
}


void GuiPadLayout::rotated(int val)
{
	//function is called once the pad rotate slider drag has ended to rotate individual pads to original angle 
	rotateValue = val;
	rotateValue = -rotateValue;
	repaint();
}

void GuiPadLayout::paint (Graphics& g)
{
		
	if (s == 1) 
    {
		//used to create image snapshot of component and children
		paintEntireComponent(g, false);
	}
	
	for (int i = 0; i <= 47; i++) 
    {
		//variables to find centre point of individual pads
		pivotX = pads[i]->getX() + (pads[i]->getWidth() * 0.5);
		pivotY = pads[i]->getY() + (pads[i]->getHeight() * 0.5);
		
		//transformation to rotate pads back to original angle
		pads[i]->setTransform (AffineTransform::rotation ((float) (rotateValue / (180.0 / double_Pi)), pivotX , pivotY));	
		
    }
	
	
	
}


bool GuiPadLayout::hitTest (int x, int y)
{
    //Do we need a custom hit test for this component? - CPU usage is minimal (if at all different)
    //all the child components have their own custom hit tests,
    //which should be enough.
    
	//returns true if mouse is within path hitpath
	return hitPath.contains(x, y);

}

 
void GuiPadLayout::buttonClicked(Button *button)
{
    //get modifier key so we can handle cmd-clicks and shift-clicks when selecting pads
    ModifierKeys modifier = ModifierKeys::getCurrentModifiers();
    
    //if a regular click is performed, clear the selectedPads array and clear all pads
    if (modifier.isCommandDown() == false && modifier.isShiftDown() == false)
    {
        selectedPads.clear(); //should I call clearQuick() instead?
        for (int i = 0; i <=47; i++)
            turnOff(i);
    }
	
    //individual pads
    for (int i = 0; i <=47; i++)
    {
        if (button == pads[i]->getButton())
        {
            //if shift is down....
            if (modifier.isShiftDown() == true)
            {
                //select and turn on all the pads within the range if the 
                //newly selected pad and the last selected pad
                
                int newPad = i;
                int lastPad = selectedPads.getLast(); //this will return 0 if the array is empty (which is also a pad number),
                                                        //but the array should never be empty at this point, right?
                
                if (newPad > lastPad)
                {
                    for (int i = lastPad; i <= newPad; i++)
                    {
                        selectedPads.addIfNotAlreadyThere(i);
                        turnOn(i); 
                    }
                }
                else if (newPad < lastPad)
                {
                    for (int i = lastPad; i >= newPad; i--)
                    {
                        selectedPads.addIfNotAlreadyThere(i);
                        turnOn(i); 
                    }
                }
                //if newPad == lastPad, do nothing as that pad would have already been selected
            }
            //else, cmd-click or regular click...
            else
            {
                if (modifier.isCommandDown() == true && selectedPads.contains(i))
                {
                    //if cmd-click and pad is already selected, unselected
                    selectedPads.removeFirstMatchingValue(i);
                    turnOff(i);
                }
                else
                {
                    //select and turn on pad
                    selectedPads.addIfNotAlreadyThere(i);
                    turnOn(i);
                }
            }
            
            break; // so it doesn't check for other pads afterwards
        }
    }
    
	
    
    //row buttons...
	if (button == row1Sb || button == row2Sb || button == row3Sb || button == row4Sb ||button == row5Sb || button == row6Sb) 
    {
        //for shift click stuff
        int lastPad = selectedPads.getLast();
        int newPad;
        
        int row; 
        
        //get row based on clicked button...
        if (button == row1Sb)
            row = 1;
        else if (button == row2Sb)
            row = 2;
        else if (button == row3Sb)
            row = 3;
        else if (button == row4Sb)
            row = 4;
        else if (button == row5Sb)
            row = 5;
        else if (button == row6Sb)
            row = 6;
               
        //select and turn on pads is selected row
        for(int i = (row*8)-8; i <= (row*8)-1; i++) 
        {
            if (modifier.isCommandDown() == true && selectedPads.contains(i))
            {
                selectedPads.removeFirstMatchingValue(i);
                turnOff(i);
                newPad = i; //for shift-click stuff
            }
            else
            {
                selectedPads.addIfNotAlreadyThere(i);
                turnOn(i);
                newPad = i;
            }
        }
        
        
        //if shift is down....
        if (modifier.isShiftDown() == true)
        {
            if (newPad > lastPad)
            {
                for (int i = lastPad; i <= newPad; i++)
                {
                    selectedPads.addIfNotAlreadyThere(i);
                    turnOn(i); 
                }
            }
            else if (newPad < lastPad)
            {
                for (int i = lastPad; i >= newPad; i--)
                {
                    selectedPads.addIfNotAlreadyThere(i);
                    turnOn(i); 
                }
            }
        }
	}
    
    
    //global button...
	else if (button == globalSb)
    {
		for (int i = 0; i <=47; i++) 
        {
            if (modifier.isCommandDown() == true && selectedPads.contains(i))
            {
                selectedPads.removeFirstMatchingValue(i);
                turnOff(i);
            }
            else
            {
                selectedPads.addIfNotAlreadyThere(i);
                turnOn(i);
            }
		}
	}
	
    //deselect all row button...
    else if (button == row7Sb)
    {
        //Is there any logic to having cmd-click and shift-click actions here?
        
        selectedPads.clear(); //should I call clearQuick() instead?
        for (int i = 0; i <=47; i++)
            turnOff(i);
    }
    
    
    
    if (selectedPads.size() > 0)
    {
        //pass on the selected pads to mainComponent, which then passes it further into the application..
        mainComponentRef.setCurrentlySelectedPad(selectedPads);
    }
	
}

void GuiPadLayout::turnOn(int pad)
{
    pads[pad]->turnOn();
    pads[pad]->toggleChange();
}

void GuiPadLayout::turnOff(int pad)
{
    pads[pad]->turnOff();
    pads[pad]->toggleChange();
}


void GuiPadLayout::modeChange(int padNumber, int modeNumber)
{
    pads[padNumber]->modeChange(modeNumber);
}


Image GuiPadLayout::snapshot()
{
	//returns an image of a complete snapshot of this component, including children
	
	s = 1;
	repaint();
	s = 0;
	snap = createComponentSnapshot(Rectangle<int>(0, 0, getWidth(), getHeight()), true);
	return snap;
	
}

/*
MainComponent& GuiPadLayout::getMainComponent()
{
    return mainComponentRef;
}
 */

AlphaLiveEngine& GuiPadLayout::getAlphaLiveEngine()
{
    return mSubject;
}

MainComponent& GuiPadLayout::getMainComponent()
{
    return mainComponentRef;
}

void GuiPadLayout::updateCurrentlySelectedPad()
{
    mainComponentRef.setCurrentlySelectedPad(selectedPads);
    
}

void GuiPadLayout::setShouldDisplaySettings (bool value)
{
    shouldDisplaySettings = value;
}

void GuiPadLayout::mouseEnter (const MouseEvent &e)
{
    for (int i = 0; i <= 47; i++) 
    {
        if (pads[i]->isMouseOver(true))
        {
            mainComponentRef.setInfoTextBoxText("Pad " + String(i+1) + ". Click here to view and edit the settings of this pad. Right-click to view copy & paste options. Alt-Click and drag to emulate a pad press. Drag audio files here for the Sampler Mode. The outer-ring colour signifies the pad's mode.");
        }
    }
    
    if (globalSb->isMouseOver(true))
    {
        mainComponentRef.setInfoTextBoxText("Select All Pads. Click here to apply settings to all of the pads on the AlphaSphere.");
    }
    else if (row1Sb->isMouseOver(true))
    {
        mainComponentRef.setInfoTextBoxText("Select Row 1. Click here to apply settings to the pads on row 1 (bottom row) of the AlphaSphere.");
    }
    else if (row2Sb->isMouseOver(true))
    {
        mainComponentRef.setInfoTextBoxText("Select Row 2. Click here to apply settings to the pads on row 2 (second from bottom row) of the AlphaSphere.");
    }
    else if (row3Sb->isMouseOver(true))
    {
        mainComponentRef.setInfoTextBoxText("Select Row 3. Click here to apply settings to the pads on row 3 (bottom-middle row) of the AlphaSphere.");
    }
    else if (row4Sb->isMouseOver(true))
    {
        mainComponentRef.setInfoTextBoxText("Select Row 4. Click here to apply settings to the pads on row 4 (top-middle row) of the AlphaSphere.");
    }
    else if (row5Sb->isMouseOver(true))
    {
        mainComponentRef.setInfoTextBoxText("Select Row 5. Click here to apply settings to the pads on row 5 (second from top row) of the AlphaSphere.");
    }
    else if (row6Sb->isMouseOver(true))
    {
        mainComponentRef.setInfoTextBoxText("Select Row 6. Click here to apply settings to the pads on row 6 (top row) of the AlphaSphere.");
    }
       
    
}

void GuiPadLayout::mouseExit (const MouseEvent &e)
{
    //remove any text
    mainComponentRef.setInfoTextBoxText (String::empty); 
}




void GuiPadLayout::copyPadSettings()
{
    //this function should only be called if a single pad is selected
    pads[selectedPads[0]]->copyPadSettings();
}


void GuiPadLayout::pastePadSettings()
{
    //this function should only be called if a single pad is selected
    pads[selectedPads[0]]->pastePadSettings();
}


