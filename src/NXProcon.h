/**
 * NXProcon.h - .ino から IMU を制御できる拡張版
 */
#pragma once
#include <Arduino.h>
#include "PluggableUSB.h"
#include "HID.h"

// ═══════════════════════════════════════════════
//  ボタン定数
// ═══════════════════════════════════════════════
#define BUTTON_Y       (1UL<<0)
#define BUTTON_X       (1UL<<1)
#define BUTTON_B       (1UL<<2)
#define BUTTON_A       (1UL<<3)
#define BUTTON_SR_R    (1UL<<4)
#define BUTTON_SL_R    (1UL<<5)
#define BUTTON_R       (1UL<<6)
#define BUTTON_ZR      (1UL<<7)
#define BUTTON_MINUS   (1UL<<8)
#define BUTTON_PLUS    (1UL<<9)
#define BUTTON_RSTICK  (1UL<<10)
#define BUTTON_LSTICK  (1UL<<11)
#define BUTTON_HOME    (1UL<<12)
#define BUTTON_CAPTURE (1UL<<13)
#define BUTTON_DOWN    (1UL<<16)
#define BUTTON_UP      (1UL<<17)
#define BUTTON_RIGHT   (1UL<<18)
#define BUTTON_LEFT    (1UL<<19)
#define BUTTON_SR_L    (1UL<<20)
#define BUTTON_SL_L    (1UL<<21)
#define BUTTON_L       (1UL<<22)
#define BUTTON_ZL      (1UL<<23)

#define DPAD_UP    BUTTON_UP
#define DPAD_DOWN  BUTTON_DOWN
#define DPAD_LEFT  BUTTON_LEFT
#define DPAD_RIGHT BUTTON_RIGHT

#define STICK_CENTER 0x800
#define STICK_MIN    0x000
#define STICK_MAX    0xFFF

#define IMU_SAMPLES 3
#define IMU_1G      4096

// ═══════════════════════════════════════════════
//  USB HID レイヤ
// ═══════════════════════════════════════════════
static const uint8_t PROCON_HID_DESCRIPTOR[] PROGMEM = {
    0x05, 0x01, 0x15, 0x00, 0x09, 0x04, 0xA1, 0x01,
    0x85, 0x30, 0x05, 0x01, 0x05, 0x09, 0x19, 0x01,
    0x29, 0x0A, 0x15, 0x00, 0x25, 0x01, 0x75, 0x01,
    0x95, 0x0A, 0x55, 0x00, 0x65, 0x00, 0x81, 0x02,
    0x05, 0x09, 0x19, 0x0B, 0x29, 0x0E, 0x15, 0x00,
    0x25, 0x01, 0x75, 0x01, 0x95, 0x04, 0x81, 0x02,
    0x75, 0x01, 0x95, 0x02, 0x81, 0x03, 0x0B, 0x01,
    0x00, 0x01, 0x00, 0xA1, 0x00, 0x0B, 0x30, 0x00,
    0x01, 0x00, 0x0B, 0x31, 0x00, 0x01, 0x00, 0x0B,
    0x32, 0x00, 0x01, 0x00, 0x0B, 0x35, 0x00, 0x01,
    0x00, 0x15, 0x00, 0x27, 0xFF, 0xFF, 0x00, 0x00,
    0x75, 0x10, 0x95, 0x04, 0x81, 0x02, 0xC0, 0x0B,
    0x39, 0x00, 0x01, 0x00, 0x15, 0x00, 0x25, 0x07,
    0x35, 0x00, 0x46, 0x3B, 0x01, 0x65, 0x14, 0x75,
    0x04, 0x95, 0x01, 0x81, 0x02, 0x05, 0x09, 0x19,
    0x0F, 0x29, 0x12, 0x15, 0x00, 0x25, 0x01, 0x75,
    0x01, 0x95, 0x04, 0x81, 0x02, 0x75, 0x08, 0x95,
    0x34, 0x81, 0x03, 0x06, 0x00, 0xFF, 0x85, 0x21,
    0x09, 0x01, 0x75, 0x08, 0x95, 0x3F, 0x81, 0x03,
    0x85, 0x81, 0x09, 0x02, 0x75, 0x08, 0x95, 0x3F,
    0x81, 0x03, 0x85, 0x01, 0x09, 0x03, 0x75, 0x08,
    0x95, 0x3F, 0x91, 0x83, 0x85, 0x10, 0x09, 0x04,
    0x75, 0x08, 0x95, 0x3F, 0x91, 0x83, 0x85, 0x80,
    0x09, 0x05, 0x75, 0x08, 0x95, 0x3F, 0x91, 0x83,
    0x85, 0x82, 0x09, 0x06, 0x75, 0x08, 0x95, 0x3F,
    0x91, 0x83, 0xC0
};

