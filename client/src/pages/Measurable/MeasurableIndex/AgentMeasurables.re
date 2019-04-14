open Utils;
open Foretold__GraphQL;

let make' = (loggedInUser, measurables) =>
  <C.Measurables.BasicTable measurables showExtraData=true />;

module GetMeasurablesReducerConfig = {
  type itemType = Context.Primary.Measurable.t;
  type callFnParams = string;
  let getId = (e: Context.Primary.Measurable.t) => e.id;
  let callFn = (e: callFnParams) =>
    Foretold__GraphQL.Queries.Measurables.component4(~creatorId=e);
  let isEqual = (a: itemType, b: itemType) => a.id == b.id;
};

module SelectWithPaginationReducer =
  SelectWithPaginationReducerFunctor.Make(GetMeasurablesReducerConfig);

let component = ReasonReact.statelessComponent("AgentMeasurables");
let make =
    (
      ~id: string,
      ~loggedInUser: Context.Primary.User.t,
      ~layout=SLayout.FullPage.makeWithEl,
      _children,
    ) => {
  ...component,
  render: _ => {
    let lmake = SLayout.LayoutConfig.make;
    SelectWithPaginationReducer.make(
      ~itemsPerPage=20,
      ~callFnParams=id,
      ~subComponent=selectWithPaginationParams =>
      lmake(
        ~head=
          switch (selectWithPaginationParams.selection) {
          | Some(selection) =>
            <>
              {
                SelectWithPaginationReducer.Components.deselectButton(
                  selectWithPaginationParams.send,
                )
              }
              {
                SelectWithPaginationReducer.Components.correctButtonDuo(
                  selectWithPaginationParams,
                )
              }
            </>
          | None =>
            <>
              {
                SelectWithPaginationReducer.Components.correctButtonDuo(
                  selectWithPaginationParams,
                )
              }
            </>
          | _ => <div />
          },
        ~body=
          switch (
            selectWithPaginationParams.response,
            selectWithPaginationParams.selection,
          ) {
          | (_, Some(measurable)) =>
            <C.Measurable.FullPresentation id={measurable.id} loggedInUser />
          | (Success(measurables), None) =>
            <C.Measurables.BasicTable
              measurables
              showExtraData=true
              onSelect=(
                e =>
                  SelectWithPaginationReducer.Components.sendSelectItem(
                    selectWithPaginationParams,
                    e.id,
                  )
              )
            />
          | _ => <div />
          },
      )
      |> layout
    )
    |> E.React.makeToEl;
  },
};