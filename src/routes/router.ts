import { Router } from 'express';

import graphql from './graphql';

import Simulation from '../services/simulation';

const router = Router();

// router.use('/', graphql);

// send index.html
router.get('/*', (req, res) => {
  let s:Simulation = new Simulation;
  s.simulate();
  res.sendFile('index.html', { root: '../web/dist' });
});

export default router;
