'use strict';

module.exports = {
  up: async function (queryInterface, Sequelize) {
      var ID_TYPE = Sequelize.UUID;

      const ID = {
        allowNull: false,
        primaryKey: true,
        type: ID_TYPE
      };

      const referenceTo = (target, allowNull) => ({
        type: ID_TYPE,
        allowNull,
        references: {
          model: target,
          key: "id"
        }
      });

      const standardColumns = {
        id: ID,
        createdAt: {
          type: Sequelize.DATE
        },
        updatedAt: {
          type: Sequelize.DATE
        },
      };

      await queryInterface.createTable('Agents', {
        ...standardColumns,
        type: {
          allowNull: false,
          type: Sequelize.ENUM(["USER", "BOT"])
        },
      });

      await queryInterface.createTable('Users', {
        ...standardColumns,
        name: {
          allowNull: false,
          type: Sequelize.STRING
        },
        agentId: referenceTo("Agents", false),
      });

      await queryInterface.createTable('Bots', {
        ...standardColumns,
        name: {
          allowNull: true,
          type: Sequelize.STRING
        },
        description: {
          allowNull: true,
          type: Sequelize.TEXT
        },
        competitorType: {
          type: Sequelize.ENUM(["COMPETITIVE", "AGGREGATION", "OBJECTIVE"]),
          defaultValue: "COMPETITIVE",
          allowNull: true,
        },
        userId: referenceTo("Users", false),
        agentId: referenceTo("Agents", false),
      });

      await queryInterface.createTable('Measurables', {
        ...standardColumns,
        name: {
          allowNull: false,
          type: Sequelize.STRING
        },
        valueType: {
          allowNull: false,
          type: Sequelize.ENUM(["FLOAT", "DATE", "PERCENTAGE"])
        },
        isLocked: {
          allowNull: false,
          defaultValue: false,
          type: Sequelize.BOOLEAN
        },
        lockedAt: {
          allowNull: true,
          type: Sequelize.DATE
        },
      });

      await queryInterface.createTable('Measurements', {
        ...standardColumns,
        value: {
          type: Sequelize.JSON,
          allowNull: false
        },
        competitorType: {
          type: Sequelize.ENUM(["COMPETITIVE", "AGGREGATION", "OBJECTIVE"]),
          defaultValue: "COMPETITIVE",
          allowNull: true,
        },
        agentId: referenceTo("Agents", false),
        measurableId: referenceTo("Measurables", true),
        taggedMeasurement: referenceTo("Measurables", true),
        relevantAt: {
          allowNull: true,
          type: Sequelize.DATE
        },
      });
    },

    down: async function (queryInterface, Sequelize) {
      await queryInterface.dropTable('Measurements');
      await queryInterface.dropTable('Measureables');
      await queryInterface.dropTable('Users');
    }
};
