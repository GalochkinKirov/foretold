const series = require('./series');
const data = require('../data');

describe('Series Resolvers', () => {

  describe('one()', () => {
    const root = {};
    const args = { id: 'id1' };
    const context = { user: { agentId: 'agentId1' } };
    const info = {};
    beforeEach(() => {
      jest.spyOn(data.series, 'getOne').mockReturnValue(
        Promise.resolve(true),
      );
    });
    it('returns series', () => {
      return series.one(root, args, context, info).then((result) => {
        expect(data.series.getOne).toHaveBeenCalledWith(
          'id1',
          { "agentId": "agentId1" },
        );
        expect(result).toEqual(true);
      });
    });
  });

  describe('all()', () => {
    const root = {};
    const args = {};
    const context = { user: { agentId: 'agentId1' } };
    const info = {};
    beforeEach(() => {
      jest.spyOn(data.series, 'getAll').mockReturnValue(
        Promise.resolve(true),
      );
    });
    it('returns series collection', () => {
      return series.all(root, args, context, info).then((result) => {
        expect(data.series.getAll).toHaveBeenCalledWith(
          { "agentId": "agentId1" },
        );
        expect(result).toEqual(true);
      });
    });
  });

  describe('create()', () => {
    const root = {};
    const args = { input: { a: 'a1' } };
    const context = { user: { agentId: 'agentId1' } };
    const info = {};
    beforeEach(() => {
      jest.spyOn(data.series, 'createOne').mockReturnValue(
        Promise.resolve(true),
      );
    });
    it('creates series', () => {
      return series.create(root, args, context, info).then((result) => {
        expect(data.series.createOne).toHaveBeenCalledWith(
          {"a": "a1", "creatorId": "agentId1"},
        );
        expect(result).toBe(true);
      });
    });
  });

});
