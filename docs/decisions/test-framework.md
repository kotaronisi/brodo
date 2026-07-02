# テストフレームワークの決定

## 目的

Brodo で使用する C++ テストフレームワークを決める。

## 現在の状態

decision_required

## 推奨案

まずは GoogleTest を既定候補として議論を始める。

理由:

- C++ プロジェクトで広く使われている。
- CMake と組み合わせやすい。
- エンジン基盤のユニットテストに向いている。

## 候補

### GoogleTest

推奨する開始点。

トレードオフ:

- エコシステムとドキュメントが強い。
- 多くの C++ 開発者に馴染みがある。
- 小さな single-header 系フレームワークよりは少し重い。

### Catch2

より軽く、表現力のあるテスト記述を重視する場合の有力な代替候補。

トレードオフ:

- 読み書きしやすい。
- プロジェクト設定が比較的小さい。
- 大規模な C++ エンジンコードベースでは GoogleTest ほど標準的ではない。

## 決定するための質問

1. Brodo は、小さく単純なフレームワークよりも、広く採用されているフレームワークを優先するか？
2. 最初から CMake 経由でテストを統合するか？
3. 最初のテスト対象は math、memory、asset loading、platform code のどれにするか？

## 決定後の反映先

決定したら `docs/development-environment.yaml` を更新する:

```yaml
quality:
  test_framework:
    name: gtest
    status: decided
```
