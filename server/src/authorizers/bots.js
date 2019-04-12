const _ = require('lodash');
const { rule } = require('graphql-shield');

/**
 * @param {object} root
 * @param {object} args
 * @param {Schema.Context} context
 * @param {object} info
 * @return {Promise<boolean>}
 */
async function isOwnerRule(root, args, context, info) {
  const botUserId = _.get(root, 'userId');
  const userId = _.get(context, 'user.id');
  const isOwner = botUserId === userId;
  console.log(`\x1b[33m Rule Bots (isOwner) ${isOwner} \x1b[0m`);
  return isOwner;
}

/** @type {Rule} */
const isOwner = rule()(isOwnerRule);

module.exports = {
  isOwner,
  isOwnerRule,
};
