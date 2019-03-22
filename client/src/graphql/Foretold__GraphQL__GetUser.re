type channel = {
  id: string,
  name: string,
  isPublic: bool,
};

let toChannel = (ch: channel) =>
  DataModel.Channel.make(
    ~id=ch.id,
    ~name=ch.name,
    ~isArchived=false,
    ~isPublic=ch.isPublic,
    (),
  );

type agent = {
  id: string,
  channels: Js.Array.t(option(channel)),
};

let toAgent = (a: agent) =>
  DataModel.Agent.make(
    ~id=a.id,
    ~channels=a.channels |> E.A.O.concatSomes |> E.A.fmap(toChannel),
    (),
  );

type user = {
  id: string,
  name: string,
  auth0Id: option(string),
  agentId: option(string),
  agent: option(agent),
};

type t = option(user);

let toUser = (a: user) =>
  DataModel.User.make(
    ~id=a.id,
    ~auth0Id=a.auth0Id,
    ~agent=a.agent |> E.O.fmap(toAgent),
    (),
  );

module Query = [%graphql
  {|
    query user ($auth0Id: String) {
        user:
          user(auth0Id: $auth0Id)  @bsRecord{
            id
            auth0Id
            name
            agentId
            agent: Agent  @bsRecord{
              id
              channels: Channels @bsRecord{
                name
                id
                isPublic
              }
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
  | Some(auth0Id) =>
    let query = Query.make(~auth0Id, ());
    QueryComponent.make(~variables=query##variables, ({result}) =>
      result
      |> ApolloUtils.apolloResponseToResult
      |> E.R.fmap(e =>
           e##user
           |> (
             e => {
               let channel = (ch: channel) =>
                 DataModel.Channel.make(
                   ~id=ch.id,
                   ~name=ch.name,
                   ~isArchived=false,
                   ~isPublic=ch.isPublic,
                   (),
                 );
               e;
             }
           )
         )
      |> E.R.fmap(e => innerComponentFn(e))
      |> E.R.id
    )
    |> E.React.el;
  | None => innerComponentFn(None)
  };

let withLoggedInUserQuery = (innerComponentFn: 'a => ReasonReact.reactElement) => {
  let isLoggedIn = Auth0.isLoggedIn();
  isLoggedIn ?
    component(Auth0.userId(), innerComponentFn) : innerComponentFn(None);
};