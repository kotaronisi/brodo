---
name: issue-planner
description: GitHub Issue 作成支援。課題、バグ、新機能、タスク、リファクタ、ドキュメント変更をヒアリングし、Issue forms と同じ構造の本文を作成し、必要なら gh issue create で Issue を作成する。
---

# Issue Planner

GitHub Issue を作成するためのスキル。必ず `docs/workflow.md` と `.pi/workflow.json` を確認してから進める。

## 事前確認

作業前に確認する。

```bash
git status --short --branch
gh auth status
```

## 対象 Issue 種別

- `bug`
- `feature`
- `task`
- `refactor`
- `docs`

## 基本ルール

- Issue 作成前に、種別ごとの必須項目が埋まるまで一問ずつヒアリングする。
- すべての Issue で Acceptance Criteria を必須にする。
- Out of Scope は任意。ただし `feature` / `refactor` では記載を推奨する。
- 初期 label は `type:*`, `status: triage`, `priority: medium`。
- ユーザーが別 priority を指定した場合は `priority: low|medium|high` から選ぶ。
- エージェント作成 Issue の本文は、Issue forms と同じ見出し構造の Markdown にする。

## 必須項目

### bug

- Summary
- Steps to Reproduce
- Expected Behavior
- Actual Behavior
- Acceptance Criteria

### feature

- Summary
- Use Case
- Proposed Behavior
- Acceptance Criteria

### task

- Summary
- Purpose
- Acceptance Criteria

### refactor

- Summary
- Purpose
- Behavior Preservation
- Acceptance Criteria

### docs

- Summary
- Target Audience / Location
- Acceptance Criteria

## Issue 作成

ユーザーが作成を許可したら GitHub CLI で作成する。

```bash
gh issue create --title "..." --body-file /path/to/body.md --label "type: feature" --label "status: triage" --label "priority: medium"
```

作成後、Issue 番号と URL を報告する。
