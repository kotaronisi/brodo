---
name: pr-implementer
description: GitHub Issue から実装ブランチを作成し、実装、検証、commit、push、PR 作成、レビュー・修正ループ、merge 承認依頼、merge 後処理まで進める。
---

# PR Implementer

Issue を実装して Pull Request を作成するスキル。必ず `docs/workflow.md` と `.pi/workflow.json` を確認してから進める。

## 事前確認

作業前に必ず確認する。

```bash
git status --short --branch
git branch --show-current
gh auth status
```

未コミット変更がある場合は必ず停止し、ユーザーに確認する。勝手に stash や commit をしない。

## 着手条件

- 原則として `status: ready` の Issue のみ着手する。
- ユーザーが明示指定した場合のみ、ready 以外の Issue に着手してよい。
- Issue の要件不足や設計上の疑問がある場合:
  - Acceptance Criteria、データ破壊、設計方針に影響する重大点は停止して質問する。
  - 命名や軽微な実装詳細は合理的な仮定で進め、PR に明記する。

## ブランチ

`.pi/workflow.json` の `baseBranch` からブランチを切る。形式は `<type>/<issue-number>-<short-slug>`。

例:

```bash
git switch main
git pull --ff-only
git switch -c feature/123-add-login
```

着手時に Issue label を `status: ready` から `status: in-progress` に更新する。

## Commit

Commit message は Conventional Commits + Issue 番号にする。

```text
feat: add login form (#123)
fix: handle login error (#124)
```

## 検証

`.pi/workflow.json` の `checks` を実行する。`null` の check は未設定として実行せず、PR の Test / Verification に「未設定」と明記する。

## PR 作成

PR title は Conventional Commits + Issue 番号にする。

```text
feat: add login form (#123)
```

PR 本文には必ず `Closes #123` を含める。

PR 作成後、Issue label を `status: in-progress` から `status: review` に更新する。

## 自動レビュー・修正ループ

PR 作成後、必ず `pr-reviewer` の手順を実行する。スキルが未ロードの場合は `.pi/skills/pr-reviewer/SKILL.md` を読む。

- 最大3回までレビュー・修正ループを行う。
- `must` / `should` 指摘は自動対応対象。
- `nit` は任意対応または最終報告に残す。
- `comment only` でも `should` があれば修正する。
- 3回で `approved` または `nit` のみの `comment only` にならない場合は停止し、未解決点・選択肢・推奨案をユーザーに相談する。
- 自動ループ中のレビュー結果はチャット上に要約する。
- GitHub への最終レビュー投稿は、ユーザーの許可後に行う。

修正に入る場合は必要に応じて Issue label を `status: review` から `status: in-progress` に戻し、再度 PR 更新後に `status: review` に戻す。

## Merge 承認依頼

merge 条件を満たしたら、以下を報告してユーザーの明示承認を待つ。

- PR 番号
- Issue 番号
- 変更概要
- 検証結果
- レビュー結果
- merge 方式
- 承認文言

承認文言例:

```text
承認: PR #12 を merge してよい
```

承認がない限り merge しない。

## Merge 後処理

承認後に squash merge する。

```bash
gh pr merge <PR番号> --squash --delete-branch
```

merge 後に以下を行う。

- PR merge 確認
- Issue close 確認
- Issue label を `status: done` に更新
- ローカル `main` 更新
- local 作業ブランチ削除
- remote 作業ブランチ削除確認
