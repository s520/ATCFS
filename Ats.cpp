#include "stdafx.h"
#include <Shlwapi.h>
#include <deque>
#include "Accel.h"
#include "Atc.h"
#include "AtsP.h"
#include "Wiper.h"
#include "Sub.h"
#include "AtcfsCfg.h"
#include "atsplugin.h"
#include "Ats.h"

BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        g_accel.Time = &g_time;
        g_accel.TrainSpeed = &g_speed;

        g_atc.TrainSpeed = &g_speed;
        g_atc.Accel = &g_accel.ema_accel_;
        g_atc.Time = &g_time;
        g_atc.BrakeNotch = &g_output.Brake;
        g_atc.ManualBrakeNotch = &g_brakeNotch;
        g_atc.PowerNotch = &g_output.Power;

        g_atsp.TrainSpeed = &g_speed;
        g_atsp.Time = &g_time;
        g_atsp.ManualBrakeNotch = &g_brakeNotch;

        g_wiper.DeltaT = &g_deltaT;

        g_sub.TrainSpeed = &g_speed;
        g_sub.Time = &g_time;
        g_sub.Reverser = &g_reverser;
        g_sub.atc_brake_notch_ = &g_atc.atc_brake_notch_;
        break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        break;
    default:
        break;
    }

    // CFGファイル読み込み
    char file_path[_MAX_PATH + 1] = _T("");  // ファイルパス格納
    bool load_check;  // CFGファイルのロードに成功したかどうか

    // dllのファイルパスを取得する
    ::GetModuleFileName((HMODULE)hModule, file_path, _MAX_PATH);

    // .dllを.cfgに置換する
    ::PathRenameExtension(file_path, ".cfg");

    // CFGファイルをロードして結果を取得する
    load_check = g_ini.load(file_path);

    return TRUE;
}

/// <summary>
/// プラグインが読み込まれたときに呼び出される関数
/// </summary>
ATS_API void WINAPI Load() {
}

/// <summary>
/// プラグインが解放されたときに呼び出される関数
/// </summary>
ATS_API void WINAPI Dispose() {
}

/// <summary>
/// このATSプラグインが準じているフォーマットを返す関数
/// </summary>
/// <returns>フォーマットのバージョン</returns>
ATS_API int WINAPI GetPluginVersion() {
    return ATS_VERSION;
}

/// <summary>
/// 車両読み込み時に呼び出される関数
/// </summary>
/// <param name="vehicleSpec">車両諸元</param>
ATS_API void WINAPI SetVehicleSpec(ATS_VEHICLESPEC vehicleSpec) {
    g_svcBrake = vehicleSpec.BrakeNotches;
    g_emgBrake = g_svcBrake + 1;

    g_atc.ServiceNotch = vehicleSpec.BrakeNotches;
    g_atc.AtsNotch = vehicleSpec.AtsNotch;

    g_atsp.ServiceNotch = vehicleSpec.BrakeNotches;
    g_atsp.AtsNotch = vehicleSpec.AtsNotch;
}

