/**
 * @todo: Rename table to "channels".
 * @param sequelize
 * @param DataTypes
 * @return {*}
 */
module.exports = (sequelize, DataTypes) => {
  const Channel = sequelize.define('Channel', {
    id: {
      type: DataTypes.UUID(),
      primaryKey: true,
      defaultValue: sequelize.fn('uuid_generate_v4'),
      allowNull: false,
    },
    name: {
      type: DataTypes.STRING,
      allowNull: false,
      unique: true,
    },
    description: {
      type: DataTypes.TEXT,
      allowNull: true,
    },
    isArchived: {
      type: DataTypes.BOOLEAN,
      allowNull: false,
      defaultValue: false,
    },
    isPublic: {
      type: DataTypes.BOOLEAN,
      allowNull: false,
      defaultValue: true,
    },
  });


  Channel.associate = function (models) {
    // Usage:
    //
    // const ch = await models.Channel.findById('406da139-e440-4c74-bb3c-514ed1872cea');
    // const cr = await ch.getCreator();
    //
    // models.Channel.findAll({
    //   include: [
    //     {model: models.Agent, as: 'creator'},
    //     {model: models.Agent}
    //   ]
    // })
    //
    Channel.Creator = Channel.belongsTo(models.Agent, {
      foreignKey: 'creatorId',
      as: 'creator',
    });
    // const ch = await models.Channel.findById('406da139-e440-4c74-bb3c-514ed1872cea');
    // const ag = await ch.getAgents();
    Channel.Agents = Channel.belongsToMany(models.Agent, {
      through: models.ChannelsMemberships,
      foreignKey: 'channelId',
      otherKey: 'agentId',
      as: 'agents'
    });
  };

  return Channel;
};
