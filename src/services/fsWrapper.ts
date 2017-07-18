import { promisify } from 'util';
import { writeFile as writeFileCallback, unlink as unlinkCallback } from 'fs';

import logger from '../env/debug';

const writeFile = promisify(writeFileCallback);
const unlink = promisify(unlinkCallback);

const debug = logger('fsWrapper');

export interface File {
  name: string;
  contents: string;
}

export class fsWrapper {

  path: string;

  constructor(path: string) {
    this.path = path;
  }

  async addFile(file: File): Promise<void> {
    writeFile(`${this.path}${file.name}`, file.contents); 
  }

  async addFiles(files: File[]): Promise<void> {
    files.forEach( async(file) => { await this.addFile(file) } );
    return Promise.resolve(undefined);
  }

  async removeFile(file: File): Promise<void> {
    await unlink(`${this.path}${file.name}`); 
  }

  async removeFiles(files: File[]): Promise<void> {
    files.forEach( async(file) => { await this.removeFile(file) } );
    return Promise.resolve(undefined);
  }
}