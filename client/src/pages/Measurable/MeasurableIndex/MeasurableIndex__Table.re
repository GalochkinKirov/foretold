open Utils;
open Rationale;
open Queries;
open HandsOnTable;
open MeasurableColumns;
open Table;

let component = ReasonReact.statelessComponent("MeasurableIndexTable");

let make = (~measurables: array(DataModel.measurable), _children) => {
  ...component,
  render: _self => {
    let _measurables = MeasurableTableStyles.sortMeasurables(measurables);
    <UseRouterForLinks>
      <div className=PrimaryTableStyles.group>
        {
          _measurables
          |> Array.map(m =>
               <div className=PrimaryTableStyles.row>
                 <div className=PrimaryTableStyles.mainColumn>
                   <div className=PrimaryTableStyles.mainColumnTop>
                     {MeasurableTableStyles.link(~m)}
                   </div>
                   <div className=PrimaryTableStyles.mainColumnBottom>
                     {MeasurableTableStyles.creatorLink(~m)}
                     {MeasurableTableStyles.measurements(~m)}
                     {MeasurableTableStyles.measurers(~m)}
                   </div>
                 </div>
                 <div className=PrimaryTableStyles.rightColumn>
                   {MeasurableTableStyles.dateStatus(~measurable=m)}
                 </div>
               </div>
             )
          |> ReasonReact.array
        }
      </div>
    </UseRouterForLinks>;
  },
};