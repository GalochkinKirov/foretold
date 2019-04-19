const _ = require('lodash');

const models = require('../models');
const { ModelPostgres } = require('./model-postgres');

/**
 * @implements {Layers.AbstractModelsLayer.AbstractModel}
 */
class MeasurementModel extends ModelPostgres {

  constructor() {
    super({
      model: models.Measurement,
      sequelize: models.sequelize,
    });
  }

  /**
   * @public
   * @param {Layers.AbstractModelsLayer.filter} [filter]
   * @param {string} [filter.measurableId]
   * @param {string} [filter.agentId]
   * @param {string} [filter.after]
   * @param {string} [filter.before]
   * @param {Layers.AbstractModelsLayer.pagination} [pagination]
   * @param {Layers.AbstractModelsLayer.restrictions} [restrictions]
   * @return {Promise<{data: Models.Measurement[], total: number}>}
   */
  async getAll(filter, pagination, restrictions) {
    const where = {};

    this.applyRestrictions(where, restrictions);
    this.applyCursors(where, filter);

    if (filter.measurableId) where.measurableId = filter.measurableId;
    if (filter.agentId) where.agentId = filter.agentId;

    const cond = {
      limit: pagination.limit,
      offset: pagination.offset,
      where,
      order: [
        ['createdAt', 'DESC'],
      ],
    };

    /** @type {Models.Measurement[]} */
    const data = await this.model.findAll(cond);
    /** @type {number} */
    const total = await this.model.count({ where });

    return { data, total };
  }
}

module.exports = {
  MeasurementModel,
};
