'use strict';

const Sequelize = require('sequelize');

module.exports = (sequelize, DataTypes) => {
  var Model = sequelize.define('Agent', {
    id: {
      type: DataTypes.UUID(),
      primaryKey: true,
      defaultValue: Sequelize.UUIDV4,
      allowNull: false
    },
    type: {
      type: DataTypes.ENUM(["USER", "BOT"]),
      allowNull: false
    },
    measurementCount: {
      allowNull: true,
      type: Sequelize.VIRTUAL(DataTypes.INTEGER),
      get: async function () {
        const items = await this.getMeasurements();
        return items.length;
      }
    }
  });
  Model.associate = function (models) {
    Model.User = Model.hasOne(models.User, {
      foreignKey: 'agentId'
    });
    Model.Bot = Model.hasOne(models.Bot, {
      foreignKey: 'agentId'
    });
    Model.Measurements = Model.hasMany(models.Measurement, {
      foreignKey: 'agentId',
      as: 'Measurements'
    });
  };
  return Model;
};
//# sourceMappingURL=agent.js.map