/// <summary>
/// ゲーム開始時に呼び出される関数
/// </summary>
/// <param name="brake">ゲーム開始時のブレーキ弁の状態</param>
ATS_API void WINAPI BVE_Initialize(int brake) {
    g_atc.max_deceleration_ = g_ini.Emulate.MaxDeceleration / 1000.0f;
    g_atc.lever_delay_ = g_ini.Emulate.LeverDelay / 1000.0f;
    g_atc.atc_use_ = g_ini.ATC.AtcUse;
    g_atc.atc_type_ = g_ini.ATC.AtcType;
    g_atc.atc_max_spd_ = g_ini.ATC.AtcMax;
    boost::get<0>(g_atc.atc_spd_list_) = 0;
    boost::get<1>(g_atc.atc_spd_list_) = 30;
    boost::get<2>(g_atc.atc_spd_list_) = 70;
    boost::get<3>(g_atc.atc_spd_list_) = g_ini.ATC.AtcSpeed3;
    boost::get<4>(g_atc.atc_spd_list_) = g_ini.ATC.AtcSpeed4;
    boost::get<5>(g_atc.atc_spd_list_) = g_ini.ATC.AtcSpeed5;
    boost::get<6>(g_atc.atc_spd_list_) = g_ini.ATC.AtcSpeed6;
    boost::get<7>(g_atc.atc_spd_list_) = g_ini.ATC.AtcSpeed7;
    boost::get<8>(g_atc.atc_spd_list_) = g_ini.ATC.AtcSpeed8;
    boost::get<9>(g_atc.atc_spd_list_) = g_ini.ATC.AtcSpeed9;
    boost::get<10>(g_atc.atc_spd_list_) = g_ini.ATC.AtcSpeed10;
    boost::get<11>(g_atc.atc_spd_list_) = g_ini.ATC.AtcSpeed11;
    boost::get<0>(g_atc.atc_deceleration_) = g_ini.ATC.AtcDeceleration1 / 1000.0f;
    boost::get<1>(g_atc.atc_deceleration_) = g_ini.ATC.AtcDeceleration2 / 1000.0f;
    boost::get<2>(g_atc.atc_deceleration_) = g_ini.ATC.AtcDeceleration3 / 1000.0f;
    boost::get<3>(g_atc.atc_deceleration_) = g_ini.ATC.AtcDeceleration4 / 1000.0f;
    g_atc.Xe = g_ini.ATC.Xe / 1000.0f;
    g_atc.Xo = g_ini.ATC.Xo / 1000.0f;
    g_atc.Xk = g_ini.ATC.Xk / 1000.0f;

    g_atsp.max_deceleration_ = g_ini.Emulate.MaxDeceleration / 1000.0f;
    g_atsp.atsp_use_ = g_ini.ATS_P.AtspUse;
    g_atsp.atsp_max_spd_ = g_ini.ATS_P.AtspMax;
    g_atsp.atsp_deceleration_ = g_ini.ATS_P.AtspDeceleration / 1000.0f;

    g_wiper.wiper_rate_ = g_ini.Wiper.WiperRate;
    g_wiper.wiper_hold_position_ = g_ini.Wiper.WiperHoldPosition;
    g_wiper.wiper_delay_ = g_ini.Wiper.WiperDelay;
    g_wiper.wiper_sound_behaviour_ = g_ini.Wiper.WiperSoundBehaviour;

    g_accel.Init();
    g_atc.InitNS();
    g_atsp.InitP();
    g_wiper.Init();
    g_sub.Init();
    g_speed = 0;
}

