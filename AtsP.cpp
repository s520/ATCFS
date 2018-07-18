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
#include <algorithm>
#include <boost/container/vector.hpp>
#include <boost/array.hpp>
#include <boost/typeof/typeof.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/math/special_functions/round.hpp>
#include "atsplugin.h"
#include "AtsP.h"

Atsp::Atsp() {
    section_p_.reset(new SectionP(this));
    pattern_p_.reset(new PatternP(this));
}

Atsp::SectionP::SectionP(Atsp *atsp) {
    atsp_ = atsp;
}

Atsp::PatternP::PatternP(Atsp *atsp) {
    atsp_ = atsp;
}

Atsp::~Atsp() {
}

Atsp::SectionP::~SectionP() {
}

Atsp::PatternP::~PatternP() {
}

/// <summary>
/// Initializeで実行する関数
/// </summary>
void Atsp::InitP() {
    max_brake_notch_ = ServiceNotch - AtsNotch + 1;
    default_notch_ = boost::math::iround((atsp_deceleration_ / max_deceleration_) * max_brake_notch_);
    atsp_power_ = 1;
    pattern_list_.resize((atsp_max_spd_ + 1));
    SetPatternList();
    adj_deceleration_ = 0.0;
    for (int i = 0; i < ALL_PATTERN_P; i++) {
        pattern_end_loc_list_[i] = 0;
        pattern_is_valid_list_[i] = 0;
        pattern_tget_spd_list_[i] = 0;
        pattern_pre_spd_list_[i] = 0;
        pattern_aprch_list_[i] = 0;
    }
    debug_timer_ = 0;
    atsp_brake_notch_ = 0;
    atsp_aprch_lamp_ = 0;
    prev_aprch_lamp_ = 0;
    atsp_brake_lamp_ = 0;
    atsp_reset_lamp_ = 0;
    atsp_ding_ = ATS_SOUND_STOP;
    next_atsp_ding_ = ATS_SOUND_STOP;
    section_p_->Init();
    pattern_p_->Init();
}

/// <summary>
/// Initializeで実行する関数
/// </summary>
void Atsp::SectionP::Init() {
    red_signal_loc_ = 0.0f;
    is_immediate_stop_eb_ = 0;
    is_immediate_stop_svc_ = 0;
    is_immediate_stop_ = 0;
    is_stop_svc_ = 0;
    is__brake_reset_ = 0;
    pattern_end_loc_ = 0.0f;
    pattern_is_valid_ = 0;
    pattern_tget_spd_ = 0;
}

/// <summary>
/// Initializeで実行する関数
/// </summary>
void Atsp::PatternP::Init() {
    for (int i = 0; i < USR_PATTERN_P; i++) {
        pattern_end_loc_[i] = 0;
        pattern_is_valid_[i] = 0;
        pattern_tget_spd_[i] = 0;
    }
}

/// <summary>
/// 速度照査パターンを作成する関数
/// </summary>
void Atsp::SetPatternList() {
    for (int v = 0; v < atsp_max_spd_ + 1; v++) {
        pattern_list_[v] = ((v / 3.6) * (v / 3.6)) / (2.0 * (((max_deceleration_ / max_brake_notch_) * default_notch_) / 3.6 + adj_deceleration_));
    }
}

/// <summary>
/// ATS-Pを投入する際に実行する関数
/// </summary>
void Atsp::StartP() {
    if (atsp_use_ == 0) {
        InitP();
        atsp_use_ = 1;
        next_atsp_ding_ = ATS_SOUND_PLAY;  // ATS-Pベル
    }
}

/// <summary>
/// ATS-Pを遮断する際に実行する関数
/// </summary>
void Atsp::ExitP() {
    if (atsp_use_ == 1) {
        atsp_use_ = 0;
        InitP();
    }
}

/// <summary>
/// SetBeaconDataで実行される関数
/// </summary>
/// <param name="index">地上子種別</param>
/// <param name="signal">対となるセクションの信号番号</param>
/// <param name="distance">対となるセクションまでの距離[m]</param>
/// <param name="optional">地上子に設定された任意の値</param>
void Atsp::PassedBeaconP(int index, int signal, float distance, int optional) {
    switch (index) {
    case 3:
        section_p_->GetSection(distance);
        break;
    case 4:
        section_p_->PassedStopEb(distance);
        break;
    case 5:
        section_p_->PassedStopSvc(distance);
        break;
    case 6:
        pattern_p_->RegPattern(0, optional);
        break;
    case 7:
        pattern_p_->RegPattern(1, optional);
        break;
    case 8:
        pattern_p_->RegPattern(2, optional);
        break;
    case 9:
        pattern_p_->RegPattern(3, optional);
        break;
    case 10:
        pattern_p_->RegPattern(4, optional);
        break;
    case 16:
        pattern_p_->DelPattern(0);
        break;
    case 18:
        pattern_p_->DelPattern(2);
        break;
    case 19:
        pattern_p_->DelPattern(3);
        break;
    case 20:
        pattern_p_->DelPattern(4);
        break;
    case 203:
        AdjDeceleration(optional);
        break;
    default:
        break;
    }
}

