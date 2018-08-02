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
#include <float.h>
#include <algorithm>
#include <cmath>
#include <boost/container/vector.hpp>
#include <boost/array.hpp>
#include <boost/typeof/typeof.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/math/special_functions/round.hpp>
#include "atsplugin.h"
#include "Atc.h"

Atc::Atc() {
    atc_a_.reset(new AtcA(this));
    atc_d_.reset(new AtcD(this));
    fuzzy_.reset(new Fuzzy(this));
}

Atc::AtcA::AtcA(const Atc *atc) {
    atc_ = atc;
}

Atc::AtcD::AtcD(const Atc *atc) {
    atc_ = atc;
    section_d_.reset(new SectionD(atc));
    station_d_.reset(new StationD(atc));
    pattern_d_.reset(new PatternD(atc));
}

Atc::AtcD::SectionD::SectionD(const Atc *atc) {
    atc_ = atc;
}

Atc::AtcD::StationD::StationD(const Atc *atc) {
    atc_ = atc;
}

Atc::AtcD::PatternD::PatternD(const Atc *atc) {
    atc_ = atc;
}

Atc::Fuzzy::Fuzzy(const Atc *atc) {
    atc_ = atc;
}

Atc::~Atc() {
}

Atc::AtcA::~AtcA() {
}

Atc::AtcD::~AtcD() {
}

Atc::AtcD::SectionD::~SectionD() {
}

Atc::AtcD::StationD::~StationD() {
}

Atc::AtcD::PatternD::~PatternD() {
}

Atc::Fuzzy::~Fuzzy() {
}

/// <summary>
/// Initializeで実行する関数
/// </summary>
void Atc::InitNS() {
    max_brake_notch_ = ServiceNotch - AtsNotch + 1;
    for (int i = 0; i < BRAKE_STEP; i++) {
        default_notch_[i] = boost::math::iround((atc_deceleration_[i] / max_deceleration_) * max_brake_notch_);
    }
    atc_power_ = 1;
    max_signal_ = SearchSignal(atc_max_spd_);
    pattern_list_.resize((atc_max_spd_ + 1));
    SetPatternList();
    for (int i = 0; i < ALL_PATTERN; i++) {
        pattarn_end_loc_list_[i] = 0;
        pattern_is_valid_list_[i] = 0;
        pattern_tget_spd_list_[i] = 0;
        pattern_arrow_spd_list_[i] = 0;
        pattern_start_loc_list_[i] = 0;
    }
    brake_notch_ = 0;
    debug_timer_ = 0;
    brake_timer_ = 0;
    atc_reset_sw_ = 0;
    atc_red_signal_ = 0;
    atc_green_signal_ = 0;
    atc_eb_lamp_ = 0;
    atc_svc_lamp_ = 0;
    atc_brake_notch_ = 0;
    atc_spd_ = 0;
    atc_signal_index_ = 0;
    for (std::size_t i = 0; i < atc_sig_indicator_.size(); i++) {
        atc_sig_indicator_[i] = 0;
    }
    for (std::size_t i = 0; i < atc_spd_7seg_.size(); i++) {
        atc_spd_7seg_[i] = 0;
    }
    dsatc_arrow_spd_ = 0;
    for (std::size_t i = 0; i < sub_spd_label_1_.size(); i++) {
        sub_spd_label_1_[i] = 0;
        sub_spd_label_2_[i] = 0;
        sub_spd_label_3_[i] = 0;
        sub_spd_label_4_[i] = 0;
    }
    sub_atc_spd_ = 0;
    sub_train_spd_ = 0;
    atc_ding_ = ATS_SOUND_STOP;
    reset_sw_down_sound_ = ATS_SOUND_STOP;
    reset_sw_up_sound_ = ATS_SOUND_STOP;
    next_reset_sw_down_sound_ = ATS_SOUND_STOP;
    next_reset_sw_up_sound_ = ATS_SOUND_STOP;
    atc_a_->Init();
    atc_d_->Init();
    fuzzy_->Init();
}

/// <summary>
/// Initializeで実行する関数
/// </summary>
void Atc::AtcA::Init() {
    signal_ = 0;
    is_stop_eb_ = 0;
    is_stop_svc_ = 0;
    is__brake_reset_ = 0;
    next_atc_ding_ = ATS_SOUND_STOP;
}

/// <summary>
/// Initializeで実行する関数
/// </summary>
void Atc::AtcD::Init() {
    is_stop_eb_ = 0;
    is_stop_svc_ = 0;
    is__brake_reset_ = 0;
    prev_spd_ = 0.0f;
    arrow_spd_ = 0;
    arrow_signal_ = 0;
    prev_arrow_signal_ = 0;
    tget_spd_ = 0;
    tget_signal_ = 0;
    prev_tget_signal_ = 0;
    next_atc_ding_ = ATS_SOUND_STOP;
    section_d_->Init();
    station_d_->Init();
    pattern_d_->Init();
}

/// <summary>
/// Initializeで実行する関数
/// </summary>
void Atc::AtcD::SectionD::Init() {
    section_loc_list_.reserve(10);
    prev_loc_ = 0.0;
    track_path_ = 0;
    red_signal_loc_ = 0.0f;
}

/// <summary>
/// Initializeで実行する関数
/// </summary>
void Atc::AtcD::StationD::Init() {
    for (int i = 0; i < STA_PATTERN; i++) {
        pattern_is_ready_[i] = 0;
        pattern_end_loc_[i] = 0.0f;
        pattern_is_valid_[i] = 0;
        pattern_tget_spd_[i] = 0;
    }
    is_stop_sta_ = 0;
}

/// <summary>
/// Initializeで実行する関数
/// </summary>
void Atc::AtcD::PatternD::Init() {
    for (int i = 0; i < USR_PATTERN; i++) {
        pattern_end_loc_[i] = 0.0f;
        pattern_is_valid_[i] = 0;
        pattern_tget_spd_[i] = 0;
    }
}

/// <summary>
/// Initializeで実行する関数
/// </summary>
void Atc::Fuzzy::Init() {
    prev_brake_notch_ = 0;
    adj_deceleration_ = 0;
    adj_timer_ = 0;
    fuzzy_prev_Tc_ = 0.0f;
    fuzzy_prev_Nc_ = 0;
    fuzzy_Ucg_ = 0.0f;
    fuzzy_Ucb_ = 0.0f;
    fuzzy_Usb_ = 0.0f;
    fuzzy_Usvb_ = 0.0f;
    for (int i = 0; i < ALL_PATTERN; i++) {
        fuzzy_step_[i] = 0;
        brake_timer_[i] = 0;
        prev_tget_spd_[i] = 0;
        prev_pattern_end_loc_[i] = 0.0f;
        fuzzy_brake_notch_list_[i] = 0;
    }
}

