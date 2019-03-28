const data = require('../data');

/**
 * @param root
 * @param values
 * @returns {Promise<Model>}
 */
async function create(root, values) {
  return await data.agentsChannelsData.createOne(values.channelId, values.agentId, values.roles);
}

/**
 * @param root
 * @param values
 * @returns {Promise<Model>}
 */
async function update(root, values) {
  return await data.agentsChannelsData.updateOne(values.channelId, values.agentId, values.roles);
}

/**
 * @param root
 * @param values
 * @returns {Promise<Model>}
 */
async function remove(root, values) {
  return await data.agentsChannelsData.deleteOne(values.channelId, values.agentId);
}

module.exports = {
  remove,
  create,
  update,
};
