open Utils;
open Rationale;
open Style.Grid;
open Foretold__GraphQL;

module Styles = {
  open Css;
  let header =
    style([
      backgroundColor(hex("f5f7f9")),
      padding2(~v=`px(10), ~h=`px(13)),
      border(`px(1), `solid, `hex("e8f2f9")),
      borderRadius(`px(3)),
      marginBottom(`px(10)),
    ]);
};

type state = {selected: option(string)};

type action =
  | UpdateSelected(string);

let component = ReasonReact.reducerComponent("Measurables");

let seriesHero = (series: DataModel.Series.t) =>
  <Div flexDirection=`column styles=[Styles.header]>
    <Div flex=1>
      <Div flexDirection=`row>
        <Div flex=6>
          <h2>
            <Icon.Icon icon="LAYERS" />
            {series.name |> Option.default("") |> ste}
          </h2>
          <p> {series.description |> Option.default("") |> ste} </p>
          {
            switch (series.creator) {
            | Some({name: Some(name), id}) =>
              <div className=PrimaryTableStyles.item>
                <a href={Urls.mapLinkToUrl(AgentShow(id))}>
                  {name |> ste}
                </a>
              </div>
            | _ => ReasonReact.null
            }
          }
        </Div>
        <Div flex=1 />
      </Div>
    </Div>
    <Div flex=1 />
  </Div>;

let make =
    (~channel: string, ~id: string, ~loggedInUser: Queries.User.t, _children) => {
  ...component,
  initialState: () => {selected: None},
  reducer: (action, _state) =>
    switch (action) {
    | UpdateSelected(str) => ReasonReact.Update({selected: Some(str)})
    },
  render: ({state, send}) => {
    let medium =
      Queries.Measurables.componentWithSeries(channel, id, measurables =>
        <SeriesShowTable
          measurables
          selected={state.selected}
          onClick={id => send(UpdateSelected(id))}
        />
      );

    let bottom =
      state.selected
      |> E.O.fmap(elId =>
           <C.Measurable.FullPresentation id=elId loggedInUser />
         )
      |> E.O.React.defaultNull;

    Queries.Series.component(~id)
    |> E.F.apply(series =>
         <>
           <SLayout.Header>
             {
               SLayout.seriesHead(
                 channel,
                 series
                 |> E.O.bind(_, (s: DataModel.Series.t) => s.name)
                 |> E.O.default(""),
               )
             }
           </SLayout.Header>
           <SLayout.MainSection>
             {series |> E.O.fmap(seriesHero) |> E.O.React.defaultNull}
             <div className=SeriesShowTableStyles.topPart> medium </div>
             bottom
           </SLayout.MainSection>
         </>
       );
  },
};