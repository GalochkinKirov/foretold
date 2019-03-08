open Utils;
open Rationale;
open MomentRe;
open Antd;

let formatDate = Moment.format("MMM DD, YYYY HH:MM:SS");
let withQuery = (~id, fn) => {
  let query = Queries.GetMeasurable.Query.make(~id, ());
  Result.Infix.(
    Queries.GetMeasurable.QueryComponent.make(
      ~variables=query##variables, ({result}) =>
      result
      |> ApolloUtils.apolloResponseToResult
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
             mutation editMeasurable($id: String!, $name: String!, $description: String!, $expectedResolutionDate:Date, $resolutionEndpoint: String!, $descriptionEntity: String!,$descriptionProperty: String, $descriptionDate: Date) {
                 editMeasurable(id: $id, name: $name, description: $description, expectedResolutionDate: $expectedResolutionDate, resolutionEndpoint: $resolutionEndpoint, descriptionEntity: $descriptionEntity,descriptionProperty: $descriptionProperty, descriptionDate: $descriptionDate) {
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
        description: string,
        expectedResolutionDate: string,
        resolutionEndpoint: string,
        descriptionEntity: string,
        descriptionDate: string,
        showDescriptionDate: string,
        descriptionProperty: string,
      ) => {
    let m =
      showDescriptionDate == "TRUE" ?
        GraphQL.make(
          ~id,
          ~name,
          ~description,
          ~descriptionProperty,
          ~descriptionDate=descriptionDate |> Js.Json.string,
          ~expectedResolutionDate=expectedResolutionDate |> Js.Json.string,
          ~resolutionEndpoint,
          ~descriptionEntity,
          (),
        ) :
        GraphQL.make(
          ~id,
          ~name,
          ~description,
          ~descriptionProperty,
          ~descriptionDate="" |> Js.Json.string,
          ~expectedResolutionDate=expectedResolutionDate |> Js.Json.string,
          ~resolutionEndpoint,
          ~descriptionEntity,
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
    descriptionEntity: string,
    descriptionDate: string,
    descriptionProperty: string,
    expectedResolutionDate: string,
    resolutionEndpoint: string,
    showDescriptionDate: string,
    showDescriptionProperty: string,
  };
  type fields = [
    | `name
    | `description
    | `descriptionEntity
    | `descriptionProperty
    | `descriptionDate
    | `expectedResolutionDate
    | `resolutionEndpoint
    | `showDescriptionDate
    | `showDescriptionProperty
  ];
  let lens = [
    (`name, s => s.name, (s, name) => {...s, name}),
    (
      `description,
      s => s.description,
      (s, description) => {...s, description},
    ),
    (
      `descriptionEntity,
      s => s.descriptionEntity,
      (s, descriptionEntity) => {...s, descriptionEntity},
    ),
    (
      `descriptionProperty,
      s => s.descriptionProperty,
      (s, descriptionProperty) => {...s, descriptionProperty},
    ),
    (
      `showDescriptionDate,
      s => s.showDescriptionDate,
      (s, showDescriptionDate) => {...s, showDescriptionDate},
    ),
    (
      `showDescriptionProperty,
      s => s.showDescriptionProperty,
      (s, showDescriptionProperty) => {...s, showDescriptionProperty},
    ),
    (
      `descriptionDate,
      s => s.descriptionDate,
      (s, descriptionDate) => {...s, descriptionDate},
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
    <h2> {"Create a new Measurable" |> ste} </h2>
    <Form>
      <Form.Item>
        <h3> {"Relevant Entity (optional)" |> ste} </h3>
        <Input
          value={form.values.descriptionEntity}
          onChange={
            ReForm.Helpers.handleDomFormChange(
              handleChange(`descriptionEntity),
            )
          }
        />
      </Form.Item>
      <Form.Item>
        <h3> {"Property Type" |> ste} </h3>
        <Antd.Radio.Group
          value={form.values.showDescriptionProperty}
          defaultValue={form.values.showDescriptionProperty}
          onChange={
            ReForm.Helpers.handleDomFormChange(
              handleChange(`showDescriptionProperty),
            )
          }>
          <Antd.Radio value="FALSE"> {"Custom Name" |> ste} </Antd.Radio>
          <Antd.Radio value="TRUE"> {"Property Entity" |> ste} </Antd.Radio>
        </Antd.Radio.Group>
      </Form.Item>
      {
        form.values.showDescriptionProperty == "TRUE" ?
          <Form.Item>
            <h3> {"Property Entity Name" |> ste} </h3>
            <Input
              value={form.values.descriptionProperty}
              onChange={
                ReForm.Helpers.handleDomFormChange(
                  handleChange(`descriptionProperty),
                )
              }
            />
          </Form.Item> :
          <div />
      }
      {
        form.values.showDescriptionProperty == "FALSE" ?
          <Form.Item>
            <h3> {"Custom Name" |> ste} </h3>
            <Input
              value={form.values.name}
              onChange={
                ReForm.Helpers.handleDomFormChange(handleChange(`name))
              }
            />
          </Form.Item> :
          <div />
      }
      <Form.Item>
        <h3> {"Include a Specific Date in Name" |> ste} </h3>
        <AntdSwitch
          checked={form.values.showDescriptionDate == "TRUE"}
          onChange={
            e => handleChange(`showDescriptionDate, e ? "TRUE" : "FALSE")
          }
        />
      </Form.Item>
      {
        form.values.showDescriptionDate == "TRUE" ?
          <Form.Item>
            <h3> {"'On' Date" |> ste} </h3>
            <DatePicker
              value={
                form.values.descriptionDate |> MomentRe.momentDefaultFormat
              }
              onChange={e => handleChange(`descriptionDate, e |> formatDate)}
            />
          </Form.Item> :
          <div />
      }
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
                        values.description,
                        values.expectedResolutionDate,
                        values.resolutionEndpoint,
                        values.descriptionEntity,
                        values.descriptionDate,
                        values.showDescriptionDate,
                        values.descriptionProperty,
                      ),
                  ~initialState={
                    name: measurable.name,
                    descriptionDate:
                      measurable.descriptionDate
                      |> Option.default(MomentRe.momentNow())
                      |> formatDate,
                    showDescriptionDate:
                      measurable.descriptionDate
                      |> Rationale.Option.isSome
                      |> (e => e ? "TRUE" : "FALSE"),
                    descriptionEntity:
                      measurable.descriptionEntity |> Option.default(""),
                    description: measurable.description |> Option.default(""),
                    expectedResolutionDate:
                      measurable.expectedResolutionDate
                      |> Option.default(MomentRe.momentNow())
                      |> formatDate,
                    resolutionEndpoint:
                      measurable.resolutionEndpoint |> Option.default(""),
                    showDescriptionProperty:
                      measurable.name == "" ? "TRUE" : "FALSE",
                    descriptionProperty:
                      measurable.descriptionProperty |> Option.default(""),
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