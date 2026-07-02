# CI の決定

## 目的

Brodo で CI による自動チェックを実行するか、実行する場合はどのように行うかを決める。

## 現在の状態

decision_required

## 推奨案

最初のビルド可能なターゲットができるまでは、CI を必須にしない。将来きちんと導入できるように、決定用ドキュメントだけ先に残しておく。

## 導入開始の目安

リポジトリに次のものが揃った時点で GitHub Actions を使う:

- CMake の configure コマンド。
- build コマンド。
- 少なくとも 1 つの自動テスト、または smoke test。

## 最初の CI チェック

最初に入れるチェックの推奨:

- CMake で configure する。
- 既定ターゲットをビルドする。
- テストを実行する。
- フォーマットチェックを実行する。
- 実行時間が重すぎなければ静的解析を実行する。

## 決定するための質問

1. リポジトリは GitHub でホストするか？
2. CI はすべての pull request で実行するか？
3. 最初の CI プラットフォームは Windows にするか？
4. Linux はエンジンがクロスプラットフォーム化してから追加するか？
5. フォーマット違反は merge をブロックする扱いにするか？

## 決定後の反映先

決定したら `docs/development-environment.yaml` を更新する:

```yaml
ci:
  provider: github_actions
  status: decided
  required_checks:
    - configure
    - build
    - test
```