/// <summary>
/// 停止信号地点を取得する関数
/// </summary>
/// <param name="distance">停止信号までの距離[m]</param>
void Atsp::SectionP::GetSection(float distance) {
    red_signal_loc_ = static_cast<float>(atsp_->Location + distance);
}

/// <summary>
/// 即時停止(非常)地上子を通過した際に実行する関数
/// </summary>
/// <param name="distance">停止信号までの距離[m]</param>
void Atsp::SectionP::PassedStopEb(float distance) {
    GetSection(distance);
    if (distance < 50.0f) {
        is_immediate_stop_eb_ = 1;
        is__brake_reset_ = 0;
    }
}

/// <summary>
/// 即時停止(常用)地上子を通過した際に実行する関数
/// </summary>
/// <param name="distance">停止信号までの距離[m]</param>
void Atsp::SectionP::PassedStopSvc(float distance) {
    GetSection(distance);
    if (distance < 50.0f) {
        is_immediate_stop_svc_ = 1;
        is__brake_reset_ = 0;
    }
}

/// <summary>
/// 停止信号パターンを生成する関数
/// </summary>
void Atsp::SectionP::CalcSection() {
    if (atsp_->Location <= red_signal_loc_) {
        if (is_immediate_stop_ == 1) {
            pattern_end_loc_ = red_signal_loc_ - 50.0f;
            pattern_is_valid_ = 1;
            pattern_tget_spd_ = 10;
        } else {
            pattern_end_loc_ = red_signal_loc_ - 10.0f;
            pattern_is_valid_ = 1;
            pattern_tget_spd_ = 10;
        }
    } else if (atsp_->Location > red_signal_loc_ && atsp_->Location <= (red_signal_loc_ + 50.0f)) {
        pattern_end_loc_ = red_signal_loc_;
        pattern_is_valid_ = 1;
        pattern_tget_spd_ = 15;
    } else {
        pattern_is_valid_ = 0;
        is_immediate_stop_ = 0;
        is__brake_reset_ = 0;
    }
}

/// <summary>
/// 速度制限パターンの登録を行う関数
/// </summary>
/// <param name="type">パターン番号</param>
/// <param name="optional">減速完了地点までの相対距離[m]*1000+目標速度[km/h]</param>
void Atsp::PatternP::RegPattern(int type, int optional) {
    int distance = optional / 1000;
    if (distance < 0) { distance = 0; }
    int tget_spd = optional % 1000;
    pattern_end_loc_[type] = static_cast<float>(atsp_->Location + distance);
    pattern_tget_spd_[type] = tget_spd;
    pattern_is_valid_[type] = 1;
}

/// <summary>
/// 速度制限パターンの消去を行う関数
/// </summary>
/// <param name="type">パターン番号</param>
void Atsp::PatternP::DelPattern(int type) {
    pattern_is_valid_[type] = 0;
}

/// <summary>
/// 勾配補正設定を行う関数
/// </summary>
/// <param name="decleration">減速度補正値[m/s^2]*1000</param>
void Atsp::AdjDeceleration(int decleration) {
    if (decleration > 0) {
        adj_deceleration_ = 0.0;
    } else if (decleration < -35) {
        adj_deceleration_ = -0.035;
    } else {
        adj_deceleration_ = decleration / 1000.0;
    }
    SetPatternList();
}

/// <summary>
/// 距離に対応する速度を返す関数
/// </summary>
/// <remarks>指定された距離の近似値に対応する速度を検索する</remarks>
/// <param name="distance">距離[m]</param>
/// <returns>距離に対応する速度[km/h]</returns>
int Atsp::SearchPattern(float distance) {
    int back_index = static_cast<int>(std::lower_bound(pattern_list_.begin() + 1, pattern_list_.end() - 1, distance) - pattern_list_.begin());
    int front_index = back_index - 1;

    double x = pattern_list_[front_index] - distance;
    double y = pattern_list_[back_index] - distance;

    int approx_index = 0;
    if (x * x < y * y) {
        approx_index = front_index;
    } else {
        approx_index = back_index;
    }
    return approx_index;
}

