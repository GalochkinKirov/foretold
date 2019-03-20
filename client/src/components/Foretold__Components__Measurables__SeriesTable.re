open Utils;
open Rationale;
open DataModel;

let component = ReasonReact.statelessComponent("MeasurablesSeriesTable");

module MeasurableItems = Foretold__Components__Measurable__Items;
module Styles = {
  open Css;

  let row = selected =>
    style([
      width(`percent(100.0)),
      borderBottom(`px(1), `solid, hex("eee")),
      display(`flex),
      flexDirection(`row),
      paddingLeft(`px(8)),
      paddingRight(`px(8)),
      paddingTop(`px(8)),
      paddingBottom(`px(7)),
      cursor(`pointer),
      backgroundColor(`hex(selected ? "dce4ef" : "fff")),
      selector(":last-child", [borderBottom(`px(0), `solid, hex("eee"))]),
      selector(":hover", [backgroundColor(`hex("eef0f3"))]),
    ]);

  let column = style([flex(1)]);
};

let make =
    (
      ~measurables: array(Measurable.t),
      ~selected: option(string),
      ~onClick,
      _children,
    ) => {
  ...component,
  render: _self => {
    let _measurables =
      Measurable.stableSort(measurables)
      |> E.JsArray.filter((e: Measurable.t) =>
           Measurable.toStatus(e) != ARCHIVED
         );
    <div className=PrimaryTableStyles.group>
      {
        _measurables
        |> Js_array.sortInPlaceWith((a: Measurable.t, b: Measurable.t) =>
             switch (a.descriptionEntity, b.descriptionEntity) {
             | (Some(aa), Some(bb)) => bb > aa ? (-1) : 1
             | _ => 1
             }
           )
        |> Array.map((m: Measurable.t) =>
             <div
               className={Styles.row(Some(m.id) == selected)}
               onClick={_e => onClick(m.id)}>
               <div className=Styles.column>
                 {
                   MeasurableItems.MeasurableEntityLinks.nameEntityLink(
                     ~m,
                     ~className=PrimaryTableStyles.itemLink,
                   )
                   |> E.O.React.defaultNull
                 }
               </div>
               <div className=Styles.column>
                 {
                   MeasurableItems.MeasurableEntityLinks.propertyEntityLink(
                     ~m,
                     ~className=PrimaryTableStyles.propertyLink,
                   )
                   |> E.O.React.defaultNull
                 }
               </div>
               <div className=Styles.column>
                 {
                   switch (MeasurableItems.formatDate(m.descriptionDate)) {
                   | "" => E.React.null
                   | e =>
                     <span className=PrimaryTableStyles.calDateO>
                       {e |> ste}
                     </span>
                   }
                 }
               </div>
               <div className=Styles.column>
                 {MeasurableItems.measurements(~m)}
                 {MeasurableItems.measurers(~m)}
               </div>
               <div className=Styles.column>
                 <Foretold__Components__Measurable.StatusDisplay
                   measurable=m
                   dateDisplay=TOP
                 />
               </div>
               <div className=Styles.column>
                 <Foretold__Components__Measurable.StatusDisplay
                   measurable=m
                   dateDisplay=BOTTOM
                 />
               </div>
             </div>
           )
        |> ReasonReact.array
      }
    </div>;
  },
};