/// <summary>
/// 速度に対応する信号インデックスを返す関数
/// </summary>
/// <remarks>指定された速度以下の近似値に対応する信号インデックスを検索する</remarks>
/// <param name="spd">速度[km/h]</param>
/// <returns>速度に対応する信号インデックス</returns>
int Atc::SearchSignal(int spd) {
    return static_cast<int>(std::upper_bound(atc_spd_list_.begin(), atc_spd_list_.end(), spd) - atc_spd_list_.begin() - 1);
}

/// <summary>
/// 信号インデックスを速度に変換する関数
/// </summary>
/// <remarks>ATC-1のみ220km/h以上300km/h未満は"対応速度+5km/h"、300km/h以上は"対応速度+3km/h"を返す</remarks>
/// <param name="index">信号インデックス</param>
/// <returns>信号インデックスに対応する速度[km/h]</returns>
int Atc::ItoV(int index) {
    int atc_spd = ArrayGetOrDefault(atc_spd_list_, index, LOGARGS);
    if (atc_type_ == 0 && atc_spd >= 220) {
        if (atc_spd >= 300) {
            atc_spd += 3;
        } else {
            atc_spd += 5;
        }
    }
    return atc_spd;
}

/// <summary>
/// 信号インデックスを速度に変換する関数
/// </summary>
/// <remarks><paramref name="is_display"/>が偽かつATC-1のみ220km/h以上300km/h未満は"対応速度+5km/h"、300km/h以上は"対応速度+3km/h"を返す</remarks>
/// <param name="index">信号インデックス</param>
/// <param name="is_display">パネル表示用かどうか</param>
/// <returns>信号インデックスに対応する速度[km/h]</returns>
int Atc::ItoV(int index, bool is_display) {
    int atc_spd = ArrayGetOrDefault(atc_spd_list_, index, LOGARGS);
    if (atc_type_ == 0 && atc_spd >= 220 && !is_display) {
        if (atc_spd >= 300) {
            atc_spd += 3;
        } else {
            atc_spd += 5;
        }
    }
    return atc_spd;
}

/// <summary>
/// デジタルATC用速度照査パターンを作成する関数
/// </summary>
void Atc::SetPatternList() {
    for (int v = 0; v < 71 && v < atc_max_spd_ + 1; v++) {
        pattern_list_[v] = ((v / 3.6) * (v / 3.6)) / (2.0 * (((max_deceleration_ / max_brake_notch_) * boost::get<3>(default_notch_)) / 3.6));
    }
    for (int v = 71; v < 111 && v < atc_max_spd_ + 1; v++) {
        pattern_list_[v] = ((v / 3.6) * (v / 3.6) - (70.0 / 3.6) * (70.0 / 3.6)) / (2.0 * (((max_deceleration_ / max_brake_notch_) * boost::get<2>(default_notch_)) / 3.6)) + pattern_list_[70];
    }
    for (int v = 111; v < 161 && v < atc_max_spd_ + 1; v++) {
        pattern_list_[v] = ((v / 3.6) * (v / 3.6) - (110.0 / 3.6) * (110.0 / 3.6)) / (2.0 * (((max_deceleration_ / max_brake_notch_) * boost::get<1>(default_notch_)) / 3.6)) + pattern_list_[110];
    }
    for (int v = 161; v < atc_max_spd_ + 1; v++) {
        pattern_list_[v] = ((v / 3.6) * (v / 3.6) - (160.0 / 3.6) * (160.0 / 3.6)) / (2.0 * (((max_deceleration_ / max_brake_notch_) * boost::get<0>(default_notch_)) / 3.6)) + pattern_list_[160];
    }
}

/// <summary>
/// ATCを投入する際に実行する関数
/// </summary>
void Atc::StartNS() {
    if (atc_use_ == 0) {
        InitNS();
        atc_use_ = 1;

        // ATCベル
        if (atc_type_ < 2) {
            atc_a_->next_atc_ding_ = ATS_SOUND_PLAY;
        } else {
            atc_d_->next_atc_ding_ = ATS_SOUND_PLAY;
        }
    }
}

/// <summary>
/// ATCを遮断する際に実行する関数
/// </summary>
void Atc::ExitNS() {
    if (atc_use_ == 1) {
        atc_use_ = 0;
        InitNS();
    }
}

/// <summary>
/// SetSignalで実行される関数
/// </summary>
/// <param name="signal">現在のセクションの信号番号</param>
void Atc::ChangedSignalNS(int signal) {
    if (atc_use_ == 1) {
        if (atc_type_ < 2) {
            atc_a_->ChangedSignal(signal);
        } else {
            atc_d_->ChangedSignal(signal);
        }
    }
}

/// <summary>
/// SetSignalで実行され、車内信号を更新する関数
/// </summary>
/// <remarks>ATC-02, 03, 30信号のブレーキフラグ管理および、とーほぐ新幹線信号インデックスの変換を含む</remarks>
/// <param name="signal">現在のセクションの信号番号</param>
void Atc::AtcA::ChangedSignal(int signal) {
    // とーほぐ新幹線信号インデックス変換
    if (atc_->atc_type_ == 1 && signal >= 102) {
        signal = signal % 100 - 2;
    }

    // For Safety
    if (signal < ALL_SIGNAL) {
        // 車両最高速度以上の信号は出さない
        if (signal > atc_->max_signal_) { signal = atc_->max_signal_; }

        // ATC-02, 03信号ブレーキフラグOFF
        if (signal != 0) { is_stop_eb_ = 0; }

        // ATC-30信号ブレーキフラグOFF
        if (signal != 1) { is_stop_svc_ = 0; }

        // ATC-02信号ブレーキフラグON & ブレーキ開放フラグOFF
        if (signal == 0 && signal_ != 0) {
            is_stop_eb_ = 1;
            is__brake_reset_ = (*atc_->TrainSpeed != 0 || is__brake_reset_ != 1) ? 0 : 1;
        }

        // ATC-30信号ブレーキフラグON & ブレーキ開放フラグOFF
        if (signal == 1 && signal_ != 1) {
            is_stop_svc_ = 1;
            is__brake_reset_ = (*atc_->TrainSpeed != 0 || is__brake_reset_ != 1) ? 0 : 1;
        }

        // ATCベル
        if (signal != signal_) { next_atc_ding_ = ATS_SOUND_PLAY; }

        // 車内信号の更新
        signal_ = signal;
    } else {
        is_stop_eb_ = 1;
    }
}

/// <summary>
/// SetSignalで実行され、開通区間数を更新する関数
/// </summary>
/// <param name="signal">現在のセクションの信号番号</param>
void Atc::AtcD::ChangedSignal(int signal) {
    section_d_->track_path_ = signal;
}

