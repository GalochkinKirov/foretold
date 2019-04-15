const graphql = require('graphql');
const models = require('../models');

const { createConnection } = require('graphql-sequelize');

const measurableMeasurementsConnection = createConnection({
  name: 'measurableMeasurements',
  nodeType: require('../types').measurements.measurement,
  target: models.Measurable.Measurements,
  connectionFields: {
    total: {
      type: graphql.GraphQLInt,
      resolve: ({ fullCount }) => fullCount
    }
  },
});

const agentMeasurementsConnection = createConnection({
  name: 'agentMeasurementsConnection',
  nodeType: require('../types').measurements.measurement,
  target: models.Agent.Measurements,
  connectionFields: {
    total: {
      type: graphql.GraphQLInt,
      resolve: ({ fullCount }) => fullCount
    }
  },
});

const channelAgentsConnection = createConnection({
  name: 'channelAgentsConnection',
  nodeType: require('../types').agents.agent,
  target: models.Channel.Agents,
  connectionFields: {
    total: {
      type: graphql.GraphQLInt,
      resolve: ({ fullCount }) => fullCount
    }
  },
});

const seriesMeasurablesConnection = createConnection({
  name: 'seriesMeasurablesConnection',
  nodeType: require('../types').measurables.measurable,
  target: models.Series.Measurables,
  connectionFields: {
    total: {
      type: graphql.GraphQLInt,
      resolve: ({ fullCount }) => fullCount
    }
  },
});

module.exports = {
  measurableMeasurementsConnection,
  agentMeasurementsConnection,
  channelAgentsConnection,
  seriesMeasurablesConnection,
};