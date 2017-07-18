import logger from '../../env/debug';
import env from './../../env/env';
import { promisify } from 'util';
import { exec as execCallback, ChildProcess } from 'child_process';
import FileWriter from '../fileWriter';

const exec = promisify(execCallback);
const debug = logger('dockerSandbox');

class DockerSandbox {

  path: string;
  fileWriter: FileWriter;

  constructor() {
    this.path = 'codebee_sandbox/';
    this.fileWriter = new FileWriter(this.path);
  }

  async createImage(imageName: string): Promise<ChildProcess> {
    debug("createImage");
    return exec(`docker build -f ${this.path}Dockerfile -t ${imageName} .`);
  }
   
  async run(imageName: string): Promise<ChildProcess> {
    debug("run");
    return exec(`docker run -e ARGS="a b c d" ${imageName}`);
  }

  async simulate(files: BotCode[]): Promise<string> {
    debug("simulate");
    let imageName = "codebee_sandbox";
    try {
      await this.fileWriter.addFiles(files);
      await this.createImage(imageName);
      await this.fileWriter.removeFiles(files);
      return new Promise<string>( async (resolve, reject) =>{
        const { stdout, stderr } = await this.run(imageName);
        resolve(stdout);
        reject(stderr);
      });
    } catch(e) {
      return e;
    }
  }
}

export default DockerSandbox;