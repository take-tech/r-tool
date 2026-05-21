# R-Tool

R-Tool は、ゲイン調整・チャンネル反転・L/R スワップ・パン・ステレオ幅調整・アウトプットメーターをひとつにまとめた、軽量なステレオユーティリティプラグインです。

Bitwig Studio の「Tool」デバイスのように、ミックス作業で頻繁に必要になる処理をまとめ、どのトラックにも気軽に挿して使えることを目指しています。

---

## 機能

| コントロール | 概要 |
|---|---|
| **L-** | 左チャンネルの位相反転 |
| **Swap L/R** | 左右チャンネルの入れ替え |
| **R-** | 右チャンネルの位相反転 |
| **Volume** | 音量調整（-∞ ～ +18.1 dB） |
| **Gain** | ワイドレンジのゲイン調整（-36 ～ +36 dB） |
| **Pan** | イコールパワー・ステレオパン（L100 ～ C ～ R100） |
| **Width** | M/S方式によるステレオ幅調整（0% ～ 200%） |
| **Output Meter** | 全処理後の L/R ピークメーター |

---

## 対応フォーマット

- **VST3**（Windows / macOS / Linux）
- **AU**（macOS のみ）

---

## ビルド方法

### 必要な環境

- CMake 3.22 以上
- C++17 対応コンパイラ（Xcode / MSVC / GCC）
- `external/juce/` に JUCE が配置済みであること

### 手順

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

ビルド成果物は `build/RTool_artefacts/` 以下に出力されます。

---

## 開発状況

**v0.1.0 — プロトタイプ**

以下の機能が動作します：

- APVTS によるパラメータ管理
- DSP処理を `ToolProcessor` に分離
- メーター描画を `MeterComponent` に分離
- テーマ切り替え（Ember / Amber / Ruby / Graphite / Blush）

今後の予定：

- ピークホールド付きメーター
- クリップインジケーター
- プリセット管理
