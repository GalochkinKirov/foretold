const _ = require('lodash');

const models = require('../models');

const { AgentsChannelsData } = require('./agents-channels-data');

class ChannelsData {

  constructor() {
    this.agentsChannelsData = new AgentsChannelsData();
  }

  /**
   * @tested
   * @public
   * @param {Models.User} user
   * @param {Schema.ChannelsInput} input
   * @return {Promise<Models.Channel>}
   */
  async createOne(user, input) {
    let channel = await models.Channel.findOne({
      where: { name: input.name },
    });

    if (!channel) {
      channel = await models.Channel.create({
        ...input,
        creatorId: user.agentId,
      });
      await this.agentsChannelsData.createOne(channel.id, user.agentId);
    }
    return channel;
  }

  /**
   * @tested
   * @public
   * @param {string} id
   * @param {object} input
   * @return {Promise<Models.Channel>}
   */
  async updateOne(id, input) {
    const channel = await models.Channel.findOne({
      where: { id },
    });
    if (channel) await channel.update(input);
    return channel;
  }

  /**
   * @tested
   * @public
   * @param {string} id
   * @return {Promise<Model[]>}
   */
  async getAgentsByChannelId(id) {
    const channel = await models.Channel.findOne({
      where: { id },
      include: [{ model: models.Agent, as: 'agents' }],
    });
    return _.get(channel, 'agents', []);
  }

  /**
   * @tested
   * @public
   * @param {string} id
   * @return {Promise<Model>}
   */
  async getCreatorByChannelId(id) {
    const channel = await models.Channel.findOne({
      where: { id },
      include: [{ model: models.Agent, as: 'creator' }],
    });
    return _.get(channel, 'creator');
  }

  /**
   * @tested
   * @public
   * @param {object} options
   * @param {number} [options.offset]
   * @param {number} [options.limit]
   * @param {object} [options.restrictions]
   * @return {Promise<*|Array<Model>>}
   */
  async getAll(options = {}) {
    const offset = _.get(options, 'offset', 0);
    const limit = _.get(options, 'limit', 10);
    const query = this.getRestrictionsSync(options);
    return await models.Channel.findAll({
      limit,
      offset,
      ...query,
      order: [['createdAt', 'DESC']],
    });
  }

  /**
   * @tested
   * @public
   * @param {string} id
   * @param {object} options
   * @param {number} [options.offset]
   * @param {number} [options.limit]
   * @param {object} [options.restrictions]
   * @return {Promise<*|Array<Model>>}
   */
  async getOne(id, options = {}) {
    const query = this.getRestrictionsSync(options);
    return await models.Channel.findOne({
      ...query,
      where: { id, ...query.where }
    });
  }

  /**
   * @tested
   * @protected
   * @param {object} [options]
   * @param {object} [options.restrictions]
   * @param {string[]} [options.restrictions.channelIds]
   * @return {object}
   */
  getRestrictionsSync(options = {}) {
    const restrictions = _.get(options, 'restrictions', {});
    const query = { where: {} };
    if (restrictions.channelIds) {
      query.where = {
        [models.sequelize.Op.or]: [
          {
            id: restrictions.channelIds,
            isPublic: false,
          },
          {
            isPublic: true,
          }
        ]
      };
    }
    return query;
  }
}

module.exports = {
  ChannelsData,
};
