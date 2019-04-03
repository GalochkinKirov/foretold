type channel = {
  id: string,
  name: string,
  isPublic: bool,
};

type channelMembership = {
  role: Context.Primary.channelMembershipRole,
  channel: option(channel),
};

type channelMemberships = Js.Array.t(option(channelMembership));

let toChannel = (ch: channel) =>
  Context.Primary.Channel.make(
    ~id=ch.id,
    ~name=ch.name,
    ~isArchived=false,
    ~isPublic=ch.isPublic,
    (),
  );

let toChannelMembership =
    (ch: channelMembership): Context.Primary.Types.channelMembership => {
  channel: ch.channel |> E.O.fmap(toChannel),
  role: `ADMIN,
  agent: None,
};

type agent = {
  id: string,
  channelMemberships,
};

let toAgent = (a: agent) =>
  Context.Primary.Agent.make(
    ~id=a.id,
    ~channelMemberships=
      a.channelMemberships
      |> E.A.O.concatSomes
      |> E.A.fmap(toChannelMembership)
      |> E.O.some,
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
  Context.Primary.User.make(
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
              channelMemberships @bsRecord{
                role
                channel @bsRecord{
                  name
                  id
                  isPublic
                }
              }
            }
        }
    }
  |}
];

module QueryComponent = ReasonApollo.CreateQuery(Query);

let withLoggedInUserQuery = innerComponentFn =>
  switch (Context.Auth.AuthTokens.make_from_storage()) {
  | None => innerComponentFn(Context.Me.WithoutTokens)
  | Some(tokens) =>
    Context.Auth.Actions.logoutIfTokenIsObsolete(tokens);
    switch (tokens |> Context.Auth.AuthTokens.auth0Id) {
    | None => innerComponentFn(Context.Me.WithoutTokens)
    | Some(auth0Id) =>
      let query = Query.make(~auth0Id, ());
      QueryComponent.make(~variables=query##variables, ({result}) =>
        result
        |> E.HttpResponse.fromApollo
        |> E.HttpResponse.fmap(e => e##user |> E.O.fmap(toUser))
        |> E.HttpResponse.optionalToMissing
        |> (
          e =>
            switch (e) {
            | Success(c) =>
              innerComponentFn(
                Context.Me.WithTokensAndUserData({
                  authTokens: tokens,
                  userData: c,
                }),
              )
            | _ =>
              innerComponentFn(
                Context.Me.WithTokensAndUserLoading({
                  authTokens: tokens,
                  loadingUserData: e,
                }),
              )
            }
        )
      )
      |> E.React.el;
    };
  };