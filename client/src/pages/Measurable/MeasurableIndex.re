open Utils;
open Rationale;
open Queries;
open HandsOnTable;
open MomentRe;

let toMeasurableLink = m => {
  let id = m.id;
  let name = m.name;
  {j|<a href="/measurables/$id">$name</a>|j};
};

let toCreatorLink = (c: Queries.creator) => {
  let id = c.id;
  let name = c.name;
  {j|<a href="/agents/$id">$name</a>|j};
};

let showQueryList = (~data, ~urlFn, ~render) => {
  let ddata =
    Array.map(
      e => {
        open Option.Infix;
        let creator = e.creator;
        let creatorName = creator <$> toCreatorLink |> Option.default("");
        Js.Dict.fromList([
          ("name", toMeasurableLink(e)),
          ("id", e.id),
          (
            "type",
            switch (e.valueType) {
            | `DATE => "Date"
            | `FLOAT => "Float"
            | `PERCENTAGE => "Percentage"
            },
          ),
          (
            "measurementCount",
            e.measurementCount |> Option.default(0) |> string_of_int,
          ),
          (
            "measurerCount",
            e.measurerCount |> Option.default(0) |> string_of_int,
          ),
          ("isLocked", e.isLocked |> (e => e ? "True" : "False")),
          ("createdAt", e.createdAt |> Moment.format("L")),
          (
            "expectedResolutionDate",
            e.expectedResolutionDate
            <$> Moment.format("L")
            |> Option.default(""),
          ),
          ("creator", creatorName),
        ]);
      },
      data,
    );

  let columns = [|
    makeColumn(~data="name", ~renderer="html", ()),
    makeColumn(~data="measurementCount", ()),
    makeColumn(~data="measurerCount", ()),
    makeColumn(~data="createdAt", ()),
    makeColumn(~data="creator", ~renderer="html", ()),
    makeColumn(~data="expectedResolutionDate", ()),
    makeColumn(~data="isLocked", ()),
  |];

  <UseRouterForLinks>
    <HandsOnTable
      data=ddata
      columns
      colHeaders=[|
        "Name",
        "Measurements",
        "Measurers",
        "Created At",
        "Creator / Judge",
        "Resolves At",
        "Is Locked",
      |]
    />
  </UseRouterForLinks>;
};

let itemList =
  showQueryList(
    ~urlFn=e => "/measurables/" ++ e##id,
    ~render=e => e##name |> ste,
  );

let component = ReasonReact.statelessComponent("Measurables");
let make = _children => {
  ...component,
  render: self =>
    Result.Infix.(
      Queries.GetMeasurablesQuery.make(o =>
        o.result
        |> apolloResponseToResult
        <$> (d => d##measurables)
        <$> Extensions.Array.concatSomes
        <$> (e => <div> (itemList(~data=e)) </div>)
        |> Result.result(idd, idd)
      )
      |> ReasonReact.element
      |> NormalLayout.make(~name="Measurables")
      |> ReasonReact.element
    ),
};