/// <summary>
/// SetBeaconDataで実行される関数
/// </summary>
/// <param name="index">地上子種別</param>
/// <param name="signal">対となるセクションの信号番号</param>
/// <param name="distance">対となるセクションまでの距離[m]</param>
/// <param name="optional">地上子に設定された任意の値</param>
void Atc::PassedBeaconNS(int index, int signal, float distance, int optional) {
    switch (index) {
    case 70:
        PassedLoop(signal);
        break;
    case 80:
        atc_d_->section_d_->RegSection(distance);
        break;
    case 81:
        atc_d_->station_d_->RegStaStop(signal);
        break;
    case 82:
        atc_d_->station_d_->RegStaBranch(optional);
        break;
    case 83:
        atc_d_->station_d_->RegStaManual(optional);
        break;
    case 84:
        atc_d_->station_d_->RegStaEnd(optional);
        break;
    case 85:
        atc_d_->station_d_->RegStaLoc();
        break;
    case 86:
        atc_d_->pattern_d_->RegPattern(0, optional);
        break;
    case 87:
        atc_d_->pattern_d_->RegPattern(1, optional);
        break;
    case 88:
        atc_d_->pattern_d_->RegPattern(2, optional);
        break;
    case 90:
        ChangedAtcType(optional);
        break;
    default:
        break;
    }
}

/// <summary>
/// ATC方式を切り替える関数
/// </summary>
/// <param name="atc_type">ATC方式</param>
void Atc::ChangedAtcType(int atc_type) {
    if (atc_type != atc_type_ && atc_type < 5) {
        if (atc_type < 2 && atc_type_ > 1) {  // デジタルATCからアナログATCへの切り替え
            atc_a_->signal_ = atc_d_->section_d_->track_path_;
            atc_a_->next_atc_ding_ = atc_d_->next_atc_ding_;
            atc_d_->Init();
        } else if (atc_type > 1 && atc_type_ < 2) {  // アナログATCからデジタルATCへの切り替え
            atc_d_->section_d_->track_path_ = atc_a_->signal_;
            atc_d_->next_atc_ding_ = atc_a_->next_atc_ding_;
            atc_a_->Init();
        }
        atc_type_ = atc_type;
    }
}

/// <summary>
/// ATC-1, 2, NSにおいて停止限界(ループコイル)を通過した際に実行する関数
/// </summary>
/// <remarks>信号番号が0の際に03信号を発信する</remarks>
/// <param name="signal">対となるセクションの信号番号</param>
void Atc::PassedLoop(int signal) {
    if (atc_use_ == 1) {
        if (signal == 0) {  // ATC-03信号ブレーキフラグON & ブレーキ開放フラグOFF
            if (atc_type_ < 2 && atc_a_->is_stop_eb_ != 1) {
                atc_a_->is_stop_eb_ = 1;
                atc_a_->is__brake_reset_ = 0;
                atc_a_->next_atc_ding_ = ATS_SOUND_PLAY;  // ATCベル
            }
            if (atc_type_ == 2 && atc_d_->is_stop_eb_ != 1) {
                atc_d_->is_stop_eb_ = 1;
                atc_d_->is__brake_reset_ = 0;
                atc_d_->next_atc_ding_ = ATS_SOUND_PLAY;  // ATCベル
            }
        } else {  // ATC-03信号ブレーキフラグOFF
            if (atc_type_ == 2 && atc_d_->is_stop_eb_ == 1) {
                atc_d_->is_stop_eb_ = 0;
            }
        }
    }
}

/// <summary>
/// ATC-NSにおいてATC-30信号ブレーキフラグのON, OFFを行う関数
/// </summary>
/// <remarks>車内信号がATC-30かつ列車速度が30km/h以上から以下へ変化した場合にONになる</remarks>
void Atc::AtcD::AtcCheck() {
    if (atc_->atc_type_ == 2) {
        if (tget_signal_ == 1 && abs(*atc_->TrainSpeed) <= 30.0f && prev_spd_ > 30.0f) {  // ATC-30信号ブレーキフラグON & ブレーキ開放フラグOFF
            is_stop_svc_ = 1;
            is__brake_reset_ = 0;
        } else if (abs(*atc_->TrainSpeed) > 30.0f && prev_spd_ <= 30.0f) {  // ATC-30信号ブレーキフラグOFF
            is_stop_svc_ = 0;
        }
    }
    prev_spd_ = abs(*atc_->TrainSpeed);
}

/// <summary>
/// ATC-1, 2, NSにおいて確認扱いの判定を行う関数
/// </summary>
/// <remarks>ATC-03信号の場合は列車速度が0km/h、ATC-30信号の場合は30km/h以下である場合、ブレーキ開放フラグがONになる</remarks>
void Atc::Reset() {
    if (atc_use_ == 1) {
        if (*TrainSpeed == 0.0f) {
            if (atc_type_ < 2 && atc_a_->is_stop_eb_ == 1) {
                atc_a_->is__brake_reset_ = 1;
            }
            if (atc_type_ == 2 && atc_d_->is_stop_eb_ == 1) {
                atc_d_->is__brake_reset_ = 1;
            }
        }
        if (abs(*TrainSpeed) <= 30.0f) {
            if (atc_type_ < 2 && atc_a_->is_stop_svc_ == 1) { atc_a_->is__brake_reset_ = 1; }
            if (atc_type_ == 2 && atc_d_->is_stop_svc_ == 1) { atc_d_->is__brake_reset_ = 1; }
        }
    }
}

/// <summary>
/// 確認ボタンが押下された際に実行する関数
/// </summary>
void Atc::ResetSwDownNS() {
    Reset();
    if (atc_reset_sw_ == 0) {
        atc_reset_sw_ = 1;
        next_reset_sw_down_sound_ = ATS_SOUND_PLAY;
    }
}

/// <summary>
/// 確認ボタンが開放された際に実行する関数
/// </summary>
void Atc::ResetSwUpNS() {
    if (atc_reset_sw_ == 1) {
        atc_reset_sw_ = 0;
        next_reset_sw_up_sound_ = ATS_SOUND_PLAY;
    }
}

/// <summary>
/// 閉塞境界位置を登録する関数
/// </summary>
/// <param name="distance">対となるセクションまでの距離[m]</param>
void Atc::AtcD::SectionD::RegSection(float distance) {
    int def = static_cast<int>(atc_->Location - prev_loc_);
    if (def != 0) {
        prev_loc_ = atc_->Location;
        section_loc_list_.clear();
    }
    section_loc_list_.push_back(static_cast<float>(prev_loc_ + distance));
}

/// <summary>
/// 停止位置を算出する関数
/// </summary>
void Atc::AtcD::SectionD::CalcSection() {
    red_signal_loc_ = VectorGetOrDefault(section_loc_list_, track_path_, LOGARGS);
}

/// <summary>
/// 駅への停車開始判定を行う関数
/// </summary>
/// <remarks>出発信号が停止現示の場合に駅停車パターンが有効になる</remarks>
/// <param name="signal">出発信号の信号番号</param>
void Atc::AtcD::StationD::RegStaStop(int signal) {
    if (atc_->atc_type_ > 1 && signal == 0) {
        boost::get<0>(pattern_is_ready_) = (atc_->atc_type_ < 4) ? 1 : 0;
        boost::get<1>(pattern_is_ready_) = 1;
        boost::get<2>(pattern_is_ready_) = (atc_->atc_type_ > 2) ? 1 : 0;
    }
}

