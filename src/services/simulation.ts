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
        fileName: 'testFile',
        contents: 'content line 1\ncontent line 2\ncontent line 3 \n'
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