// ═══════════════════════════════════════════════
//  USB クラス
// ═══════════════════════════════════════════════
class NXProconUSB : public PluggableUSBModule {
public:
    NXProconUSB() : PluggableUSBModule(2, 1, epType) {
        epType[0] = EP_TYPE_INTERRUPT_IN;
        epType[1] = EP_TYPE_INTERRUPT_OUT;
        PluggableUSB().plug(this);
    }

    void begin() {}

    uint8_t getEPIn()  { return pluggedEndpoint; }
    uint8_t getEPOut() { return pluggedEndpoint + 1; }

    int sendReport(uint8_t* buf, uint8_t len) {
        return USB_Send(pluggedEndpoint | TRANSFER_RELEASE, buf, len);
    }

protected:
    int getInterface(uint8_t* interfaceCount) override {
        *interfaceCount += 1;

        uint8_t iface[] = {
            0x09, 0x04,
            pluggedInterface,
            0x00,
            0x02,
            0x03,
            0x00, 0x00, 0x00
        };
        uint8_t hidDesc[] = {
            0x09, 0x21,
            0x11, 0x01,
            0x00,
            0x01,
            0x22,
            (uint8_t)(sizeof(PROCON_HID_DESCRIPTOR) & 0xFF),
            (uint8_t)((sizeof(PROCON_HID_DESCRIPTOR) >> 8) & 0xFF)
        };
        uint8_t epIn[] = {
            0x07, 0x05,
            (uint8_t)(pluggedEndpoint | 0x80),
            0x03, 0x40, 0x00, 0x08
        };
        uint8_t epOut[] = {
            0x07, 0x05,
            (uint8_t)(pluggedEndpoint + 1),
            0x03, 0x40, 0x00, 0x08
        };

        USB_SendControl(0, iface,    sizeof(iface));
        USB_SendControl(0, hidDesc,  sizeof(hidDesc));
        USB_SendControl(0, epIn,     sizeof(epIn));
        USB_SendControl(0, epOut,    sizeof(epOut));
        return 0;
    }

    int getDescriptor(USBSetup& setup) override {
        if (setup.bmRequestType == 0x81 &&
            setup.bRequest == GET_DESCRIPTOR &&
            setup.wValueH == 0x22) {
            USB_SendControl(TRANSFER_PGM,
                PROCON_HID_DESCRIPTOR,
                sizeof(PROCON_HID_DESCRIPTOR));
            return sizeof(PROCON_HID_DESCRIPTOR);
        }
        return 0;
    }

    bool setup(USBSetup& s) override {
        if (s.bmRequestType == 0x21) return true;
        return false;
    }

    uint8_t getShortName(char* name) override {
        name[0] = 'N'; name[1] = 'X';
        return 2;
    }

private:
    uint8_t epType[2];
};

// ═══════════════════════════════════════════════
//  メインクラス（.ino から制御可能）
// ═══════════════════════════════════════════════
class NXProconClass {
public:
    NXProconClass() : _buttons(0),
        _lx(STICK_CENTER), _ly(STICK_CENTER),
        _rx(STICK_CENTER), _ry(STICK_CENTER),
        _connected(false), _inputMode(false),
        _imuEnabled(false),
        _lastSendMs(0), _counter(0) {
        // IMUデータをゼロ初期化
        memset(_imu, 0, sizeof(_imu));
        _hidOutReady = false;
        _hidOutLen = 0;
        memset((void*)_hidOutBuf, 0, 64);
    }

    void begin() {
        _usb.begin();
        delay(1500);
        _connected = true;
    }

