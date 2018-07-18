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

#ifndef ACCEL_H_
#define ACCEL_H_

/// <summary>
/// 列車の加速度を算出するクラス
/// </summary>
class Accel {
 private:
    float prev_spd_;  //!< 1フレーム前の列車速度[km/h]
    int prev_time_;  //!< 1フレーム前のゲーム内時刻[ms]
    float prev_raw_accel_;  //!< 1フレーム前の生の加速度[km/h/s]
    float alpha_;  //!< 平滑化係数(0<α<1)

    void CalcRawAccel(void);
    void CalcEmaAccel(void);

 public:
    const float *TrainSpeed;  //!< 列車速度[km/h]
    const int *Time;  //!< ゲーム内時刻[ms]
    float raw_accel_;  //!< 生の加速度[km/h/s]
    float ema_accel_;  //!< 指数移動平均加速度[km/h/s]

    Accel(void);
    virtual ~Accel(void);
    void Init(void);
    void Exe(void);
};

#endif  // ACCEL_H_
