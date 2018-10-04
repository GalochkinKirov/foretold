open Utils;
open Rationale;
open Rationale.Option.Infix;
open Rationale.Function.Infix;
open Result.Infix;
open Queries;
open HandsOnTable;
open MomentRe;
open MeasurableTypes;
let component = ReasonReact.statelessComponent("MeasurableChart");

let toUnix = x => x##createdAt |> Moment.toUnix;

module Styles = {
  open Css;
  let plot = style([maxWidth(px(500))]);
};

let onlyWithFloatCdf =
  filterAndFold((e, fnYes, fnNo) =>
    switch (e##value) {
    | Belt.Result.Ok(`FloatCdf(r)) => fnYes(e)
    | _ => fnNo()
    }
  );

let firstA = (_, n) => Some(n);

let make = (~measurements: MeasurableTypes.measurements, _children) => {
  ...component,
  render: _ => {
    let sorted =
      measurements
      |> Extensions.Array.concatSomes
      |> Js.Array.filter(e => e##value |> Belt.Result.isOk)
      |> Js_array.sortInPlaceWith((a, b) =>
           toUnix(b) > toUnix(a) ? (-1) : 1
         );

    let toChartMeasurement =
        (m: MeasurableTypes.measurement)
        : option(TimeCdfChart.measurement) =>
      switch (m##value) {
      | Belt.Result.Ok(`FloatCdf(r)) =>
        switch (
          FloatCdf_F.firstAboveValue(0.05, r),
          FloatCdf_F.firstAboveValue(0.50, r),
          FloatCdf_F.firstAboveValue(0.95, r),
        ) {
        | (Some(low), Some(median), Some(high)) =>
          Some({
            createdAt: m##createdAt,
            competitorType: m##competitorType,
            low,
            median,
            high,
          })
        | _ => None
        }
      | _ => None
      };

    let values: array(TimeCdfChart.measurement) =
      sorted
      |> onlyWithFloatCdf
      |> Array.map(toChartMeasurement)
      |> Extensions.Array.concatSomes;

    <div className=Styles.plot> <TimeCdfChart measurements=values /> </div>;
  },
};