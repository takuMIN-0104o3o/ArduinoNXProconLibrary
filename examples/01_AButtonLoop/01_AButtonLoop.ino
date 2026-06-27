#include <NXProcon.h>

const float MOUSE_THRESHOLD = 3000.0;
const float GYRO_SCALE_Y = 57.3 / 0.4;

// ★ 変数名を変更（SPEED → GYRO_SPEED）
const int16_t GYRO_SPEED = 300;       // 適度な速度
const int16_t MAX_VALUE = 3000;

void setup() {
    Serial.begin(115200);
    NXProcon.begin();
    delay(1500);
    NXProcon.enableIMU(true);
    NXProcon.setIMU(0, 0, 4096, 0, 0, 0);
}

void loop() {
    NXProcon.update();
    
    static uint32_t lastUpdate = 0;
    static int16_t y_movement = 0;
    static bool goingUp = true;
    
    if (millis() - lastUpdate >= 10) {
        lastUpdate = millis();
        
        if (goingUp) {
            y_movement += GYRO_SPEED;  // ★ 変更
            if (y_movement >= MAX_VALUE) {
                y_movement = MAX_VALUE;
                goingUp = false;
            }
        } else {
            y_movement -= GYRO_SPEED;  // ★ 変更
            if (y_movement <= -MAX_VALUE) {
                y_movement = -MAX_VALUE;
                goingUp = true;
            }
        }
        
        int16_t gyroYValue = (int16_t)((y_movement / MOUSE_THRESHOLD) * GYRO_SCALE_Y);
        NXProcon.setIMU(0, 0, 4096, 0, gyroYValue, 0);
    }
    
    NXProcon.press(BUTTON_A);
    delay(1);
}