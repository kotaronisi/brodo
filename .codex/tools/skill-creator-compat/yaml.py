"""
Minimal YAML compatibility layer for the bundled skill-creator scripts.

This intentionally supports only the subset needed by SKILL.md frontmatter:
- mappings
- nested mappings
- scalar lists
- quoted and plain scalar values
"""

from __future__ import annotations

import re


class YAMLError(Exception):
    pass


def safe_load(text: str):
    lines = _normalize_lines(text)
    if not lines:
        return None

    value, index = _parse_block(lines, 0, 0)
    if index != len(lines):
        raise YAMLError("Unexpected trailing content")
    return value


def _normalize_lines(text: str):
    normalized = []
    for raw_line in text.splitlines():
        if not raw_line.strip():
            continue
        if raw_line.lstrip().startswith("#"):
            continue
        indent = len(raw_line) - len(raw_line.lstrip(" "))
        normalized.append((indent, raw_line[indent:]))
    return normalized


def _parse_block(lines, index: int, indent: int):
    if index >= len(lines):
        return {}, index
    current_indent, current_text = lines[index]
    if current_indent < indent:
        return {}, index
    if current_text.startswith("- "):
        return _parse_list(lines, index, indent)
    return _parse_dict(lines, index, indent)


def _parse_dict(lines, index: int, indent: int):
    result = {}

    while index < len(lines):
        current_indent, current_text = lines[index]
        if current_indent < indent:
            break
        if current_indent > indent:
            raise YAMLError(f"Unexpected indentation near '{current_text}'")
        if current_text.startswith("- "):
            raise YAMLError("Mixed list and mapping at the same indentation level")

        if ":" not in current_text:
            raise YAMLError(f"Expected key/value pair, got '{current_text}'")

        key, raw_value = current_text.split(":", 1)
        key = key.strip()
        if not key:
            raise YAMLError("Empty mapping key")

        value_text = raw_value.strip()
        index += 1

        if value_text:
            result[key] = _parse_scalar(value_text)
            continue

        if index < len(lines) and lines[index][0] > indent:
            child, index = _parse_block(lines, index, lines[index][0])
            result[key] = child
        else:
            result[key] = {}

    return result, index


def _parse_list(lines, index: int, indent: int):
    result = []

    while index < len(lines):
        current_indent, current_text = lines[index]
        if current_indent < indent:
            break
        if current_indent > indent:
            raise YAMLError(f"Unexpected indentation near '{current_text}'")
        if not current_text.startswith("- "):
            raise YAMLError("Mixed mapping and list at the same indentation level")

        value_text = current_text[2:].strip()
        index += 1

        if value_text:
            result.append(_parse_scalar(value_text))
            continue

        if index < len(lines) and lines[index][0] > indent:
            child, index = _parse_block(lines, index, lines[index][0])
            result.append(child)
        else:
            result.append(None)

    return result, index


def _parse_scalar(value: str):
    if len(value) >= 2 and value[0] == '"' and value[-1] == '"':
        inner = value[1:-1]
        return (
            inner.replace("\\n", "\n")
            .replace('\\"', '"')
            .replace("\\\\", "\\")
        )
    if len(value) >= 2 and value[0] == "'" and value[-1] == "'":
        return value[1:-1]

    lowered = value.lower()
    if lowered == "true":
        return True
    if lowered == "false":
        return False
    if lowered in {"null", "~"}:
        return None
    if re.fullmatch(r"[+-]?\d+", value):
        return int(value)
    if re.fullmatch(r"[+-]?\d+\.\d+", value):
        return float(value)
    return value
