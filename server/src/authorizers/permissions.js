const _ = require('lodash');
const { shield, allow, and, or, not } = require('graphql-shield');

const { isAuthenticated } = require('./agents');
const { isAdmin, isViewer, isInChannel } = require('./channel-memberships');
const { isMinOneAdmin } = require('./channel-memberships');
const { isSubjectAsObject } = require('./channel-memberships');
const { isObjectAdmin } = require('./channel-memberships');
const { isChannelPublic } = require('./channels');
const measurables = require('./measurables');
const bots = require('./bots');

const rulesChannel = {
  Query: {},
  Mutation: {
    channelUpdate: and(isAuthenticated, isAdmin),
    leaveChannel: and(
      isAuthenticated,
      isInChannel,
      or(and(isAdmin, isMinOneAdmin), not(isAdmin)),
    ),
    joinChannel: and(isAuthenticated, isChannelPublic, not(isInChannel)),
    channelMembershipCreate: and(isAuthenticated, isAdmin),
  }
};

const rulesChannelMemberships = {
  Query: {},
  Mutation: {
    channelMembershipDelete: and(
      isAuthenticated,
      isAdmin,
      or(and(isObjectAdmin, isMinOneAdmin), not(isObjectAdmin)),
    ),
    channelMembershipRoleUpdate: and(
      isAuthenticated,
      isAdmin,
      or(isObjectAdmin, and(not(isObjectAdmin), not(isSubjectAsObject))),
    ),
  }
};

const rulesMeasurables = {
  Query: {},
  Mutation: {
    measurementCreate: and(isAuthenticated, or(isChannelPublic, or(isAdmin, isViewer))),
    measurableArchive: and(isAuthenticated, measurables.isOwner, not(measurables.isArchived)),
    measurableUnarchive: and(isAuthenticated, measurables.isOwner, measurables.isArchived),
    measurableUpdate: and(isAuthenticated, measurables.isOwner),
  }
};

const rules = {
  Bot: {
    jwt: bots.isOwner,
  },
  Query: {
    '*': allow,
    permissions: allow,
    user: allow,
    users: allow,
    measurement: allow,
    measurements: allow,
    measurable: allow,
    measurables: allow,
    bot: allow,
    bots: allow,
    agent: allow,
    agents: allow,
    series: allow,
    seriesCollection: allow,
    channel: allow,
    channels: allow,
    stats: allow,
  },
  Mutation: {
    '*': isAuthenticated,
    botCreate: isAuthenticated,
    channelCreate: isAuthenticated,
    userUpdate: isAuthenticated,
    seriesCreate: and(isAuthenticated, or(isChannelPublic, isAdmin)),
    measurableCreate: and(isAuthenticated, or(isChannelPublic, or(isAdmin, isViewer))),

    ...rulesMeasurables.Mutation,
    ...rulesChannel.Mutation,
    ...rulesChannelMemberships.Mutation,
  }
};

function getPermissions() {
  return shield(
    _.cloneDeep(rules),
    { debug: false }
  );
}

module.exports = {
  rules,
  rulesChannel,
  rulesMeasurables,
  rulesChannelMemberships,
  getPermissions,
};