/// <summary>
/// パターンが無効の場合に目標速度を車両ATS-P最高速度に修正する関数
/// </summary>
/// <param name="tget_spd">目標速度[km/h]</param>
/// <param name="pattern_status">パターン状態(0: 無効, 1: 有効)</param>
void Atsp::ValidPattern(int& tget_spd, int pattern_status) {
    if (pattern_status == 0) { tget_spd = atsp_max_spd_; }
}

/// <summary>
/// パターン照査速度を算出する関数
/// </summary>
/// <remarks>減速完了地点内方は目標速度のフラットパターンとなる</remarks>
/// <param name="tget_spd">目標速度[km/h]</param>
/// <param name="pattern_end_loc">減速完了地点[m]</param>
/// <returns>パターン照査速度[km/h]</returns>
int Atsp::CalcPatternSpd(int tget_spd, float pattern_end_loc) {
    int pattern_spd = 0;
    if (pattern_end_loc <= Location) {
        if (tget_spd > atsp_max_spd_) { pattern_spd = atsp_max_spd_; } else if (tget_spd < 0) { pattern_spd = 0; } else { pattern_spd = tget_spd; }
    } else {
        pattern_spd = SearchPattern(static_cast<float>(VectorGetOrDefault(pattern_list_, tget_spd, LOGARGS) + pattern_end_loc - Location));
    }
    return pattern_spd;
}

/// <summary>
/// パターン接近警報を発信する関数
/// </summary>
/// <param name="tget_spd">目標速度[km/h]</param>
/// <param name="pattern_end_loc">減速完了地点[m]</param>
/// <returns>パターン接近警報の状態(0: 無効, 1: 有効)</returns>
int Atsp::IsAprchPattern(int tget_spd, float pattern_end_loc) {
    int aprch_pattern = 0;
    int aprchSpd1 = CalcPatternSpd(tget_spd - 5, pattern_end_loc - *TrainSpeed / 3.6f * 5.5f);
    int aprchSpd2 = CalcPatternSpd(tget_spd - 5, pattern_end_loc - 50.0f);
    if (abs(*TrainSpeed) >= aprchSpd1 || abs(*TrainSpeed) >= aprchSpd2) {
        aprch_pattern = 1;
    }
    return aprch_pattern;
}

/// <summary>
/// パターン情報を集約する関数
/// </summary>
void Atsp::CollectPattern() {
    boost::get<0>(pattern_tget_spd_list_) = section_p_->pattern_tget_spd_;
    boost::get<0>(pattern_end_loc_list_) = section_p_->pattern_end_loc_;
    boost::get<0>(pattern_is_valid_list_) = section_p_->pattern_is_valid_;
    for (int i = 0; i < USR_PATTERN_P; i++) {
        pattern_tget_spd_list_[1 + i] = pattern_p_->pattern_tget_spd_[i];
        pattern_end_loc_list_[1 + i] = pattern_p_->pattern_end_loc_[i];
        pattern_is_valid_list_[1 + i] = pattern_p_->pattern_is_valid_[i];
    }
    boost::get<6>(pattern_tget_spd_list_) = atsp_max_spd_;
    boost::get<6>(pattern_end_loc_list_) = 0.0f;
    boost::get<6>(pattern_is_valid_list_) = 1;
}

/// <summary>
/// 各パターン情報からパターン照査速度を算出し、パターン接近警報の状態を取得する関数
/// </summary>
void Atsp::CalcPattern() {
    for (int i = 0; i < ALL_PATTERN_P; i++) {
        ValidPattern(pattern_tget_spd_list_[i], pattern_is_valid_list_[i]);
        pattern_pre_spd_list_[i] = CalcPatternSpd(pattern_tget_spd_list_[i], pattern_end_loc_list_[i]);
        pattern_aprch_list_[i] = IsAprchPattern(pattern_tget_spd_list_[i], pattern_end_loc_list_[i]);
    }
}

/// <summary>
/// フラグのON, OFFを行う関数
/// </summary>
void Atsp::AtspCheck() {
    if (abs(*TrainSpeed) > boost::get<0>(pattern_pre_spd_list_) && boost::get<0>(pattern_pre_spd_list_) != atsp_max_spd_) {
        section_p_->is_stop_svc_ = 1;
        section_p_->is__brake_reset_ = 0;
    }
    if (section_p_->is_immediate_stop_eb_ == 1 || section_p_->is_immediate_stop_svc_ == 1) {
        section_p_->is_immediate_stop_ = 1;
    }
}

