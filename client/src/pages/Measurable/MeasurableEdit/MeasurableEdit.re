open Utils;
open Rationale;
open Queries;
open MomentRe;
open Antd;

let formatDate = Moment.format("MMM DD, YYYY HH:MM:SS");
let withQuery = (~id, fn) => {
  let query = Queries.GetMeasurable.Query.make(~id, ());
  Result.Infix.(
    Queries.GetMeasurable.QueryComponent.make(
      ~variables=query##variables, ({result}) =>
      result
      |> apolloResponseToResult
      >>= (
        e =>
          e##measurable |> filterOptionalResult("Measurable not found" |> ste)
      )
      <$> fn
      |> Result.result(idd, idd)
    )
    |> ReasonReact.element
  );
};

module WithEditMutation = {
  module GraphQL = [%graphql
    {|
             mutation editMeasurable($id: String!, $name: String!, $description: String!, $isLocked: Boolean!,$expectedResolutionDate:Date, $resolutionEndpoint: String!) {
                 editMeasurable(id: $id, name: $name, description: $description, isLocked: $isLocked, expectedResolutionDate: $expectedResolutionDate, resolutionEndpoint: $resolutionEndpoint) {
                   id
                 }
             }
     |}
  ];

  module Mutation = ReasonApollo.CreateMutation(GraphQL);

  let mutate =
      (
        mutation: Mutation.apolloMutation,
        id: string,
        name: string,
        isLocked: string,
        description: string,
        expectedResolutionDate: string,
        resolutionEndpoint: string,
      ) => {
    let m =
      GraphQL.make(
        ~id,
        ~name,
        ~isLocked=isLocked == "true" ? true : false,
        ~description,
        ~expectedResolutionDate=expectedResolutionDate |> Js.Json.string,
        ~resolutionEndpoint,
        (),
      );
    mutation(~variables=m##variables, ~refetchQueries=[|"getAgent"|], ())
    |> ignore;
  };
};

module SignUpParams = {
  type state = {
    name: string,
    description: string,
    expectedResolutionDate: string,
    resolutionEndpoint: string,
    isLocked: string,
  };
  type fields = [
    | `name
    | `description
    | `isLocked
    | `expectedResolutionDate
    | `resolutionEndpoint
  ];
  let lens = [
    (`name, s => s.name, (s, name) => {...s, name}),
    (`isLocked, s => s.isLocked, (s, isLocked) => {...s, isLocked}),
    (
      `description,
      s => s.description,
      (s, description) => {...s, description},
    ),
    (
      `expectedResolutionDate,
      s => s.expectedResolutionDate,
      (s, expectedResolutionDate) => {...s, expectedResolutionDate},
    ),
    (
      `resolutionEndpoint,
      s => s.resolutionEndpoint,
      (s, resolutionEndpoint) => {...s, resolutionEndpoint},
    ),
  ];
};

module SignUpForm = ReForm.Create(SignUpParams);

let showForm = (~form: SignUpForm.state, ~handleSubmit, ~handleChange) =>
  <form onSubmit={ReForm.Helpers.handleDomFormSubmit(handleSubmit)}>
    <Form>
      <Form.Item>
        <h3> {"Name" |> ste} </h3>
        <Input
          value={form.values.name}
          onChange={ReForm.Helpers.handleDomFormChange(handleChange(`name))}
        />
      </Form.Item>
      <Form.Item>
        <h3> {"Description" |> ste} </h3>
        <Input
          value={form.values.description}
          onChange={
            ReForm.Helpers.handleDomFormChange(handleChange(`description))
          }
        />
      </Form.Item>
      <Form.Item>
        <h3> {"Locked?" |> ste} </h3>
        <p>
          {
            "Lock this measurable if you do not want others to add measurements to it."
            |> ste
          }
        </p>
        <AntdSwitch
          checked={form.values.isLocked == "true" ? true : false}
          onChange={e => handleChange(`isLocked, e ? "true" : "false")}
        />
      </Form.Item>
      <Form.Item>
        <h3> {"Resolution Endpoint (Optional)" |> ste} </h3>
        <p>
          {
            "If you enter an url that returns a number, this will be called when the resolution date occurs, and entered as a judgement value."
            |> ste
          }
        </p>
        <Input
          value={form.values.resolutionEndpoint}
          onChange={
            ReForm.Helpers.handleDomFormChange(
              handleChange(`resolutionEndpoint),
            )
          }
        />
      </Form.Item>
      <Form.Item>
        <h3> {"Expected Resolution Date" |> ste} </h3>
        <DatePicker
          value={
            form.values.expectedResolutionDate |> MomentRe.momentDefaultFormat
          }
          onChange={
            e => handleChange(`expectedResolutionDate, e |> formatDate)
          }
        />
      </Form.Item>
      <Form.Item>
        <Button _type=`primary onClick={_ => handleSubmit()}>
          {"Submit" |> ste}
        </Button>
      </Form.Item>
    </Form>
  </form>;

let component = ReasonReact.statelessComponent("MeasurableEdit");
let make = (~id: string, _children) => {
  ...component,
  render: _self =>
    <div>
      <div>
        {
          withQuery(
            ~id,
            m => {
              let measurable = Queries.GetMeasurable.toMeasurable(m);
              WithEditMutation.Mutation.make((mutation, data) =>
                SignUpForm.make(
                  ~onSubmit=
                    ({values}) =>
                      WithEditMutation.mutate(
                        mutation,
                        id,
                        values.name,
                        values.isLocked,
                        values.description,
                        values.expectedResolutionDate,
                        values.resolutionEndpoint,
                      ),
                  ~initialState={
                    name: measurable.name,
                    description: measurable.description |> Option.default(""),
                    expectedResolutionDate:
                      measurable.expectedResolutionDate
                      |> Option.default(MomentRe.momentNow())
                      |> formatDate,
                    resolutionEndpoint:
                      measurable.resolutionEndpoint |> Option.default(""),
                    isLocked: measurable.isLocked ? "true" : "false",
                  },
                  ~schema=[(`name, Custom(_ => None))],
                  ({handleSubmit, handleChange, form, _}) =>
                    <div>
                      <h2> {"Edit Measurable" |> ste} </h2>
                      {
                        switch (data.result) {
                        | Loading => <div> {"Loading" |> ste} </div>
                        | Error(e) =>
                          <div>
                            {"Error: " ++ e##message |> ste}
                            {showForm(~form, ~handleSubmit, ~handleChange)}
                          </div>
                        | Data(_) =>
                          <div>
                            <h3>
                              {"Measurable successfully updated." |> ste}
                            </h3>
                            <div>
                              {showForm(~form, ~handleSubmit, ~handleChange)}
                            </div>
                          </div>
                        | NotCalled =>
                          showForm(~form, ~handleSubmit, ~handleChange)
                        }
                      }
                    </div>,
                )
                |> ReasonReact.element
              )
              |> ReasonReact.element;
            },
          )
        }
      </div>
    </div>,
};