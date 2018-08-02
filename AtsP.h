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

#ifndef ATSP_H_
#define ATSP_H_
#include <boost/container/vector.hpp>
#include <boost/array.hpp>
#include <boost/scoped_ptr.hpp>
#include "Base.h"

#define LOGARGS __FILE__, __FUNCTION__, __LINE__  // For Debug
#define ALL_PATTERN_P 7  //!< パターンの総数
#define USR_PATTERN_P 5  //!< 速度制限パターンの総数

/// <summary>
/// ATS-Pを再現するクラス
/// </summary>
class Atsp :private Base {
  private:
    int max_brake_notch_;  //!< 常用最大ブレーキノッチ(HBを含まない)
    int default_notch_;  //!< 標準ブレーキノッチ
    boost::container::vector<double>pattern_list_;  //!< 速度照査パターン
    double adj_deceleration_;  //!< 各ブレーキノッチの減速度補正値[m/s^2]
    boost::array<float, ALL_PATTERN_P>pattern_end_loc_list_;  //!< 全パターンの減速完了地点[m]
    boost::array<int, ALL_PATTERN_P>pattern_is_valid_list_;  //!< 全パターンの状態(0: 無効, 1: 有効)
    boost::array<int, ALL_PATTERN_P>pattern_tget_spd_list_;  //!< 全パターンの目標速度[km/h]
    boost::array<int, ALL_PATTERN_P>pattern_pre_spd_list_;  //!< 全パターンのパターン照査速度[km/h]
    boost::array<int, ALL_PATTERN_P>pattern_aprch_list_;  //!< 全パターンのパターン接近警報の状態(0: 無効, 1: 有効)
    int next_atsp_ding_;  //!< 1フレーム後のATS-Pベルの状態
    int prev_aprch_lamp_;  //!< 以前のパターン接近灯(0: 消灯, 1: 点灯)
    int debug_timer_;  //!< Debug出力する次のゲーム内時刻[ms]

    void SetPatternList(void);
    void AdjDeceleration(int deceleration);
    int SearchPattern(float distance);
    void ValidPattern(int& tget_spd, int pattern_status);
    int CalcPatternSpd(int tget_spd, float pattern_end_loc);
    int IsAprchPattern(int tget_spd, float pattern_end_loc);
    void CollectPattern(void);
    void CalcPattern(void);
    void AtspCheck(void);
    void Reset(void);
    void BrakeExe(void);
    void DisplayLamp(void);
    void PlaySound(void);

    ////////////////////////////////////////////////////////

    /// <summary>
    /// 停止信号パターン関連を記述するクラス
    /// </summary>
    class SectionP {
      private:
        const Atsp *atsp_;  //!< Atspクラスのオブジェクトを格納するポインタ
        float red_signal_loc_;  //!< 停止信号地点[m]

      public:
        int is_immediate_stop_eb_;  //!< 即時停止(非常)フラグ
        int is_immediate_stop_svc_;  //!< 即時停止(常用)フラグ
        int is_immediate_stop_;  //!< 即時停止(非常, 常用)フラグ
        int is_stop_svc_;  //!< パターン接触フラグ
        int is__brake_reset_;  //!< ブレーキ開放フラグ
        float pattern_end_loc_;  //!< 減速完了地点[m]
        int pattern_is_valid_;  //!< パターンの状態(0: 無効, 1: 有効)
        int pattern_tget_spd_;  //!< 目標速度[km/h]

        explicit SectionP(Atsp *atsp);
        virtual ~SectionP(void);
        void Init(void);
        void GetSection(float distance);
        void PassedStopEb(float distance);
        void PassedStopSvc(float distance);
        void CalcSection(void);
    };
    ////////////////////////////////////////////////////////

    /// <summary>
    /// 制限速度パターン関連を記述するクラス
    /// </summary>
    class PatternP {
      private:
        const Atsp *atsp_;  //!< Atspクラスのオブジェクトを格納するポインタ

      public:
        boost::array<float, USR_PATTERN_P>pattern_end_loc_;  //!< 減速完了地点[m]
        boost::array<int, USR_PATTERN_P>pattern_is_valid_;  //!< パターンの状態(0: 無効, 1: 有効)
        boost::array<int, USR_PATTERN_P>pattern_tget_spd_;  //!< 目標速度[km/h]

        explicit PatternP(Atsp *atsp);
        virtual ~PatternP(void);
        void Init(void);
        void RegPattern(int type, int optional);
        void DelPattern(int type);
    };
    ////////////////////////////////////////////////////////
    boost::scoped_ptr<SectionP>section_p_;
    boost::scoped_ptr<PatternP>pattern_p_;

  public:
    int EmergencyNotch;  //!< 非常ブレーキノッチ(HBを含む)
    int ServiceNotch;  //!< 常用最大ブレーキノッチ(HBを含む)
    int AtsNotch;  //!< ATS確認ノッチ(B1)
    const float *TrainSpeed;  //!< 列車速度[km/h]
    const int *Time;  //!< ゲーム内時刻[ms]
    const int *ManualBrakeNotch;  //!< 手動ブレーキノッチ(HBを含む)
    double Location;  //!< 列車位置[m]
    float max_deceleration_;  //!< 常用最大減速度[km/h/s]
    int atsp_power_;  //!< ATS-P電源(0: 消灯, 1: 点灯)
    int atsp_use_;  //!< ATS-P(0: 消灯, 1: 点灯)
    int atsp_max_spd_;  //!< 車両ATS-P最高速度[km/h]
    float atsp_deceleration_;  //!< ATS-Pブレーキ減速度[km/h/s]
    int atsp_brake_notch_;  //!< ATS-P出力ブレーキノッチ(HBを含まない)
    int atsp_aprch_lamp_;  //!< パターン接近(0: 消灯, 1: 点灯)
    int atsp_brake_lamp_;  //!< ブレーキ作動(0: 消灯, 1: 点灯)
    int atsp_reset_lamp_;  //!< ブレーキ解放(0: 消灯, 1: 点灯)
    int atsp_ding_;  //!< ATS-Pベルの状態

    Atsp(void);
    virtual ~Atsp(void);
    void InitP(void);
    void StartP(void);
    void ExitP(void);
    void PassedBeaconP(int index, int signal, float distance, int optional);
    void ResetSwDownP(void);
    void ExeP(void);
};

#endif  // ATSP_H_
