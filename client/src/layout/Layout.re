open Context.Routing;
open Foretold__GraphQL;
open Rationale.Function.Infix;

let defaultChannel = "4c5af8ad-1258-4b9c-8fa0-cc522708fc1c";

let tochannelId = (route: Route.t) =>
  switch (route) {
  | ChannelShow(c) => Some(c)
  | ChannelEdit(c) => Some(c)
  | ChannelMembers(c) => Some(c)
  | ChannelInvite(c) => Some(c)
  | MeasurableNew(c) => Some(c)
  | Series(c, _) => Some(c)
  | Home => Some(defaultChannel)
  | _ => None
  };

let defaultPage =
    (loggedInUser: Context.Primary.User.t, channelId: option(string), layout) => {
  let sidebar =
    Layout__Component__FillWithSidebar.make(~channelId, ~loggedInUser);

  let inApp = (~key="") =>
    E.React.makeToEl ||> E.React.withParent(~key, sidebar);

  let firstUserChannel =
    loggedInUser.agent
    |> E.O.bind(_, (a: Context.Primary.Agent.t) => a.channelMemberships)
    |> E.A.O.defaultEmpty
    |> E.A.get(_, 0)
    |> E.O.bind(_, (r: Context.Primary.Types.channelMembership) => r.channel);

  /* This should always be Some */
  switch (firstUserChannel) {
  | Some({id: channelIdSome}) =>
    MeasurableIndex.make(
      ~channelId=channelIdSome,
      ~loggedInUser,
      ~layout,
      ~itemsPerPage=20,
    )
    |> inApp(~key=channelIdSome)
  | _ => ChannelIndex.make(~loggedInUser, ~layout) |> inApp
  };
};

let toRoutePage = (route: Route.t, me: Context.Me.me) =>
  switch (me) {
  | WithTokensAndUserData({userData}) =>
    let loggedInUser = userData;
    let channelId = tochannelId(route);

    let sidebar =
      Layout__Component__FillWithSidebar.make(~channelId, ~loggedInUser);

    let inApp = (~key="") =>
      E.React.makeToEl ||> E.React.withParent(~key, sidebar);

    let layout = SLayout.FullPage.makeWithEl;

    switch (route) {
    | AgentMeasurables(id) =>
      AgentMeasurables.make(~id, ~loggedInUser, ~layout) |> inApp
    | AgentIndex => AgentIndex.make(~layout) |> inApp
    | EntityShow(id) => EntityShow.make(~id, ~layout) |> inApp
    | Redirect => Auth0Redirect.make(~me) |> inApp
    | EntityIndex => EntityIndex.make(~layout) |> inApp
    | Profile => Profile.make(~loggedInUser, ~layout) |> inApp
    | SeriesNew(channelId) =>
      SeriesNew.make(~channelId, ~loggedInUser, ~layout) |> inApp
    | AgentShow(id) => AgentShow.make(~id, ~layout) |> inApp
    | ChannelShow(channelId) =>
      MeasurableIndex.make(
        ~channelId,
        ~loggedInUser,
        ~layout,
        ~itemsPerPage=20,
      )
      |> inApp(~key=channelId)
    | ChannelEdit(channelId) =>
      ChannelEdit.make(~channelId, ~layout) |> inApp
    | ChannelInvite(channelId) =>
      ChannelInvite.make(~channelId, ~loggedInUser, ~layout) |> inApp
    | ChannelMembers(channelId) =>
      ChannelMembers.make(~channelId, ~loggedInUser, ~layout) |> inApp
    | ChannelIndex => ChannelIndex.make(~loggedInUser, ~layout) |> inApp
    | ChannelNew => ChannelNew.make(~layout) |> inApp
    | MeasurableNew(channelId) =>
      MeasurableNew.make(~channelId, ~layout) |> inApp
    | MeasurableEdit(id) => MeasurableEdit.make(~id, ~layout) |> inApp
    | Series(channelId, id) =>
      SeriesShow.make(~id, ~channelId, ~loggedInUser, ~layout) |> inApp
    | _ => defaultPage(loggedInUser, channelId, layout)
    };
  | _ =>
    switch (route) {
    | Home => <Home />
    | Login => <Login />
    | Redirect => <Auth0Redirect me />
    | _ => <Home />
    }
  };

let make = (route: Route.t) =>
  route |> toRoutePage |> Queries.User.withLoggedInUserQuery;