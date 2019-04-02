const { shield, allow, and, or } = require('graphql-shield');

const { isAuthenticated } = require('./users');
const { isAdmin, isViewer } = require('./channel-memberships');
const { isChannelPublic } = require('./channels');
const { isOwner } = require('./measurables');

function getPermissions() {
  return shield(
    {
      Query: {
        '*': allow,
        user: isAuthenticated,
      },
      Mutation: {
        '*': isAuthenticated,
        measurementCreate: and(isAuthenticated, or(isChannelPublic, or(isAdmin, isViewer))),
        measurableCreate: and(isAuthenticated, or(isChannelPublic, or(isAdmin, isViewer))),
        channelUpdate: and(isAuthenticated, or(isChannelPublic, isAdmin)),
        channelMembershipCreate: and(isAuthenticated, or(isChannelPublic, isAdmin)),
        channelMembershipDelete: and(isAuthenticated, or(isChannelPublic, isAdmin)),
        channelMembershipRoleUpdate: and(isAuthenticated, or(isChannelPublic, isAdmin)),
        seriesCreate: and(isAuthenticated, or(isChannelPublic, isAdmin)),
        measurableArchive: and(isAuthenticated, isOwner),
        measurableUnarchive: and(isAuthenticated, isOwner),
        measurableUpdate: and(isAuthenticated, isOwner),
      }
    },
    { debug: false }
  );
}

module.exports = {
  getPermissions,
};
