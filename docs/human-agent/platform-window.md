# Platform Window 仕様

## 目的

Brodo の最初の Window 表示システムを、Win32 直接実装の platform モジュールとして追加する。

## 決定

- 実装対象は Windows のみ。
- Windowing API は Win32 を直接使用する。
- 公開 API は `brodo::platform::Window` と `WindowConfig` とする。
- `WindowConfig` の既定値はタイトル `Brodo`、サイズ `1280x720` とする。
- `Window` は `HWND` を RAII で所有する。
- コピーは禁止し、ムーブは許可する。
- ユーザーが閉じる操作をしたら `shouldClose()` が `true` を返す。
- メッセージ処理は `pollEvents()` として公開し、エンジン全体のループは Window 側では持たない。
- DirectX 12 連携のため `nativeHandle()` で `HWND` を取得できるようにする。

## 含めないもの

次の項目は初回実装の範囲外とする。

- 描画処理。
- DirectX 12 の swap chain 作成。
- 詳細な入力処理。
- DPI awareness 設定。
- Linux 対応。

## 検証方針

GoogleTest では GUI に依存しない `WindowConfig` の検証だけを行う。

- 既定値が `Brodo`、`1280x720` である。
- 正のサイズを受け付ける。
- 0 以下の幅または高さは `std::runtime_error` になる。

実際の Window 表示は `brodo_window_sample` を実行し、空の Window が表示され、閉じる操作で終了することを確認する。
