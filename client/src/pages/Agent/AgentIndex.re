open Utils;
open Rationale;
open Rationale.Option;
open Result.Infix;
open Queries;

let toAgentLink = (id, name) => {j|<a href="/agents/$id">$name</a>|j};

let component = ReasonReact.statelessComponent("Measurables");
let make = _children => {
  ...component,
  render: _ =>
    <div>
      {
        Queries.GetAgents.QueryComponent.make(({result}) =>
          result
          |> ApolloUtils.apolloResponseToResult
          <$> (d => d##agents)
          <$> (e => <div />)
          |> Result.result(idd, idd)
        )
        |> ReasonReact.element
      }
    </div>,
};