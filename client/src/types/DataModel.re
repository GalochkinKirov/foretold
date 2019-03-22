open Rationale.Function.Infix;
open Utils;

type user = {
  id: string,
  name: string,
};

module Bot = {
  type t = {
    competitorType: [ | `AGGREGATION | `COMPETITIVE | `OBJECTIVE],
    description: option(string),
    id: string,
    name: option(string),
  };
};

module Route = {
  type t =
    | Home
    | AgentIndex
    | Redirect
    | Login
    | Profile
    | EntityShow(string)
    | AgentShow(string)
    | AgentMeasurables(string)
    | ChannelShow(string)
    | ChannelIndex
    | ChannelNew
    | MeasurableEdit(string)
    | MeasurableNew(string)
    | Series(string, string)
    | NotFound;

  let fromUrl = (url: ReasonReact.Router.url) =>
    switch (url.path) {
    | [] => Home
    | ["login"] => Login
    | ["callback"] =>
      Auth0.handleAuth(url);
      Redirect;
    | ["redirect"] => Redirect
    | ["agents"] => AgentIndex
    | ["profile"] =>
      switch (Auth0.userId()) {
      | Some(_) => Profile
      | None => Home
      }
    | ["agents", id] => AgentShow(id)
    | ["entities", ...id] => EntityShow(String.concat("/", id))
    | ["agents", id, "measurables"] => AgentMeasurables(id)
    | ["channels", "new"] => ChannelNew
    | ["channels"] => ChannelIndex
    | ["c"] => ChannelIndex
    | ["c", id] => ChannelShow(id)
    | ["c", id, "new"] => MeasurableNew(id)
    | ["measurables", id, "edit"] => MeasurableEdit(id)
    | ["c", channel, "s", id] => Series(channel, id)
    | _ => NotFound
    };
};

module Url = {
  type t =
    | Home
    | AgentIndex
    | Profile
    | EntityShow(string)
    | AgentShow(string)
    | AgentMeasurables(string)
    | ChannelShow(string)
    | ChannelNew
    | ChannelIndex
    | SeriesShow(string, string)
    | MeasurableEdit(string)
    | MeasurableNew(string);

  let toString = (r: t) =>
    switch ((r: t)) {
    | Home => "/"
    | AgentIndex => "/agents"
    | Profile => "/profile/"
    | EntityShow(id) => "/entities/" ++ id
    | AgentShow(id) => "/agents/" ++ id
    | AgentMeasurables(id) => "/agents/" ++ id ++ "/measurables"
    | ChannelNew => "/channels/" ++ "new"
    | ChannelShow(id) => "/c/" ++ id
    | ChannelIndex => "/channels"
    | MeasurableEdit(id) => "/measurables/" ++ id ++ "/edit"
    | MeasurableNew(channel) => "/c/" ++ channel ++ "/new"
    | SeriesShow(channel, id) => "/c/" ++ channel ++ "/s/" ++ id
    };

  let push = (r: t) => r |> toString |> ReasonReact.Router.push;
};
module Agent = {
  type agentType =
    | Bot(Bot.t)
    | User(user);

  type t = {
    id: string,
    name: option(string),
    measurementCount: option(int),
    agentType: option(agentType),
  };

  let name = (a: t): option(string) =>
    switch (a.agentType) {
    | Some(Bot(b)) => b.name
    | Some(User(u)) => Some(u.name)
    | None => None
    };

  let make = (~id, ~name=None, ~measurementCount=None, ~agentType=None, ()) => {
    id,
    name,
    measurementCount,
    agentType,
  };
};

module Channel = {
  type t = {
    id: string,
    name: string,
    description: option(string),
    isArchived: bool,
    isPublic: bool,
    creator: option(Agent.t),
  };
  let showLink = t => Url.ChannelShow(t.id);
  let showUrl = showLink ||> Url.toString;
  let showPush = showLink ||> Url.push;
  module Styles = {
    open Css;
    let hash = style([marginRight(`em(0.1))]);
    let lock =
      style([fontSize(`em(0.8)), float(`left), marginRight(`px(4))]);
  };
  let present = (~hashClassName="", s: t) =>
    <span>
      <span className=hashClassName>
        {
          s.isPublic ?
            <span className=Styles.hash> {"#" |> ste} </span> :
            <span className=Styles.lock> <Icon.Icon icon="LOCK" /> </span>
        }
      </span>
      <span> {s.name |> ste} </span>
    </span>;

  let make =
      (
        ~id,
        ~name,
        ~description=None,
        ~isArchived,
        ~isPublic,
        ~creator=None,
        (),
      ) => {
    id,
    name,
    description,
    isArchived,
    isPublic,
    creator,
  };
};

module Series = {
  type t = {
    id: string,
    description: option(string),
    name: option(string),
    creator: option(Agent.t),
  };

  let make = (~id, ~name=None, ~description=None, ~creator=None, ()) => {
    id,
    name,
    description,
    creator,
  };
};

type valueType = [ | `DATE | `FLOAT | `PERCENTAGE];

module MeasurableState = {
  type t = [ | `OPEN | `ARCHIVED | `JUDGED];

  let fromString = e =>
    switch (e) {
    | "OPEN" => `OPEN
    | "JUDGED" => `JUDGED
    | "ARCHIVED" => `ARCHIVED
    | _ => Js.Exn.raiseError("Invalid GraphQL State")
    };
};

module CompetitorType = {
  type t = [ | `AGGREGATION | `COMPETITIVE | `OBJECTIVE];
  let toString = e =>
    switch (e) {
    | `AGGREGATION => "Aggregation"
    | `COMPETITIVE => "Competitive"
    | `OBJECTIVE => "Objective"
    };
};

module Measurement = {
  type t = {
    id: string,
    description: option(string),
    value: Belt.Result.t(Value.t, string),
    competitorType: CompetitorType.t,
    taggedMeasurementId: option(string),
    createdAt: option(MomentRe.Moment.t),
    relevantAt: option(MomentRe.Moment.t),
    measurableId: option(string),
    agent: option(Agent.t),
  };

  let isJudgement = (m: t) => m.competitorType == `OBJECTIVE;

  let make =
      (
        ~id,
        ~value,
        ~description=None,
        ~competitorType=`COMPETITIVE,
        ~taggedMeasurementId=None,
        ~createdAt=None,
        ~relevantAt=None,
        ~agent=None,
        ~measurableId=None,
        (),
      ) => {
    id,
    value,
    description,
    competitorType,
    taggedMeasurementId,
    createdAt,
    relevantAt,
    agent,
    measurableId,
  };
};

module MeasurableStatus = {
  type t =
    | OPEN
    | PENDING_REVIEW
    | ARCHIVED
    | JUDGED;

  /* TODO: CHange closed to judged */

  let toInt = (status: t) =>
    switch (status) {
    | OPEN => 3
    | PENDING_REVIEW => 2
    | JUDGED => 1
    | ARCHIVED => 0
    };
};

module Measurable = {
  type t = {
    id: string,
    name: string,
    valueType,
    channel: option(string),
    description: option(string),
    resolutionEndpoint: option(string),
    resolutionEndpointResponse: option(float),
    measurementCount: option(int),
    measurerCount: option(int),
    state: option(MeasurableState.t),
    descriptionEntity: option(string),
    descriptionDate: option(MomentRe.Moment.t),
    descriptionProperty: option(string),
    createdAt: option(MomentRe.Moment.t),
    updatedAt: option(MomentRe.Moment.t),
    expectedResolutionDate: option(MomentRe.Moment.t),
    stateUpdatedAt: option(MomentRe.Moment.t),
    creator: option(Agent.t),
    measurements: option(list(Measurement.t)),
    series: option(Series.t),
  };

  let toStatus = (measurable: t) => {
    let state = measurable.state |> E.O.toExn("Needs state from GraphQL");
    if (state === `ARCHIVED) {
      MeasurableStatus.ARCHIVED;
    } else if (state === `JUDGED) {
      JUDGED;
    } else {
      let pastExpectedResolutionDate =
        switch (measurable.expectedResolutionDate) {
        | None => false
        | Some(e) => MomentRe.Moment.isAfter(MomentRe.momentNow(), e)
        };
      if (pastExpectedResolutionDate) {PENDING_REVIEW} else {OPEN};
    };
  };

  let compare = (measurableA: t, measurableB: t) =>
    switch (
      toStatus(measurableA),
      toStatus(measurableB),
      measurableA.expectedResolutionDate,
      measurableB.expectedResolutionDate,
    ) {
    | (a, b, Some(aa), Some(bb)) when a == b =>
      MomentRe.Moment.isAfter(aa, bb) ? 1 : (-1)
    | (a, b, _, _) =>
      MeasurableStatus.toInt(a) > MeasurableStatus.toInt(b) ? (-1) : 1
    };

  let stableSort = m => E.A.stableSortBy(m, compare);

  let make =
      (
        ~id,
        ~name,
        ~valueType=`FLOAT,
        ~channel=None,
        ~description=None,
        ~resolutionEndpoint=None,
        ~resolutionEndpointResponse=None,
        ~measurementCount=None,
        ~measurerCount=None,
        ~state=None,
        ~createdAt=None,
        ~updatedAt=None,
        ~expectedResolutionDate=None,
        ~stateUpdatedAt=None,
        ~creator=None,
        ~measurements=None,
        ~descriptionEntity=None,
        ~descriptionDate=None,
        ~descriptionProperty=None,
        ~series=None,
        (),
      ) => {
    id,
    name,
    channel,
    valueType,
    description,
    resolutionEndpoint,
    resolutionEndpointResponse,
    measurementCount,
    measurerCount,
    state,
    createdAt,
    updatedAt,
    expectedResolutionDate,
    stateUpdatedAt,
    creator,
    measurements,
    descriptionEntity,
    descriptionDate,
    descriptionProperty,
    series,
  };
};