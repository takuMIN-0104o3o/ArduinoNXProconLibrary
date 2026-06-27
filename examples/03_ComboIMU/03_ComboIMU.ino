/* =====================================================
 * NXProcon Example 3: Combo + IMU
 * 複数ボタン同時押し & IMU注入の例
 * ===================================================== */

#include <NXProcon.h>

void setup() {
    NXProcon.begin();
}

void loop() {
    NXProcon.update();

    // ZL+ZR+L+R を同時に押す (特定ゲームのリセット等)
    NXProcon.tap(BUTTON_ZL | BUTTON_ZR | BUTTON_L | BUTTON_R, 200, 500);

    // 方向キーで上→右→下→左 (回転入力)
    NXProcon.tap(BUTTON_UP,    80, 40);
    NXProcon.tap(BUTTON_RIGHT, 80, 40);
    NXProcon.tap(BUTTON_DOWN,  80, 40);
    NXProcon.tap(BUTTON_LEFT,  80, 40);

    // IMUを傾いた状態にする (accelY に傾き値を注入)
    NXProcon.setIMU(
        0,       0x1000, 0x2000,  // accel X/Y/Z
        0,       0,      0        // gyro  X/Y/Z
    );
    delay(1000);

    // IMUを静止状態に戻す
    NXProcon.resetIMU();
    delay(1000);
}
