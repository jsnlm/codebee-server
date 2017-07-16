import logger from '../../env/debug';
import env from './../../env/env';
import { promisify } from 'util';
import { exec as execCallback, ChildProcess } from 'child_process';
import { writeFile as writeFileCallback } from 'fs';

const exec = promisify(execCallback);
const writeFile = promisify(writeFileCallback);
const debug = logger('dockerSandbox');

class DockerSandbox {

  path: string;

  constructor() {
    debug("Bot Constructor");
    this.path = 'codebee_sandbox/';
  }

  async addFile(file: BotCode): Promise<string> {
    return writeFile(`${this.path}${file.name}`, file.contents); 
  }

  async addFiles(files: BotCode[]): Promise<void> {
    debug("addFiles");
    files.forEach( async(file) => { await this.addFile(file) } );
    return Promise.resolve(undefined);
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

    await this.addFiles(files);
    await this.createImage(imageName).then((child_process) => {
      debug("createImage has finished");
      return child_process;
    });
    
    return new Promise<string>(
      async(resolve, reject) => {
        const { stdout, stderr } = await this.run(imageName);

        debug("run has finished");

        resolve(stdout);
        reject(stderr);
      }
    );
  }
}

export default DockerSandbox;