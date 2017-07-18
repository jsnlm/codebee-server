import logger from '../env/debug';
import DockerSandbox from './docker/dockerSandbox';

const debug = logger('Simulation');

class Simulation {

  sandbox:DockerSandbox;

  constructor() {
    this.sandbox = new DockerSandbox();
    debug('Bot Constructor');
  }

  async simulate(): Promise<string> {
    try {
      let bots: BotCode[] = [];
      bots.push({
        fileName: 'testFile1',
        contents: 'content line 1\ncontent line 2\ncontent line 3\n'
      }, {
        fileName: 'testFile2',
        contents: 'content line 4\ncontent line 5\ncontent line 6\n'
      });
      let result:string = await this.sandbox.simulate(bots);
      debug('result: ', result);
      return Promise.resolve(result);
    } catch(e) {
      debug('error: ', e);
      return Promise.reject(e);
    }
  }
}

export default Simulation;
