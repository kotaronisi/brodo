---
name: pr-reviewer
description: GitHub Pull Request をレビューし、Issue の受け入れ条件、検証、設計、保守性、影響範囲、ドキュメント要否、破壊的変更を確認して approved / changes requested / comment only を判定する。
---

# PR Reviewer

Pull Request をレビューするスキル。必ず `docs/workflow.md` と `.pi/workflow.json` を確認してから進める。

## 事前確認

レビュー前に確認する。

```bash
git status --short --branch
gh auth status
gh pr view <PR番号> --json number,title,body,state,mergeable,headRefName,baseRefName,closingIssuesReferences
```

必要に応じて diff を確認する。

```bash
gh pr diff <PR番号>
```

## レビュー観点

- Issue の Acceptance Criteria を満たすか
- Test / Verification が十分か
- 明らかなバグがないか
- 設計の妥当性
- 保守性
- 影響範囲
- ドキュメント更新要否
- 破壊的変更の有無

セキュリティ、パフォーマンス、アクセシビリティ、互換性、運用影響は必要時に追加観点として扱う。

## 指摘優先度

- `must`: merge 前に必須対応
- `should`: 原則対応
- `nit`: 任意または軽微

## 最終判定

- `approved`: `must` / `should` がなく、merge 承認待ちに進める。
- `changes requested`: `must` がある、または修正必須の問題がある。
- `comment only`: ブロッカーではないコメント。`should` が含まれる場合は自動修正対象、`nit` のみなら merge 承認待ちに進める。

## 出力形式

レビュー結果は次の形式で出す。

```md
## Review Result

Decision: approved | changes requested | comment only

## Findings

### must
- ...

### should
- ...

### nit
- ...

## Verification Notes
- ...

## Recommendation
- ...
```

## GitHub 投稿

自動レビュー・修正ループ中は GitHub に投稿しない。チャット上に要約する。

最終レビュー結果を GitHub に投稿する場合は、ユーザーの許可を得てから行う。

```bash
gh pr review <PR番号> --approve --body-file review.md
gh pr review <PR番号> --request-changes --body-file review.md
gh pr comment <PR番号> --body-file review.md
```
