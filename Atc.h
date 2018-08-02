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

#ifndef ATC_H_
#define ATC_H_
#include <boost/container/vector.hpp>
#include <boost/array.hpp>
#include <boost/scoped_ptr.hpp>
#include "Base.h"

#define LOGARGS __FILE__, __FUNCTION__, __LINE__  // For Debug
#define BRAKE_STEP 4  //!< ATCブレーキ減速度切り替え段数
#define ALL_PATTERN 8  //!< パターンの総数
#define STA_PATTERN 3  //!< 駅停車パターンの総数
#define USR_PATTERN 3  //!< 速度制限パターンの総数
#define ALL_SIGNAL 12  //!< 信号インデックスの総数

/// <summary>
/// ATC-1, ATC-2, ATC-NS, KS-ATC, DS-ATCを再現するクラス
/// </summary>
class Atc :private Base {
  private:
    int max_brake_notch_;  //!< 常用最大ブレーキノッチ(HBを含まない)
    int brake_notch_;  //!< 出力ブレーキノッチ(HBを含まない)
    boost::array<int, BRAKE_STEP>default_notch_;  //!< 標準ブレーキノッチ
    int max_signal_;  //!< 車両ATC最高速度に対応する信号インデックス
    boost::container::vector<double>pattern_list_;  //!< デジタルATC用速度照査パターン
    int next_reset_sw_down_sound_;  //!< 1フレーム後の確認ボタンの押下音の状態
    int next_reset_sw_up_sound_;  //!< 1フレーム後の確認ボタンの開放音の状態
    boost::array<float, ALL_PATTERN>pattarn_end_loc_list_;  //!< 全パターンの減速完了地点[m]
    boost::array<int, ALL_PATTERN>pattern_is_valid_list_;  //!< 全パターンの状態(0: 無効, 1: 有効)
    boost::array<int, ALL_PATTERN>pattern_tget_spd_list_;  //!< 全パターンの目標速度[km/h]
    boost::array<int, ALL_PATTERN>pattern_arrow_spd_list_;  //!< 全パターンのパターン照査速度[km/h]
    boost::array<float, ALL_PATTERN>pattern_start_loc_list_;  //!< 全パターンのパターン降下開始地点[m]
    int debug_timer_;  //!< Debug出力する次のゲーム内時刻[ms]
    int brake_timer_;  //!< ブレーキノッチを変更する次のゲーム内時刻[ms]

    int SearchSignal(int spd);
    int ItoV(int index);
    int ItoV(int index, bool is_display);
    void Reset(void);
    void SetPatternList(void);
    void ChangedAtcType(int atc_type);
    void PassedLoop(int signal);
    int SearchPattern(float distance);
    void ValidPattern(int& tget_spd, int pattern_status);
    int CalcPatternSpd(int tget_spd, float pattern_end_loc);
    float CalcPatternLoc(int tget_spd, float pattern_end_loc, int pattern_status);
    void CollectPattern(void);
    void CalcPattern(void);
    void ChangedArrowSignal(void);
    int CalcBrake(int index, int tget_spd, float pattern_start_loc, float pattern_end_loc);
    int SelectBrake(void);
    int NonFuzzyCtrExe(void);
    void BrakeExeA(void);
    void BrakeExeD(void);
    void DisplaySpd(int spd, boost::array<int, 3>&display_spd);
    void ResetIndicatorNS(void);
    void RunIndicatorNS(void);
    void RunIndicatorA(void);
    void RunIndicatorD(void);
    void RunSubSpeedometer(void);
    void PlaySound(void);

    ////////////////////////////////////////////////////////

    /// <summary>
    /// ATC-1, ATC-2を再現するクラス
    /// </summary>
    class AtcA {
      private:
        const Atc *atc_;  //!< Atcクラスのオブジェクトを格納するポインタ

