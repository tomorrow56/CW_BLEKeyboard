# CW BLE Keyboard

本プロジェクトはBLEキーボードとして機能する１キーのモールス符号デコーダーです。
スケッチはM5StickCを使って動作させていますが、M5Unifiedライブラリを使用していますので、入力ピンの設定を変更すれば他のM5Stackシリーズでも動作します。

This project is a one-key Morse code(CW) decoder that functions as a BLE keyboard.
The sketch is made to work with M5StickC. It uses the M5Unified library, so it will work with other M5Stack series by changing the input pin settings.

キーボードとして接続する前は、左上のマークが赤くなります。

Before connecting as a keyboard, the symbol in the upper left corner turns red.

![no-connect](https://user-images.githubusercontent.com/13660495/206889213-c73abf8c-c0b1-4310-a6d9-9d3fbc643325.jpg)

キーボードとして接続すると、左上のマークが緑になります。

When connected as a keyboard, the symbol in the upper left corner turns green.

![connected](https://user-images.githubusercontent.com/13660495/206889583-3d1ca39f-69c7-4e30-8aa4-d7930b1f6a67.jpg)

PCからは"CW-Keyboard"という名前のBluetoothキーボードとして認識されます。

The PC will recognize it as a Bluetooth keyboard named "CW-Keyboard".

![Pairing](https://user-images.githubusercontent.com/13660495/206889008-5ec49743-a46a-4575-bf12-8d215be76d71.png)



https://user-images.githubusercontent.com/13660495/206889086-9dff52f5-0117-41d5-aa47-e9632d1e677c.mov