/// <summary>
/// 1フレームごとに呼び出される関数
/// </summary>
/// <param name="vehicleState">車両の状態量</param>
/// <param name="panel">パネルに渡す値</param>
/// <param name="sound">サウンド</param>
/// <returns>BVEに渡す制御値</returns>
ATS_API ATS_HANDLES WINAPI Elapse(ATS_VEHICLESTATE vehicleState, int *panel, int *sound) {
    g_deltaT = vehicleState.Time - g_time;
    g_time = vehicleState.Time;
    g_speed = vehicleState.Speed;

    g_atc.Location = vehicleState.Location;  // 列車位置[m]
    g_atsp.Location = vehicleState.Location;  // 列車位置[m]

    PassedBeacon();  // SetBeaconData

    g_accel.Exe();  // 加速度算出
    g_atc.ExeNS();  // ATC
    g_atsp.ExeP();  // ATS-P
    g_wiper.Exe();  // ワイパー
    g_sub.Exe();  // その他機能

    // ハンドル出力
    if (g_atsp.atsp_use_ == 1 && g_atsp.atsp_brake_notch_ > g_brakeNotch) {
        g_output.Brake = g_atsp.atsp_brake_notch_ + g_atsp.AtsNotch - 1;
        g_output.ConstantSpeed = ATS_CONSTANTSPEED_DISABLE;

        g_powerNotch = 0;  // 手放し運転防止
    } else if (g_atc.atc_use_ == 1 && g_atc.atc_brake_notch_ > g_brakeNotch) {
        g_output.Brake = g_atc.atc_brake_notch_ + g_atc.AtsNotch - 1;
        g_output.ConstantSpeed = ATS_CONSTANTSPEED_DISABLE;

        g_powerNotch = 0;  // 手放し運転防止
    } else {
        g_output.Brake = g_brakeNotch;
        g_output.ConstantSpeed = ATS_CONSTANTSPEED_CONTINUE;
    }
    if (g_pilotlamp) {
        g_output.Reverser = g_reverser;
    } else {
        g_output.Reverser = 0;
    }
    g_output.Power = g_powerNotch;

    // パネル表示リセット
    for (int i = 0; i < 256; i++) { panel[i] = 0; }

    // パネル出力
    // ATS-P
    panel[2] = g_atsp.atsp_power_;  // ATS-P電源
    panel[3] = g_atsp.atsp_aprch_lamp_;  // ATS-Pパターン接近
    panel[4] = g_atsp.atsp_reset_lamp_;  // ATS-Pブレーキ開放
    panel[5] = g_atsp.atsp_brake_lamp_;  // ATS-Pブレーキ動作
    panel[6] = g_atsp.atsp_use_;  // ATS-P
    panel[7] = 0;  // 故障

    // EB
    //panel[8] =  // EB作動

    // 時計
    panel[10] = g_time / 3600000;  // デジタル時計(時)
    panel[11] = g_time / 60000 % 60;  // デジタル時計(分)
    panel[12] = g_time / 1000 % 60;  // デジタル時計(秒)

    // ATC-NS
    panel[13] = g_atc.atc_power_;  // ATC電源
    panel[14] = g_atc.atc_use_;  // ATC
    panel[9] = g_atc.atc_reset_sw_;  // ATC確認ボタン
    panel[15] = g_atc.atc_eb_lamp_;  // ATC非常
    panel[16] = g_atc.atc_svc_lamp_;  // ATC常用
    panel[17] = g_atc.atc_red_signal_;  // ATC停止現示
    panel[18] = g_atc.atc_green_signal_;  // ATC進行現示
    panel[19] = g_atc.atc_spd_;  // ATC速度
    panel[20] = boost::get<0>(g_atc.atc_sig_indicator_);  // ATC-01
    panel[21] = boost::get<1>(g_atc.atc_sig_indicator_);  // ATC-30
    panel[22] = boost::get<2>(g_atc.atc_sig_indicator_);  // ATC-70
    panel[23] = boost::get<3>(g_atc.atc_sig_indicator_);  // ATC-120 (東北, 上越, 北陸: ATC-110)
    panel[24] = boost::get<4>(g_atc.atc_sig_indicator_);  // ATC-170 (東北, 上越, 北陸: ATC-160)
    panel[25] = boost::get<5>(g_atc.atc_sig_indicator_);  // ATC-220 (東北, 上越, 北陸: ATC-210)
    panel[26] = boost::get<6>(g_atc.atc_sig_indicator_);  // ATC-230 (東北, 上越, 北陸: ATC-240)
    panel[27] = boost::get<7>(g_atc.atc_sig_indicator_);  // ATC-255 (東北, 上越: ATC-245, 北陸: ATC-260)
    panel[28] = boost::get<8>(g_atc.atc_sig_indicator_);  // ATC-270 (東北, 上越: ATC-275, 北陸: ATC-260)
    panel[29] = boost::get<9>(g_atc.atc_sig_indicator_);  // ATC-275 (東北, 上越: ATC-300, 北陸: ATC-260)
    panel[30] = boost::get<10>(g_atc.atc_sig_indicator_);  // ATC-285 (東北, 上越: ATC-320, 北陸: ATC-260)
    panel[31] = boost::get<11>(g_atc.atc_sig_indicator_);  // ATC-300 (東北, 上越: ATC-360, 北陸: ATC-260)
    panel[33] = g_atc.atc_type_;  // ATC方式
    panel[34] = g_atc.atc_signal_index_;  // ATC速度のインデックス
    panel[35] = boost::get<0>(g_atc.atc_spd_7seg_);  // ATC速度の百の位
    panel[36] = boost::get<1>(g_atc.atc_spd_7seg_);  // ATC速度の十の位
    panel[37] = boost::get<2>(g_atc.atc_spd_7seg_);  // ATC速度の一の位
    panel[38] = boost::get<0>(g_atc.sub_spd_label_1_);  // 副速度計用目盛 ATC速度-20 km/hの百の位
    panel[39] = boost::get<1>(g_atc.sub_spd_label_1_);  // 副速度計用目盛 ATC速度-20 km/hの十の位
    panel[40] = boost::get<2>(g_atc.sub_spd_label_1_);  // 副速度計用目盛 ATC速度-20 km/hの一の位
    panel[41] = boost::get<0>(g_atc.sub_spd_label_2_);  // 副速度計用目盛 ATC速度-10 km/hの百の位
    panel[42] = boost::get<1>(g_atc.sub_spd_label_2_);  // 副速度計用目盛 ATC速度-10 km/hの十の位
    panel[43] = boost::get<2>(g_atc.sub_spd_label_2_);  // 副速度計用目盛 ATC速度-10 km/hの一の位
    panel[44] = boost::get<0>(g_atc.sub_spd_label_3_);  // 副速度計用目盛 ATC速度の百の位
    panel[45] = boost::get<1>(g_atc.sub_spd_label_3_);  // 副速度計用目盛 ATC速度の十の位
    panel[46] = boost::get<2>(g_atc.sub_spd_label_3_);  // 副速度計用目盛 ATC速度の一の位
    panel[47] = boost::get<0>(g_atc.sub_spd_label_4_);  // 副速度計用目盛 ATC速度+10 km/hの百の位
    panel[48] = boost::get<1>(g_atc.sub_spd_label_4_);  // 副速度計用目盛 ATC速度+10 km/hの十の位
    panel[49] = boost::get<2>(g_atc.sub_spd_label_4_);  // 副速度計用目盛 ATC速度+10 km/hの一の位
    panel[50] = g_atc.sub_atc_spd_;  // 副速度計用 ATC速度
    panel[51] = g_atc.sub_train_spd_;  // 副速度計用 車両速度
    panel[52] = (g_atc.atc_type_ == 4) ? g_atc.dsatc_arrow_spd_ : 0;  // DS-ATC用パターン照査速度
    panel[120] = boost::get<0>(g_atc.atc_sig_indicator_);  // ATC-01
    panel[126] = boost::get<1>(g_atc.atc_sig_indicator_);  // ATC-30
    panel[134] = boost::get<2>(g_atc.atc_sig_indicator_);  // ATC-70
    panel[142] = (g_atc.atc_type_ == 1 || g_atc.atc_type_ == 4) ? boost::get<3>(g_atc.atc_sig_indicator_) : 0;  // 東北, 上越, 北陸: ATC-110
    panel[144] = (g_atc.atc_type_ != 1 && g_atc.atc_type_ != 4) ? boost::get<3>(g_atc.atc_sig_indicator_) : 0;  // 東海, 山陽:       ATC-120
    panel[152] = (g_atc.atc_type_ == 1 || g_atc.atc_type_ == 4) ? boost::get<4>(g_atc.atc_sig_indicator_) : 0;  // 東北, 上越, 北陸: ATC-160
    panel[154] = (g_atc.atc_type_ != 1 && g_atc.atc_type_ != 4) ? boost::get<4>(g_atc.atc_sig_indicator_) : 0;  // 東海, 山陽:       ATC-170
    panel[162] = (g_atc.atc_type_ == 1 || g_atc.atc_type_ == 4) ? boost::get<5>(g_atc.atc_sig_indicator_) : 0;  // 東北, 上越, 北陸: ATC-210
    panel[164] = (g_atc.atc_type_ != 1 && g_atc.atc_type_ != 4) ? boost::get<5>(g_atc.atc_sig_indicator_) : 0;  // 東海, 山陽:       ATC-220
    panel[166] = (g_atc.atc_type_ != 1 && g_atc.atc_type_ != 4) ? boost::get<6>(g_atc.atc_sig_indicator_) : 0;  // 東海, 山陽:       ATC-230
    panel[168] = (g_atc.atc_type_ == 1 || g_atc.atc_type_ == 4) ? boost::get<6>(g_atc.atc_sig_indicator_) : 0;  // 東北, 上越, 北陸: ATC-240
    panel[169] = (g_atc.atc_type_ == 1 || g_atc.atc_type_ == 4) ? boost::get<7>(g_atc.atc_sig_indicator_) : 0;  // 東北, 上越:       ATC-245
    panel[171] = (g_atc.atc_type_ != 1 && g_atc.atc_type_ != 4) ? boost::get<7>(g_atc.atc_sig_indicator_) : 0;  // 東海, 山陽:       ATC-255
    panel[172] = (g_atc.atc_type_ == 1 || g_atc.atc_type_ == 4) ? boost::get<7>(g_atc.atc_sig_indicator_) : 0;  // 北陸:             ATC-260
    panel[174] = (g_atc.atc_type_ != 1 && g_atc.atc_type_ != 4) ? boost::get<8>(g_atc.atc_sig_indicator_) : 0;  // 東海, 山陽:       ATC-270
    panel[175] = (g_atc.atc_type_ == 1 || g_atc.atc_type_ == 4) ? boost::get<8>(g_atc.atc_sig_indicator_) : boost::get<9>(g_atc.atc_sig_indicator_);  // 東海, 山陽, 東北, 上越: ATC-275
    panel[177] = (g_atc.atc_type_ != 1 && g_atc.atc_type_ != 4) ? boost::get<10>(g_atc.atc_sig_indicator_) : 0;  // 東海, 山陽: ATC-285
    panel[180] = (g_atc.atc_type_ == 1 || g_atc.atc_type_ == 4) ? boost::get<9>(g_atc.atc_sig_indicator_) : boost::get<11>(g_atc.atc_sig_indicator_);  // 東海, 山陽, 東北, 上越: ATC-300
    panel[184] = (g_atc.atc_type_ == 1 || g_atc.atc_type_ == 4) ? boost::get<10>(g_atc.atc_sig_indicator_) : 0;  // 東北, 上越: ATC-320
    panel[192] = (g_atc.atc_type_ == 1 || g_atc.atc_type_ == 4) ? boost::get<11>(g_atc.atc_sig_indicator_) : 0;  // 東北, 上越: ATC-360

    // その他の機能
    panel[32] = g_sub.reverser_postion_;  // レバーサ表示
    panel[61] = g_sub.lcd_status_;  // LCD表示
    panel[62] = g_sub.light_status_;  // 手元灯
    //panel[192] = g_wiper.wiper_speed_;
    panel[193] = g_wiper.wiper_current_position_;  // ワイパー
    panel[194] = (static_cast<int>(g_atc.Location + g_sub.adj_loc_) % 1000000) / 100000;  // 距離程100kmの位
    panel[195] = (static_cast<int>(g_atc.Location + g_sub.adj_loc_) % 100000) / 10000;  // 距離程10kmの位
    panel[196] = (static_cast<int>(g_atc.Location + g_sub.adj_loc_) % 10000) / 1000;  // 距離程1kmの位
    panel[197] = (static_cast<int>(g_atc.Location + g_sub.adj_loc_) % 1000) / 100;  // 距離程100mの位
    panel[198] = (static_cast<int>(g_atc.Location + g_sub.adj_loc_) % 100) / 10;  // 距離程10mの位
    panel[199] = static_cast<int>(g_atc.Location + g_sub.adj_loc_) % 10;  // 距離程1mの位
    panel[200] = boost::get<0>(g_sub.digital_clock_);  // デジタル時計 (時)の十の位
    panel[201] = boost::get<1>(g_sub.digital_clock_);  // デジタル時計 (時)の一の位
    panel[202] = boost::get<2>(g_sub.digital_clock_);  // デジタル時計 (分)の十の位
    panel[203] = boost::get<3>(g_sub.digital_clock_);  // デジタル時計 (分)の一の位
    panel[204] = boost::get<4>(g_sub.digital_clock_);  // デジタル時計 (秒)の十の位
    panel[205] = boost::get<5>(g_sub.digital_clock_);  // デジタル時計 (秒)の一の位
    panel[206] = static_cast<int>(abs(vehicleState.Current) * 1000);  // 電流値

    // 0系, 200系用
    panel[214] = g_sub.ac_voltage_;  // 電車線電圧計
    panel[215] = g_sub.cv_voltage_;  // インバータ電圧計
    panel[216] = static_cast<int>(vehicleState.BcPressure / 98.0665f * 100.0f);
    panel[217] = static_cast<int>(vehicleState.SapPressure / 98.0665f * 100.0f);
    panel[218] = static_cast<int>(vehicleState.MrPressure / 98.0665f * 100.0f);
    panel[219] = (g_speed >= 30.0f && (g_output.Brake != 0));  // 電気ブレーキ作動灯
    panel[220] = boost::get<0>(g_sub.speedometer_);  // 0系/200系用速度計の針0-9[km/h]
    panel[221] = boost::get<1>(g_sub.speedometer_);  // 0系/200系用速度計の針10-19[km/h]
    panel[222] = boost::get<2>(g_sub.speedometer_);  // 0系/200系用速度計の針20-29[km/h]
    panel[223] = boost::get<3>(g_sub.speedometer_);  // 0系/200系用速度計の針30-39[km/h]
    panel[224] = boost::get<4>(g_sub.speedometer_);  // 0系/200系用速度計の針40-49[km/h]
    panel[225] = boost::get<5>(g_sub.speedometer_);  // 0系/200系用速度計の針50-59[km/h]
    panel[226] = boost::get<6>(g_sub.speedometer_);  // 0系/200系用速度計の針60-69[km/h]
    panel[227] = boost::get<7>(g_sub.speedometer_);  // 0系/200系用速度計の針70-79[km/h]
    panel[228] = boost::get<8>(g_sub.speedometer_);  // 0系/200系用速度計の針80-89[km/h]
    panel[229] = boost::get<9>(g_sub.speedometer_);  // 0系/200系用速度計の針90-99[km/h]
    panel[230] = boost::get<10>(g_sub.speedometer_);  // 0系/200系用速度計の針100-109[km/h]
    panel[231] = boost::get<11>(g_sub.speedometer_);  // 0系/200系用速度計の針110-119[km/h]
    panel[232] = boost::get<12>(g_sub.speedometer_);  // 0系/200系用速度計の針120-129[km/h]
    panel[233] = boost::get<13>(g_sub.speedometer_);  // 0系/200系用速度計の針130-139[km/h]
    panel[234] = boost::get<14>(g_sub.speedometer_);  // 0系/200系用速度計の針140-149[km/h]
    panel[235] = boost::get<15>(g_sub.speedometer_);  // 0系/200系用速度計の針150-159[km/h]
    panel[236] = boost::get<16>(g_sub.speedometer_);  // 0系/200系用速度計の針160-169[km/h]
    panel[237] = boost::get<17>(g_sub.speedometer_);  // 0系/200系用速度計の針170-179[km/h]
    panel[238] = boost::get<18>(g_sub.speedometer_);  // 0系/200系用速度計の針180-189[km/h]
    panel[239] = boost::get<19>(g_sub.speedometer_);  // 0系/200系用速度計の針190-199[km/h]
    panel[240] = boost::get<20>(g_sub.speedometer_);  // 0系/200系用速度計の針200-209[km/h]
    panel[241] = boost::get<21>(g_sub.speedometer_);  // 0系/200系用速度計の針210-219[km/h]
    panel[242] = boost::get<22>(g_sub.speedometer_);  // 0系/200系用速度計の針220-229[km/h]
    panel[243] = boost::get<23>(g_sub.speedometer_);  // 0系/200系用速度計の針230-239[km/h]
    panel[244] = boost::get<24>(g_sub.speedometer_);  // 0系/200系用速度計の針240-249[km/h]
    panel[245] = boost::get<25>(g_sub.speedometer_);  // 0系/200系用速度計の針250-259[km/h]
    panel[246] = boost::get<26>(g_sub.speedometer_);  // 0系/200系用速度計の針260-269[km/h]
    panel[247] = boost::get<27>(g_sub.speedometer_);  // 0系/200系用速度計の針270-279[km/h]

    // サウンド出力
    sound[2] = g_atsp.atsp_ding_;  // ATS-Pベル
    sound[6] = g_wiper.wiper_sw_sound_;  // ワイパーON, OFFの速度調節スイッチ
    sound[7] = g_atc.atc_ding_;  // ATCベル
    sound[8] = g_atc.reset_sw_down_sound_;  // ATC確認ボタンの開放音
    sound[9] = g_atc.reset_sw_up_sound_;  // ATC確認ボタンの押下音
    sound[10] = g_sub.atc_air_sound_;  // ATCブレーキ緩解音
    sound[17] = (g_ini.Wiper.WiperWet == 0) ? g_wiper.wiper_sound_ : ATS_SOUND_STOP;  // ワイパー動作サウンド (空動作の時)
    sound[18] = (g_ini.Wiper.WiperWet != 0) ? g_wiper.wiper_sound_ : ATS_SOUND_STOP;  // ワイパー動作サウンド (降雨時)
    sound[61] = g_sub.lcd_sw_down_sound_;  // LCD表示切り替えスイッチの押下音
    sound[62] = g_sub.lcd_sw_up_sound_;  // LCD表示切り替えスイッチの開放音
    sound[63] = g_sub.light_sw_down_sound_;  // 手元灯スイッチの押下音
    sound[64] = g_sub.light_sw_up_sound_;  // 手元灯スイッチの開放音

    return g_output;
}

