# Spec Requirements

## Accepted sources

Use a specification only when it can be pointed to directly. Acceptable sources include:

- a repository file path
- a ticket or issue with stable text
- a decision document
- a written acceptance criteria list

Reject sources that are only implied, paraphrased from memory, or too vague to quote back precisely.

## Reading order

Use this priority order:

1. Explicit acceptance criteria
2. Explicit behavior specification
3. Relevant decision documents
4. Existing code and existing tests for context only

Existing code is not the source of truth when it conflicts with a clearer written requirement.

## Minimum quality bar

The spec must answer all of these:

- What behavior is being tested?
- Under what input or precondition?
- What observable outcome is expected?
- What boundary conditions or error conditions matter?

If one of these is missing, stop and ask for clarification instead of guessing.

## Stop conditions

Stop before editing tests when:

- the target module is not identified
- the behavior is not observable in a test
- the requirement is ambiguous
- acceptance criteria are missing and cannot be derived safely

When stopping, name the missing artifact or ambiguous statement directly.
