open Utils;
open Foretold__GraphQL;
open Rationale.Function.Infix;
open Rationale.Option.Infix;

module type Config = {
  type itemType;
  let isEqual: (itemType, itemType) => bool;
  let getId: itemType => string;
  type callFnParams = string;
  let callFn:
    (
      callFnParams,
      ~pageLimit: int,
      ~after: option(string),
      ~innerComponentFn: Client.E.HttpResponse.t(
                           Context.Primary.Connection.t(itemType),
                         ) =>
                         ReasonReact.reactElement
    ) =>
    ReasonReact.reactElement;
};

module Make = (Config: Config) => {
  module Config = Config;
  module Types = {
    type after = option(string);
    type pageConfig = {after};

    type itemSelected = {selectedIndex: int};

    type itemState =
      | ItemUnselected
      | ItemSelected(itemSelected);

    type connection = Context.Primary.Connection.t(Config.itemType);

    type response = Client.E.HttpResponse.t(connection);

    type action =
      | UpdateResponse(response)
      | NextPage
      | LastPage
      | Deselect
      | SelectIndex(int)
      | NextSelection
      | LastSelection;

    let reducerName = action =>
      switch (action) {
      | NextPage => "NextPage"
      | LastPage => "LastPage"
      | Deselect => "Deselect"
      | SelectIndex(int) => "SelectIndex"
      | NextSelection => "nextSelection"
      | LastSelection => "LastSelection"
      | _ => "Other"
      };

    type send = action => unit;

    type reducerParams = {
      itemsPerPage: int,
      itemState,
      response,
      selection: option(Config.itemType),
      send,
    };

    type state = {
      itemState,
      response,
      pageConfig,
    };
  };

  open Types;

  module Reducers = {
    module ItemSelected = {
      type t = itemSelected;

      let deselect = (_, t: t) => ItemUnselected;

      let nextSelection = (itemsPerPage, t: t) =>
        E.BoundedInt.increment(t.selectedIndex, itemsPerPage)
        <$> (selectedIndex => ItemSelected({selectedIndex: t.selectedIndex}));

      let lastSelection = (itemsPerPage, t: t) =>
        E.BoundedInt.decrement(t.selectedIndex, itemsPerPage)
        <$> (selectedIndex => ItemSelected({selectedIndex: selectedIndex}));

      let newState = (itemsPerPage, t: t, action: action) =>
        (
          switch (action) {
          | Deselect => Some(((a, b) => deselect(a, b) |> E.O.some))
          | NextSelection => Some(nextSelection)
          | LastSelection => Some(lastSelection)
          | _ => None
          }
        )
        |> (
          e =>
            switch (e) {
            | Some(fn) => fn(itemsPerPage, t)
            | None => None
            }
        );
    };