/// <summary>
/// 主ハンドルが扱われたときに呼び出される関数
/// </summary>
/// <param name="notch">力行ノッチ</param>
ATS_API void WINAPI SetPower(int notch) {
    g_powerNotch = notch;
}

/// <summary>
/// ブレーキが扱われたときに呼び出される関数
/// </summary>
/// <param name="notch">ブレーキノッチ</param>
ATS_API void WINAPI SetBrake(int notch) {
    g_brakeNotch = notch;
}

/// <summary>
/// レバーサーが扱われたときに呼び出される関数
/// </summary>
/// <param name="pos">レバーサー位置</param>
ATS_API void WINAPI SetReverser(int pos) {
    g_reverser = pos;
}

/// <summary>
/// ATSキーが押されたときに呼び出される関数
/// </summary>
/// <param name="atsKeyCode">ATSキー コード</param>
ATS_API void WINAPI KeyDown(int atsKeyCode) {
    switch (atsKeyCode) {
    case ATS_KEY_S:  // ATC確認
        g_atc.ResetSwDownNS();
        break;
    case ATS_KEY_B1:  // ATS-P復帰
        g_atsp.ResetSwDownP();
        break;
    case ATS_KEY_I:  // 手元灯
        g_sub.LightSwDown();
        break;
    case ATS_KEY_J:  // ワイパースピードアップ
        g_wiper.WiperRequest(0);
        break;
    case ATS_KEY_K:  // ワイパースピードダウン
        g_wiper.WiperRequest(1);
        break;
    case ATS_KEY_L:  // LCD表示切り替え
        g_sub.LcdSwDown();
        break;
    default:
        break;
    }
}

