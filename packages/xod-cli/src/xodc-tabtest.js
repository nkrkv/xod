// xodc tabtest [--workspace=<dir>] <input> <path>
import path from 'path';
import * as R from 'ramda';
import fs from 'fs-extra';

import { foldEither, allPromises } from 'xod-func-tools';
import { loadProject } from 'xod-fs';
import * as Tabtest from 'xod-tabtest';
import * as msg from './messageUtils';
import { getWorkspacePath } from './utils';

const bundledWorkspace = path.resolve(__dirname, '..');
const tabtestWorkspace = path.resolve(
  __dirname,
  '..',
  '..',
  'xod-tabtest',
  'workspace'
);

const tabtestSources = path.resolve(
  __dirname,
  '..',
  '..',
  'xod-tabtest',
  'cpp'
);

const catch2Sources = path.resolve(
  __dirname,
  '..',
  '..',
  '..',
  'vendor',
  'catch2'
);

const showErrorAndExit = err => {
  msg.error(err);
  process.exit(1);
};

export default (input, patchPath, program) => {
  const workspaces = [
    getWorkspacePath(program.workspace),
    tabtestWorkspace,
    bundledWorkspace,
  ];

  /* TODO: OS-neutral tmp dir */
  const outDir = '/tmp/xod-tabtest';

  msg.notice(`Preparing test directory: ${outDir} ...`);

  fs
    .ensureDir(outDir)
    .then(R.tap(() => msg.notice('Loading project...')))
    .then(() => loadProject(workspaces, input))
    .then(R.tap(() => msg.notice('Generating C++ code...')))
    .then(project => Tabtest.generateSuite(project, patchPath))
    .then(foldEither(err => showErrorAndExit(err), R.identity))
    .then(R.toPairs)
    .then(R.tap(() => msg.notice('Saving files...')))
    .then(
      R.map(([filename, content]) =>
        fs.outputFile(path.join(outDir, filename), content)
      )
    )
    .then(R.append(fs.copy(tabtestSources, outDir)))
    .then(R.append(fs.copy(catch2Sources, outDir)))
    .then(allPromises)
    .then(() => {
      msg.success('Generated successfully');
      process.exit(0);
    })
    .catch(showErrorAndExit);
};
