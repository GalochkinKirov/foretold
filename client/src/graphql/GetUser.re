module Query = [%graphql
  {|
    query user ($auth0Id: String) {
        user:
          user(auth0Id: $auth0Id)  {
            id
            auth0Id
            name
            agentId
            agent: Agent  {
              id
            }
        }
    }
  |}
];

module QueryComponent = ReasonApollo.CreateQuery(Query);

let component =
    (
      auth0Id: option(string),
      innerComponentFn: 'a => ReasonReact.reactElement,
    ) =>
  switch (auth0Id) {
  | Some(auth) =>
    let query = Query.make(~auth0Id=auth, ());
    QueryComponent.make(
      ~variables=query##variables, ~pollInterval=5000, ({result}) =>
      result
      |> ApolloUtils.apolloResponseToResult
      |> E.R.fmap(e => innerComponentFn(Some(e)))
      |> E.R.id
    )
    |> ReasonReact.element;
  | None => innerComponentFn(None)
  };