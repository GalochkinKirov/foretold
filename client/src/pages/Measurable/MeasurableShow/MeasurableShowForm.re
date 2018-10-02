open Utils;
open Rationale;
open Rationale.Option.Infix;
open Rationale.Function.Infix;
open Antd;

let ste = ReasonReact.string;

module CreateMeasurement = [%graphql
  {|
            mutation createMeasurement($value: SequelizeJSON!, $competitorType:competitorType!, $measurableId:String!, $agentId:String!) {
                createMeasurement(value: $value, competitorType: $competitorType, measurableId:$measurableId, agentId:$agentId) {
                  createdAt
                }
            }
    |}
];

module CreateMeasurementMutation =
  ReasonApollo.CreateMutation(CreateMeasurement);

module SignUpParams = {
  type state = {
    p25: string,
    p50: string,
    p75: string,
    pointFloat: string,
    percentage: string,
    binary: string,
    dataType: string,
  };
  type fields = [
    | `p25
    | `p50
    | `p75
    | `pointFloat
    | `percentage
    | `binary
    | `dataType
  ];
  let lens = [
    (`p25, s => s.p25, (s, p25) => {...s, p25}),
    (`p50, s => s.p50, (s, p50) => {...s, p50}),
    (`p75, s => s.p75, (s, p75) => {...s, p75}),
    (`pointFloat, s => s.pointFloat, (s, pointFloat) => {...s, pointFloat}),
    (`percentage, s => s.percentage, (s, percentage) => {...s, percentage}),
    (`binary, s => s.binary, (s, binary) => {...s, binary}),
    (`dataType, s => s.dataType, (s, dataType) => {...s, dataType}),
  ];
};

let initialState: SignUpParams.state = {
  dataType: "FloatPercentiles",
  p25: "",
  p50: "",
  p75: "",
  pointFloat: "",
  percentage: "",
  binary: "",
};

module SignUpForm = ReForm.Create(SignUpParams);

let safe_a_of_string = (fn, s: string) : option('a) =>
  try (Some(fn(s))) {
  | _ => None
  };

let safe_float_of_string = safe_a_of_string(float_of_string);
let safe_int_of_string = safe_a_of_string(int_of_string);

let keepIfValid = n => Value.isValid(n) ? Some(n) : None;
let mutate =
    (
      mutation: CreateMeasurementMutation.apolloMutation,
      measurableId: string,
      values: SignUpForm.values,
    ) => {
  let value =
    switch (values.dataType) {
    | "FloatPercentiles" =>
      (
        switch (
          safe_float_of_string(values.p25),
          safe_float_of_string(values.p50),
          safe_float_of_string(values.p75),
        ) {
        | (Some(a), Some(b), Some(c)) =>
          Some(
            `FloatCdf(
              Value.FloatCdf.fromArray([|
                (25.0, a),
                (50.0, b),
                (75.0, c),
              |]),
            ),
          )
        | (_, _, _) => None
        }
      )
      >>= keepIfValid
    | "FloatPoint" =>
      values.pointFloat
      |> safe_float_of_string
      <$> (n => `FloatPoint(n))
      >>= keepIfValid
    | "Percentage" =>
      values.percentage
      |> safe_float_of_string
      <$> (n => `Percentage(n))
      >>= keepIfValid
    | "Binary" =>
      values.binary
      |> safe_int_of_string
      <$> (n => `Binary(n))
      >>= keepIfValid
    | _ => None
    };
  switch (value) {
  | Some(v) =>
    let m =
      CreateMeasurement.make(
        ~measurableId,
        ~agentId="c4aefed8-83c1-422d-9364-313071287758",
        ~value=v |> Value.encode,
        ~competitorType=`COMPETITIVE,
        (),
      );
    mutation(
      ~variables=m##variables,
      ~refetchQueries=[|"getMeasurable"|],
      (),
    )
    |> ignore;
  | None => ()
  };
};

let valueList =
  Array.mapi(
    (i, e) =>
      <Select.Option value=e key=(string_of_int(i))>
        (e |> ste)
      </Select.Option>,
    [|"FloatPercentiles", "FloatPoint", "Percentage", "Binary"|],
  );

let component = ReasonReact.statelessComponent("Measurables");
let errorOfFloat = (fn1, fn2, e) => {
  let asNumber = fn1(e);
  switch (asNumber) {
  | None => Some("Couldn't parse Number")
  | Some(r) => Value.error(fn2(r))
  };
};

let input = (handleChange, value, b, key) =>
  <Form.Item key>
    <Input
      value
      onChange=(ReForm.Helpers.handleDomFormChange(handleChange(b)))
    />
  </Form.Item>;

let make = (~measurableId: string, _children) => {
  ...component,
  render: _ =>
    CreateMeasurementMutation.make(
      ~onCompleted=e => Js.log("Request submitted"),
      (mutation, _) =>
        SignUpForm.make(
          ~onSubmit=({values}) => mutate(mutation, measurableId, values),
          ~initialState,
          ~schema=[],
          ({handleSubmit, handleChange, form, _}) =>
            <form onSubmit=(ReForm.Helpers.handleDomFormSubmit(handleSubmit))>
              <h2> ("Create a new Measurement" |> ste) </h2>
              <Form.Item key="ee">
                <Select
                  value=form.values.dataType
                  onChange=(e => handleChange(`dataType, e) |> ignore)>
                  valueList
                </Select>
              </Form.Item>
              (
                form.values.dataType != "FloatPercentiles" ?
                  <div /> :
                  <div>
                    (input(handleChange, form.values.p25, `p25, "1"))
                    (input(handleChange, form.values.p50, `p50, "2"))
                    (input(handleChange, form.values.p75, `p75, "3"))
                  </div>
              )
              (
                form.values.dataType != "FloatPoint" ?
                  <div /> :
                  input(
                    handleChange,
                    form.values.pointFloat,
                    `pointFloat,
                    "4",
                  )
              )
              (
                form.values.dataType != "Percentage" ?
                  <div /> :
                  input(
                    handleChange,
                    form.values.percentage,
                    `percentage,
                    "5",
                  )
              )
              (
                form.values.dataType != "Binary" ?
                  <div /> :
                  input(handleChange, form.values.binary, `binary, "6")
              )
              <Form.Item>
                <Button _type=`primary onClick=(_ => handleSubmit())>
                  ("Submit" |> ste)
                </Button>
              </Form.Item>
            </form>,
        )
        |> ReasonReact.element,
    )
    |> ReasonReact.element,
};