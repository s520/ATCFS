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

#ifndef SUB_H_
#define SUB_H_
#include <boost/array.hpp>

/// <summary>
/// その他機能を再現するクラス
/// </summary>
class Sub {
  private:
    int brake_status_;  //!< ATCブレーキの状態(0: 緩解, 1: 作動)
    int prev_brake_status_;  //!< 1フレーム前のATCブレーキの状態(0: 緩解, 1: 作動)
    int next_atc_air_sound_;  //!< 1フレーム後のATCブレーキの緩解音の状態
    int next_lcd_sw_down_sound_;  //!< 1フレーム後のLCD切り替えSWの押下音の状態
    int next_lcd_sw_up_sound_;  //!< 1フレーム後のLCD切り替えSWの開放音の状態
    int next_light_sw_down_sound_;  //!< 1フレーム後の手元灯SWの押下音の状態
    int next_light_sw_up_sound_;  //!< 1フレーム後の手元灯SWの開放音の状態

    void PlayAtcAirSound(void);
    void DisplayClock(void);
    void DisplayAnalogClock(void);
    void DisplayCurrent(void);
    void DisplayReverser(void);
    void ResetSpeedometer(void);
    void RunSpeedometer(void);
    void PlaySound(void);

  public:
    const float *TrainSpeed;  //!< 列車速度[km/h]
    const int *Time;  //!< ゲーム内時刻[ms]
    const int *Reverser;  //!< レバーサー位置(-1: 後, 0: 中立, 1: 前)
    const int *atc_brake_notch_;  //!< ATCブレーキ出力ノッチ
    float Current;  //!< 電流値[A]
    int current_negative_;  //!< 電流値[A]の負号
    boost::array<int, 4>current_list_;  //!< 1桁ごと表示する電流値[A]
    int ac_voltage_;  //!< 交流電圧
    int cv_voltage_;  //!< 制御電圧
    int atc_air_sound_;  //!< ATCブレーキの緩解音の状態
    int reverser_position_;  //!< レバーサー位置(0: 中立, 1: 前, 2: 後)
    int lcd_sw_;  //!< LCD切り替えSWの状態(0: 開放, 1: 押下)
    int lcd_status_;  //!< LCDの状態(0: 表示1, 1: 表示2)
    int lcd_sw_down_sound_;  //!< LCD切り替えSWの押下音の状態
    int lcd_sw_up_sound_;  //!< LCD切り替えSWの開放音の状態
    int light_sw_;  //!< 手元灯SWの状態(0: 開放, 1: 押下)
    int light_status_;  //!< 手元灯の状態(0: 消灯, 1: 点灯)
    int light_sw_down_sound_;  //!< 手元灯SWの押下音の状態
    int light_sw_up_sound_;  //!< 手元灯SWの開放音の状態
    boost::array<int, 6>digital_clock_;  //!< 1桁ごと表示するデジタル時計
    boost::array<int, 3>analog_clock_;  //!< アナログ時計
    boost::array<int, 28>speedometer_;  //!< 10km/h刻みの0系/200系用速度計の針
    int adj_loc_;  //!< 距離表示に加算する補正値[m]

    Sub(void);
    virtual ~Sub(void);
    void Init(void);
    void LcdSwDown(void);
    void LcdSwUp(void);
    void LightSwDown(void);
    void LightSwUp(void);
    void SetAdjLoc(int distance);
    void Exe(void);
};

#endif  // SUB_H_