      public:
        int signal_;  //!< 車内信号の信号インデックス
        int is_stop_eb_;  //!< ATC-02, 03信号ブレーキフラグ
        int is_stop_svc_;  //!< ATC-30信号ブレーキフラグ
        int is__brake_reset_;  //!< ブレーキ開放フラグ
        int next_atc_ding_;  //!< 1フレーム後のATCベルの状態

        explicit AtcA(const Atc *atc);
        virtual ~AtcA(void);
        void Init(void);
        void ChangedSignal(int signal);
    };
    ////////////////////////////////////////////////////////

    /// <summary>
    /// ATC-NS, KS-ATC, DS-ATCを再現するクラス
    /// </summary>
    class AtcD {
      private:
        const Atc *atc_;  //!< Atcクラスのオブジェクトを格納するポインタ
        float prev_spd_;  //!< 1フレーム前の列車速度[km/h]

      public:
        int is_stop_eb_;  //!< ATC-02, 03信号ブレーキフラグ
        int is_stop_svc_;  //!< ATC-30信号ブレーキフラグ
        int is__brake_reset_;  //!< ブレーキ開放フラグ
        int next_atc_ding_;  //!< 1フレーム後のATCベルの状態
        int arrow_spd_;  //!< パターン照査速度[km/h]
        int arrow_signal_;  //!< パターン照査速度の信号インデックス
        int prev_arrow_signal_;  //!< 以前のパターン照査速度の信号インデックス
        int tget_spd_;  //!< 目標速度[km/h]
        int tget_signal_;  //!< 目標速度の信号インデックス
        int prev_tget_signal_;  //!< 以前の目標速度の信号インデックス

        explicit AtcD(const Atc *atc);
        virtual ~AtcD(void);
        void Init(void);
        void ChangedSignal(int signal);
        void AtcCheck(void);

        ////////////////////////////////////////////////////////

        /// <summary>
        /// 停止信号パターン関連を記述するクラス
        /// </summary>
        class SectionD :private Base {
          private:
            const Atc *atc_;  //!< Atcクラスのオブジェクトを格納するポインタ
            double prev_loc_;  //! 以前の列車位置[m]
            boost::container::vector<float>section_loc_list_;  //!< 閉塞境界位置[m]リスト

          public:
            int track_path_;  //!< 開通区間数
            float red_signal_loc_;  //!< 停止信号地点[m]

            explicit SectionD(const Atc *atc);
            virtual ~SectionD(void);
            void Init(void);
            void RegSection(float distance);
            void CalcSection(void);
        };
        ////////////////////////////////////////////////////////

        /// <summary>
        /// 駅停車パターン関連を記述するクラス
        /// </summary>
        class StationD {
          private:
            const Atc *atc_;  //!< Atcクラスのオブジェクトを格納するポインタ
            int is_stop_sta_;  //!< 駅停車後方許容地点フラグ
            boost::array<int, STA_PATTERN>pattern_is_ready_;  //!< 駅への停車開始判定フラグ

          public:
            boost::array<float, STA_PATTERN>pattern_end_loc_;  //!< 減速完了地点[m]
            boost::array<int, STA_PATTERN>pattern_is_valid_;  //!< パターンの状態(0: 無効, 1: 有効)
            boost::array<int, STA_PATTERN>pattern_tget_spd_;  //!< 目標速度[km/h]

            explicit StationD(const Atc *atc);
            virtual ~StationD(void);
            void Init(void);
            void RegStaStop(int signal);
            void RegStaBranch(int distance);
            void RegStaManual(int distance);
            void RegStaEnd(int distance);
            void RegStaLoc(void);
            void IsStopSta(void);
        };
        ////////////////////////////////////////////////////////

        /// <summary>
        /// 制限速度パターン関連を記述するクラス
        /// </summary>
        class PatternD {
          private:
            const Atc *atc_;  //!< Atcクラスのオブジェクトを格納するポインタ