/// <summary>
/// 復帰扱いの判定を行う関数
/// </summary>
void Atsp::Reset() {
    if (*TrainSpeed == 0 && *ManualBrakeNotch >= ServiceNotch) {
        if (*ManualBrakeNotch >= EmergencyNotch && section_p_->is_immediate_stop_eb_ == 1) {
            section_p_->is_immediate_stop_eb_ = 0;
            section_p_->is__brake_reset_ = 1;
        }
        if (section_p_->is_immediate_stop_svc_ == 1) {
            section_p_->is_immediate_stop_svc_ = 0;
            section_p_->is__brake_reset_ = 1;
        }
        if (section_p_->is_stop_svc_ == 1) {
            section_p_->is_stop_svc_ = 0;
            section_p_->is__brake_reset_ = 1;
        }
    }
}

/// <summary>
/// 復帰ボタンが押下された際に実行する関数
/// </summary>
void Atsp::ResetSwDownP() {
    Reset();
}

/// <summary>
/// ブレーキノッチを出力する関数
/// </summary>
void Atsp::BrakeExe() {
    if (section_p_->is__brake_reset_ == 1) {
        atsp_brake_notch_ = 0;
    } else if (section_p_->is_immediate_stop_eb_ == 1) {
        atsp_brake_notch_ = max_brake_notch_ + 1;
    } else if (section_p_->is_immediate_stop_svc_ == 1 || section_p_->is_stop_svc_ == 1 || abs(*TrainSpeed) > *std::min_element(pattern_pre_spd_list_.begin(), pattern_pre_spd_list_.end())) {
        atsp_brake_notch_ = default_notch_;
    } else {
        atsp_brake_notch_ = 0;
    }
}

/// <summary>
/// ランプの点灯, 消灯を行う関数
/// </summary>
void Atsp::DisplayLamp() {
    if (*std::max_element(pattern_aprch_list_.begin(), pattern_aprch_list_.end()) == 1 || atsp_brake_notch_ != 0) {
        atsp_aprch_lamp_ = 1;
    } else {
        atsp_aprch_lamp_ = 0;
    }
    if (atsp_brake_notch_ != 0) {
        atsp_brake_lamp_ = 1;
    } else {
        atsp_brake_lamp_ = 0;
    }
    if (section_p_->is__brake_reset_ == 1) {
        atsp_reset_lamp_ = 1;
    } else {
        atsp_reset_lamp_ = 0;
    }

    // ATS-Pベル
    if (atsp_aprch_lamp_ != prev_aprch_lamp_) {
        next_atsp_ding_ = ATS_SOUND_PLAY;
        prev_aprch_lamp_ = atsp_aprch_lamp_;
    }
}

/// <summary>
/// サウンドを再生する関数
/// </summary>
void Atsp::PlaySound() {
    atsp_ding_ = next_atsp_ding_;
    next_atsp_ding_ = ATS_SOUND_CONTINUE;
}

/// <summary>
/// Elapseで実行する関数
/// </summary>
void Atsp::ExeP() {
    if (atsp_use_ == 0) {
        atsp_brake_notch_ = 0;
    } else {
        section_p_->CalcSection();
        CollectPattern();
        CalcPattern();
        AtspCheck();
        BrakeExe();
        DisplayLamp();
        PlaySound();

        // For debug
        if (*Time >= debug_timer_) {
            char debugstr[1000];
            BOOST_AUTO(iter, std::min_element(pattern_pre_spd_list_.cbegin(), pattern_pre_spd_list_.cend()));
            int index = static_cast<int>(std::distance(pattern_pre_spd_list_.cbegin(), iter));
            int tget_spd = pattern_tget_spd_list_[index];
            float pattern_end_loc = pattern_end_loc_list_[index];
            int pattern_pre_spd = pattern_pre_spd_list_[index];
            sprintf_s(debugstr, sizeof(debugstr), "Loc: %f / tget_spd: %d / PattEnd: %f / pattern_pre_spd: %d\n", Location, tget_spd, pattern_end_loc, pattern_pre_spd);
            OutputDebugString(debugstr);
            sprintf_s(debugstr, sizeof(debugstr), "出力B: B%d / AprchLamp: %d / BrakeLamp: %d / ResetLamp: %d\n", atsp_brake_notch_, atsp_aprch_lamp_, atsp_brake_lamp_, atsp_reset_lamp_);
            OutputDebugString(debugstr);
            debug_timer_ = *Time + 1000;
        }
    }
}
