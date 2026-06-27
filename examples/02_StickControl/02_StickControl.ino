/* =====================================================
 * NXProcon Example 2: Stick Control
 * スティックで左回転しながらAボタンを押す例
 * ===================================================== */

#include <NXProcon.h>

void setup() {
    NXProcon.begin();
}

void loop() {
    NXProcon.update();

    // 左スティックを左に倒す
    NXProcon.leftStick(STICK_MIN, STICK_CENTER);
    delay(500);

    // 中央に戻す + Aボタン
    NXProcon.leftStick(STICK_CENTER, STICK_CENTER);
    NXProcon.tap(BUTTON_A, 80, 80);

    // 右スティックを右上に倒す
    NXProcon.rightStick(STICK_MAX, STICK_MAX);
    delay(300);
    NXProcon.rightStick(STICK_CENTER, STICK_CENTER);
    delay(200);
}
