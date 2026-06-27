# 開発ワークフロー

このリポジトリは GitHub の Issue / Pull Request を中心に、個人開発でありながらエージェントを使って擬似的な複数人開発として運用する。

## 基本方針

- Issue で課題・バグ・新機能・リファクタ・ドキュメント変更を提案する。
- Pull Request で実装し、レビューを経て merge する。
- エージェントは Issue 作成、実装、PR 作成、label 更新、レビュー支援を行ってよい。
- merge はユーザーの明示承認後に限り、エージェントが実行してよい。
- merge 方式は squash merge とする。

承認文言例:

```text
承認: PR #12 を merge してよい
```

## Issue 種別

- `bug`: バグ修正
- `feature`: 新機能・改善
- `task`: 実装タスク・保守作業
- `refactor`: 振る舞いを変えない構造改善
- `docs`: ドキュメント変更

すべての Issue で Acceptance Criteria を必須とする。Out of Scope は任意。ただし feature / refactor では記載を推奨する。

## Labels

### Type

- `type: bug`
- `type: feature`
- `type: task`
- `type: refactor`
- `type: docs`

### Status

- `status: triage`: 作成直後。優先度・実施可否・実装準備を確認する状態。
- `status: ready`: 実装着手可能。
- `status: in-progress`: 実装中。
- `status: blocked`: 判断待ち、情報不足、依存関係、権限・環境不足などで進められない状態。
- `status: review`: PR 作成済みでレビュー中。
- `status: done`: merge 済み。

### Priority

- `priority: low`
- `priority: medium`
- `priority: high`

Issue 作成時の初期 status は常に `status: triage`。初期 priority は `priority: medium` とし、必要に応じて変更する。

## Issue ライフサイクル

```text
triage -> ready -> in-progress -> review -> done
                    ^              |
                    | 指摘対応     |
                    +--------------+
```

- 新規 Issue は `status: triage`。
- ユーザーが実装対象と判断したら `status: ready`。
- `pr-implementer` が着手したら `status: in-progress`。
- PR 作成後に `status: review`。
- レビュー指摘が実装修正を要する場合は `status: in-progress` に戻す。
- 判断待ちや依存関係で止まる場合は `status: blocked`。
- PR merge 後に `status: done`。

## Branch

実装ブランチは常に `main` から切る。

形式:

```text
<type>/<issue-number>-<short-slug>
```

例:

```text
feature/123-add-login
bug/124-fix-login-error
docs/125-update-workflow-guide
```

## Commit / PR Title

Commit message と PR title は Conventional Commits + Issue 番号を必須とする。

```text
feat: add login form (#123)
fix: handle login error (#124)
docs: update workflow guide (#125)
```

## PR 要件

PR 本文には必ず `Closes #<issue-number>` を含める。

PR テンプレートの必須セクション:

- Summary
- Changes
- Test / Verification
- Review Focus
- Related Issue
- Checklist

Checklist:

- Related Issue を記載した
- Acceptance Criteria を満たした
- テスト/検証を実施した
- レビュー観点を記載した
- ドキュメント更新要否を確認した
- 破壊的変更の有無を確認した

## レビュー・修正ループ

`pr-implementer` は PR 作成後、必ず `pr-reviewer` の手順でレビューを行う。

- 自動レビュー・修正ループは最大3回。
- `must` / `should` 指摘は自動対応対象。
- `nit` は任意対応または最終報告に残す。
- `comment only` でも `should` があれば修正する。
- 3回で解決しない場合は、未解決点と選択肢をまとめてユーザーに相談する。
- 自動ループ中のレビュー結果はチャット上に要約する。
- GitHub への最終レビュー投稿は、ユーザーの許可後に行う。

レビュー判定:

- `approved`
- `changes requested`
- `comment only`

指摘優先度:

- `must`: merge 前に必須対応
- `should`: 原則対応
- `nit`: 任意または軽微

## Merge 条件

以下を満たした場合のみ merge 承認を求める。

- PR checklist 完了
- `.pi/workflow.json` の検証コマンド成功、または未設定であることを明記
- `pr-reviewer` が `approved`、または `nit` のみの `comment only`
- 未解決の `must` / `should` がない
- ユーザーの明示承認がある

merge 承認依頼では以下を報告する。

- PR 番号
- Issue 番号
- 変更概要
- 検証結果
- レビュー結果
- merge 方式
- 承認文言

## Merge 後処理

merge 後、エージェントは以下を行う。

- PR merge 確認
- Issue close 確認
- label を `status: done` に更新
- ローカル `main` 更新
- remote / local の作業ブランチ削除

## 検証コマンド

検証コマンドは `.pi/workflow.json` に定義する。`null` は未設定を表す。

```json
{
  "checks": {
    "test": null,
    "lint": null,
    "build": null
  }
}
```

未設定の検証は実行せず、PR の Test / Verification に「未設定」と明記する。

## 初期セットアップ

GitHub labels を作成・更新する。

```bash
node scripts/setup-labels.mjs
```

GitHub Actions は placeholder として `.github/workflows/ci.yml` を置く。プロジェクトの実装言語・ツールが決まったら編集する。