    void update() {
        uint8_t data[64] = {0};
        int len = 0;
        
        if (_usb.getEPOut()) {
            len = USB_Recv(_usb.getEPOut(), data, 64);
            if (len > 0) {
                if (data[0] == 0x80) {
                    _handle80(data);
                } else if (data[0] == 0x01 && len > 10) {
                    _handleUART(data);
                }
            }
        }

        if (!_inputMode && millis() > 8000) {
            _inputMode = true;
        }

        if (_inputMode) {
            uint32_t now = millis();
            if (now - _lastSendMs >= 5) {
                _sendInput();
                _counter = (_counter + 3) % 256;
                _lastSendMs = now;
            }
        }
    }

    // ── ボタン操作 ──
    void press(uint32_t b) {
        _buttons |= b;
        if (_inputMode) {
            _sendInput();
            _counter = (_counter + 3) % 256;
        }
    }

    void release(uint32_t b) {
        _buttons &= ~b;
        if (_inputMode) {
            _sendInput();
            _counter = (_counter + 3) % 256;
        }
    }

    void releaseAll() {
        _buttons = 0;
        if (_inputMode) {
            _sendInput();
            _counter = (_counter + 3) % 256;
        }
    }

    void tap(uint32_t b, uint16_t holdMs = 80, uint16_t afterMs = 80) {
        press(b);
        wait(holdMs);
        release(b);
        wait(afterMs);
    }

    uint32_t buttonState() const { return _buttons; }

    void wait(uint16_t ms) {
        uint32_t t = millis();
        while (millis() - t < ms) {
            update();
        }
    }

    // ── スティック操作 ──
    void leftStick(uint16_t x, uint16_t y) {
        _lx = constrain(x, (uint16_t)STICK_MIN, (uint16_t)STICK_MAX);
        _ly = constrain(y, (uint16_t)STICK_MIN, (uint16_t)STICK_MAX);
    }

    void rightStick(uint16_t x, uint16_t y) {
        _rx = constrain(x, (uint16_t)STICK_MIN, (uint16_t)STICK_MAX);
        _ry = constrain(y, (uint16_t)STICK_MIN, (uint16_t)STICK_MAX);
    }

    void resetSticks() {
        _lx = _ly = _rx = _ry = STICK_CENTER;
    }

    // ── IMU 制御（.ino から自由に設定可能） ──
    
    /**
     * IMUを有効化/無効化
     */
    void enableIMU(bool enable) {
        _imuEnabled = enable;
        if (!enable) {
            memset(_imu, 0, sizeof(_imu));
        }
    }

    /**
     * 3フレーム分のIMUデータを一括設定（.ino から呼び出し可能）
     * @param sample0 最新フレーム
     * @param sample1 中間フレーム
     * @param sample2 最古フレーム
     */
    void setIMUSamples(const int16_t sample0[6], const int16_t sample1[6], const int16_t sample2[6]) {
        if (sample0) {
            for (int i = 0; i < 6; i++) _imu[0][i] = sample0[i];
        }
        if (sample1) {
            for (int i = 0; i < 6; i++) _imu[1][i] = sample1[i];
        }
        if (sample2) {
            for (int i = 0; i < 6; i++) _imu[2][i] = sample2[i];
        }
        _imuEnabled = true;
    }

    /**
     * 全フレーム同じIMUデータを設定（簡易版）
     * データ順序: [accelX, accelY, accelZ, gyroX, gyroY, gyroZ]
     */
    void setIMU(int16_t ax, int16_t ay, int16_t az,
                int16_t gx, int16_t gy, int16_t gz) {
        for (int i = 0; i < IMU_SAMPLES; i++) {
            _imu[i][0] = ax;
            _imu[i][1] = ay;
            _imu[i][2] = az;
            _imu[i][3] = gx;
            _imu[i][4] = gy;
            _imu[i][5] = gz;
        }
        _imuEnabled = true;
    }

    /**
     * 特定フレームのIMUデータを設定
     * @param frame 0=最新, 1=中間, 2=最古
     */
    void setIMUFrame(int frame, int16_t ax, int16_t ay, int16_t az,
                     int16_t gx, int16_t gy, int16_t gz) {
        if (frame < 0 || frame >= IMU_SAMPLES) return;
        _imu[frame][0] = ax;
        _imu[frame][1] = ay;
        _imu[frame][2] = az;
        _imu[frame][3] = gx;
        _imu[frame][4] = gy;
        _imu[frame][5] = gz;
        _imuEnabled = true;
    }

