const _ = require('lodash');
const { shield, allow, and, or, not } = require('graphql-shield');

const { isAuthenticated } = require('./agents');
const { isChannelPublic } = require('./channels');
const { isAdmin, isViewer } = require('./channel-memberships');
const { channelHasMembershipWithCurrentAgent } = require('./channel-memberships');
const { channelHasMultipleAdmins } = require('./channel-memberships');
const { membershipBelongsToCurrentAgent } = require('./channel-memberships');
const { membershipHasAdminRole } = require('./channel-memberships');
const { measurableIsOwnedByCurrentAgent } = require('./measurables');
const { measurableIsArchived } = require('./measurables');
const { botBelongsToCurrentUser } = require('./bots');

const rulesChannel = {
  Query: {},
  Mutation: {
    channelUpdate: and(
      isAuthenticated,
      isAdmin,
    ),
    leaveChannel: and(
      isAuthenticated,
      channelHasMembershipWithCurrentAgent,
      or(and(isAdmin, channelHasMultipleAdmins), not(isAdmin)),
    ),
    joinChannel: and(
      isAuthenticated,
      isChannelPublic,
      not(channelHasMembershipWithCurrentAgent),
    ),
    channelMembershipCreate: and(
      isAuthenticated,
      isAdmin,
    ),
  }
};

const rulesChannelMemberships = {
  Query: {},
  Mutation: {
    channelMembershipDelete: and(
      isAuthenticated,
      isAdmin,
      or(
        and(membershipHasAdminRole, channelHasMultipleAdmins),
        not(membershipBelongsToCurrentAgent),
      ),
    ),
    channelMembershipRoleUpdate: and(
      isAuthenticated,
      isAdmin,
      or(
        and(channelHasMultipleAdmins, membershipBelongsToCurrentAgent),
        and(channelHasMultipleAdmins, membershipHasAdminRole),
        not(membershipBelongsToCurrentAgent),
      ),
    ),
  }
};

const rulesMeasurables = {
  Query: {},
  Mutation: {
    measurementCreate: and(
      isAuthenticated,
      or(isChannelPublic, or(isAdmin, isViewer)),
    ),
    measurableArchive: and(
      isAuthenticated,
      measurableIsOwnedByCurrentAgent,
      not(measurableIsArchived),
    ),
    measurableUnarchive: and(
      isAuthenticated,
      measurableIsOwnedByCurrentAgent,
      measurableIsArchived,
    ),
    measurableUpdate: and(
      isAuthenticated,
      measurableIsOwnedByCurrentAgent,
    ),
  }
};

const rules = {
  Bot: {
    jwt: botBelongsToCurrentUser,
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
    measurableCreate: and(
      isAuthenticated,
      or(isChannelPublic, or(isAdmin, isViewer)),
    ),

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
