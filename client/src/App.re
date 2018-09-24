%bs.raw
"require('../node_modules/handsontable/dist/handsontable.full.css');";

type route =
  | Home
  | AgentIndex
  | AgentShow(string)
  | MeasurableIndex
  | MeasurableShow(string)
  | NotFound;

type state = {route};
type action =
  | ChangeRoute(route);

let reducer = (action, _state) =>
  switch (action) {
  | ChangeRoute(route) => ReasonReact.Update({route: route})
  };

let mapUrlToRoute = (url: ReasonReact.Router.url) =>
  switch (url.path) {
  | [] => Home
  | ["agents"] => AgentIndex
  | ["agents", id] => AgentShow(id)
  | ["measurables"] => MeasurableIndex
  | ["measurables", id] => MeasurableShow(id)
  | _ => Home
  };

let mapUrlToAction = (url: ReasonReact.Router.url) =>
  ChangeRoute(url |> mapUrlToRoute);

let component = ReasonReact.reducerComponent("App");

let inside = r =>
  switch (r) {
  | Home => <AgentIndex />
  | AgentIndex => <AgentIndex />
  | NotFound => <AgentIndex />
  | AgentShow(id) => <AgentShow id />
  | MeasurableIndex => <MeasurableIndex />
  | MeasurableShow("new") => <MeasurableNew />
  | MeasurableShow(id) => <MeasurableShow id />
  };

let make = _children => {
  ...component,
  reducer,
  initialState: () => {route: Home},
  didMount: self => {
    ReasonReact.Router.dangerouslyGetInitialUrl()
    |> mapUrlToAction
    |> self.send;
    let watcherID =
      ReasonReact.Router.watchUrl(url => url |> mapUrlToAction |> self.send);
    self.onUnmount(() => ReasonReact.Router.unwatchUrl(watcherID));
  },
  render: self => <PaddedLayout> (self.state.route |> inside) </PaddedLayout>,
};