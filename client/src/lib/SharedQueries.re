open Utils;
open Rationale.Result.Infix;

let withUserQuery =
    (
      auth0Id: option(string),
      innerComponentFn: 'a => ReasonReact.reactElement,
    ) =>
  switch (auth0Id) {
  | Some(auth) =>
    let query = Queries.GetUser.Query.make(~auth0Id=auth, ());
    Queries.GetUser.QueryComponent.make(
      ~variables=query##variables, ({result}) =>
      result
      |> apolloResponseToResult
      <$> (e => innerComponentFn(Some(e)))
      |> Rationale.Result.result(idd, idd)
    )
    |> ReasonReact.element;
  | None => innerComponentFn(None)
  };

let withLoggedInUserQuery = (innerComponentFn: 'a => ReasonReact.reactElement) => {
  let isLoggedIn = Auth0.isLoggedIn();
  isLoggedIn ?
    withUserQuery(Auth0.userId(), innerComponentFn) : innerComponentFn(None);
};