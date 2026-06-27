#!/usr/bin/env node
import { readFile } from 'node:fs/promises';
import { spawnSync } from 'node:child_process';
import { dirname, resolve } from 'node:path';
import { fileURLToPath } from 'node:url';

const root = resolve(dirname(fileURLToPath(import.meta.url)), '..');
const labelsPath = resolve(root, '.github', 'labels.json');
const labels = JSON.parse(await readFile(labelsPath, 'utf8'));

function gh(args) {
  const result = spawnSync('gh', args, { cwd: root, encoding: 'utf8' });
  return result;
}

for (const label of labels) {
  const common = ['--color', label.color, '--description', label.description];
  const edit = gh(['label', 'edit', label.name, ...common]);
  if (edit.status === 0) {
    console.log(`updated: ${label.name}`);
    continue;
  }

  const create = gh(['label', 'create', label.name, ...common]);
  if (create.status === 0) {
    console.log(`created: ${label.name}`);
    continue;
  }

  console.error(`failed: ${label.name}`);
  console.error(create.stderr || edit.stderr);
  process.exitCode = 1;
}
