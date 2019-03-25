open DataModel;
open Foretold__GraphQL;

let defaultChannel = "4c5af8ad-1258-4b9c-8fa0-cc522708fc1c";
let component = ReasonReact.statelessComponent("TopLevel");
let make = (~route: Route.t, _children) => {
  ...component,
  render: _ => {
    let channelId =
      switch (route) {
      | ChannelShow(c) => Some(c)
      | MeasurableNew(c) => Some(c)
      | Series(c, _) => Some(c)
      | Home => Some(defaultChannel)
      | _ => None
      };

    Queries.User.withLoggedInUserQuery2(result =>
      switch (result) {
      | WithTokensAndUserData({userData}) =>
        let loggedInUser = userData;
        let inApp = (~key="", e) =>
          Layout__Component__FillWithSidebar.make(~channelId, ~loggedInUser)
          |> E.React.makeToEl(~children=E.React.makeToEl(e), ~key);
        switch (route) {
        | AgentMeasurables(id) =>
          AgentMeasurables.make(~id, ~loggedInUser) |> inApp
        | AgentIndex => AgentIndex.make |> inApp
        | EntityShow(id) => EntityShow.make(~id) |> inApp
        | Redirect => Redirect.make(~me=result) |> inApp
        | Profile => Profile.make(~loggedInUser) |> inApp
        | AgentShow(id) => AgentShow.make(~id) |> inApp
        | ChannelShow(channelId) =>
          MeasurableIndex.make(~channelId, ~loggedInUser)
          |> inApp(~key=channelId)
        | ChannelIndex => ChannelIndex.make(~loggedInUser) |> inApp
        | ChannelNew => ChannelNew.make |> inApp
        | MeasurableNew(channelId) => MeasurableNew.make(~channelId) |> inApp
        | MeasurableEdit(id) => MeasurableEdit.make(~id) |> inApp
        | Series(channelId, id) =>
          Queries.Channel.component(~id=channelId, channel =>
            SeriesShow.make(~id, ~channel, ~loggedInUser) |> inApp
          )
        | _ =>
          MeasurableIndex.make(~channelId="defaultChannel", ~loggedInUser)
          |> inApp(~key=defaultChannel)
        };
      | _ =>
        Js.log(result);
        switch (route) {
        | Home => <Home />
        | Login => <Login />
        | Redirect => <Redirect me=result />
        | _ => <Home />
        };
      }
    );
  },
};