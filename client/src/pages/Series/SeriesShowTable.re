open Utils;
open Rationale;
open Queries;
open HandsOnTable;
open MeasurableColumns;
open Table;

let component = ReasonReact.statelessComponent("MeasurableIndexTable");

let make =
    (
      ~measurables: array(DataModel.measurable),
      ~showExtraData: bool,
      _children,
    ) => {
  ...component,
  render: _self => {
    let _measurables =
      MeasurableTableStyles.sortMeasurables(measurables)
      |> Js.Array.filter((e: DataModel.measurable) =>
           PrimaryTableBase.status(e) != ARCHIVED
         );
    <div className=PrimaryTableStyles.group>
      {
        _measurables
        |> Js_array.sortInPlaceWith(
             (a: DataModel.measurable, b: DataModel.measurable) =>
             switch (a.descriptionEntity, b.descriptionEntity) {
             | (Some(aa), Some(bb)) => bb > aa ? (-1) : 1
             | _ => 1
             }
           )
        |> Array.map((m: DataModel.measurable) =>
             <div
               className={SeriesShowTableStyles.row(m)}
               onClick={
                 _e =>
                   ReasonReact.Router.push(
                     "/c/"
                     ++ (m.channel |> Option.default("general"))
                     ++ "/m/"
                     ++ m.id,
                   )
               }>
               <div className=SeriesShowTableStyles.column>
                 {
                   MeasurableTableStyles.nameEntityLink(
                     ~m,
                     ~className=PrimaryTableStyles.itemLink,
                   )
                   |> Option.default(ReasonReact.null)
                 }
               </div>
               <div className=SeriesShowTableStyles.column>
                 {
                   MeasurableTableStyles.propertyEntityLink(
                     ~m,
                     ~className=PrimaryTableStyles.propertyLink,
                   )
                   |> Option.default(ReasonReact.null)
                 }
               </div>
               <div className=SeriesShowTableStyles.column>
                 {
                   switch (MeasurableTableStyles.formatDate(m.descriptionDate)) {
                   | "" => ReasonReact.null
                   | e =>
                     <span className=PrimaryTableStyles.calDateO>
                       {e |> ste}
                     </span>
                   }
                 }
               </div>
               <div className=SeriesShowTableStyles.column>
                 {MeasurableTableStyles.measurements(~m)}
                 {MeasurableTableStyles.measurers(~m)}
               </div>
               <div className=SeriesShowTableStyles.column>
                 MeasurableTableStyles.(
                   MeasurableTableStyles.dateStatusI(
                     ~measurable=m,
                     ~dateDisplay=TOP,
                   )
                 )
               </div>
               <div className=SeriesShowTableStyles.column>
                 MeasurableTableStyles.(
                   MeasurableTableStyles.dateStatusI(
                     ~measurable=m,
                     ~dateDisplay=BOTTOM,
                   )
                 )
               </div>
             </div>
           )
        |> ReasonReact.array
      }
    </div>;
  },
};