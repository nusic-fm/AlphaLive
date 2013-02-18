//
//  HidComms.cpp
//  AlphaLive
//
//  Created by Liam Meredith-Lacey on 08/11/2012.
//  Copyright 2012 nu desine.
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

#include "HidComms.h"


HidComms::HidComms() : Thread("HidThread")
{
    appHasInitialised = false;
    sendOutputReport = false;
    midiOutExists = hidDeviceExists =  false;
    
    startThread();
}

HidComms::~HidComms()
{
    stopThread(1500);
    
    hid_close(handle);
    hid_exit();
}

void HidComms::run()
{
    handle = nullptr;
    res = 0;
    
    while( ! threadShouldExit())
    {
        
        //=== if device is connected ===
        if (handle)
        {
            res = hid_read(handle, buf, sizeof(buf));
            
            //std::cout << res << std::endl;
            
            if (res == 0)
            {
                //printf("no report...\n");
            }
            if (res < 0)
            {
                //printf("Unable to read()\n");
                
                //if this statement is entered does it always means that
                //the device has been disconnected?
                handle = nullptr;
                hidDeviceExists = false;
            }
            
            if (res > 0)
            {
                //            for (int i = 0; i < res; i++)
                //                printf("%02hhx ", buf[i]);
                //            printf("\n");
                
                //encode the recieved command byte here based on the report ID
                if (appHasInitialised == true)
                {
                    if (buf[0] == 0x01) //pad data report
                    {
                        unsigned short int pressure = 0;
                        pressure = buf[2] + (buf[3]<<8);
                        
                        hidInputCallback(buf[1], pressure, buf[4]);
                        
                        
//                        for (int i = 0; i < 48; i++)
//                        {
//                            unsigned short int padNum = buf[1+(i*4)];
//                            unsigned short int pressure = buf[2+(i*4)] + (buf[3+(i*4)]<<8);
//                            unsigned short int padVelocity = buf[4+(i*4)];
//                            
//                            //need to check here if the previous pressure value of said pad
//                            //and only call the below line if the pressure has changed
//                            hidInputCallback(padNum, pressure, padVelocity);
//                        }
                        
                    }
                    
                    //The elite dials and buttons could probably use a the same
                    //command ID now we're not using such a specific report descriptor.
                    
                    else if (buf[0] == 0x02) //elite button report
                    {
                        //set 'pad' value to be 102-104 to represent the elite buttons
                        hidInputCallback(buf[1]+102, buf[2], 0);
                    }
                    else if (buf[0] == 0x03) //elite dial
                    {
                        //set 'pad' value to be 100-101 to represent the elite dials
                        hidInputCallback(buf[1]+100, buf[2], 0);
                    }
                    
                }
                
                memset(buf,0,sizeof(buf));
                
            }
            
            //what should the following sleep value be?
            #ifdef WIN32
            sleep(1); //should this actually be Sleep() which need a windows library defined? See hidtest.
            #else
            usleep(1*1000);
            #endif
        }
        
        //=== if device is not currently connected ===
        else
        {
            //try and connect to the device
            connectToDevice();
            
            //std::cout << "no device connected" << std::endl;
            //what should the following sleep value be?
            #ifdef WIN32
            sleep(1000); //should this actually be Sleep() which need a windows library defined? See hidtest.
            #else
            usleep(1000*1000);
            #endif
        }
        
        //std::cout << "listening... ";
    }
}

//should i be passing in a pointer here instead of an array?
void HidComms::sendHidControlReport (uint8 *bytesToSend)
{

    if (handle)
    {
//        std::cout << "writing to device: ";
//        printf("%02hhx ", bytesToSend[0]);
//        printf("%02hhx ", bytesToSend[1]);
//        printf("%02hhx ", bytesToSend[2]);
//        printf("%02hhx ", bytesToSend[3]);
//        printf("%02hhx ", bytesToSend[4]);
//        printf("\n");
        hid_write(handle, bytesToSend, 9);
    }
}



