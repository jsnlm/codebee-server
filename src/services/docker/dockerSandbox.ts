import { exec as execCallback, ChildProcess } from 'child_process';
import { random } from 'lodash';

import logger from '../../env/debug';
import env from './../../env/env';
import { promisify } from 'util';

import { fsWrapper, File } from '../fsWrapper';

const exec = promisify(execCallback);
const debug = logger('dockerSandbox');

export interface dockerEnvVar {
  name: string;
  value: string;
}

export class DockerSandbox {

  path: string;
  imageName: string;
  fswrapper: fsWrapper;

  async initialize(files: File[]) {
    this.path = 'codebee_sandbox/';
    this.fswrapper = new fsWrapper(this.path);
    this.imageName = `codebee_sandbox${random(1, 1000)}`; //make random image name

    await this.fswrapper.addFiles(files);
    await this.createImage();
    await this.fswrapper.removeFiles(files);
    return Promise.resolve();
  }

  async createImage(): Promise<ChildProcess> {
    return exec(`docker build -f ${this.path}Dockerfile -t ${this.imageName} .`);
  }
   
  async run(args: dockerEnvVar[]): Promise<string> {
    return new Promise<string>( async (resolve, reject) =>{
      let stringifiedArgs = args.map( arg => `-e ${arg.name}="${arg.value}"` ).join(' ');
      let runThisCommand = `docker run ${stringifiedArgs} ${this.imageName}`;
      debug("runThisCommand : ", runThisCommand);
      const { stdout, stderr } = await exec(runThisCommand);
      resolve(stdout);
      reject(stderr);
    });
  }
}

export default DockerSandbox;