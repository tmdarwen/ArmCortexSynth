/*
 * ARM Cortex-M4F Synthesizer
 *
 * Copyright (c) 2017 Terence M. Darwen - tmdarwen.com
 *
 * The MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#pragma once

#include <stdint.h>

enum ChannelType
{
    PRIMARY,
    ALTERNATE
};

// Call this to initialize DMA for a specific channel/encoding
void InitializeDMAChannelAssignment(uint8_t channel, uint8_t encoding);

// Setup the control structure for the given channel
void SetChannelControlStructure(uint32_t channel, enum ChannelType channelType, uint32_t channelControlWord);

// Setup the source end address for DMA
void SetChannelSourceEndAddress(uint32_t channel, enum ChannelType channelType, uint32_t sourceAddressValue);

// Setup the destination end address for DMA
void SetChannelDestinationEndAddress(uint32_t channel, enum ChannelType channelType, uint32_t destinationAddressValue);

// After configuring the DMA channel call this to enable and start DMA
void EnableDMAChannel(uint8_t channel);

// While DMA is happening, call this to check for completion
bool CheckForTranferCompletion(uint32_t channel, enum ChannelType channelType);

// Reset DMA channel information after completion
void ReSetChannelControlStructureModeAndSize(uint32_t channel, enum ChannelType channelType, uint8_t xFerMode, uint32_t xFerSize);