/// <summary>
/// 駅停車パターン(分岐制限)を登録する関数
/// </summary>
/// <param name="distance">減速完了地点までの相対距離[m]</param>
void Atc::AtcD::StationD::RegStaBranch(int distance) {
    boost::get<0>(pattern_end_loc_) = static_cast<float>(atc_->Location + distance);
    boost::get<0>(pattern_tget_spd_) = 70;
    boost::get<0>(pattern_is_valid_) = (boost::get<0>(pattern_is_ready_) == 1) ? 1 : 0;
}

/// <summary>
/// 駅停車パターン(手動頭打ち)を登録する関数
/// </summary>
/// <param name="distance">減速完了地点までの相対距離[m]</param>
void Atc::AtcD::StationD::RegStaManual(int distance) {
    boost::get<1>(pattern_end_loc_) = static_cast<float>(atc_->Location + distance);
    switch (atc_->atc_type_) {
    case 2:
        boost::get<1>(pattern_tget_spd_) = 30;
        break;
    case 3:
        boost::get<1>(pattern_tget_spd_) = 15;
        break;
    case 4:
        boost::get<1>(pattern_tget_spd_) = 75;
        break;
    default:
        break;
    }
    boost::get<1>(pattern_is_valid_) = (boost::get<1>(pattern_is_ready_) == 1) ? 1 : 0;
}

/// <summary>
/// 駅停車パターン(オーバーラン防止)を登録する関数
/// </summary>
/// <param name="distance">減速完了地点までの相対距離[m]</param>
void Atc::AtcD::StationD::RegStaEnd(int distance) {
    boost::get<2>(pattern_end_loc_) = static_cast<float>(atc_->Location + distance);
    boost::get<2>(pattern_tget_spd_) = 0;
    boost::get<2>(pattern_is_valid_) = (boost::get<2>(pattern_is_ready_) == 1) ? 1 : 0;
}

/// <summary>
/// 駅停車許容フラグをONにする関数
/// </summary>
void Atc::AtcD::StationD::RegStaLoc() {
    is_stop_sta_ = 1;
}

/// <summary>
/// 駅停車完了判定を行う関数
/// </summary>
/// <remarks>駅停車完了判定が真の場合は駅停車パターンが消去される</remarks>
void Atc::AtcD::StationD::IsStopSta() {
    if (is_stop_sta_ == 1 && *atc_->TrainSpeed == 0 && *atc_->ManualBrakeNotch > 0) {
        for (int i = 0; i < STA_PATTERN; i++) {
            pattern_is_ready_[i] = 0;
            pattern_is_valid_[i] = 0;
        }
        is_stop_sta_ = 0;
    }
}

/// <summary>
/// 速度制限パターンの登録および消去を行う関数
/// </summary>
/// <param name="type">パターン番号</param>
/// <param name="optional">減速完了地点までの相対距離[m]*1000+目標速度[km/h]</param>
void Atc::AtcD::PatternD::RegPattern(int type, int optional) {
    int distance = optional / 1000;
    if (distance < 0) { distance = 0; }
    int tget_spd = optional % 1000;
    if (tget_spd != 999) {
        pattern_end_loc_[type] = static_cast<float>(atc_->Location + distance);
        pattern_tget_spd_[type] = tget_spd;
        pattern_is_valid_[type] = 1;
    } else {
        pattern_is_valid_[type] = 0;
    }
}