          public:
            boost::array<float, USR_PATTERN>pattern_end_loc_;  //!< 減速完了地点[m]
            boost::array<int, USR_PATTERN>pattern_is_valid_;  //!< パターンの状態(0: 無効, 1: 有効)
            boost::array<int, USR_PATTERN>pattern_tget_spd_;  //!< 目標速度[km/h]

            explicit PatternD(const Atc *atc);
            virtual ~PatternD(void);
            void Init(void);
            void RegPattern(int type, int optional);
        };
        ////////////////////////////////////////////////////////
        boost::scoped_ptr<SectionD>section_d_;
        boost::scoped_ptr<StationD>station_d_;
        boost::scoped_ptr<PatternD>pattern_d_;
    };
    ////////////////////////////////////////////////////////

    /// <summary>
    /// 予見Fuzzy制御を再現するクラス
    /// </summary>
    class Fuzzy :private Base {
      private:
        const Atc *atc_;  //!< Atcクラスのオブジェクトを格納するポインタ
        int prev_brake_notch_;  //!< 以前の出力ブレーキノッチ(HBを含まない)
        int adj_timer_;  //!< 減速度補正を行う次のゲーム内時刻[ms]
        float fuzzy_prev_Tc_;  //!< 最後に出力ブレーキノッチが変化したゲーム内時刻[s]
        int fuzzy_prev_Nc_;  //!< 最後に変化した出力ブレーキノッチの変化量の絶対値
        boost::container::vector<float>fuzzy_Xp_;  //!< 予測減速完了地点[m]
        boost::container::vector<float>fuzzy_Ugg_;  //!< 「うまく停止する(GG)」の評価値
        boost::container::vector<float>fuzzy_Uga_;  //!< 「正確に停止する(GA)」の評価値
        float fuzzy_Ucg_;  //!< 「乗り心地が良い(CG)」の評価値
        float fuzzy_Ucb_;  //!< 「乗り心地が悪い(CB)」の評価値
        boost::container::vector<float>fuzzy_U_;  //!< "(CG >= CB) And GA"の評価値
        float fuzzy_Usb_;  //!< 「安全性が悪い(SB)」の評価値
        float fuzzy_Usvb_;  //!< 「安全性がとても悪い(SVB)」の評価値

        float FuzzyFuncL(float x, float a, float b);
        float FuzzyFuncF(float x, float a, float b);
        float FuzzyFuncA(float x, float a, float b);
        float FuzzyFuncG(float x, float a, float b);
        float FuzzyFuncQ(float x, float a, float b);
        float FuzzyEstLoc(int tget_spd, int notch_num);

      public:
        float adj_deceleration_;  //!< 各ブレーキノッチの減速度補正値[m/s^2]
        boost::array<int, ALL_PATTERN>fuzzy_step_;  //!< 全パターンの予見Fuzzy制御の制御段階
        boost::array<int, ALL_PATTERN>brake_timer_;  //!< 全パターンの予見Fuzzy制御を実行する次のゲーム内時刻[ms]
        boost::array<int, ALL_PATTERN>prev_tget_spd_;  //!< 全パターンの以前の目標速度[km/h]
        boost::array<float, ALL_PATTERN>prev_pattern_end_loc_;  //!< 全パターンの以前の減速完了地点[m]
        boost::array<int, ALL_PATTERN>fuzzy_brake_notch_list_;  //!< 全パターンの予見Fuzzy制御の最適ブレーキノッチ

        explicit Fuzzy(const Atc *atc);
        virtual ~Fuzzy(void);
        void Init(void);
        void FuzzyAdjDeceleration(void);
        int FuzzyCtrInit(int index, float pattern_start_loc);
        int FuzzyCtrExe(int index, int tget_spd, float pattern_end_loc);
    };
    ////////////////////////////////////////////////////////
    boost::scoped_ptr<AtcA>atc_a_;
    boost::scoped_ptr<AtcD>atc_d_;
    boost::scoped_ptr<Fuzzy>fuzzy_;

