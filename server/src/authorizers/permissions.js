const { shield } = require('graphql-shield');

const { isChannelAllowed } = require('./channels');

function getPermissions() {
  return shield({
    Query: {
      measurement: isChannelAllowed,
      measurements: isChannelAllowed,
      measurable: isChannelAllowed,
      measurables: isChannelAllowed,
      series: isChannelAllowed,
      seriesCollection: isChannelAllowed,
    },
  });
}

module.exports = {
  getPermissions,
};
