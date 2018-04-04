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

  msg.notice(`Generating C++ test sources ...`);

  /* TODO: OS-neutral tmp dir */
  const outDir = '/tmp/xod-tabtest';
  fs
    .ensureDir(outDir)
    .then(() => loadProject(workspaces, input))
    .then(project => Tabtest.generateSuite(project, patchPath))
    .then(foldEither(err => showErrorAndExit(err), R.identity))
    .then(R.toPairs)
    .then(
      R.map(([filename, content]) =>
        fs.outputFile(path.join(outDir, filename), content)
      )
    )
    .then(allPromises)
    // .then(promises => Promise.all(promises))
    .then(() => {
      msg.success('Generated successfully');
      process.exit(0);
    })
    .catch(showErrorAndExit);
};
