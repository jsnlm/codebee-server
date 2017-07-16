import logger from '../../env/debug';
import env from './../../env/env';
import * as util from 'util';
import { exec, ChildProcess } from 'child_process';

const debug = logger('dockerSandbox');

class DockerSandbox {

  constructor() {
    debug("Bot Constructor");
  }

  async createImage(imageName: string, files: string[]): Promise<ChildProcess> {
    debug("createImage");
    return exec(`docker build -f codebee_sandbox/Dockerfile -t ${imageName} .`);
  }
   
  async run(imageName: string): Promise<ChildProcess> {
    debug("run");
    return exec(`docker run -e ARGS="a b c d" ${imageName}`);
  }

  async simulate(files: string[]): Promise<string> {
    debug("simulate");
    let imageName = "codebee_sandbox";
    await this.createImage(imageName, files);
    
    return new Promise<string>(
      async(resolve, reject) => {
        let res = await this.run(imageName);

        let outData = '';
        res.stdout.on('data', block => outData+=block);//it is possible for stdout data to be sent in blocks
        res.stdout.on('end', () => resolve(outData));        

        res.stderr.on('data', data => reject(data));
      }
    );
  }
}

export default DockerSandbox;