const { notify } = require("../lib/notifications");

const { DataBase } = require('./data-base');

const { MeasurementModel } = require('../models-abstract');

/**
 * @implements {Layers.DataSourceLayer.DataSource}
 * @property {MeasurementModel} MeasurementModel
 */
class MeasurementsData extends DataBase {

  constructor() {
    super();
    this.MeasurementModel = new MeasurementModel();
  }

  /**
   * @todo: fix interface
   * @todo: rename, move down
   * @param {object} data
   * @param {Models.Creator} creator
   * @return {Promise<Models.Measurement>}
   */
  async createOne(data, creator) {
    const measurement = await this.models.Measurement.create(data);
    /** @type {Models.Measurement} */
    const notification = await measurement.creationNotification(creator);
    notify(notification);
    return measurement;
  }

  /**
   * @param {object} [filter]
   * @param {object} [pagination]
   * @param {object} [options]
   * @param {boolean} [options.measurableId]
   * @param {string} options.agentId
   * @return {Promise<void>}
   */
  async getAll(filter = {}, pagination = {}, options = {}) {
    const restrictions = {
      measurableId: true,
      agentId: options.agentId,
    };
    return this.MeasurementModel.getAll(filter, pagination, restrictions);
  }

  /**
   * @todo: fix interface
   * @todo: move down
   * @param {string} id
   * @param {object} options
   * @param {string} [options.agentId]
   * @return {Promise<*>}
   */
  async getOne(id, options = {}) {
    return await this.models.Measurement.findOne({
      where: {
        id,
        measurableId: {
          [this.models.sequelize.Op.in]: this.MeasurementModel.measurableIdsLiteral(options.agentId)
        },
      }
    });
  }
}

module.exports = {
  MeasurementsData,
};
