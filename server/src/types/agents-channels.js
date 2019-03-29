const graphql = require("graphql");

const roles = new graphql.GraphQLEnumType({
  name: 'AgentChannelRoles',
  values: {
    ADMIN: { value: 'ADMIN' },
    VIEWER: { value: 'VIEWER' },
  },
});

const agentsChannel = new graphql.GraphQLObjectType({
  name: 'AgentsChannel',
  fields: {
    agentId: { type: graphql.GraphQLNonNull(graphql.GraphQLString) },
    channelId: { type: graphql.GraphQLNonNull(graphql.GraphQLString) },
    roles: { type: graphql.GraphQLList(roles) },
  },
});

module.exports = {
  agentsChannel,
  roles,
};