    /**
     * ジャイロY軸のみ設定（高速更新用）
     */
    void setGyroY(int16_t gy) {
        for (int i = 0; i < IMU_SAMPLES; i++) {
            _imu[i][4] = gy;  // gyroY
        }
        _imuEnabled = true;
    }

    /**
     * 加速度Z軸のみ設定（高速更新用）
     */
    void setAccelZ(int16_t az) {
        for (int i = 0; i < IMU_SAMPLES; i++) {
            _imu[i][2] = az;  // accelZ
        }
        _imuEnabled = true;
    }

    void resetIMU() {
        memset(_imu, 0, sizeof(_imu));
        _imuEnabled = false;
    }

    bool isIMUEnabled() const { return _imuEnabled; }

    void sendNow() {
        _sendInput();
        _counter = (_counter + 3) % 256;
    }

    bool isConnected() const { return _connected; }
    bool isInputMode() const { return _inputMode; }

private:
    NXProconUSB _usb;
    uint32_t _buttons;
    uint16_t _lx, _ly, _rx, _ry;
    int16_t _imu[IMU_SAMPLES][6];  // [フレーム][accelX, accelY, accelZ, gyroX, gyroY, gyroZ]
    bool _connected;
    bool _inputMode;
    bool _imuEnabled;
    uint32_t _lastSendMs;
    uint8_t _counter;

    uint8_t _hidOutBuf[64];
    uint8_t _hidOutLen;
    bool _hidOutReady;

    static const uint8_t _MAC[6];

    void _sendRaw(uint8_t* buf) {
        _usb.sendReport(buf, 64);
    }

    void _sendInput() {
        uint8_t buf[64] = {0};
        buf[0x00] = 0x30;
        buf[0x01] = _counter;
        buf[0x02] = 0x00;

        // ボタン
        buf[0x03] = (uint8_t)(_buttons & 0xFF);
        buf[0x04] = (uint8_t)((_buttons >> 8) & 0xFF);
        buf[0x05] = (uint8_t)((_buttons >> 16) & 0xFF);

        // スティック
        _setStick(buf, 0x06, _lx, _ly);
        _setStick(buf, 0x09, _rx, _ry);

        // ★★★ IMUデータ（.ino で設定した値を使用） ★★★
        if (_imuEnabled) {
            // フレーム0 (最新) - オフセット 0x0D (Accel) / 0x13 (Gyro)
            _setAccel(buf, 0x0D, _imu[0][0], _imu[0][1], _imu[0][2]);
            _setGyro(buf, 0x13, _imu[0][3], _imu[0][4], _imu[0][5]);

            // フレーム1 - オフセット 0x19 (Accel) / 0x1F (Gyro)
            _setAccel(buf, 0x19, _imu[1][0], _imu[1][1], _imu[1][2]);
            _setGyro(buf, 0x1F, _imu[1][3], _imu[1][4], _imu[1][5]);

            // フレーム2 (最古) - オフセット 0x25 (Accel) / 0x2B (Gyro)
            _setAccel(buf, 0x25, _imu[2][0], _imu[2][1], _imu[2][2]);
            _setGyro(buf, 0x2B, _imu[2][3], _imu[2][4], _imu[2][5]);
        }

        // 残りパディング
        for (int i = 0x31; i < 0x40; i++) {
            buf[i] = 0;
        }

        _sendRaw(buf);
    }

    void _setAccel(uint8_t* buf, int offset, int16_t x, int16_t y, int16_t z) {
        buf[offset]     = x & 0xFF;
        buf[offset + 1] = (x >> 8) & 0xFF;
        buf[offset + 2] = y & 0xFF;
        buf[offset + 3] = (y >> 8) & 0xFF;
        buf[offset + 4] = z & 0xFF;
        buf[offset + 5] = (z >> 8) & 0xFF;
    }

    void _setGyro(uint8_t* buf, int offset, int16_t x, int16_t y, int16_t z) {
        buf[offset]     = x & 0xFF;
        buf[offset + 1] = (x >> 8) & 0xFF;
        buf[offset + 2] = y & 0xFF;
        buf[offset + 3] = (y >> 8) & 0xFF;
        buf[offset + 4] = z & 0xFF;
        buf[offset + 5] = (z >> 8) & 0xFF;
    }

