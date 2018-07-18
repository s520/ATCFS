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
#include "Accel.h"

Accel::Accel() {
}

Accel::~Accel() {
}

/// <summary>
/// Initializeで実行する関数
/// </summary>
void Accel::Init() {
    alpha_ = 0.2f;
}

/// <summary>
/// 列車の生の加速度を算出する関数
/// </summary>
void Accel::CalcRawAccel() {
    int delta_time = *Time - prev_time_;
    if (delta_time != 0) {
        raw_accel_ = (*TrainSpeed - prev_spd_) / delta_time * 1000;
    }
    prev_spd_ = *TrainSpeed;
    prev_time_ = *Time;
}

/// <summary>
/// 列車の指数移動平均加速度を算出する関数
/// </summary>
void Accel::CalcEmaAccel() {
    ema_accel_ = alpha_ * prev_raw_accel_ + (1.0f - alpha_) * ema_accel_;
    prev_raw_accel_ = raw_accel_;
}

/// <summary>
/// Elapseで実行する関数
/// </summary>
void Accel::Exe() {
    CalcRawAccel();
    CalcEmaAccel();
}
