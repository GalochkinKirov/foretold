const graphqlClient = require('graphql-client');

const queries = require('./queries');
const config = require('../config');

class API {
  /**
   * @param {string} token
   */
  constructor(token) {
    this.token = token;
    this.graphqlClient = graphqlClient;
    this.queries = queries;
    this.config = config;

    this.client = this.getClient();
  }

  /**
   * @protected
   * @return {{query}}
   */
  getClient() {
    return this.graphqlClient({
      url: this.config.API_URL,
      headers: {
        Authorization: 'Bearer ' + this.token,
      }
    });
  }

  /**
   * @protected
   * @param query
   * @param {object} [variables]
   * @return {*}
   */
  query(query, variables = {}) {
    return this.client.query(query, variables, (req, res) => {
      if (res.status === 401) {
        throw new Error('Not authorized');
      }
    }).then((body) => {
      console.log(body);
      return body;
    }).catch((err) => {
      console.log(err.message);
      return Promise.reject(err);
    });
  }

  /**
   * @public
   * @return {*}
   */
  measurables() {
    return this.query(this.queries.measurables);
  }
}

module.exports = {
  API,
};