/// <summary>
/// ATSキーが離されたときに呼び出される関数
/// </summary>
/// <param name="atsKeyCode">ATSキー コード</param>
ATS_API void WINAPI KeyUp(int atsKeyCode) {
    switch (atsKeyCode) {
    case ATS_KEY_S:  // ATC確認
        g_atc.ResetSwUpNS();
        break;
    case ATS_KEY_I:  // 手元灯
        g_sub.LightSwUp();
        break;
    case ATS_KEY_L:  // LCD表示切り替え
        g_sub.LcdSwUp();
        break;
    default:
        break;
    }
}

/// <summary>
/// 警笛が扱われたときに呼び出される関数
/// </summary>
/// <param name="hornType">警笛のタイプ</param>
ATS_API void WINAPI HornBlow(int hornType) {
}

/// <summary>
/// 客室ドアが開いたときに呼び出される関数
/// </summary>
ATS_API void WINAPI DoorOpen() {
    g_pilotlamp = false;
}

/// <summary>
/// 客室ドアが閉まったときに呼び出される関数
/// </summary>
ATS_API void WINAPI DoorClose() {
    g_pilotlamp = true;
}

/// <summary>
/// 現在の閉塞の信号が変化したときに呼び出される関数
/// </summary>
/// <param name="signal">信号番号</param>
ATS_API void WINAPI SetSignal(int signal) {
    g_atc.ChangedSignalNS(signal);
}

