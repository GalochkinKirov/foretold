const _ = require('lodash');

const { notify } = require("../lib/notifications");
const { MeasurableModel } = require('../models-abstract/measurable-model');

const { DataBase } = require('./data-base');

/**
 * @implements {Layers.DataSourceLayer.DataSource}
 */
class MeasurablesData extends DataBase {

  constructor() {
    super();
    this.MeasurableModel = new MeasurableModel();
  }

  /**
   * @param data
   * @param {object} user
   * @return {Promise<*>}
   */
  async createOne(data, user) {
    const newMeasurable = await this.models.Measurable.create(data);
    let notification = await newMeasurable.creationNotification(user);
    notify(notification);
    return newMeasurable;
  }

  /**
   * @param {string} id
   * @return {Promise<Models.Measurable>}
   */
  async archive(id) {
    let measurable = await this.models.Measurable.findById(id);
    return measurable.archive();
  }

  /**
   * @param {string} id
   * @return {Promise<Models.Measurable>}
   */
  async unArchive(id) {
    let measurable = await this.models.Measurable.findById(id);
    return measurable.unarchive();
  }

  /**
   * @param {string} id
   * @param {object} data
   * @param {object} user
   * @return {Promise<Models.Measurable>}
   */
  async updateOne(id, data, user) {
    let measurable = await this.models.Measurable.findById(id);
    let notification = await measurable.updateNotifications(user, data);
    notify(notification);
    return measurable.update(data);
  }

  /**
   * @todo: Move me into MeasurableModel.
   * @param {object} options
   * @param {string[]} options.states
   * @param {string} options.agentId
   * @return {Promise<*|Array<Model>>}
   */
  async getAll(options) {
    const { offset, limit, channelId, seriesId, creatorId } = options;
    const sequelize = this.models.sequelize;
    const Op = sequelize.Op;

    const where = { [Op.and]: [] };

    // @todo: Restrictions, move it upper, into resolvers!
    where[Op.and].push({
      channelId: { [Op.in]: this.channelIdsLiteral(options.agentId) }
    });

    // Filter
    if (_.isArray(options.states)) {
      where.state = { [Op.in]: options.states };
    }
    if (channelId) where.channelId = channelId;
    if (seriesId) where.seriesId = seriesId;
    if (creatorId) where.creatorId = creatorId;
    where.isArchived = false;

    // Query
    return await this.models.Measurable.findAll({
      limit,
      offset,
      where,
      order: [
        [sequelize.col('stateOrder'), 'ASC'],
        ['createdAt', 'DESC'],
      ],
      attributes: {
        include: [
          this.MeasurableModel.getStateOrderField(),
        ]
      }
    });
  }

  /**
   * @param {string} id
   * @param {object} options
   * @param {string} options.agentId
   * @return {Promise<*>}
   */
  async getOne(id, options = {}) {
    const restrictions = 'agentId' in options ? {
      channelId: {
        [this.models.sequelize.Op.in]: this.channelIdsLiteral(options.agentId)
      }
    } : {};
    return await this.models.Measurable.findOne({
      where: {
        id,
        ...restrictions,
      }
    });
  }

  /**
   * @return {Promise<boolean>}
   */
  needsToBePending() {
    return this.MeasurableModel.needsToBePending();
  }
}

module.exports = {
  MeasurablesData,
};