/// <summary>
/// 距離に対応する速度を返す関数
/// </summary>
/// <remarks>指定された距離の近似値に対応する速度を検索する</remarks>
/// <param name="distance">距離[m]</param>
/// <returns>距離に対応する速度[km/h]</returns>
int Atc::SearchPattern(float distance) {
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
/// パターンが無効の場合に目標速度を車両ATC最高速度に修正する関数
/// </summary>
/// <param name="tget_spd">目標速度[km/h]</param>
/// <param name="pattern_status">パターン状態(0: 無効, 1: 有効)</param>
void Atc::ValidPattern(int& tget_spd, int pattern_status) {
    if (pattern_status == 0) { tget_spd = atc_max_spd_; }
}

/// <summary>
/// パターン照査速度を算出する関数
/// </summary>
/// <remarks>減速完了地点内方は目標速度のフラットパターンとなる</remarks>
/// <param name="tget_spd">目標速度[km/h]</param>
/// <param name="pattern_end_loc">減速完了地点[m]</param>
/// <returns>パターン照査速度[km/h]</returns>
int Atc::CalcPatternSpd(int tget_spd, float pattern_end_loc) {
    int arrow_spd = 0;
    if (pattern_end_loc <= Location) {
        if (tget_spd > atc_max_spd_) {
            arrow_spd = atc_max_spd_;
        } else if (tget_spd < 0) {
            arrow_spd = 0;
        } else {
            arrow_spd = tget_spd;
        }
    } else {
        arrow_spd = SearchPattern(static_cast<float>(VectorGetOrDefault(pattern_list_, tget_spd, LOGARGS) + pattern_end_loc - Location));
    }
    return arrow_spd;
}

/// <summary>
/// パターン降下開始地点を算出する関数
/// </summary>
/// <param name="tget_spd">目標速度[km/h]</param>
/// <param name="pattern_end_loc">減速完了地点[m]</param>
/// <param name="pattern_status">パターン状態(0: 無効, 1: 有効)</param>
/// <returns>パターン降下開始地点[m]</returns>
float Atc::CalcPatternLoc(int tget_spd, float pattern_end_loc, int pattern_status) {
    float pattern_start_loc = 0;
    if (pattern_status == 0) {
        pattern_start_loc = FLT_MAX;
    } else if (abs(*TrainSpeed) <= tget_spd) {
        pattern_start_loc = pattern_end_loc;
    } else {
        pattern_start_loc = static_cast<float>(VectorGetOrDefault(pattern_list_, tget_spd, LOGARGS) - VectorGetOrDefault(pattern_list_, static_cast<int>(abs(*TrainSpeed)), LOGARGS) + pattern_end_loc);
    }
    return pattern_start_loc;
}

/// <summary>
/// パターン情報を集約する関数
/// </summary>
void Atc::CollectPattern() {
    boost::get<0>(pattern_tget_spd_list_) = 0;
    boost::get<0>(pattarn_end_loc_list_) = atc_d_->section_d_->red_signal_loc_;
    boost::get<0>(pattern_is_valid_list_) = 1;
    for (int i = 0; i < STA_PATTERN; i++) {
        pattern_tget_spd_list_[1 + i] = atc_d_->station_d_->pattern_tget_spd_[i];
        pattarn_end_loc_list_[1 + i] = atc_d_->station_d_->pattern_end_loc_[i];
        pattern_is_valid_list_[1 + i] = atc_d_->station_d_->pattern_is_valid_[i];
    }
    for (int i = 0; i < USR_PATTERN; i++) {
        pattern_tget_spd_list_[1 + STA_PATTERN + i] = atc_d_->pattern_d_->pattern_tget_spd_[i];
        pattarn_end_loc_list_[1 + STA_PATTERN + i] = atc_d_->pattern_d_->pattern_end_loc_[i];
        pattern_is_valid_list_[1 + STA_PATTERN + i] = atc_d_->pattern_d_->pattern_is_valid_[i];
    }
    boost::get<1 + STA_PATTERN + USR_PATTERN>(pattern_tget_spd_list_) = atc_max_spd_;
    boost::get<1 + STA_PATTERN + USR_PATTERN>(pattarn_end_loc_list_) = 0.0f;
    boost::get<1 + STA_PATTERN + USR_PATTERN>(pattern_is_valid_list_) = 1;
}

/// <summary>
/// 各パターン情報からパターン照査速度およびパターン降下開始地点を算出する関数
/// </summary>
void Atc::CalcPattern() {
    for (int i = 0; i < ALL_PATTERN; i++) {
        ValidPattern(pattern_tget_spd_list_[i], pattern_is_valid_list_[i]);
        pattern_arrow_spd_list_[i] = CalcPatternSpd(pattern_tget_spd_list_[i], pattarn_end_loc_list_[i]);
        pattern_start_loc_list_[i] = CalcPatternLoc(pattern_tget_spd_list_[i], pattarn_end_loc_list_[i], pattern_is_valid_list_[i]);
    }
}

/// <summary>
/// デジタルATC用車内信号を生成する関数
/// </summary>
void Atc::ChangedArrowSignal() {
    BOOST_AUTO(iter, std::min_element(pattern_arrow_spd_list_.cbegin(), pattern_arrow_spd_list_.cend()));
    int index = static_cast<int>(std::distance(pattern_arrow_spd_list_.cbegin(), iter));
    if (atc_type_ == 2) {
        if (atc_d_->is_stop_eb_ == 1 && atc_d_->is__brake_reset_ != 1) {
            atc_d_->arrow_signal_ = 0;
            atc_d_->arrow_spd_ = 0;
        } else {
            atc_d_->arrow_signal_ = SearchSignal(*iter);
            atc_d_->arrow_spd_ = ItoV(atc_d_->arrow_signal_);
        }
        atc_d_->tget_signal_ = SearchSignal(pattern_tget_spd_list_[index]);

        // ATCベル
        if (atc_d_->arrow_signal_ != atc_d_->prev_arrow_signal_) {
            atc_d_->next_atc_ding_ = ATS_SOUND_PLAY;
        }
    } else {
        atc_d_->arrow_signal_ = SearchSignal(*iter);
        atc_d_->arrow_spd_ = *iter;
        if (Location >= static_cast<float>(VectorGetOrDefault(pattern_list_, pattern_tget_spd_list_[index], LOGARGS) - VectorGetOrDefault(pattern_list_, atc_max_spd_, LOGARGS) + pattarn_end_loc_list_[index])) {
            atc_d_->tget_signal_ = SearchSignal(pattern_tget_spd_list_[index]);
            atc_d_->tget_spd_ = pattern_tget_spd_list_[index];
        } else {
            atc_d_->tget_signal_ = atc_d_->arrow_signal_;
            atc_d_->tget_spd_ = atc_d_->arrow_spd_;
        }

        // ATCベル
        if (atc_d_->tget_signal_ != atc_d_->prev_tget_signal_) {
            atc_d_->next_atc_ding_ = ATS_SOUND_PLAY;
        }
    }
    atc_d_->prev_arrow_signal_ = atc_d_->arrow_signal_;
    atc_d_->prev_tget_signal_ = atc_d_->tget_signal_;
}

/// <summary>
/// Fuzzyメンバシップ関数L
/// </summary>
/// <remarks>定義域を(a - b, a + b)とする三角形状の関数</remarks>
/// <param name="x">x</param>
/// <param name="a">a</param>
/// <param name="b">b</param>
/// <returns>評価値</returns>
float Atc::Fuzzy::FuzzyFuncL(float x, float a, float b) {
    if (x <= (a - b) || x >= (a + b)) {
        return 0.0f;
    } else {
        return (1.0f - abs(x - a) / b);
    }
}

/// <summary>
/// Fuzzyメンバシップ関数F
/// </summary>
/// <remarks>関数Lでaより大きい部分と1.0とした関数</remarks>
/// <param name="x">x</param>
/// <param name="a">a</param>
/// <param name="b">b</param>
/// <returns>評価値</returns>
float Atc::Fuzzy::FuzzyFuncF(float x, float a, float b) {
    if (x <= (a - b)) {
        return 0.0f;
    } else if (x >= a) {
        return 1.0f;
    } else {
        return (1.0f - abs(x - a) / b);
    }
}

/// <summary>
/// Fuzzyメンバシップ関数A
/// </summary>
/// <remarks>定義域を(-∞, +∞)とした尖った関数</remarks>
/// <param name="x">x</param>
/// <param name="a">a</param>
/// <param name="b">b</param>
/// <returns>評価値</returns>
float Atc::Fuzzy::FuzzyFuncA(float x, float a, float b) {
    return (b / (abs(x - a) + b));
}

/// <summary>
/// Fuzzyメンバシップ関数G
/// </summary>
/// <remarks>定義域を(-∞, +∞)とした台形状の関数</remarks>
/// <param name="x">x</param>
/// <param name="a">a</param>
/// <param name="b">b</param>
/// <returns>評価値</returns>
float Atc::Fuzzy::FuzzyFuncG(float x, float a, float b) {
    if (x >= (a - b) && x <= (a + b)) {
        return 1.0f;
    } else {
        return (b / abs(x - a));
    }
}

/// <summary>
/// Fuzzyメンバシップ関数Gの形状を制御する関数
/// </summary>
/// <remarks>メンバシップ関数Gの形状(頂上の平坦部の幅)を速度に応じて連続的に制御する</remarks>
/// <param name="x">速度[km/h]</param>
/// <param name="a">頂上の平坦部の幅の初期値</param>
/// <param name="b">補正係数</param>
/// <returns>頂上の平坦部の幅</returns>
float Atc::Fuzzy::FuzzyFuncQ(float x, float a, float b) {
    if (b <= 0.0f) {
        return a;
    } else if (x <= sqrtf(a / b)) {
        return a;
    } else {
        return (b * x * x);
    }
}

/// <summary>
/// 各ブレーキノッチの減速度の補正値を算出する関数
/// </summary>
/// <remarks>ブレーキノッチが2秒間変化しなかった場合に補正値を算出する</remarks>
void Atc::Fuzzy::FuzzyAdjDeceleration() {
    if (atc_->brake_notch_ != prev_brake_notch_) {
        adj_timer_ = *atc_->Time + 2000;
        fuzzy_prev_Tc_ = *atc_->Time / 1000.0f;
        fuzzy_prev_Nc_ = abs(atc_->brake_notch_ - prev_brake_notch_);
        prev_brake_notch_ = atc_->brake_notch_;
    } else if (*atc_->Time >= adj_timer_) {
        if (atc_->brake_notch_ < atc_->max_brake_notch_ + 1) {
            if (abs(*atc_->TrainSpeed) != 0 && *atc_->PowerNotch == 0) {
                adj_deceleration_ = *atc_->Accel / 3.6f + (atc_->max_deceleration_ / 3.6f) * (atc_->brake_notch_ / static_cast<float>(atc_->max_brake_notch_));
            }
        }
    }
}

/// <summary>
/// 減速完了地点を予測する関数
/// </summary>
/// <param name="tget_spd">目標速度[km/h]</param>
/// <param name="notch_num">ブレーキノッチ</param>
/// <returns>予測減速完了地点[m]</returns>
float Atc::Fuzzy::FuzzyEstLoc(int tget_spd, int notch_num) {
    float deceleration = (atc_->max_deceleration_ / 3.6f) * (notch_num / static_cast<float>(atc_->max_brake_notch_)) - adj_deceleration_;
    float est_patt_end_loc = FLT_MAX;
    if (deceleration != 0) {
        est_patt_end_loc = static_cast<float>(((*atc_->TrainSpeed / 3.6f) * (*atc_->TrainSpeed / 3.6f) - (tget_spd / 3.6f) * (tget_spd / 3.6f)) / (2.0f * deceleration) + atc_->Location);
        if (notch_num != atc_->brake_notch_) {
            est_patt_end_loc += (*atc_->TrainSpeed / 3.6f) * atc_->lever_delay_;
        }
    }
    return est_patt_end_loc;
}

/// <summary>
/// 予見Fuzzy制御を初期化する関数
/// </summary>
/// <param name="index">パターン番号</param>
/// <param name="pattern_start_loc">パターン降下開始地点[m]</param>
/// <returns>最適ブレーキノッチ</returns>
int Atc::Fuzzy::FuzzyCtrInit(int index, float pattern_start_loc) {
    int brake_notch = 0;
    float est_loc = static_cast<float>(atc_->Location + (*atc_->TrainSpeed / 3.6f) * 2.0f + 0.5f * (*atc_->Accel / 3.6f) * 2.0f * 2.0f);
    if (est_loc >= pattern_start_loc) {
        if (atc_->brake_notch_ == 0) {
            brake_notch = 1;
        } else {
            brake_notch = atc_->brake_notch_;
        }
        brake_timer_[index] = *atc_->Time + 2000;
        fuzzy_step_[index] = 1;
    }
    return brake_notch;
}

/// <summary>
/// 予見Fuzzy制御を行う関数
/// </summary>
/// <param name="index">パターン番号</param>
/// <param name="tget_spd">目標速度[km/h]</param>
/// <param name="Xt">減速完了地点[m]</param>
/// <returns>最適ブレーキノッチ</returns>
int Atc::Fuzzy::FuzzyCtrExe(int index, int tget_spd, float Xt) {
    int brake_notch = fuzzy_brake_notch_list_[index];

    // ノッチ変化後の経過時間[s]
    float fuzzy_Tc = *atc_->Time / 1000.0f - fuzzy_prev_Tc_;

    // 速度による補正をかけたXe[m]
    float fuzzy_Xe = FuzzyFuncQ(abs(*atc_->TrainSpeed) - tget_spd, atc_->Xe, atc_->Xk);

    // 速度による補正をかけたXo[m]
    float fuzzy_Xo = FuzzyFuncQ(abs(*atc_->TrainSpeed) - tget_spd, atc_->Xo, atc_->Xk);

    fuzzy_Xp_.clear();
    fuzzy_Ugg_.clear();
    fuzzy_Uga_.clear();
    fuzzy_U_.clear();

    if (*atc_->Time >= brake_timer_[index]) {
        // 各制御則の評価
        for (int Np = 0; Np <= atc_->max_brake_notch_; Np++) {
            float Xp = FuzzyEstLoc(tget_spd, Np);
            fuzzy_Xp_.push_back(Xp);
            float Ugg = FuzzyFuncG(fuzzy_Xp_[Np], Xt, fuzzy_Xe);
            fuzzy_Ugg_.push_back(Ugg);
            float Uga = FuzzyFuncA(fuzzy_Xp_[Np], Xt, fuzzy_Xe);
            fuzzy_Uga_.push_back(Uga);
        }
        fuzzy_Ucg_ = FuzzyFuncF(fuzzy_Tc, (1.0f + fuzzy_prev_Nc_ / 2.0f), (fuzzy_prev_Nc_ / 2.0f));
        fuzzy_Ucb_ = 1.0f - fuzzy_Ucg_;
        fuzzy_Usb_ = FuzzyFuncL(fuzzy_Xp_[atc_->max_brake_notch_], Xt + fuzzy_Xo / 2.0f, fuzzy_Xo);
        fuzzy_Usvb_ = FuzzyFuncF(fuzzy_Xp_[atc_->max_brake_notch_], Xt + 3.0f / 2.0f * fuzzy_Xo, fuzzy_Xo);

        // 最適ブレーキノッチの決定
        if (fuzzy_Usvb_ > 0.5f) {
            brake_notch = atc_->max_brake_notch_ + 1;
        } else if (fuzzy_Usb_ > 0.5f) {
            brake_notch = atc_->max_brake_notch_;
        } else if (VectorGetOrDefault(fuzzy_Ugg_, atc_->brake_notch_, LOGARGS) == 1 || fuzzy_Ucg_ < fuzzy_Ucb_) {
            brake_notch = (atc_->brake_notch_ > atc_->max_brake_notch_) ? atc_->max_brake_notch_ : atc_->brake_notch_;
        } else if (fuzzy_Ucg_ >= fuzzy_Ucb_) {
            int Np_min = atc_->brake_notch_ - 2;
            if (Np_min < 0) { Np_min = 0; }
            int Np_max = atc_->brake_notch_ + 2;
            if (Np_max > atc_->max_brake_notch_) { Np_max = atc_->max_brake_notch_; }
            for (int Np = Np_min; Np <= Np_max; Np++) {
                float U = fuzzy_Uga_[Np];
                fuzzy_U_.push_back(U);

                ////// for debug
                //char debugstr[1000];
                //sprintf_s(debugstr, sizeof(debugstr), "Fuzzy制御評価: B%d / U: %f / Ucg: %f / Uga: %f\n", Np, U, fuzzy_Ucg_, fuzzy_Uga_[Np]);
                //OutputDebugString(debugstr);
            }
            BOOST_AUTO(iter, std::max_element(fuzzy_U_.cbegin(), fuzzy_U_.cend()));
            if (*iter == 0) {
                brake_notch = atc_->brake_notch_;
            } else {
                int Np_best = static_cast<int>(std::distance(fuzzy_U_.cbegin(), iter));
                brake_notch = Np_min + Np_best;
            }
        }
        brake_timer_[index] = *atc_->Time + 1000;
    }
    return brake_notch;
}

/// <summary>
/// パターンに対する最適ブレーキノッチを出力する関数
/// </summary>
/// <param name="index">パターン番号</param>
/// <param name="tget_spd">目標速度[km/h]</param>
/// <param name="pattern_start_loc">パターン降下開始地点[m]</param>
/// <param name="pattern_end_loc">減速完了地点[m]</param>
/// <returns>最適ブレーキノッチ</returns>
int Atc::CalcBrake(int index, int tget_spd, float pattern_start_loc, float pattern_end_loc) {
    int brake_notch = 0;
    if (abs(*TrainSpeed) > tget_spd) {
        if (tget_spd != fuzzy_->prev_tget_spd_[index] || pattern_end_loc != fuzzy_->prev_pattern_end_loc_[index]) {
            fuzzy_->fuzzy_step_[index] = 0;
            fuzzy_->prev_tget_spd_[index] = tget_spd;
            fuzzy_->prev_pattern_end_loc_[index] = pattern_end_loc;
        }
        if (fuzzy_->fuzzy_step_[index] == 0) {
            brake_notch = fuzzy_->FuzzyCtrInit(index, pattern_start_loc);
        } else if (fuzzy_->fuzzy_step_[index] == 1) {
            brake_notch = fuzzy_->FuzzyCtrExe(index, tget_spd, pattern_end_loc);
        }
    } else {
        fuzzy_->fuzzy_step_[index] = 0;
    }
    return brake_notch;
}

/// <summary>
/// 全パターンに対する最大の最適ブレーキノッチを出力する関数
/// </summary>
/// <returns>最適ブレーキノッチ</returns>
int Atc::SelectBrake() {
    int brake_notch = 0;
    fuzzy_->FuzzyAdjDeceleration();
    for (int i = 0; i < ALL_PATTERN; i++) {
        fuzzy_->fuzzy_brake_notch_list_[i] = CalcBrake(i, pattern_tget_spd_list_[i], pattern_start_loc_list_[i], pattarn_end_loc_list_[i]);
    }
    brake_notch = *std::max_element(fuzzy_->fuzzy_brake_notch_list_.begin(), fuzzy_->fuzzy_brake_notch_list_.end());
    return brake_notch;
}

/// <summary>
/// 予見Fuzzy制御ではないブレーキノッチを出力する関数
/// </summary>
/// <returns>ブレーキノッチ</returns>
int Atc::NonFuzzyCtrExe() {
    int brake_notch = brake_notch_;
    float default_deceleration = max_deceleration_;
    if (abs(*TrainSpeed) > 160) {
        //default_deceleration = ((max_deceleration_ / max_brake_notch_) * boost::get<0>(default_notch_)) / 3.6f;
        default_deceleration = ((max_deceleration_ / max_brake_notch_) * boost::get<1>(default_notch_)) / 3.6f;
    } else if (abs(*TrainSpeed) > 110) {
        default_deceleration = ((max_deceleration_ / max_brake_notch_) * boost::get<1>(default_notch_)) / 3.6f;
    } else if (abs(*TrainSpeed) > 70) {
        default_deceleration = ((max_deceleration_ / max_brake_notch_) * boost::get<2>(default_notch_)) / 3.6f;
    } else {
        default_deceleration = ((max_deceleration_ / max_brake_notch_) * boost::get<3>(default_notch_)) / 3.6f;
    }

    // 必要ブレーキノッチの決定
    int req_brake_notch = static_cast<int>(ceilf((default_deceleration + fuzzy_->adj_deceleration_) / (max_deceleration_ / 3.6f) * max_brake_notch_));
    if (req_brake_notch < 0) { req_brake_notch = 0; }
    if (req_brake_notch > max_brake_notch_) { req_brake_notch = max_brake_notch_; }

    // ブレーキノッチの出力
    if (brake_notch < req_brake_notch && *Time > brake_timer_) {
        brake_notch++;
        brake_timer_ = *Time + 1000;
    }
    return brake_notch;
}

/// <summary>
/// アナログATC用のブレーキノッチを出力する関数
/// </summary>
void Atc::BrakeExeA() {
    fuzzy_->FuzzyAdjDeceleration();
    if ((atc_a_->is_stop_eb_ == 1 || atc_a_->is_stop_svc_ == 1) && atc_a_->is__brake_reset_ == 1) {
        atc_brake_notch_ = 0;
    } else if (atc_a_->is_stop_eb_ == 1) {
        atc_brake_notch_ = max_brake_notch_ + 1;
    } else if (atc_a_->is_stop_svc_ == 1) {
        atc_brake_notch_ = max_brake_notch_;
    } else if (abs(*TrainSpeed) > ItoV(atc_a_->signal_) || abs(*TrainSpeed) > atc_max_spd_) {
        atc_brake_notch_ = NonFuzzyCtrExe();
    } else {
        atc_brake_notch_ = 0;
    }
}

/// <summary>
/// デジタルATC用のブレーキノッチを出力する関数
/// </summary>
void Atc::BrakeExeD() {
    atc_d_->AtcCheck();
    if ((atc_d_->is_stop_eb_ == 1 || atc_d_->is_stop_svc_ == 1) && atc_d_->is__brake_reset_ == 1) {
        atc_brake_notch_ = 0;
    } else if (atc_d_->is_stop_eb_ == 1) {
        atc_brake_notch_ = max_brake_notch_ + 1;
    } else if (atc_d_->is_stop_svc_ == 1) {
        atc_brake_notch_ = max_brake_notch_;
    } else {
        atc_brake_notch_ = SelectBrake();
    }
}

/// <summary>
/// 速度を7セグに表示する関数
/// </summary>
/// <param name="spd">速度[km/h]</param>
/// <param name="display_spd">速度を表示する配列</param>
void Atc::DisplaySpd(int spd, boost::array<int, 3>&display_spd) {
    int spd100 = spd / 100;
    int spd10 = (spd % 100) / 10;
    int spd1 = spd % 10;
    if (spd100 >= 10) {
        for (std::size_t i = 0; i < display_spd.size(); i++) {
            display_spd[i] = 9;
        }
    } else {
        boost::get<0>(display_spd) = (spd100 == 0) ? 10 : spd100;
        boost::get<1>(display_spd) = (spd100 == 0 && spd10 == 0) ? 10 : spd10;
        boost::get<2>(display_spd) = spd1;
    }
}

/// <summary>
/// インジケーターの表示を初期化する関数
/// </summary>
void Atc::ResetIndicatorNS() {
    atc_red_signal_ = 0;
    atc_green_signal_ = 0;
    for (std::size_t i = 0; i < atc_sig_indicator_.size(); i++) { atc_sig_indicator_[i] = 0; }
}

/// <summary>
/// インジケーターの表示を実行する関数
/// </summary>
void Atc::RunIndicatorNS() {
    if (atc_type_ < 2) {
        RunIndicatorA();
    } else {
        RunIndicatorD();
    }
}

/// <summary>
/// アナログATC用インジケーターの表示を実行する関数
/// </summary>
void Atc::RunIndicatorA() {
    if (atc_a_->signal_ == 0) {
        atc_red_signal_ = 1;
    } else {
        atc_green_signal_ = 1;
    }
    atc_sig_indicator_[atc_a_->signal_] = 1;
    atc_signal_index_ = atc_a_->signal_;
    atc_spd_ = ItoV(atc_a_->signal_, true);
    DisplaySpd(ItoV(atc_a_->signal_, true), atc_spd_7seg_);
}

/// <summary>
/// デジタルATC用インジケーターの表示を実行する関数
/// </summary>
void Atc::RunIndicatorD() {
    if (atc_d_->section_d_->track_path_ <= 1) {
        atc_red_signal_ = 1;
    } else {
        atc_green_signal_ = 1;
    }
    if (atc_type_ == 2) {
        atc_sig_indicator_[atc_d_->arrow_signal_] = 1;
        atc_signal_index_ = atc_d_->arrow_signal_;
        atc_spd_ = atc_d_->arrow_spd_;
        DisplaySpd(atc_d_->arrow_spd_, atc_spd_7seg_);
    } else {
        atc_sig_indicator_[atc_d_->tget_signal_] = 1;
        atc_signal_index_ = atc_d_->tget_signal_;
        atc_spd_ = atc_d_->tget_spd_;
        DisplaySpd(atc_d_->tget_spd_, atc_spd_7seg_);
        if (atc_type_ == 4) {
            dsatc_arrow_spd_ = atc_d_->arrow_spd_;
        }
    }
}

/// <summary>
/// 副速度計の表示を実行する関数
/// </summary>
void Atc::RunSubSpeedometer() {
    if (atc_spd_ <= 10) {
        DisplaySpd(0, sub_spd_label_1_);
        DisplaySpd(10, sub_spd_label_2_);
        DisplaySpd(20, sub_spd_label_3_);
        DisplaySpd(30, sub_spd_label_4_);
        sub_atc_spd_ = atc_spd_;
        sub_train_spd_ = static_cast<int>(abs(*TrainSpeed));
        if (sub_train_spd_ > 30) { sub_train_spd_ = 30; }
    } else {
        DisplaySpd(atc_spd_ - 20, sub_spd_label_1_);
        DisplaySpd(atc_spd_ - 10, sub_spd_label_2_);
        DisplaySpd(atc_spd_, sub_spd_label_3_);
        DisplaySpd(atc_spd_ + 10, sub_spd_label_4_);
        sub_atc_spd_ = 20;
        sub_train_spd_ = static_cast<int>(abs(*TrainSpeed) - (atc_spd_ - 20));
        if (sub_train_spd_ < 0) { sub_train_spd_ = 0; }
        if (sub_train_spd_ > 30) { sub_train_spd_ = 30; }
    }
}

/// <summary>
/// サウンドを再生する関数
/// </summary>
void Atc::PlaySound() {
    if (atc_type_ < 2) {
        atc_ding_ = atc_a_->next_atc_ding_;
        atc_a_->next_atc_ding_ = ATS_SOUND_CONTINUE;
    } else {
        atc_ding_ = atc_d_->next_atc_ding_;
        atc_d_->next_atc_ding_ = ATS_SOUND_CONTINUE;
    }

    reset_sw_down_sound_ = next_reset_sw_down_sound_;
    next_reset_sw_down_sound_ = ATS_SOUND_CONTINUE;

    reset_sw_up_sound_ = next_reset_sw_up_sound_;
    next_reset_sw_down_sound_ = ATS_SOUND_CONTINUE;
}

/// <summary>
/// Elapseで実行する関数
/// </summary>
void Atc::ExeNS() {
    brake_notch_ = (*BrakeNotch != 0) ? (*BrakeNotch - AtsNotch + 1) : 0;
    if (atc_use_ == 0) {
        atc_brake_notch_ = 0;
    } else {
        if (atc_type_ < 2) {
            BrakeExeA();
        } else {
            atc_d_->section_d_->CalcSection();
            atc_d_->station_d_->IsStopSta();
            CollectPattern();
            CalcPattern();
            ChangedArrowSignal();
            BrakeExeD();
        }
        ResetIndicatorNS();
        RunIndicatorNS();
        RunSubSpeedometer();
        PlaySound();

        atc_eb_lamp_ = (atc_brake_notch_ == max_brake_notch_) ? 1 : 0;  // ATC非常
        atc_svc_lamp_ = (atc_brake_notch_ > 0) ? 1 : 0;  // ATC常用

        // For debug
        if (*Time >= debug_timer_) {
            char debugstr[1000];
            if (atc_type_ < 2) {
                sprintf_s(debugstr, sizeof(debugstr), "Loc: %f / Accel: %f / TargetSpd: %d\n", Location, *Accel, ItoV(atc_a_->signal_));
                OutputDebugString(debugstr);
                sprintf_s(debugstr, sizeof(debugstr), "出力B: B%d / is_stop_eb_: %d / is_stop_svc_: %d / is__brake_reset_: %d\n", atc_brake_notch_, atc_a_->is_stop_eb_, atc_a_->is_stop_svc_, atc_a_->is__brake_reset_);
                OutputDebugString(debugstr);
            } else {
                BOOST_AUTO(iter, std::min_element(pattern_arrow_spd_list_.cbegin(), pattern_arrow_spd_list_.cend()));
                int index = static_cast<int>(std::distance(pattern_arrow_spd_list_.cbegin(), iter));
                int tget_spd = pattern_tget_spd_list_[index];
                float pattern_start_loc = pattern_start_loc_list_[index];
                float pattern_end_loc = pattarn_end_loc_list_[index];
                int arrow_spd = *iter;
                BOOST_AUTO(iter2, std::max_element(fuzzy_->fuzzy_brake_notch_list_.cbegin(), fuzzy_->fuzzy_brake_notch_list_.cend()));
                index = static_cast<int>(std::distance(fuzzy_->fuzzy_brake_notch_list_.cbegin(), iter2));
                sprintf_s(debugstr, sizeof(debugstr), "Loc: %f / Accel: %f / TargetSpd: %d / PattStart: %f / PattEnd: %f / Arrow: %d\n", Location, *Accel, tget_spd, pattern_start_loc, pattern_end_loc, arrow_spd);
                OutputDebugString(debugstr);
                sprintf_s(debugstr, sizeof(debugstr), "Index: %d / 出力B: B%d / is_stop_eb_: %d / is_stop_svc_: %d / is__brake_reset_: %d\n", index, atc_brake_notch_, atc_d_->is_stop_eb_, atc_d_->is_stop_svc_, atc_d_->is__brake_reset_);
                OutputDebugString(debugstr);
            }
            debug_timer_ = *Time + 1000;
        }
    }
}
