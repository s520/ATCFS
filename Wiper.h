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

#ifndef WIPER_H_
#define WIPER_H_

class Wiper {
 private:
    int next_wiper_sw_sound_;
    int wiper_timer_;
    int wiper_direction_;
    int wiper_held_timer_;
    bool wiper_held_;
    int next_wiper_sound_;

 public:
    int wiper_speed_;
    int wiper_sw_sound_;
    int wiper_current_position_;
    int *DeltaT;
    int wiper_rate_;
    int wiper_hold_position_;
    int wiper_delay_;
    int wiper_sound_behaviour_;
    int wiper_sound_;

    Wiper(void);
    virtual ~Wiper(void);
    void Init(void);
    void WiperRequest(int request);
    void MoveWiper();
    void Exe(void);
};

#endif  // WIPER_H_
