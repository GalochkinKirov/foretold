/* O for option */
open Rationale;
open Rationale.Function.Infix;

/* Utils */
module U = {
  let isEqual = (a, b) => a == b;
  let toA = a => [|a|];
  let id = e => e;
};

module O = {
  let dimap = (sFn, rFn, e) =>
    switch (e) {
    | Some(r) => sFn(r)
    | None => rFn()
    };
  ();
  let fmap = Rationale.Option.fmap;
  let bind = Rationale.Option.bind;
  let default = Rationale.Option.default;
  let isSome = Rationale.Option.isSome;
  let toExn = Option.toExn;

  module React = {
    let defaultNull = default(ReasonReact.null);
    let flatten = default(ReasonReact.null);
  };
};

/* R for Result */
module R = {
  let result = Rationale.Result.result;
  let id = e => e |> result(U.id, U.id);
  let fmap = Rationale.Result.fmap;
  let bind = Rationale.Result.bind;
};

let safe_a_of_string = (fn, s: string): option('a) =>
  try (Some(fn(s))) {
  | _ => None
  };

module S = {
  let toReact = ReasonReact.string;
  let safe_float = safe_a_of_string(float_of_string);
  let safe_int = safe_a_of_string(int_of_string);
};

module J = {
  let toString = Js.Json.decodeString ||> O.default("");
  let toMoment = toString ||> MomentRe.moment;
  module O = {
    let toMoment = O.fmap(toMoment);
  };
};

/* List */
module L = {
  let fmap = List.map;
  let concat = List.concat;
  let find = List.find;
  let filter = List.filter;
  let for_all = List.for_all;
  let exists = List.exists;
  let sort = List.sort;
  let filter_opt = RList.filter_opt;
  let uniqBy = RList.uniqBy;
  let join = RList.join;
  let head = RList.head;
  let uniq = RList.uniq;
  let flatten = List.flatten;
  let last = RList.last;
  let append = List.append;
  let getBy = Belt.List.getBy;
  let dropLast = RList.dropLast;
  let contains = RList.contains;
  let without = RList.without;
  let iter = List.iter;
  let findIndex = RList.findIndex;
};

/* A for Array */
module A = {
  let fmap = Array.map;
  let fmapi = Array.mapi;
  let to_list = Array.to_list;
  let of_list = Array.of_list;
  let length = Array.length;
  let empty = [||];
  let unsafe_get = Array.unsafe_get;
  let get = Belt.Array.get;
  let fold_left = Array.fold_left;
  let fold_right = Array.fold_right;
  let concatMany = Belt.Array.concatMany;
  let keepMap = Belt.Array.keepMap;
  let stableSortBy = Belt.SortArray.stableSortBy;
  /* TODO: Is there a better way of doing this? */

  /* TODO: Is -1 still the indicator that this is false (as is true with js findIndex)? Wasn't sure. */
  let findIndex = (e, i) =>
    Js.Array.findIndex(e, i)
    |> (
      r =>
        switch (r) {
        | (-1) => None
        | r => Some(r)
        }
    );
  let filter = (o, e) => Js.Array.filter(o, e);
  module Optional = {
    let concatSomes = (optionals: Js.Array.t(option('a))): Js.Array.t('a) =>
      optionals
      |> Js.Array.filter(Option.isSome)
      |> Js.Array.map(Option.toExn("Warning: This should not have happened"));
    let concatSome = (optionals: array(option('a))): array('a) =>
      optionals
      |> Js.Array.filter(Option.isSome)
      |> Js.Array.map(Option.toExn("Warning: This should not have happened"));
  };
};

module JsArray = {
  let concatSomes = (optionals: Js.Array.t(option('a))): Js.Array.t('a) =>
    optionals
    |> Js.Array.filter(Option.isSome)
    |> Js.Array.map(Option.toExn("Warning: This should not have happened"));
  let filter = Js.Array.filter;
};

module FloatCdf = {
  type t = {
    xs: array(float),
    ys: array(float),
  };
  let empty: t = {xs: [||], ys: [||]};

  let firstAbove = (min: float, t: Value.FloatCdf.t) =>
    Belt.Map.findFirstBy(t, (k, _v) => k > min);

  let firstAboveValue = (min: float, t: Value.FloatCdf.t) =>
    Rationale.Option.fmap(((_, x)) => x, firstAbove(min, t));
};

module React = {
  let el = ReasonReact.element;
  let null = ReasonReact.null;
  let makeToEl = (~key="", ~children=<div />, e) =>
    children |> e |> el(~key);
  let showIf = (cond, comp) => cond ? comp : ReasonReact.null;
};