    void _setStick(uint8_t* buf, int offset, uint16_t x, uint16_t y) {
        x = constrain(x, (uint16_t)0, (uint16_t)0xFFF);
        y = constrain(y, (uint16_t)0, (uint16_t)0xFFF);
        uint16_t yInv = STICK_MAX - y;

        buf[offset]     = x & 0xFF;
        buf[offset + 1] = ((x >> 8) & 0x0F) | ((yInv & 0x0F) << 4);
        buf[offset + 2] = (yInv >> 4) & 0xFF;
    }

    void _uartResp(uint8_t ack, uint8_t sub, uint8_t* d, uint8_t dlen) {
        uint8_t buf[64] = {0};
        buf[0x00] = 0x21;
        buf[0x01] = _counter;
        buf[0x02] = 0x81;
        buf[0x03] = 0x00;
        buf[0x04] = 0x80;
        buf[0x05] = 0x00;
        buf[0x06] = 0xF8;
        buf[0x07] = 0xD7;
        buf[0x08] = 0x7A;
        buf[0x09] = 0x22;
        buf[0x0A] = 0xC8;
        buf[0x0B] = 0x7B;
        buf[0x0C] = 0x0C;
        buf[0x0D] = ack;
        buf[0x0E] = sub;
        if (d && dlen > 0) {
            memcpy(&buf[0x0F], d, min(dlen, (uint8_t)49));
        }
        _sendRaw(buf);
    }

    void _spiResp(uint8_t a0, uint8_t a1, uint8_t* d, uint8_t dlen) {
        uint8_t buf[64] = {0};
        buf[0] = a0;
        buf[1] = a1;
        buf[4] = dlen;
        memcpy(&buf[5], d, min(dlen, (uint8_t)59));
        _uartResp(0x90, 0x10, buf, 5 + dlen);
    }