    module ItemUnselected = {
      let nextPage = (r: response) =>
        r
        |> E.HttpResponse.fmap((r: Context.Primary.Connection.t('a)) =>
             {after: r.pageInfo.endCursor}
           )
        |> E.HttpResponse.flatten(r => Some(r), _ => None, () => None)
        |> (
          e => {
            Js.log("HI");
            e;
          }
        )
        |> E.O.default({after: None});

      let lastPage = (r: response): pageConfig =>
        r
        |> E.HttpResponse.fmap((r: Context.Primary.Connection.t('a)) =>
             {after: r.pageInfo.startCursor}
           )
        |> E.HttpResponse.flatten(r => Some(r), _ => None, () => None)
        |> E.O.default({after: None});

      let selectIndex = (i, itemsPerPage) =>
        E.BoundedInt.make(i, itemsPerPage)
        <$> (s => ItemSelected({selectedIndex: s}));

      let newState = (itemsPerPage, t: response, action: action, pageConfig) =>
        switch (action) {
        | NextPage => Some((ItemUnselected, nextPage(t)))
        | LastPage => Some((ItemUnselected, lastPage(t)))
        | SelectIndex(i) =>
          selectIndex(i, itemsPerPage) |> E.O.fmap(r => (r, pageConfig))
        | _ => None
        };
    };

    module State = {
      type t = state;
      let selection = (t: t) =>
        switch (t.itemState, t.response) {
        | (ItemSelected({selectedIndex}), Success(m)) =>
          E.A.get(m.edges, selectedIndex)
        | _ => None
        };
    };

    module ReducerParams = {
      type t = reducerParams;

      let pageIndex = (t: t) =>
        switch (t.itemState) {
        | ItemSelected(r) => Some(r.selectedIndex)
        | ItemUnselected(_) => None
        };

      let canDecrementPage = (t: t) =>
        t.response
        |> E.HttpResponse.fmap((r: connection) => r.pageInfo.hasPreviousPage)
        |> E.HttpResponse.flatten(r => r, _ => false, () => false);

      let canIncrementPage = (t: t) =>
        t.response
        |> E.HttpResponse.fmap((r: connection) => r.pageInfo.hasNextPage)
        |> E.HttpResponse.flatten(r => r, _ => false, () => false);

      let itemExistsAtIndex = (t: t, index) =>
        switch (t.response) {
        | Success(r) => index < E.A.length(r.edges) && index >= 0
        | _ => false
        };

      let canDecrementSelection = (t: t) =>
        t
        |> pageIndex
        |> E.O.fmap(r => itemExistsAtIndex(t, r - 1))
        |> E.O.default(false);

      let canIncrementSelection = (t: t) =>
        t
        |> pageIndex
        |> E.O.fmap(r => itemExistsAtIndex(t, r + 1))
        |> E.O.default(false);
    };
  };

  module Components = {
    open Reducers.ReducerParams;

    let deselectButton = send =>
      SLayout.channelBack(~onClick=_ => send(Types.Deselect), ());

    let pageButton' = (facesRight: bool, action, canMove, params) =>
      <Antd.Button
        onClick={_ => params.send(action)} disabled={!canMove(params)}>
        <Icon.Icon icon={facesRight ? "ARROW_RIGHT" : "ARROW_LEFT"} />
      </Antd.Button>;

    type buttonType =
      | PageLast
      | PageNext
      | ItemLast
      | ItemNext;

    let pageButton' = (buttonType: buttonType) =>
      switch (buttonType) {
      | PageLast => pageButton'(false, Types.LastPage, canDecrementPage)
      | PageNext => pageButton'(true, Types.NextPage, canIncrementPage)
      | ItemLast =>
        pageButton'(false, Types.LastSelection, canDecrementSelection)
      | ItemNext =>
        pageButton'(true, Types.NextSelection, canIncrementSelection)
      };

    type buttonGroupType =
      | Page
      | Item;
    let buttonDuo = (buttonGroupType: buttonGroupType, params) =>
      switch (buttonGroupType) {
      | Page =>
        <>
          {pageButton'(PageLast, params)}
          {pageButton'(PageNext, params)}
        </>
      | Item =>
        <>
          {pageButton'(ItemLast, params)}
          {pageButton'(ItemNext, params)}
        </>
      };

    let correctButtonDuo = params =>
      switch (params.selection) {
      | Some(_) => buttonDuo(Item, params)
      | None => buttonDuo(Page, params)
      };

    let findIndexOfId = (t: Types.reducerParams, id: string) =>
      switch (t.response) {
      | Success(m) => m.edges |> E.A.findIndex(r => Config.getId(r) == id)
      | _ => None
      };

    let selectItemAction = (t: t, id) =>
      findIndexOfId(t, id) |> E.O.fmap(e => Types.SelectIndex(e));

    let sendSelectItem = (t: t, id) =>
      selectItemAction(t, id) |> E.O.fmap(t.send) |> E.O.default();
  };

  let component = ReasonReact.reducerComponent("SelectWithPaginationReducer");

  let compareItems =
      (
        a: Context.Primary.Connection.t('a),
        b: Context.Primary.Connection.t('a),
      ) =>
    Belt.Array.eq(a.edges, b.edges, Config.isEqual);

  let make =
      (
        ~itemsPerPage=20,
        ~callFnParams: Config.callFnParams,
        ~subComponent,
        _children,
      ) => {
    ...component,
    initialState: () => {
      itemState: ItemUnselected,
      response: Loading,
      pageConfig: {
        after: None,
      },
    },
    reducer: (action, state: state) =>
      switch (action) {
      | UpdateResponse(response) =>
        {response, itemState: state.itemState, pageConfig: state.pageConfig}
        ->ReasonReact.Update
      | _ =>
        let newState =
          switch (state, action) {
          | ({itemState: ItemUnselected}, _) =>
            Reducers.ItemUnselected.newState(
              itemsPerPage,
              state.response,
              action,
              state.pageConfig,
            )
          | ({itemState: ItemSelected(s)}, _) =>
            Reducers.ItemSelected.newState(itemsPerPage, s, action)
            |> E.O.fmap((r: Types.itemState) => (r, state.pageConfig))
          };
        switch (newState) {
        | Some((itemState, pageConfig)) =>
          {response: state.response, itemState, pageConfig}
          ->ReasonReact.Update
        | None => ReasonReact.NoUpdate
        };
      },
    render: ({state, send}) =>
      callFnParams
      |> Config.callFn(
           ~after=state.pageConfig.after,
           ~pageLimit=itemsPerPage,
           ~innerComponentFn=response => {
             if (!E.HttpResponse.isEq(state.response, response, compareItems)) {
               send(UpdateResponse(response));
             };
             subComponent({
               itemsPerPage,
               itemState: state.itemState,
               response: state.response,
               selection: Reducers.State.selection(state),
               send,
             });
           },
         ),
  };
};