import logger from '../env/debug';
import simulateJob from './simulate';

const debug = logger('jobs');

export default function start() {
  setInterval(simulateJob, 10000);
}