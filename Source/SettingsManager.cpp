/*
  ==============================================================================

    SettingsManager.cpp

This file is part of MIDI2LR. Copyright 2015-2016 by Rory Jaffe.

MIDI2LR is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later version.

MIDI2LR is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
MIDI2LR.  If not, see <http://www.gnu.org/licenses/>.
  ==============================================================================
*/
#include "SettingsManager.h"
#include "ProfileManager.h"

#ifndef _WIN32
//missing make_unique (C++14) in XCode
namespace std
{
    template<typename T, typename... Args>
    unique_ptr<T> make_unique(Args&&... args)
    {
        return unique_ptr<T>{new T{ forward<Args>(args)... }};
    }
}
#endif

//constexpr and auto don't work in XCode
const juce::String AutoHideSection{ "autohide" };

/**********************************************************************************************//**
 * @fn  SettingsManager::SettingsManager()
 *
 * @brief   Default constructor.
 *
 *
 **************************************************************************************************/

SettingsManager::SettingsManager(): m_lr_IPC_OUT{ nullptr }, m_profileManager{ nullptr }
{
    PropertiesFile::Options opts;
    opts.applicationName = "MIDI2LR";
    opts.commonToAllUsers = false;
    opts.filenameSuffix = "xml";
    opts.osxLibrarySubFolder = "Application Support/MIDI2LR";
    opts.storageFormat = PropertiesFile::storeAsXML;

    _propertiesFile = std::make_unique<PropertiesFile>(opts);

}

/**********************************************************************************************//**
 * @fn  void SettingsManager::setPickupEnabled(bool enabled)
 *
 * @brief   Sets pickup enabled.
 *
 *
 *
 * @param   enabled true to enable, false to disable.
 **************************************************************************************************/

void SettingsManager::setPickupEnabled(bool enabled)
{
    _propertiesFile->setValue("pickup_enabled", enabled);
    _propertiesFile->saveIfNeeded();

    auto command = String::formatted("Pickup %d\n", enabled);

    if (m_lr_IPC_OUT)
    {
        m_lr_IPC_OUT->sendCommand(command);
    }

}

/**********************************************************************************************//**
 * @fn  bool SettingsManager::getPickupEnabled() const noexcept
 *
 * @brief   Gets pickup enabled.
 *
 *
 *
 * @return  true if it succeeds, false if it fails.
 **************************************************************************************************/

bool SettingsManager::getPickupEnabled() const noexcept
{
    return _propertiesFile->getBoolValue("pickup_enabled", true);
}

/**********************************************************************************************//**
 * @fn  String SettingsManager::getProfileDirectory() const noexcept
 *
 * @brief   Gets profile directory.
 *
 *
 *
 * @return  The profile directory.
 **************************************************************************************************/

String SettingsManager::getProfileDirectory() const noexcept
{
    return _propertiesFile->getValue("profile_directory");
}

/**********************************************************************************************//**
 * @fn  void SettingsManager::setProfileDirectory(const String& profileDirStr)
 *
 * @brief   Sets profile directory.
 *
 *
 *
 * @param   profileDirStr   The profile dir string.
 **************************************************************************************************/

void SettingsManager::setProfileDirectory(const String& profileDirStr)
{
    _propertiesFile->setValue("profile_directory", profileDirStr);
    _propertiesFile->saveIfNeeded();

    if (m_profileManager)
    {
        File profileDir{ profileDirStr };
        m_profileManager->setProfileDirectory(profileDir);
    }
}

/**********************************************************************************************//**
 * @fn  void SettingsManager::connected()
 *
 * @brief   Connected this object.
 *
 *
 **************************************************************************************************/

void SettingsManager::connected()
{
    auto command = String::formatted("Pickup %d\n", getPickupEnabled());

    if (m_lr_IPC_OUT)
    {
        m_lr_IPC_OUT->sendCommand(command);
    }

}

/**********************************************************************************************//**
 * @fn  void SettingsManager::disconnected()
 *
 * @brief   Disconnect from the ed.
 *
 *
 **************************************************************************************************/

void SettingsManager::disconnected()
{

}

/**********************************************************************************************//**
 * @fn  int SettingsManager::getAutoHideTime() const
 *
 * @brief   Gets automatic hide time.
 *
 *
 *
 * @return  The automatic hide time.
 **************************************************************************************************/

int SettingsManager::getAutoHideTime() const noexcept
{
    return _propertiesFile->getIntValue(AutoHideSection, 0);

}

/**********************************************************************************************//**
 * @fn  void SettingsManager::setAutoHideTime(int newTime)
 *
 * @brief   Sets automatic hide time.
 *
 *
 *
 * @param   newTime The new time.
 **************************************************************************************************/

void SettingsManager::setAutoHideTime(int newTime)
{
    _propertiesFile->setValue(AutoHideSection, newTime);
    _propertiesFile->saveIfNeeded();

}

/**********************************************************************************************//**
 * @fn  int SettingsManager::getLastVersionFound() const
 *
 * @brief   Gets the last version found.
 *
 *
 *
 * @return  The last version found.
 **************************************************************************************************/

int SettingsManager::getLastVersionFound() const noexcept
{
    return _propertiesFile->getIntValue("LastVersionFound", 0);
}

/**********************************************************************************************//**
 * @fn  void SettingsManager::setLastVersionFound(int newversion)
 *
 * @brief   Sets last version found.
 *
 *
 *
 * @param   newversion  The newversion.
 **************************************************************************************************/

void SettingsManager::setLastVersionFound(int newversion)
{
    _propertiesFile->setValue("LastVersionFound", newversion);
    _propertiesFile->saveIfNeeded();
}

/**********************************************************************************************//**
 * @fn  void SettingsManager::Init(std::shared_ptr<LR_IPC_OUT>& lr_IPC_OUT, std::shared_ptr<ProfileManager>& profileManager)
 *
 * @brief   S.
 *
 *
 *
 * @param [in,out]  lr_IPC_OUT      If non-null, the lr ipc out.
 * @param [in,out]  profileManager  If non-null, manager for profile.
 **************************************************************************************************/

void SettingsManager::Init(std::shared_ptr<LR_IPC_OUT>& lr_IPC_OUT, std::shared_ptr<ProfileManager>& profileManager)
{
    m_lr_IPC_OUT = lr_IPC_OUT;

    if (m_lr_IPC_OUT)
    {
        // add ourselves as a listener to LR_IPC_OUT so that we can send plugin settings on connection
        m_lr_IPC_OUT->addListener(this);
    }

    m_profileManager = profileManager;

    if (m_profileManager)
    {
        // set the profile directory
        File profileDir{ getProfileDirectory() };
        profileManager->setProfileDirectory(profileDir);
    }

}