void HidComms::connectToDevice()
{
    struct hid_device_info *devs, *cur_dev;
    
    devs = hid_enumerate(0x1d50, 0x6021);
    cur_dev = devs;	
    while (cur_dev) 
    {
        printf("Device Found\n  type: %04hx %04hx\n  path: %s\n  serial_number: %ls", cur_dev->vendor_id, cur_dev->product_id, cur_dev->path, cur_dev->serial_number);
        printf("\n");
        printf("  Manufacturer: %ls\n", cur_dev->manufacturer_string);
        printf("  Product:      %ls\n", cur_dev->product_string);
        printf("  Release:      %hx\n", cur_dev->release_number);
        printf("  Interface:    %d\n",  cur_dev->interface_number);
        printf("\n");
        cur_dev = cur_dev->next;
    }
    hid_free_enumeration(devs);
    
    // Open the device using the VID, PID,
    // and optionally the Serial number.
    //handle = hid_open(0x3eb, 0x204f, NULL); // << LUFA demo HID device
    handle = hid_open(0x1d50, 0x6021, NULL); // << AlphaSphere HID device
    //handle = hid_open(0x1d50, 0x6041, NULL); // << AlphaSphere bootloader HID device 
    
    //device not found
    if (!handle) 
    {
        //printf("unable to open device\n");
        
        if (appHasInitialised == false)
        {
            // if appliication is currently initialising, flag that
            // the midi output stuff will exist. Setup of midi output
            // stuff cannot be called from here as at this point the
            // AlphaLiveEngine won't exist.
            
            midiOutExists = true;
        }
        
        hidDeviceExists = false;
    }
    
    //device found
    else
    {
        if (midiOutExists == true)
        {
            //if the midi output stuff currently exists, which would have been caused
            //by the application initialising without the hid device connected,
            //remove it.
            removeMidiOut();
            midiOutExists = false;
        }
        
        hidDeviceExists = true;
        memset(buf,0,sizeof(buf));
        
        //===============================================================================
        //Send a report to the device requesting a report containing AlphaLive setup data,
        //and then process the received reports data to set up AlphaLive correctly.
        //===============================================================================
        
        // Set the hid_read() function to be blocking to start with.
        hid_set_nonblocking(handle, 0);
        res = 0;
        
        unsigned char dataToSend[1];
        dataToSend[0] = 0x05; //host setup data request command ID
        hid_write(handle, dataToSend, 9);
        
        //TEMPORARILY COMMENTED OUT TO PREVENT PAUSING FOR THE TIME BEING
        //res = hid_read(handle, buf, sizeof(buf));
        int uncommentThisLine;
        
        if (res > 0 && buf[0] == 0x04)
        {
            std::cout << "Received AlphaLive setup report with the following data: " << std::endl;
            for (int i = 0; i < res; i++)
                printf("%02hhx ", buf[i]);
            printf("\n");
            
            //========process received report data here...========
            
            //check to see if the firmware needs updating
            File appDataDir(File::getSpecialLocation(File::currentApplicationFile).getParentDirectory().getFullPathName() + File::separatorString + "Application Data");
            String wildcard = "SphereWare*";
            Array<File> hexFile;
            appDataDir.findChildFiles(hexFile, 2, false, wildcard);
            
            if (hexFile.size() > 0) //which it should
            {
                int currentFirmwareNo = buf[1];
                int newFirmwareNo = hexFile.getLast().getFileNameWithoutExtension().getTrailingIntValue();
                
                std::cout << hexFile[0].getFileNameWithoutExtension() << " " << newFirmwareNo << std::endl;
                
                //if new firware version number is greater than current firmware number,
                //flag that the firmware needs updating. 
                if (newFirmwareNo > currentFirmwareNo)
                {
                    //On app launch the updateFirmware function is called from main.
                    //Can't be called from here at launch as the mainWindow/Component needs
                    //to be present, so need to just set a flag here. 
                    //However if we are current not at app launch, it is
                    //possible to call it directly from here.
                    
                    setFirmwareUpdateStatus (true);
                    
                    if (appHasInitialised == true)
                    {
                        updateFirmware();
                    }
                }
            }
            
            //set AlphaSphere device type
            setDeviceType (buf[2] + 1);
        }
                
        // Set the hid_read() function to be non-blocking.
        hid_set_nonblocking(handle, 1);
    }
}


bool HidComms::hasOpenedHidDevice()
{
    return hidDeviceExists;
}

void HidComms::setAppHasInitialised()
{
    appHasInitialised = true;
}
