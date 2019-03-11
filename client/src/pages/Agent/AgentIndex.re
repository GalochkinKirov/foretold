open Utils;
open Rationale;
open Result.Infix;

let toAgentLink = (id, name) => {j|<a href="/agents/$id">$name</a>|j};

let component = ReasonReact.statelessComponent("Measurables");
let make = _children => {
  ...component,
  render: _ =>
    <div>
      {
        GetAgents.QueryComponent.make(({result}) =>
          result
          |> ApolloUtils.apolloResponseToResult
          <$> (d => d##agents)
          <$> (_ => <div />)
          |> Result.result(idd, idd)
        )
        |> ReasonReact.element
      }
    </div>,
};