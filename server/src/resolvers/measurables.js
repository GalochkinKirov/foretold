const _ = require('lodash');
const data = require('../data');

/**
 * @param root
 * @param args
 * @param context
 * @param info
 * @returns {Promise<*|Array<Model>>}
 */
async function all(root, args, context, info) {
  const agentId = context.user.agentId;
  return await data.measurables.getAll({ ...args, agentId });
}

/**
 * @param root
 * @param args
 * @param context
 * @param info
 * @returns {Promise<*|Array<Model>>}
 */
async function one(root, args, context, info) {
  const agentId = context.user.agentId;
  return await data.measurables.getOne(args.id, { agentId });
}

/**
 * @param root
 * @param args
 * @param context
 * @param info
 * @returns {Promise<*|Array<Model>>}
 */
async function create(root, args, context, info) {
  const user = context.user;
  const datas = {
    ...args,
    creatorId: user.agentId,
  };
  return await data.measurables.createOne(datas, user);
}

/**
 * @param root
 * @param args
 * @param context
 * @param info
 * @returns {Promise<*|Array<Model>>}
 */
async function archive(root, args, context, info) {
  const { id } = args;
  return await data.measurables.archive(id);
}

/**
 * @param root
 * @param args
 * @param context
 * @param info
 * @returns {Promise<*|Array<Model>>}
 */
async function unarchive(root, args, context, info) {
  const { id } = args;
  return await data.measurables.unArchive(id);
}

/**
 * @param root
 * @param args
 * @param context
 * @param info
 * @returns {Promise<*|Array<Model>>}
 */
async function update(root, args, context, info) {
  const id = args.id;
  const datas = _.omit(args, ['id']);
  const user = context.user;
  return await data.measurables.updateOne(id, datas, user);
}


module.exports = {
  one,
  all,
  update,
  unarchive,
  archive,
  create,
};