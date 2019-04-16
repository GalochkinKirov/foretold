const { notify } = require("../lib/notifications");

const { DataBase } = require('./data-base');

const { MeasurementModel } = require('../models-abstract');

/**
 * @implements {Layers.DataSourceLayer.DataSource}
 */
class MeasurementsData extends DataBase {

  constructor() {
    super();
    this.MeasurementModel = new MeasurementModel();
  }

  /**
   * @todo: rename
   * @param {object} data
   * @param {object} user
   * @return {Promise<*>}
   */
  async createOne(data, user) {
    const newMeasurement = await this.models.Measurement.create(data);
    // @todo: user
    const notification = await newMeasurement.creationNotification(user);
    notify(notification);
    return newMeasurement;
  }

  /**
   * @public
   * @param {object} options
   * @param {number} [options.offset]
   * @param {number} [options.limit]
   * @param {string} [options.agentId]
   * @return {Promise<*|Array<Model>>}
   */
  async getAll(options = {}) {
    return await this.models.Measurement.findAll({
      where: {
        measurableId: {
          [this.models.sequelize.Op.in]: this.MeasurementModel.measurableIdsLiteral(options.agentId)
        },
      }
    });
  }

  /**
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
