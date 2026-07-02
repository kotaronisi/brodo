---
name: brodo-spec-driven-test-impl
description: Add or update GoogleTest-based tests for Brodo modules from a clearly referenced specification or acceptance criteria without changing production implementation behavior. Use when Codex is asked to implement tests only, validate existing behavior against a spec, add missing test coverage for a Brodo module, or bootstrap the minimum CMake/GoogleTest test setup needed to run those tests.
---

# Brodo Spec Driven Test Impl

## Overview

Implement tests only. Treat the referenced specification and acceptance criteria as the source of truth, add the smallest GoogleTest coverage needed for Brodo, and run the tests when the project state allows it.

Read [references/spec-requirements.md](references/spec-requirements.md) before planning tests. Read [references/test-workflow.md](references/test-workflow.md) when converting requirements into test cases. Read [references/gtest-cmake.md](references/gtest-cmake.md) only when the repository needs GoogleTest or CMake test bootstrapping.

## Rules

- Do not change production behavior, feature scope, or design to make tests pass.
- Do not invent requirements that are not traceable to a referenced spec artifact.
- Stop and report the gap if the spec source is missing, ambiguous, or not testable.
- Prefer the smallest set of tests that fully covers the stated acceptance criteria.
- Keep every test traceable to a specific requirement or acceptance condition.

## Workflow

1. Identify the exact spec artifact and the target module.
2. Verify that the spec is explicit enough to test.
3. Read the existing module code, existing tests, and current CMake test setup.
4. Convert the spec into a concrete test case list before editing files.
5. Implement only the missing GoogleTest coverage.
6. Add the minimum CMake or GoogleTest wiring needed to compile and run the tests if no test harness exists yet.
7. Run the relevant tests if the environment supports it.
8. Report requirement-to-test traceability and any requirements that could not be tested.

## Decision Points

### Missing or weak specification

Do not proceed with test authoring when any of the following is true:

- The user provides only a verbal request with no file, ticket, document, or acceptance criteria.
- The target behavior can be interpreted in multiple ways from the provided sources.
- The spec describes a user goal but not observable behavior that can be asserted in tests.

In those cases, explain exactly what is missing and ask for a clearer referenced source.

### Existing implementation disagrees with the spec

Still implement the tests when the expected behavior is unambiguous. The test may fail. Do not repair the production code in this skill unless the user explicitly changes scope.

### No test harness yet

Bootstrap the smallest viable setup:

- `vcpkg` for dependency resolution
- `find_package(GTest CONFIG REQUIRED)` in CMake
- `enable_testing()`
- a dedicated test target
- `gtest_discover_tests()` for `ctest`

Do not add CI, multiple test suites, or speculative architecture in this skill.

## Output Expectations

At the end of the task:

- summarize which spec source was used
- list the implemented test cases
- map each test case to the requirement it verifies
- report whether tests ran and what blocked execution if they did not
