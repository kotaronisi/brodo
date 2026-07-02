# 開発環境メモ

このフォルダは、人間とエージェントが議論するための場所です。

機械的に読むための正本は次のファイルです:

```text
docs/development-environment.yaml
```

判断理由、トレードオフ、質問など、自動化向け YAML に埋め込むべきではない情報は、この Markdown 層に記述します。

## 現在の決定事項

- DirectX 12 を primary graphics API とする。
- windowing と platform integration は Win32 を直接実装する。
- GPU の debugging と profiling には PIX on Windows と NVIDIA Nsight Graphics を使う。
- test framework には GoogleTest を使う。
- 初回 clone 後の依存セットアップには `scripts/setup-vcpkg.ps1` を使う。
- CI の選定はまだ未決定。

## 初回セットアップ

Windows PowerShell でリポジトリのルートから次を実行する:

```powershell
.\scripts\setup-vcpkg.ps1
cmake -S . -B build "-DCMAKE_TOOLCHAIN_FILE=tools/vcpkg/scripts/buildsystems/vcpkg.cmake"
cmake --build build
ctest --test-dir build --output-on-failure
```

## 未決定事項のドキュメント

- `docs/decisions/ci.md`