/// <summary>
/// 地上子を越えたときに呼び出される関数
/// </summary>
/// <param name="beaconData">車上子で受け取った情報</param>
ATS_API void WINAPI SetBeaconData(ATS_BEACONDATA beaconData) {
    g_beacon_type.push_back(beaconData.Type);
    g_beacon_sig.push_back(beaconData.Signal);
    g_beacon_dist.push_back(beaconData.Distance);
    g_beacon_opt.push_back(beaconData.Optional);
}

/// <summary>
/// SetBeaconDataの実行タイミングを制御するための関数
/// </summary>
void PassedBeacon() {
    while (!g_beacon_type.empty() || !g_beacon_sig.empty() || !g_beacon_dist.empty() || !g_beacon_opt.empty()) {
        int beacon_type = g_beacon_type.front();
        int beacon_sig = g_beacon_sig.front();
        float beacon_dist = g_beacon_dist.front();
        int beacon_opt = g_beacon_opt.front();
        g_atc.PassedBeaconNS(beacon_type, beacon_sig, beacon_dist, beacon_opt);
        g_atsp.PassedBeaconP(beacon_type, beacon_sig, beacon_dist, beacon_opt);
        switch (beacon_type) {
        case 100:
            g_sub.SetAdjLoc(beacon_opt);
            break;
        default:
            break;
        }
        g_beacon_type.pop_front();
        g_beacon_sig.pop_front();
        g_beacon_dist.pop_front();
        g_beacon_opt.pop_front();
    }
}