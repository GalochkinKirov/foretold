open Utils;
open Rationale;
open Queries;
open MomentRe;

let toMoment = Function.Infix.(jsonToString ||> moment);

let toOptionalMoment: option(Js.Json.t) => MomentRe.Moment.t =
  Option.Infix.(
    e => e <$> jsonToString <$> (r => r) |> Option.default("") |> moment
  );

module GetMeasurable = {
  module Query = [%graphql
    {|
      query getMeasurable ($id: String!) {
          measurable:
            measurable(id: $id){
              id
              name
              valueType
              isLocked
              createdAt @bsDecoder(fn: "toMoment")
              measurements: Measurements{
                id
                createdAt @bsDecoder(fn: "toMoment")
                value @bsDecoder(fn: "Value.decode")
                competitorType
                taggedMeasurementId
                relevantAt @bsDecoder(fn: "toOptionalMoment")
                agent: Agent {
                  id
                  user: User {
                    id
                    name
                  }
                  bot: Bot {
                    id
                    name
                    competitorType
                  }
                }
              }
          }
      }
    |}
  ];

  module QueryComponent = ReasonApollo.CreateQuery(Query);

  let withQuery = (~id, fn) => {
    let query = Query.make(~id, ());
    Result.Infix.(
      QueryComponent.make(~variables=query##variables, ({result}) =>
        result
        |> apolloResponseToResult
        >>= (
          e =>
            e##measurable
            |> filterOptionalResult("Measurable not found" |> ste)
        )
        <$> fn
        |> Result.result(idd, idd)
      )
      |> ReasonReact.element
    );
  };
};

let component = ReasonReact.statelessComponent("Measurable");

let valueString = e =>
  switch (e) {
  | `FLOAT => "Float"
  | `DATE => "Date"
  | `PERCENTAGE => "Percentage"
  };

module Styles = {
  open Css;
  let sidebar =
    style([float(`left), left(px(0)), backgroundColor(hex("eee"))]);

  let body = style([marginLeft(px(200)), padding(px(30))]);
};

let make = (~id: string, _children) => {
  ...component,
  render: _self =>
    <div>
      <div>
        (
          GetMeasurable.withQuery(~id, measurable =>
            <div>
              <h1> (measurable##name |> ste) </h1>
              <div>
                <h2> ("Aggregate" |> ste) </h2>
                <Style.BorderedBox>
                  <MeasurableChart measurements=measurable##measurements />
                </Style.BorderedBox>
                <h2> ("Add a Measurement" |> ste) </h2>
                <MeasurableShowForm measurableId=id />
                <h2> ("Previous Measurements" |> ste) </h2>
                <MeasurableTable measurements=measurable##measurements />
              </div>
            </div>
          )
        )
      </div>
    </div>,
};