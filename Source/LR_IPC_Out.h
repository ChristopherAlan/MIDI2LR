#pragma once
/*
  ==============================================================================

    LR_IPC_OUT.h

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
#ifndef LR_IPC_OUT_H_INCLUDED
#define LR_IPC_OUT_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "MIDIProcessor.h"
#include "CommandMap.h"
#include "WeakHash.h"
#include <mutex>
#include <unordered_set>

/**********************************************************************************************//**
 * @class   LRConnectionListener
 *
 * @brief   A lr connection listener.
 *
 *
 *
 **************************************************************************************************/

class LRConnectionListener: public std::enable_shared_from_this<LRConnectionListener>
{
public:
    // sent when a connection to the LR plugin is made
    virtual void connected() = 0;

    // sent if disconnected from the LR plugin
    virtual void disconnected() = 0;

    virtual ~LRConnectionListener()
    {};
///< .
};

/**********************************************************************************************//**
 * @class   LR_IPC_OUT
 *
 * @brief   A lr ipc out.
 *
 *
 *
 **************************************************************************************************/

class LR_IPC_OUT: public InterprocessConnection,
    public MIDICommandListener,
    public AsyncUpdater,
    public Timer
{
public:
    LR_IPC_OUT();
    virtual ~LR_IPC_OUT()
    {};
// closes the socket
    void shutdown();

    void addListener(std::weak_ptr<LRConnectionListener> listener);

    // sends a command to the plugin
    void sendCommand(const String& command) const;

    // IPC interface
    virtual void connectionMade() override;
    virtual void connectionLost() override;
    virtual void messageReceived(const MemoryBlock& msg) override;

    // MIDICommandListener interface
    virtual void handleMidiCC(int midiChannel, int controller, int value) override;
    virtual void handleMidiNote(int midiChannel, int note) override;



    // AsyncUpdater interface
    virtual void handleAsyncUpdate() override;

    // Timer callback
    virtual void timerCallback() override;
    void Init(std::shared_ptr<CommandMap>&  mapCommand, std::shared_ptr<MIDIProcessor>&  midiProcessor);
private:
    const static unordered_map<String, KeyPress> KPMappings;
    std::shared_ptr<const CommandMap> m_commandMap;
    std::unordered_set<std::weak_ptr<LRConnectionListener>,MyWeakPtrHash<LRConnectionListener>,MyWeakPtrEqual<LRConnectionListener>> _listeners;
    int _valueToSend;
    String _commandToSend;
///< .
};


#endif  // LR_IPC_OUT_H_INCLUDED
