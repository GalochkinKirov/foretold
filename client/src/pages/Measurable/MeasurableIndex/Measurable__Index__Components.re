open Rationale.Function.Infix;
open Rationale.Option.Infix;
open Utils;
open Style.Grid;
open Measurable__Index__Logic;

module ReducerParams = SelectWithPaginationReducer.Reducers.ReducerParams;

let leaveButton = (agent, channelId) =>
  agent
  |> E.O.fmap((e: Context.Primary.Agent.t) => e.id)
  |> E.O.React.fmapOrNull(C.Channel.SimpleHeader.leaveChannel(channelId));

let joinButton = (agent, channelId) =>
  agent
  |> E.O.fmap((e: Context.Primary.Agent.t) => e.id)
  |> E.O.React.fmapOrNull(C.Channel.SimpleHeader.joinChannel(channelId));

module LoadedAndSelected = {
  open Measurable__Index__Logic.LoadedAndSelected;

  let header = (t: t, send: SelectWithPaginationReducer.Types.send) =>
    <>
      <Div float=`left>
        {SelectWithPaginationReducer.Components.deselectButton(send)}
      </Div>
      <Div float=`right>
        {
          SelectWithPaginationReducer.Components.buttonDuo(
            Item,
            t.reducerParams,
          )
        }
      </Div>
    </>;

  let body = (t: t) =>
    <C.Measurable.FullPresentation
      id={t.selectedMeasurable |> (e => e.id)}
      loggedInUser={t.loggedInUser}
    />;
};

module LoadedAndUnselected = {
  open Measurable__Index__Logic.LoadedAndUnselected;

  let header = (t: t, send: SelectWithPaginationReducer.Types.send) =>
    <Div float=`right>
      {
        SelectWithPaginationReducer.Components.buttonDuo(Page, t.reducerParams)
      }
      {C.Channel.SimpleHeader.newMeasurable(t.channel.id)}
    </Div>;

  let seriesList = (t: t) =>
    <>
      {"Series List" |> ste |> E.React.inH2}
      {
        C.SeriesCollection.SeriesCards.make(
          t.channel.id,
          filteredSeriesCollection(t),
        )
      }
    </>;

  let body = (t: t, send: SelectWithPaginationReducer.Types.send) => {
    let measurables =
      (
        switch (t.reducerParams.response) {
        | Success(r) => Some(r)
        | _ => None
        }
      )
      |> E.O.toExn("");
    let loggedInUser = t.loggedInUser;
    <>
      {E.React.showIf(shouldShowSeriesCollection(t), seriesList(t))}
      <C.Measurables.BasicTable
        measurables
        loggedInUser
        showExtraData=true
        onSelect={
          e =>
            SelectWithPaginationReducer.Components.sendSelectItem(
              t.reducerParams,
              e.id,
            )
        }
      />
    </>;
  };
};

module MeasurableIndexDataState = {
  open Measurable__Index__Logic.MeasurableIndexDataState;

  let toLayoutInput =
      (send: SelectWithPaginationReducer.Types.send, state: state) => {
    let lmake = SLayout.LayoutConfig.make;
    switch (state) {
    | InvalidIndexError(channel) =>
      lmake(~head=E.React.null, ~body="Item Not Valid" |> ste)
    | WithChannelButNotQuery(c) =>
      lmake(~head=E.React.null, ~body="Loading Query..." |> ste)
    | LoadedAndUnselected(l) =>
      lmake(
        ~head=LoadedAndUnselected.header(l, send),
        ~body=LoadedAndUnselected.body(l, send),
      )
    | LoadedAndSelected(l) =>
      lmake(
        ~head=LoadedAndSelected.header(l, send),
        ~body=LoadedAndSelected.body(l),
      )
    | WithoutChannel(channelResponse) =>
      lmake(~head=E.React.null, ~body="No channel." |> ste)
    };
  };
};