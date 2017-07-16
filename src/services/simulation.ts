import logger from '../env/debug';
import DockerSandbox from './docker/dockerSandbox';
const debug = logger('Simulation');

class Simulation {

  sandbox:DockerSandbox;

  constructor() {
    this.sandbox = new DockerSandbox();
    debug("Bot Constructor");
  }

  async simulate(): Promise<string> {
    let srcFiles = ["../simulationFiles/*", "../botFiles/*"];
    let result:string = await this.sandbox.simulate(srcFiles);
    debug("result: ", result);
    return Promise.resolve(result);
  }
}

export default Simulation;
