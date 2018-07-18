#ifndef ATS_H_
#define ATS_H_

#include <deque>

int g_emgBrake;  // 非常ノッチ
int g_svcBrake;  // 常用最大ノッチ
int g_brakeNotch;  // ブレーキノッチ
int g_powerNotch;  // 力行ノッチ
int g_reverser;  // レバーサ
bool g_pilotlamp;  // パイロットランプ
int g_time;  // 現在時刻
float g_speed;  // 速度計の速度[km/h]
int g_deltaT;  // フレーム時間[ms/frame]

ATS_HANDLES g_output;  // 出力

void PassedBeacon(void);
std::deque<int>g_beacon_type;  //!< 地上子種別
std::deque<int>g_beacon_sig;  //!< 対となるセクションの信号
std::deque<float>g_beacon_dist;  //!< 対となるセクションまでの距離[m]
std::deque<int>g_beacon_opt;  //!< 地上子に設定された任意の値

Accel g_accel;  //!< 加速度
Atc g_atc;  //!< ATC
Atsp g_atsp;  //!< ATS-P
Wiper g_wiper;  //!< ワイパー
Sub g_sub;  //!< その他機能
AtcfsCfg g_ini;  //!< Iniファイル

#endif  // ATS_H_