    void _handleSPI(uint8_t* d) {
        uint8_t a0 = d[11], a1 = d[12], len = d[15];
        uint8_t r[64] = {0};
        uint8_t rlen = 0;

        if (a0 == 0x00 && a1 == 0x60) {
            memset(r, 0xFF, 16);
            rlen = 16;
        }
        else if (a0 == 0x50 && a1 == 0x60) {
            uint8_t t[] = {0xBC, 0x11, 0x42, 0x75, 0xA9, 0x28, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
            memcpy(r, t, sizeof(t));
            rlen = sizeof(t);
        }
        else if (a0 == 0x80 && a1 == 0x60) {
            uint8_t t[] = {0x50, 0xFD, 0x00, 0x00, 0xC6, 0x0F, 0x0F, 0x30, 0x61, 0x96, 0x30, 0xF3, 0xD4, 0x14, 0x54, 0x41, 0x15, 0x54, 0xC7, 0x79, 0x9C, 0x33, 0x36, 0x63};
            memcpy(r, t, sizeof(t));
            rlen = sizeof(t);
        }
        else if (a0 == 0x98 && a1 == 0x60) {
            uint8_t t[] = {0x0F, 0x30, 0x61, 0x96, 0x30, 0xF3, 0xD4, 0x14, 0x54, 0x41, 0x15, 0x54, 0xC7, 0x79, 0x9C, 0x33, 0x36, 0x63};
            memcpy(r, t, sizeof(t));
            rlen = sizeof(t);
        }
        else if (a0 == 0x20 && a1 == 0x60) {
            uint8_t t[] = {0x0F, 0x30, 0x61, 0x96, 0x30, 0xF3, 0xD4, 0x14, 0x54, 0x41, 0x15, 0x54, 0xC7, 0x79, 0x9C, 0x33, 0x36, 0x63, 0x0F, 0x30, 0x61, 0x96, 0x30, 0xF3, 0xD4, 0x14};
            memcpy(r, t, sizeof(t));
            rlen = sizeof(t);
        }
        else if (a0 == 0x3D && a1 == 0x60) {
            uint8_t t[] = {0xBA, 0x15, 0x62, 0x11, 0xB8, 0x7F, 0x29, 0x06, 0x5B, 0xFF, 0xE7, 0x7E, 0x0E, 0x36, 0x56, 0x9E, 0x85, 0x60, 0xFF, 0x32, 0x32, 0x32, 0xFF, 0xFF, 0xFF};
            memcpy(r, t, sizeof(t));
            rlen = sizeof(t);
        }
        else if (a0 == 0x86 && a1 == 0x60) {
            uint8_t t[] = {0x0F, 0x30, 0x61, 0x96, 0x30, 0xF3, 0xD4, 0x14, 0x54, 0x41, 0x15, 0x54, 0xC7, 0x79, 0x9C, 0x33, 0x36, 0x63};
            memcpy(r, t, sizeof(t));
            rlen = sizeof(t);
        }
        else if (a0 == 0x26 && a1 == 0x80) {
            uint8_t t[] = {0xBE, 0xFF, 0x3E, 0x00, 0xF0, 0x01, 0x00, 0x40, 0x00, 0x40, 0x00, 0x40, 0xFE, 0xFF, 0xFE, 0xFF, 0x08, 0x00, 0xE7, 0x3B};
            memcpy(r, t, sizeof(t));
            rlen = sizeof(t);
        }
        else if (a0 == 0x10 && a1 == 0x80) {
            memset(r, 0xFF, 22);
            r[22] = 0xB2;
            r[23] = 0xA1;
            rlen = 24;
        }
        else if (a0 == 0x28 && a1 == 0x80) {
            uint8_t t[] = {0xBE, 0xFF, 0x3E, 0x00, 0xF0, 0x01, 0x00, 0x40, 0x00, 0x40, 0x00, 0x40, 0xFE, 0xFF, 0xFE, 0xFF, 0x08, 0x00, 0xE7, 0x3B, 0xE7, 0x3B, 0xE7, 0x3B};
            memcpy(r, t, sizeof(t));
            rlen = sizeof(t);
        }
        else {
            memset(r, 0xFF, len);
            rlen = len;
        }

        if (len > 0 && len < rlen) rlen = len;
        _spiResp(a0, a1, r, rlen);
    }

    void _handleUART(uint8_t* d) {
        uint8_t sub = d[10];
        switch (sub) {
            case 0x01: {
                uint8_t r[] = {0x03};
                _uartResp(0x81, sub, r, 1);
                break;
            }
            case 0x02: {
                uint8_t r[] = {0x03, 0x49, 0x03, 0x02,
                    _MAC[5], _MAC[4], _MAC[3], _MAC[2], _MAC[1], _MAC[0],
                    0x03, 0x02};
                _uartResp(0x82, sub, r, sizeof(r));
                break;
            }
            case 0x03:
            case 0x08:
            case 0x30:
            case 0x33:
            case 0x38:
            case 0x40:
            case 0x41:
            case 0x48:
                _uartResp(0x80, sub, nullptr, 0);
                break;
            case 0x04:
                _uartResp(0x83, sub, nullptr, 0);
                break;
            case 0x10:
                _handleSPI(d);
                break;
            case 0x21: {
                uint8_t r[] = {0x01, 0x00, 0xFF, 0x00, 0x03, 0x00, 0x05, 0x01};
                _uartResp(0xA0, sub, r, sizeof(r));
                break;
            }
            default:
                _uartResp(0x80, sub, nullptr, 0);
                break;
        }
    }

    void _handle80(uint8_t* d) {
        switch (d[1]) {
            case 0x01: {
                uint8_t buf[64] = {0};
                buf[0] = 0x81;
                buf[1] = 0x01;
                buf[2] = 0x00;
                buf[3] = 0x03;
                buf[4] = _MAC[0];
                buf[5] = _MAC[1];
                buf[6] = _MAC[2];
                buf[7] = _MAC[3];
                buf[8] = _MAC[4];
                buf[9] = _MAC[5];
                _sendRaw(buf);
                break;
            }
            case 0x02:
            case 0x03: {
                uint8_t buf[64] = {0};
                buf[0] = 0x81;
                buf[1] = d[1];
                _sendRaw(buf);
                break;
            }
            case 0x04:
                _inputMode = true;
                break;
            case 0x05:
                _inputMode = false;
                break;
            default:
                break;
        }
    }
};

// ═══════════════════════════════════════════════
//  グローバルインスタンス
// ═══════════════════════════════════════════════
#ifndef NXPROCON_EXTERN_ONLY
NXProconClass NXProcon;
const uint8_t NXProconClass::_MAC[6] = {0x00, 0x00, 0x5E, 0x00, 0x53, 0x5E};
#endif