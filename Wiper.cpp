// Copyright 2014 Christopher Lees
// Copyright 2004 Oskari Saarekas
// Copyright 2018 S520
//
// UKTrainSYS public domain safety systems code by Anthony Bowden.
// OpenBVE public domain plugin template by Odyakufan / Michelle.
// OS_SZ_ATS derived code originally licenced under the GPL and
// relicenced with permission from Stefano Zilocchi.
// CAWS code based upon a public - domain template by Odyakufan.
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met :
//
// *Redistributions of source code must retain the above copyright notice, this
// list of conditions and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and / or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED.IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "stdafx.h"
#include "atsplugin.h"
#include "Wiper.h"

Wiper::Wiper() {
}

Wiper::~Wiper() {
}

void Wiper::Init() {
    wiper_current_position_ = wiper_hold_position_;
}

void Wiper::WiperRequest(int request) {
    if (request == 0 && wiper_speed_ <= 1) {
        wiper_speed_++;
        next_wiper_sw_sound_ = ATS_SOUND_PLAY;
    } else if (request == 1 && wiper_speed_ > 0) {
        wiper_speed_--;
        next_wiper_sw_sound_ = ATS_SOUND_PLAY;
    }
}

void Wiper::MoveWiper() {
    wiper_timer_ += *DeltaT;
    if (wiper_timer_ > (wiper_rate_ / 100)) {
        wiper_timer_ = 0;
        if (wiper_direction_ == 1) {
            wiper_current_position_++;
        } else {
            wiper_current_position_--;
        }
    }
}

void Wiper::Exe() {
    if (wiper_current_position_ > 0 && wiper_current_position_ < 100) {
        MoveWiper();
        wiper_held_timer_ = 0;
    } else if (wiper_current_position_ == 0 && wiper_current_position_ == wiper_hold_position_) {
        wiper_direction_ = 1;
        if (wiper_speed_ == 0) {
            wiper_held_ = true;
        } else if (wiper_speed_ == 1) {
            wiper_held_timer_ += *DeltaT;
            if (wiper_held_timer_ > wiper_delay_) {
                wiper_held_ = false;
                wiper_held_timer_ = 0;
            } else {
                wiper_held_ = true;
            }
        } else {
            wiper_held_ = false;
        }

        if (wiper_held_ == false) {
            MoveWiper();
        }
    } else if (wiper_current_position_ == 0 && wiper_current_position_ != wiper_hold_position_) {
        wiper_direction_ = 1;
        MoveWiper();
    } else if (wiper_current_position_ == 100 && wiper_current_position_ != wiper_hold_position_) {
        wiper_direction_ = -1;
        MoveWiper();
    } else if (wiper_current_position_ == 100 && wiper_current_position_ == wiper_hold_position_) {
        wiper_direction_ = -1;
        if (wiper_speed_ == 0) {
            wiper_held_ = true;
        } else if (wiper_speed_ == 1) {
            wiper_held_timer_ += *DeltaT;
            if (wiper_held_timer_ > wiper_delay_) {
                wiper_held_ = false;
                wiper_held_timer_ = 0;
            } else {
                wiper_held_ = true;
            }
        } else {
            wiper_held_ = false;
        }

        if (wiper_held_ == false) {
            MoveWiper();
        }
    } else {
        wiper_direction_ = 1;
        MoveWiper();
    }

    if (wiper_current_position_ == 1 && wiper_direction_ == 1) {
        if (wiper_hold_position_ == 0) {
            if (wiper_sound_behaviour_ == 0) {
                next_wiper_sound_ = ATS_SOUND_PLAY;
            }
        } else {
            if (wiper_sound_behaviour_ != 0) {
                next_wiper_sound_ = ATS_SOUND_PLAY;
            }
        }
    } else if (wiper_current_position_ == 99 && wiper_direction_ == -1) {
        if (wiper_hold_position_ == 0) {
            if (wiper_sound_behaviour_ != 0) {
                next_wiper_sound_ = ATS_SOUND_PLAY;
            }
        } else {
            if (wiper_sound_behaviour_ == 0) {
                next_wiper_sound_ = ATS_SOUND_PLAY;
            }
        }
    }

    wiper_sw_sound_ = next_wiper_sw_sound_;
    next_wiper_sw_sound_ = ATS_SOUND_CONTINUE;

    wiper_sound_ = next_wiper_sound_;
    next_wiper_sound_ = ATS_SOUND_CONTINUE;
}
