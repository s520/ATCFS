// Copyright 2018 S520
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met :
//
// 1. Redistributions of source code must retain the above copyright notice,
// this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and / or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED.IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "stdafx.h"
#include "atsplugin.h"
#include "Sub.h"

Sub::Sub() {
}

Sub::~Sub() {
}

/// <summary>
/// Initializeで実行する関数
/// </summary>
void Sub::Init() {
    ac_voltage_ = 25;
    cv_voltage_ = 100;
}

/// <summary>
/// ATCブレーキの緩解音を再生する関数
/// </summary>
void Sub::PlayAtcAirSound() {
    brake_status_ = (*atc_brake_notch_ > 0) ? 1 : 0;
    if (brake_status_ != prev_brake_status_ && *atc_brake_notch_ == 0) {
        next_atc_air_sound_ = ATS_SOUND_PLAY;
    }
    prev_brake_status_ = brake_status_;
}

/// <summary>
/// LCD切り替えSWが押された際に呼び出される関数
/// </summary>
void Sub::LcdSwDown() {
    lcd_status_ = (lcd_status_ == 0) ? 1 : 0;
    next_lcd_sw_down_sound_ = ATS_SOUND_PLAY;
}

/// <summary>
/// 距離表示に加算する補正値を受け取る関数
/// </summary>
/// <param name="distance">加算距離[m]</param>
void Sub::SetAdjLoc(int distance) {
    if (distance > 0) {
        adj_loc_ = distance;
    }
}

/// <summary>
/// 1桁ごと分割された時刻を出力する関数
/// </summary>
void Sub::DisplayClock() {
    int hour = *Time / 3600000;
    int min = *Time / 60000 % 60;
    int sec = *Time / 1000 % 60;
    boost::get<0>(digital_clock_) = hour / 10;
    boost::get<1>(digital_clock_) = hour % 10;
    boost::get<2>(digital_clock_) = min / 10;
    boost::get<3>(digital_clock_) = min % 10;
    boost::get<4>(digital_clock_) = sec / 10;
    boost::get<5>(digital_clock_) = sec % 10;
}

/// <summary>
/// レバーサの位置を表示する関数
/// </summary>
void Sub::DisplayReverser() {
    switch (*Reverser) {
    case -1: reverser_postion_ = 2; break;
    case 1: reverser_postion_ = 1; break;
    default: reverser_postion_ = 0; break;
    }
}

/// <summary>
/// 0系/200系用速度計の針の表示を初期化する関数
/// </summary>
void Sub::ResetSpeedometer() {
    for (std::size_t i = 0; i < speedometer_.size(); i++) { speedometer_[i] = 11; }
}

/// <summary>
/// 0系/200系用速度計の針を表示する関数
/// </summary>
void Sub::RunSpeedometer() {
    int spd10 = static_cast<int>(abs(*TrainSpeed)) / 10;
    int spd1 = static_cast<int>(abs(*TrainSpeed)) % 10;
    if (spd10 > static_cast<int>(speedometer_.size() - 1)) {
        speedometer_.back() = 10;
    } else {
        speedometer_[spd10] = spd1;
    }
}

/// <summary>
/// サウンドを再生する関数
/// </summary>
void Sub::PlaySound() {
    atc_air_sound_ = next_atc_air_sound_;
    next_atc_air_sound_ = ATS_SOUND_CONTINUE;

    lcd_sw_down_sound_ = next_lcd_sw_down_sound_;
    next_lcd_sw_down_sound_ = ATS_SOUND_CONTINUE;
}

/// <summary>
/// Elapseで実行する関数
/// </summary>
void Sub::Exe() {
    PlayAtcAirSound();
    DisplayClock();
    DisplayReverser();
    ResetSpeedometer();
    RunSpeedometer();
    PlaySound();
}
