# Test Workflow

## Convert spec to test cases

For each acceptance criterion:

1. Restate the requirement in one sentence.
2. Identify the public behavior or observable effect.
3. Choose the smallest test shape that proves it.
4. Note any fixtures, helpers, or setup needed.
5. Define the assertion that proves success or failure.

Write the case list before editing code when the task is larger than one trivial test.

## Test naming

Name tests so the intended behavior is obvious from the test output. Prefer names that describe:

- the target unit or behavior
- the condition under test
- the expected result

## Scope control

Add only tests required by the referenced spec plus any essential supporting harness changes.

Do not:

- add unrelated cleanup tests
- rewrite pre-existing tests for style alone
- add broad regression suites that are not justified by the requirement

## Traceability

After writing tests, provide a simple mapping from requirement to test. A compact format is enough:

- `REQ-1` -> `LoggerWritesLevelPrefix`
- `REQ-2` -> `LoggerRejectsEmptySink`

If a requirement cannot be tested at the current seam, say so explicitly and explain the missing seam or observable behavior.
