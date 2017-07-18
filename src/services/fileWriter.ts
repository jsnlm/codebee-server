import { promisify } from 'util';
import { writeFile as writeFileCallback, unlink as unlinkCallback } from 'fs';

import logger from '../env/debug';

const writeFile = promisify(writeFileCallback);
const unlink = promisify(unlinkCallback);

const debug = logger('fileWriter');

class FileWriter {

  path: string;

  constructor(path: string) {
    this.path = path;
  }

  async addFile(file: BotCode): Promise<void> {
    writeFile(`${this.path}${file.fileName}`, file.contents); 
  }

  async addFiles(files: BotCode[]): Promise<void> {
    files.forEach( async(file) => { await this.addFile(file) } );
    return Promise.resolve(undefined);
  }

  async removeFile(file: BotCode): Promise<void> {
    await unlink(`${this.path}${file.fileName}`); 
  }

  async removeFiles(files: BotCode[]): Promise<void> {
    files.forEach( async(file) => { await this.removeFile(file) } );
    return Promise.resolve(undefined);
  }
}

export default FileWriter;