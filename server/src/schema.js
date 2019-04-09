const graphql = require("graphql");
const { resolver } = require("graphql-sequelize");
const { applyMiddleware } = require('graphql-middleware');

const models = require("./models");
const resolvers = require('./resolvers');

const types = require('./types');
const { stats } = require('./types/stats');

const { permissions } = require('./authorizers');
const { middlewares } = require('./middlewares');

const schema = new graphql.GraphQLSchema({
  query: new graphql.GraphQLObjectType({
    name: 'Query',
    fields: {

      user: {
        type: types.userType,
        args: {
          id: { type: graphql.GraphQLString },
          auth0Id: { type: graphql.GraphQLString },
        },
        resolve: resolvers.users.one,
      },

      users: {
        type: graphql.GraphQLNonNull(graphql.GraphQLList(types.userType)),
        resolve: resolver(models.User),
      },

      measurement: {
        type: types.measurements.measurement,
        args: {
          id: { type: graphql.GraphQLNonNull(graphql.GraphQLString) },
        },
        resolve: resolvers.measurements.one,
      },

      measurements: {
        type: graphql.GraphQLNonNull(graphql.GraphQLList(types.measurements.measurement)),
        args: {
          measurableId: { type: graphql.GraphQLString },
        },
        resolve: resolvers.measurements.all,
      },

      measurable: {
        type: types.measurables.measurable,
        args: {
          id: { type: graphql.GraphQLNonNull(graphql.GraphQLString) },
        },
        resolve: resolvers.measurables.one,
      },

      measurables: {
        type: graphql.GraphQLNonNull(graphql.GraphQLList(types.measurables.measurable)),
        args: {
          offset: { type: graphql.GraphQLInt },
          limit: { type: graphql.GraphQLInt },
          creatorId: { type: graphql.GraphQLString },
          seriesId: { type: graphql.GraphQLString },
          channelId: { type: graphql.GraphQLString },
          states: { type: graphql.GraphQLList(types.measurables.measurableState) },
        },
        resolve: resolvers.measurables.all,
      },

      bot: {
        type: types.botType,
        args: {
          id: { type: graphql.GraphQLNonNull(graphql.GraphQLString) },
        },
        resolve: resolver(models.Bot),
      },

      bots: {
        type: graphql.GraphQLNonNull(graphql.GraphQLList(types.botType)),
        resolve: resolver(models.Bot),
      },

      agent: {
        type: types.agents.agent,
        args: {
          id: { type: graphql.GraphQLNonNull(graphql.GraphQLString) },
        },
        resolve: resolver(models.Agent),
      },

      agents: {
        type: graphql.GraphQLNonNull(graphql.GraphQLList(types.agents.agent)),
        resolve: resolver(models.Agent),
      },

      series: {
        type: types.series.series,
        args: {
          id: { type: graphql.GraphQLNonNull(graphql.GraphQLString) },
        },
        resolve: resolvers.series.one,
      },

      seriesCollection: {
        type: graphql.GraphQLNonNull(graphql.GraphQLList(types.series.series)),
        args: {
          channelId: { type: graphql.GraphQLString },
        },
        resolve: resolvers.series.all,
      },

      channel: {
        type: types.channels.channel,
        args: { id: { type: graphql.GraphQLNonNull(graphql.GraphQLString) } },
        resolve: resolvers.channels.one,
      },

      channels: {
        type: graphql.GraphQLNonNull(graphql.GraphQLList(types.channels.channel)),
        args: {
          offset: { type: graphql.GraphQLInt },
          limit: { type: graphql.GraphQLInt },
        },
        resolve: resolvers.channels.all,
      },

      stats: {
        type: new graphql.GraphQLNonNull(stats),
        resolve: async (ops, values, options) => {
          // @todo:
          return 11;
        }
      },
    }
  }),
  mutation: new graphql.GraphQLObjectType({
    name: 'Mutation',
    fields: {

      measurementCreate: {
        type: types.measurements.measurement,
        args: {
          input: { type: graphql.GraphQLNonNull(types.measurements.measurementCreateInput) },
        },
        resolve: resolvers.measurements.create,
      },

      measurableCreate: {
        type: types.measurables.measurable,
        args: {
          input: { type: graphql.GraphQLNonNull(types.measurables.measurableCreateInput) },
        },
        resolve: resolvers.measurables.create,
      },

      seriesCreate: {
        type: types.series.series,
        args: {
          input: { type: graphql.GraphQLNonNull(types.series.seriesCreateInput) },
        },
        resolve: resolvers.series.create,
      },

      measurableArchive: {
        type: types.measurables.measurable,
        args: {
          id: { type: graphql.GraphQLNonNull(graphql.GraphQLString) },
        },
        resolve: resolvers.measurables.archive,
      },

      measurableUnarchive: {
        type: types.measurables.measurable,
        args: {
          id: { type: graphql.GraphQLNonNull(graphql.GraphQLString) },
        },
        resolve: resolvers.measurables.unarchive,
      },

      measurableUpdate: {
        type: types.measurables.measurable,
        args: {
          id: { type: graphql.GraphQLNonNull(graphql.GraphQLString) },
          input: { type: graphql.GraphQLNonNull(types.measurables.measurableUpdateInput) },
        },
        resolve: resolvers.measurables.update,
      },

      userUpdate: {
        type: types.userType,
        args: {
          id: { type: graphql.GraphQLNonNull(graphql.GraphQLString) },
          input: { type: graphql.GraphQLNonNull(types.users.userUpdateInput) },
        },
        resolve: resolvers.users.update,
      },

      channelUpdate: {
        type: types.channels.channel,
        args: {
          id: { type: graphql.GraphQLString },
          input: { type: graphql.GraphQLNonNull(types.channels.channelInput) },
        },
        resolve: resolvers.channels.update,
      },

      channelCreate: {
        type: types.channels.channel,
        args: {
          input: { type: graphql.GraphQLNonNull(types.channels.channelInput) },
        },
        resolve: resolvers.channels.create,
      },

      channelMembershipCreate: {
        type: types.channelMemberships.channelsMembership,
        args: {
          input: { type: graphql.GraphQLNonNull(types.channelMemberships.channelMembershipRoleInput) },
        },
        resolve: resolvers.channelMemberships.create,
      },

      channelMembershipRoleUpdate: {
        type: types.channelMemberships.channelsMembership,
        args: {
          input: { type: graphql.GraphQLNonNull(types.channelMemberships.channelMembershipRoleInput) },
        },
        resolve: resolvers.channelMemberships.update,
      },

      channelMembershipDelete: {
        type: types.channelMemberships.channelsMembership,
        args: {
          input: { type: graphql.GraphQLNonNull(types.channelMemberships.channelMembershipDeleteInput) },
        },
        resolve: resolvers.channelMemberships.remove,
      },

      leaveChannel: {
        type: types.channelMemberships.channelsMembership,
        args: {
          input: { type: graphql.GraphQLNonNull(types.channelMemberships.joiningChannelInput) },
        },
        resolve: resolvers.channelMemberships.leave,
      },

      joinChannel: {
        type: types.channelMemberships.channelsMembership,
        args: {
          input: { type: graphql.GraphQLNonNull(types.channelMemberships.joiningChannelInput) },
        },
        resolve: resolvers.channelMemberships.join,
      },

    }
  })
});

const schemaWithMiddlewares = applyMiddleware(
  schema,
  middlewares,
  permissions,
);

module.exports = {
  schemaWithMiddlewares,
};