  public:
    int ServiceNotch;  //!< 常用最大ブレーキノッチ(HBを含む)
    int AtsNotch;  //!< ATS確認ノッチ(B1)
    const float *TrainSpeed;  //!< 列車速度[km/h]
    const float *Accel;  //!< 加速度[km/h/s]
    const int *Time;  //!< ゲーム内時刻[ms]
    const int *BrakeNotch;  //!< 出力ブレーキノッチ(HBを含む)
    const int *ManualBrakeNotch;  //!< 手動ブレーキノッチ(HBを含む)
    const int *PowerNotch;  //!< 出力力行ノッチ
    double Location;  //!< 列車位置[m]
    float max_deceleration_;  //!< 常用最大減速度[km/h/s]
    int atc_power_;  //!< ATC電源(0: 消灯, 1: 点灯)
    int atc_use_;  //!< ATC(0: 消灯, 1: 点灯)
    int atc_type_;  //!< ATC方式(0: ATC-1, 1: ATC-2, 2: ATC-NS, 3: KS-ATC, 4: DS-ATC)
    int atc_max_spd_;  //!< 車両ATC最高速度[km/h]
    boost::array<int, ALL_SIGNAL>atc_spd_list_;  //!< 信号インデックスに対応する速度[km/h]
    boost::array<float, BRAKE_STEP>atc_deceleration_;  //!< ATCブレーキ減速度[km/h/s]
    int atc_reset_sw_;  //!< 確認ボタンの状態(0: 開放, 1: 押下)
    int atc_brake_notch_;  //!< ATC出力ブレーキノッチ(HBを含まない)
    int atc_red_signal_;  //!< 停止現示(0: 消灯, 1: 点灯)
    int atc_green_signal_;  //!< 進行現示(0: 消灯, 1: 点灯)
    boost::array<int, ALL_SIGNAL>atc_sig_indicator_;  //!< ATC速度表示インジケータ(0: 消灯, 1: 点灯)
    boost::array<int, 3>atc_spd_7seg_;  //!< 7セグ用ATC速度表示
    int atc_signal_index_;  //!< ATC速度に対応する信号インデックス
    int atc_spd_;  //!< ATC速度[km/h]
    int dsatc_arrow_spd_;  //!< DS-ATC用パターン照査速度[km/h]
    boost::array<int, 3>sub_spd_label_1_;  //!< 副速度計用目盛 ATC速度-20 km/h
    boost::array<int, 3>sub_spd_label_2_;  //!< 副速度計用目盛 ATC速度-10 km/h
    boost::array<int, 3>sub_spd_label_3_;  //!< 副速度計用目盛 ATC速度
    boost::array<int, 3>sub_spd_label_4_;  //!< 副速度計用目盛 ATC速度+10 km/h
    int sub_atc_spd_;  //!< 副速度計用 ATC速度[km/h]
    int sub_train_spd_;  //!< 副速度計用 列車速度[km/h]
    float lever_delay_;  //!< ブレーキハンドルの操作から指令出力までの遅れ時間[s]
    int atc_ding_;  //!< ATCベルの状態
    int reset_sw_up_sound_;  //!< 確認ボタンの解放音の状態
    int reset_sw_down_sound_;  //!< 確認ボタンの押下音の状態
    int atc_eb_lamp_;  //!< ATC非常(0: 消灯, 1: 点灯)
    int atc_svc_lamp_;  //!< ATC常用(0: 消灯, 1: 点灯)
    float Xe;  //!< 減速完了地点からの許容誤差[m]
    float Xo;  //!< 減速完了地点からの過走限界距離[m]
    float Xk;  //!< XeおよびXoを高速域で拡大させる係数

    Atc(void);
    virtual ~Atc(void);
    void InitNS(void);
    void StartNS(void);
    void ExitNS(void);
    void ChangedSignalNS(int signal);
    void PassedBeaconNS(int index, int signal, float distance, int optional);
    void ResetSwDownNS(void);
    void ResetSwUpNS(void);
    void ExeNS(void);
};

#endif  // ATC_H_
